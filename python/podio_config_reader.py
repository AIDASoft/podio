
from __future__ import absolute_import, unicode_literals, print_function
from io import open

import copy
import re
import warnings
import yaml

from collections import OrderedDict

from generator_utils import MemberVariable, DefinitionError, BUILTIN_TYPES


class MemberParser(object):
  """Class to parse member variables from strings without doing too much validation"""
  # Doing this with regex is non-ideal, but we should be able to at least
  # enforce something that will yield valid c++ identifiers even if we might not
  # cover all possibilities that are admitted by the c++ standard

  # A type can either start with a double colon, or a character (types starting
  # with _ are technically allowed, but partially reserved for compilers)
  # Additionally we have to take int account the possible whitespaces in the
  # builtin types above. Currently this is done by simple brute-forcing.
  # To "ensure" that the builtin matching is greedy and picks up as much as
  # possible, sort the types by their length in descending order
  builtin_str = r'|'.join(r'(?:{})'.format(t) for t in sorted(BUILTIN_TYPES, key=len, reverse=True))
  type_str = r'({builtin_re}|(?:\:{{2}})?[a-zA-Z]+[a-zA-Z0-9:_]*)'.format(builtin_re=builtin_str)
  type_re = re.compile(type_str)

  # Names can be almost anything as long as it doesn't start with a digit and
  # doesn't contain anything fancy
  name_str = r'([a-zA-Z_]+\w*)'
  name_re = re.compile(name_str)

  # Comments can be anything after //
  # stripping of trailing whitespaces is done later as it is hard to do with regex
  comment_str = r'\/\/ *(.*)'
  # std::array declaration with some whitespace distribution freedom
  array_str = r' *std::array *< *{typ} *, *([0-9]+) *>'.format(typ=type_str)

  array_re = re.compile(array_str)
  full_array_re = re.compile(r'{array} *{name} *{comment}'.format(
      array=array_str, name=name_str, comment=comment_str))
  member_re = re.compile(r' *{typ} +{name} *{comment}'.format(
      typ=type_str, name=name_str, comment=comment_str))

  # For cases where we don't require a description
  bare_member_re = re.compile(r' *{typ} +{name}'.format(typ=type_str, name=name_str))
  bare_array_re = re.compile(r' *{array} +{name}'.format(array=array_str, name=name_str))

  @staticmethod
  def _parse_with_regexps(string, regexps_callbacks):
    """Parse the string using the passed regexps and corresponding callbacks that
    take the match and return a MemberVariable from it"""
    for rgx, callback in regexps_callbacks:
      match = rgx.match(string)
      if match is not None:
        return callback(match)

    raise DefinitionError("'%s' is not a valid member definition" % string)

  @staticmethod
  def _full_array_conv(match):
    typ, size, name, comment = match.groups()
    return MemberVariable(name=name, array_type=typ, array_size=size, description=comment.strip())

  @staticmethod
  def _full_member_conv(match):
    klass, name, comment = match.groups()
    return MemberVariable(name=name, type=klass, description=comment.strip())

  @staticmethod
  def _bare_array_conv(match):
    typ, size, name = match.groups()
    return MemberVariable(name=name, array_type=typ, array_size=size)

  @staticmethod
  def _bare_member_conv(match):
    klass, name = match.groups()
    return MemberVariable(name=name, type=klass)

  def parse(self, string, require_description=True):
    """Parse the passed string"""
    matchers_cbs = [
        (self.full_array_re, self._full_array_conv),
        (self.member_re, self._full_member_conv)
        ]

    if not require_description:
      matchers_cbs.extend((
          (self.bare_array_re, self._bare_array_conv),
          (self.bare_member_re, self._bare_member_conv)
          ))

    return self._parse_with_regexps(string, matchers_cbs)


class ClassDefinitionValidator(object):
  """Validate the datamodel read from the input yaml file for the most obvious
  problems.
  """
  # All these keys refer to datatypes only, the subset that is allowed for
  # components make it possible to more easily check that in the
  # _check_components method
  required_datatype_keys = (
      "Description",
      "Author",
      )
  valid_datatype_member_keys = (
      "Members",
      "VectorMembers",
      "OneToOneRelations",
      "OneToManyRelations",
      # "TransientMembers", # not used anywhere in class generator
      # "Typedefs",         # not used anywhere in class generator
      )
  valid_extra_datatype_keys = (
      "ExtraCode",
      "ConstExtraCode"
      )

  # documented but not yet implemented
  not_yet_implemented_keys = (
      "TransientMembers",
      "Typedefs",
      )

  valid_extra_code_keys = ("declaration", "implementation", "includes")
  # documented but not yet implemented
  not_yet_implemented_extra_code = ('declarationFile', 'implementationFile')

  # stl types that are allowed to appear (other than array)
  allowed_stl_types = ('string',)

  def __init__(self):
    self.components = None
    self.datatypes = None
    self.expose_pod_members = False
    self.warnings = set()

  def _clear(self):
    """Reset all the internal state such that one instance can be used for multiple
    validations"""
    self.warnings = set()

  def validate(self, components, datatypes, expose_pod_members):
    """Validate the datamodel"""
    self.components = components
    self.datatypes = datatypes
    self.expose_pod_members = expose_pod_members
    self._clear()

    self._check_components()
    self._check_datatypes()

  def _check_components(self):
    """Check the components"""
    for name, component in self.components.items():
      for field in component:
        if field not in ['Members', 'ExtraCode']:
          raise DefinitionError("{} defines a '{}' field which is not allowed for a component"
                                .format(name, field))

      if 'ExtraCode' in component:
        for key in component['ExtraCode']:
          if key not in ('declaration', 'includes'):
            raise DefinitionError("'{}' field found in 'ExtraCode' of component '{}'."
                                  " Only 'declaration' and 'includes' are allowed here".format(key, name))

      for member in component['Members']:
        is_builtin = member.is_builtin or member.is_builtin_array
        is_component_array = member.is_array and member.array_type in self.components
        is_component = member.full_type in self.components or is_component_array

        if not is_builtin and not is_component:
          raise DefinitionError("{} defines a member of type '{}' which is not allowed in components"
                                .format(member.name, member.full_type))

  def _check_datatypes(self):
    """Check the datatypes"""
    # Get all of the datatype names here to avoid depending on the order of
    # declaration. NOTE: In this way also invalid classes will be considered,
    # but they should hopefully be caught later
    for name, definition in self.datatypes.items():
      self._check_keys(name, definition)
      self._fill_defaults(definition)
      self._check_datatype(name, definition)

  def _check_datatype(self, classname, definition):
    """Check that a datatype only defines valid types and relations"""
    self._check_members(classname, definition.get("Members", []))
    self._check_relations(classname, definition)

  def _check_members(self, classname, members):
    """Check the members of a class for name clashes or undefined classes"""
    all_types = [n for n in self.datatypes] + [n for n in self.components]

    all_members = {}
    for member in members:
      for stl_type in self.allowed_stl_types:
        if member.full_type.startswith('std::' + stl_type):
          self.warnings.add("{} defines a std::{} member ('{}') that spoils PODness"
                            .format(classname, stl_type, member.name))

      is_builtin = member.is_builtin or member.is_builtin_array
      in_definitions = member.full_type in all_types or member.array_type in all_types

      if not is_builtin and not in_definitions:
        raise DefinitionError("'{}' defines member '{}' of type '{}' that is not declared!"
                              .format(classname, member.name, member.full_type))

      if member.name in all_members:
        raise DefinitionError("'{}' clashes with another member in class '{}', previously defined in '{}'"
                              .format(member.name, classname, all_members[member.name]))

      all_members[member.name] = classname
      if self.expose_pod_members and not member.is_builtin and not member.is_array:
        for sub_member in self.components[member.full_type]['Members']:
          if sub_member.name in all_members:
            raise DefinitionError("'{}' clashes with another member name in class '{}'"
                                  " (defined in the component '{}' and '{}')"
                                  .format(sub_member.name, classname, member.name, all_members[sub_member.name]))

          all_members[sub_member.name] = "member '{}'".format(member.name)

  def _check_relations(self, classname, definition):
    """Check the relations of a class"""
    many_relations = definition.get("OneToManyRelations", [])
    for relation in many_relations:
      if relation.full_type not in self.datatypes:
        raise DefinitionError("'{}' declares a non-allowed many-relation to '{}'"
                              .format(classname, relation.full_type))

    vector_members = definition.get("VectorMembers", [])
    for vecmem in vector_members:
      if not vecmem.is_builtin and vecmem.full_type not in self.components:
        raise DefinitionError("'{}' declares a non-allowed vector member of type '{}'"
                              .format(classname, vecmem.full_type))

  def _check_keys(self, classname, definition):
    """Check the keys of a datatype"""
    allowed_keys = self.required_datatype_keys + self.valid_datatype_member_keys + self.valid_extra_datatype_keys
    # Give some more info for not yet implemented features
    invalid_keys = [k for k in definition.keys() if k not in allowed_keys]
    if invalid_keys:
      not_yet_impl = [k for k in invalid_keys if k in self.not_yet_implemented_keys]
      if not_yet_impl:
        not_yet_impl = ' (not yet implemented: {})'.format(not_yet_impl)
      else:
        not_yet_impl = ''

      raise DefinitionError("'{}' defines invalid categories: {}{}"
                            .format(classname, invalid_keys, not_yet_impl))

    for key in self.required_datatype_keys:
      if key not in definition:
        raise DefinitionError("'{}' does not define '{}'".format(classname, key))

    if 'ExtraCode' in definition:
      extracode = definition['ExtraCode']
      invalid_keys = [k for k in extracode if k not in self.valid_extra_code_keys]
      if invalid_keys:
        not_yet_impl = [k for k in invalid_keys if k in self.not_yet_implemented_extra_code]
        if not_yet_impl:
          not_yet_impl = ' (not yet implemented: {})'.format(not_yet_impl)
        else:
          not_yet_impl = ''

        raise DefinitionError("{} defines invalid 'ExtraCode' categories: {} (not yet implemented: {})"
                              .format(classname, invalid_keys, not_yet_impl))

  def _fill_defaults(self, definition):
    """Fill some of the fields with empty defaults in order to make it easier to
    handle them afterwards and not having to check everytime whether they exist.
    TODO: This is a rather ugly thing to do as it strongly couples all the
    components (reader, validation, generator) to each other. But currently the
    generator assumes that all these fields are present and would require a lot
    of changes to accomodate to optionally get these. Trying to at least
    encapsulate this into one place here, such that it makes it easier to remove
    once the generator is more robust against missing fields
    """
    for field in self.valid_datatype_member_keys:
      if field not in definition:
        definition[field] = []

    for field in self.valid_extra_datatype_keys:
      if field not in definition:
        definition[field] = {}


def ordered_load(stream, Loader=yaml.Loader, object_pairs_hook=OrderedDict):

  class OrderedLoader(Loader):
    pass

  def construct_mapping(loader, node):
    loader.flatten_mapping(node)
    return object_pairs_hook(loader.construct_pairs(node))
  OrderedLoader.add_constructor(
      yaml.resolver.BaseResolver.DEFAULT_MAPPING_TAG,
      construct_mapping)
  return yaml.load(stream, OrderedLoader)


class PodioConfigReader(object):
  """Config reader that does basic parsing of the member definitions and puts
  everything into a somewhat uniform structure without doing any fancy
  validation
  """
  member_parser = MemberParser()

  def __init__(self, yamlfile):
    self.yamlfile = yamlfile
    self.datatypes = OrderedDict()
    self.components = OrderedDict()
    self.options = {
        # should getters / setters be prefixed with get / set?
        "getSyntax": False,
        # should POD members be exposed with getters/setters in classes that have them as members?
        "exposePODMembers": True,
        # use subfolder when including package header files
        "includeSubfolder": False,
        }
    self.warnings = set()

  @staticmethod
  def _handle_extracode(definition):
    return copy.deepcopy(definition)

  @staticmethod
  def _read_component_old_definition(definition):
    """Read the component and put it into a similar structure as the datatypes, i.e.
    a dict with a 'Members' and an 'ExtraCode' field for easier handling
    afterwards
    """
    WARNING_TEXT = """

    You are using the deprecated old style of defining components:

    components:
      ExampleComponent:
        x : int
        ExtraCode:
          declaration: "// some code here"

    This option will be removed with version 1.0.
    Switch to the new style of defining components (consistent with datatypes definitions):

    components:
      ExampleComponent:
        Members:
          - int x // an optional description here
        ExtraCode:
          declaration: "// some code here"

    """
    warnings.warn(WARNING_TEXT, FutureWarning, stacklevel=3)
    component = {'Members': []}
    for name, klass in definition.items():
      if name == 'ExtraCode':
        component['ExtraCode'] = klass
      else:
        valid_type = MemberParser.type_re.match(klass)
        valid_name = MemberParser.name_re.match(name)
        c_style_array = re.search(r'\[.*\]', klass)
        if valid_type and valid_name and not c_style_array:
          array_match = MemberParser.array_re.match(klass)
          if array_match:
            component['Members'].append(MemberVariable(name=name, array_type=array_match.group(1),
                                                       array_size=array_match.group(2)))
          else:
            component['Members'].append(MemberVariable(name=name, type=klass))
        else:
          raise DefinitionError("'{}: {}' is not a valid member definition".format(name, klass))

    return component

  def _read_component(self, definition):
    """Read the component and put it into an easily digestible format.

    Currently handles two versions of syntax:
    - One that is different than the one used for datatypes, deprecated and
    planned for removal with 1.0
    - A consistent one with the syntax for datatypes, with slightly less
    capabilities
    """
    # Very basic check here to differentiate between old and new style component
    # definitions
    if "Members" not in definition:
      return self._read_component_old_definition(definition)

    component = {}
    for name, category in definition.items():
      if name == 'Members':
        component['Members'] = []
        for member in definition[name]:
          # for components we do not require a description in the members
          component['Members'].append(self.member_parser.parse(member, False))
      else:
        component[name] = copy.deepcopy(category)

    return component

  def _read_datatype(self, value):
    """Read the datatype and put it into an easily digestible format"""
    datatype = {}
    for category, definition in value.items():
      # special handling of the member types. Parse them here directly
      if category in ClassDefinitionValidator.valid_datatype_member_keys:
        members = []
        for member in definition:
          members.append(self.member_parser.parse(member))
        datatype[category] = members
      else:
        datatype[category] = copy.deepcopy(definition)

    return datatype

  def read(self):
    stream = open(self.yamlfile, "r")
    content = ordered_load(stream, yaml.SafeLoader)

    if "components" in content:
      for klassname, value in content["components"].items():
        self.components[klassname] = self._read_component(value)

    if "datatypes" in content:
      for klassname, value in content["datatypes"].items():
        self.datatypes[klassname] = self._read_datatype(value)

    if "options" in content:
      for option, value in content["options"].items():
        self.options[option] = value

    # If this doesn't raise an exception everything should in principle work out
    validator = ClassDefinitionValidator()
    validator.validate(self.components, self.datatypes, self.options.get("exposePODMembers", False))
    self.warnings = validator.warnings

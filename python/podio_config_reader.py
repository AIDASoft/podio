
from __future__ import absolute_import, unicode_literals, print_function
from io import open

import yaml
import copy
import re

from collections import OrderedDict

class MemberVariable(object):
  """Simple class to hold information about a member variable"""
  def __init__(self, **kwargs):
    self.name = kwargs.pop('name', '')
    self.full_type = kwargs.pop('type', '')
    self.description = kwargs.pop('description', '')
    self.is_builtin = False
    self.is_builtin_array = False

    is_array = kwargs.pop('is_array', False)
    self.array_type = kwargs.pop('array_type', None)
    self.array_size = kwargs.pop('array_size', None)
    if is_array and not (self.array_type and self.array_size):
      array_match = MemberParser.array_re.match(self.full_type)
      if array_match:
        self.array_type, self.array_size = array_match.groups()
      else:
        raise ValueError("Trying to construct MemberVariable with 'is_array' but 'type' "
                         "is not a valid array definition")

    self.is_array = is_array or (self.array_type is not None and self.array_size is not None)
    if self.is_array:
      self.full_type = r'std::array<{}, {}>'.format(self.array_type, self.array_size)
      self.is_builtin_array = self.array_type in MemberParser.builtin_types

    self.is_builtin = self.full_type in MemberParser.builtin_types

    if kwargs:
      raise ValueError("Unused kwargs in MemberVariable: {}".format(kwargs.keys()))


  def __str__(self):
    """string representation"""
    # Make sure to include scope-operator if necessary
    # TODO: Make sure that this really does cover all use-cases
    scoped_type = self.full_type
    # if '::' in self.full_type:
    #   namespace, klassname = self.full_type.split('::')
    #   scoped_type = '::{namespace}::{type}'.format(namespace=namespace, type=klassname)

    definition = r'{} {};'.format(scoped_type, self.name)
    if self.description:
      definition += r' ///< {}'.format(self.description)
    return definition


class DefinitionError(Exception):
  """Exception raised by the ClassDefinitionValidator for invalid definitions.
  Mainly here to distinguish it from plain exceptions that are otherwise raised.
  In this way this makes it possible to selectively catch exceptions related to
  the datamodel definition without also catching all the rest which might point
  to another problem
  """
  pass


class MemberParser(object):
  """Class to parse member variables from strings without doing too much validation"""
  builtin_types = ["int", "long", "float", "double",
                   "unsigned int", "unsigned", "unsigned long",
                   "short", "bool", "long long",
                   "unsigned long long", "std::string"]

  # Doing this with regex is non-ideal, but we should be able to at least
  # enforce something that will yield valid c++ identifiers even if we might not
  # cover all possibilities that are admitted by the c++ standard

  # A type can either start with a double colon, or a character (types starting
  # with _ are technically allowed, but partially reserved for compilers)
  # Additionally we have to take int account the possible whitespaces in the
  # builtin types above. Currently this is done by simple brute-forcing
  type_str = r'((?:\:{{2}})?[a-zA-Z]+[a-zA-Z0-9:_]*|{builtin_re})'.format(
      builtin_re=r'|'.join((r'(?:{})'.format(t)) for t in builtin_types))
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


  def parse(self, string):
    """Parse the passed string"""
    array_match = self.full_array_re.match(string)
    if array_match is not None:
      typ, size, name, comment = array_match.groups()
      return MemberVariable(name=name, array_type=typ, array_size=size, description=comment.strip())

    member_match = self.member_re.match(string)
    if member_match:
      klass, name, comment = member_match.groups()
      return MemberVariable(name=name, type=klass, description=comment.strip())

    raise DefinitionError("'%s' is not a valid member definition" % string)


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
    # "ConstExtraCode"    # not used anywhere in class generator
  )

  # documented but not yet implemented
  not_yet_implemented_keys = (
    "TransientMembers",
    "Typedefs",
    "ConstExtraCode"
  )

  valid_extra_code_keys = (
    "declaration", "const_declaration",
    "implementation", "const_implementation",
    "includes"
  )
  # documented but not yet implemented
  not_yet_implemented_extra_code = (
    'declarationFile', 'implementationFile'
  )

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
      if 'ExtraCode' in component:
        for key in ('const_declaration', 'implementation', 'const_implementation'):
          if key in component['ExtraCode']:
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
  def _read_component(definition):
    """Read the component and put it into a similar structure as the datatypes, i.e.
    a dict with a 'Members' and an 'ExtraCode' field for easier handling
    afterwards
    """
    component = {'Members': []}
    for name, klass in definition.items():
      if name == 'ExtraCode':
        component['ExtraCode'] = klass
      else:
        valid_type = MemberParser.type_re.match(klass)
        valid_name = MemberParser.name_re.match(name)
        c_style_array = re.search(r'\[.*\]', klass)
        if valid_type and valid_name and not c_style_array:
          component['Members'].append(MemberVariable(name=name, type=klass,
                                                     is_array='std::array' in klass))
        else:
          raise DefinitionError("'{}: {}' is not a valid member definition".format(name, klass))

        if not MemberParser.type_re.match(klass) or '[' in klass:
          raise DefinitionError("'{}' ")


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

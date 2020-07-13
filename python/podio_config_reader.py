
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
      array_match = ClassDefinitionValidator.array_re.match(self.full_type)
      if array_match:
        self.array_type, self.array_size = array_match.groups()
      else:
        raise ValueError("Trying to construct MemberVariable with 'is_array' but 'type' "
                         "is not a valid array definition")

    self.is_array = is_array or (self.array_type is not None and self.array_size is not None)
    if self.is_array:
      self.full_type = r'std::array<{}, {}>'.format(self.array_type, self.array_size)
      self.is_builtin_array = self.array_type in ClassDefinitionValidator.buildin_types

    self.is_builtin = self.full_type in ClassDefinitionValidator.buildin_types

    if kwargs:
      raise ValueError("Unused kwargs in MemberVariable: {}".format(kwargs.keys()))


  def __str__(self):
    """string representation"""
    definition = r'{} {};'.format(self.full_type, self.name)
    if self.description:
      definition += r' ///< {}'.format(self.description)
    return definition


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


class ClassDefinitionValidator(object):
  """
  Validate the input yaml file for the
  most obvious problems.
  """

  valid_keys = (
      "Description",
      "Author",
      "Members",
      "VectorMembers",
      "OneToOneRelations",
      "OneToManyRelations",
      "TransientMembers",
      "Typedefs",
      "ExtraCode",
      "ConstExtraCode"
      )

  buildin_types = ["int", "long", "float", "double",
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
      builtin_re=r'|'.join((r'(?:{})'.format(t)) for t in buildin_types))

  # Names can be almost anything as long as it doesn't start with a digit and
  # doesn't contain anything fancy
  name_str = r'([a-zA-Z_]+\w*)'
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


  def __init__(self, configuration):
    self.components = OrderedDict()
    self.datatypes = OrderedDict()
    if "datatypes" in configuration:
      self.datatypes = configuration["datatypes"]
    if "components" in configuration:
      self.components = configuration["components"]

  @staticmethod
  def check_keys(name, definition):
    """Check that only valid keys are provided"""
    for key in definition:
      if key not in ClassDefinitionValidator.valid_keys:
        raise Exception("%s defines invalid category '%s' "
                        % (name, key))

  def check_datatype(self, name, definition):
    self.check_keys(name, definition)
    for category in ("Author", "Description"):
      if category not in definition:
        raise Exception("%s does not define '%s'." % (name, category))
    for category in ("Members", "OneToOneRelations", "OneToManyRelations"):
      if category in definition:
        self.check_members(name, definition[category])
    # TODO: handling of vector members

  def check_members(self, name, members):
    for item in members:
      member = self.parse_member(item)
      theType = member.full_type
      return  # TODO
      if theType not in self.buildin_types and \
         theType not in self.datatypes and \
         theType not in self.components:
        raise Exception("%s defines a member of not allowed type %s"
                        % (name, theType))

  def parse_member(self, string):
    """
    extract type, name, and description from
    - type name // description

    returns a dictionary with these three keys and the extracted values (stripped)
    """
    array_match = self.full_array_re.match(string)
    if array_match is not None:
      typ, size, name, comment = array_match.groups()

      if typ not in self.buildin_types and typ not in self.components:
        raise Exception("%s defines an array of disallowed type %s"
                        % (string, typ))
      klass = "std::array<{typ}, {size}>".format(typ=typ, size=size)
      return MemberVariable(**{'name': name, 'type': klass, 'description': comment.strip(),
                               'is_array': True})

    member_match = self.member_re.match(string)
    if member_match:
      klass, name, comment = member_match.groups()
      return MemberVariable(**{'name': name, 'type': klass, 'description': comment.strip()})

    raise Exception('%s is not a valid member definition' % string)


  def check_component(self, name, definition):
    """
    Check that components only contain simple types
    or other components
    """
    for mem, klass in definition.items():
      if mem == 'ExtraCode':
        if any(d in klass for d in ['const_declaration', 'implementation']):
          raise Exception("'const_declaration' or 'implementation' field found in the 'ExtraCode' "
                          " of component '{name}', which is not allowed".format(name=name))

      elif not klass in self.buildin_types or klass in self.components:
        array_match = re.match(self.array_re, klass)
        if array_match is not None:
          typ = array_match.group(1)
          if typ not in self.buildin_types and typ not in self.components:
            raise Exception("'%s' defines a member of a type '%s'"
                            % (name, klass) +
                            "which is not allowed in a component!")


class PodioConfigReader(object):

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
    self.validator = None


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
        component['Members'].append(MemberVariable(name=name, type=klass,
                                                   is_array='std::array' in klass))

    return component


  def _read_datatype(self, value):
    """Read the datatype and put it into an easily digestible format"""
    datatype = {}
    datatype["Description"] = value["Description"]
    datatype["Author"] = value["Author"]

    for category in ("Members",
                     "VectorMembers",
                     "OneToOneRelations",
                     "OneToManyRelations",
                     "TransientMembers",
                     "Typedefs"):
      definitions = []
      for definition in value.get(category, {}):
        definitions.append(self.validator.parse_member(definition))
      datatype[category] = definitions

    for category in ("ExtraCode", "ConstExtraCode"):
      if category in value:
        datatype[category] = PodioConfigReader._handle_extracode(value[category])

    return datatype


  def read(self):
    stream = open(self.yamlfile, "r")
    content = ordered_load(stream, yaml.SafeLoader)
    self.validator = ClassDefinitionValidator(content)

    if "components" in content:
      for klassname, value in content["components"].items():
        self.validator.check_component(klassname, value)
        self.components[klassname] = self._read_component(value)

    if "datatypes" in content:
      for klassname, value in content["datatypes"].items():
        self.validator.check_datatype(klassname, value)
        self.datatypes[klassname] = self._read_datatype(value)

    if "options" in content:
      for option, value in content["options"].items():
        self.options[option] = value

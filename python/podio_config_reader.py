
from __future__ import absolute_import, unicode_literals, print_function
from io import open

import yaml
import copy
import re

from collections import OrderedDict


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
      theType = member["type"]
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
      return {'name': name, 'type': klass, 'description': comment.strip()}

    member_match = self.member_re.match(string)
    if member_match:
      klass, name, comment = member_match.groups()
      return {'name': name, 'type': klass, 'description': comment.strip()}

    raise Exception('%s is not a valid member definition' % string)


  def check_component(self, name, definition):
    """
    Check that components only contain simple types
    or other components
    """
    for mem, klass in definition.items():
      if not (mem == "ExtraCode" or klass in self.buildin_types or
              klass in self.components):
        array_match = re.match(self.array_re, klass)
        if array_match is not None:
          typ = array_match.group(1)
          if typ not in self.buildin_types and typ not in self.components:
            raise Exception("'%s' defines a member of a type '%s'"
                            % (name, klass) +
                            "which is not allowed in a component!")


  def check_components(self, components):
    for klassname, value in components.items():
      self.check_component(klassname, value)


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

  @staticmethod
  def handle_extracode(definition):
    return copy.deepcopy(definition)

  def read(self):
    stream = open(self.yamlfile, "r")
    content = ordered_load(stream, yaml.SafeLoader)
    validator = ClassDefinitionValidator(content)
    if "components" in content:
      validator.check_components(content["components"])
      for klassname, value in content["components"].items():
        component = {"Members": value}
        self.components[klassname] = component
    if "datatypes" in content:
      for klassname, value in content["datatypes"].items():
        validator.check_datatype(klassname, value)
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
          if category in value:
            for definition in value[category]:
              definitions.append(validator.parse_member(definition))
            datatype[category] = definitions
          else:
            datatype[category] = []
        if "ExtraCode" in value:
          datatype["ExtraCode"] = self.handle_extracode(
              value["ExtraCode"])
        if "ConstExtraCode" in value:
          datatype["ConstExtraCode"] = self.handle_extracode(
              value["ConstExtraCode"])
        self.datatypes[klassname] = datatype
    if "options" in content:
      for option, value in content["options"].items():
        self.options[option] = value

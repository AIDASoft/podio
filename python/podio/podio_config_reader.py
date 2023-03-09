"""Datamodel yaml configuration file reading and validation utilities."""


import copy
import re
import warnings
import yaml

from podio.generator_utils import MemberVariable, DefinitionError, BUILTIN_TYPES, DataModel


class MemberParser:
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
  builtin_str = r'|'.join(rf'(?:{t})' for t in sorted(BUILTIN_TYPES, key=len, reverse=True))
  type_str = rf'({builtin_str}|(?:\:{{2}})?[a-zA-Z]+[a-zA-Z0-9:_]*)'
  type_re = re.compile(type_str)

  # Names can be almost anything as long as it doesn't start with a digit and
  # doesn't contain anything fancy
  name_str = r'([a-zA-Z_]+\w*)'
  name_re = re.compile(name_str)

  # Comments can be anything after //
  # stripping of trailing whitespaces is done later as it is hard to do with regex
  comment_str = r'\/\/ *(.*)'
  # std::array declaration with some whitespace distribution freedom
  array_str = rf' *std::array *< *{type_str} *, *([0-9]+) *>'

  # Default values can be anything that in curly braces, but not the empty
  # default initialization, since that is what we generate in any case
  def_val_str = r'(?:{(.+)})?'

  array_re = re.compile(array_str)
  full_array_re = re.compile(rf'{array_str} *{name_str} *{def_val_str} *{comment_str}')
  member_re = re.compile(rf' *{type_str} +{name_str} *{def_val_str} *{comment_str}')

  # For cases where we don't require a description
  bare_member_re = re.compile(rf' *{type_str} +{name_str} *{def_val_str}')
  bare_array_re = re.compile(rf' *{array_str} +{name_str} *{def_val_str}')

  @staticmethod
  def _parse_with_regexps(string, regexps_callbacks):
    """Parse the string using the passed regexps and corresponding callbacks that
    take the match and return a MemberVariable from it"""
    for rgx, callback in regexps_callbacks:
      result = rgx.match(string)
      if result:
        return callback(result)

    raise DefinitionError(f"'{string}' is not a valid member definition")

  @staticmethod
  def _full_array_conv(result):
    """MemberVariable construction for array members with a docstring"""
    typ, size, name, def_val, comment = result.groups()
    return MemberVariable(name=name, array_type=typ, array_size=size, description=comment.strip(), default_val=def_val)

  @staticmethod
  def _full_member_conv(result):
    """MemberVariable construction for members with a docstring"""
    klass, name, def_val, comment = result.groups()
    return MemberVariable(name=name, type=klass, description=comment.strip(), default_val=def_val)

  @staticmethod
  def _bare_array_conv(result):
    """MemberVariable construction for array members without docstring"""
    typ, size, name, def_val = result.groups()
    return MemberVariable(name=name, array_type=typ, array_size=size, default_val=def_val)

  @staticmethod
  def _bare_member_conv(result):
    """MemberVarible construction for members without docstring"""
    klass, name, def_val = result.groups()
    return MemberVariable(name=name, type=klass, default_val=def_val)

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


class ClassDefinitionValidator:
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
      "MutableExtraCode"
      )

  # documented but not yet implemented
  not_yet_implemented_keys = (
      "TransientMembers",
      "Typedefs",
      )

  valid_extra_code_keys = ("declaration", "implementation", "includes")
  # documented but not yet implemented
  not_yet_implemented_extra_code = ('declarationFile', 'implementationFile')

  @classmethod
  def validate(cls, datamodel, upstream_edm=None):
    """Validate the datamodel."""
    cls._check_components(datamodel, upstream_edm)
    expose_pod_members = datamodel.options['exposePODMembers']
    cls._check_datatypes(datamodel, expose_pod_members, upstream_edm)

  @classmethod
  def _check_comp(cls, member, components, upstream_edm):
    """Check if the passed member is a component defined in either the datamodel
    itself or in the upstream datamodel."""
    def _from_upstream():
      return upstream_edm.components if upstream_edm else []

    return member in components or member in _from_upstream()

  @classmethod
  def _check_components(cls, datamodel, upstream_edm):
    """Check the components."""
    for name, component in datamodel.components.items():
      for field in component:
        if field not in ['Members', 'ExtraCode']:
          raise DefinitionError(f"{name} defines a '{field}' field which is not allowed for a component")

      if 'ExtraCode' in component:
        for key in component['ExtraCode']:
          if key not in ('declaration', 'includes'):
            raise DefinitionError(f"'{key}' field found in 'ExtraCode' of component '{name}'."
                                  " Only 'declaration' and 'includes' are allowed here")

      for member in component['Members']:
        is_builtin = member.is_builtin or member.is_builtin_array
        is_component_array = member.is_array and cls._check_comp(member.array_type, datamodel.components, upstream_edm)
        is_component = cls._check_comp(member.full_type, datamodel.components, upstream_edm) or is_component_array

        if not is_builtin and not is_component:
          raise DefinitionError(f'{member.name} of component {name} is not a builtin type, '
                                'another component or one from the upstream EDM')

  @classmethod
  def _check_datatypes(cls, datamodel, expose_pod_members, upstream_edm):
    """Check the datatypes."""
    # Get all of the datatype names here to avoid depending on the order of
    # declaration. NOTE: In this way also invalid classes will be considered,
    # but they should hopefully be caught later
    for name, definition in datamodel.datatypes.items():
      cls._check_keys(name, definition)
      cls._fill_defaults(definition)
      cls._check_datatype(name, definition, expose_pod_members, datamodel, upstream_edm)

  @classmethod
  def _check_datatype(cls, classname, definition, expose_pod_members, datamodel, upstream_edm):
    """Check that a datatype only defines valid types and relations."""
    cls._check_members(classname, definition.get("Members", []), expose_pod_members, datamodel, upstream_edm)
    cls._check_relations(classname, definition, datamodel, upstream_edm)

  @classmethod
  def _check_members(cls, classname, members, expose_pod_members, datamodel, upstream_edm):
    """Check the members of a class for name clashes or undefined classes."""
    all_types = list(datamodel.components.keys())
    ext_types = upstream_edm.components.keys() if upstream_edm else []
    all_types.extend(ext_types)

    all_members = {}
    for member in members:
      is_builtin = member.is_builtin or member.is_builtin_array
      in_definitions = member.full_type in all_types or member.array_type in all_types

      if not is_builtin and not in_definitions:
        raise DefinitionError(f"'{classname}' defines member '{member.name}' of type '{member.full_type}' that is not"
                              " declared!")

      if member.name in all_members:
        raise DefinitionError(f"'{member.name}' clashes with another member in class '{classname}', previously defined"
                              f" in '{all_members[member.name]}'")

      all_members[member.name] = classname
      if expose_pod_members and not member.is_builtin and not member.is_array:
        for sub_member in datamodel.components[member.full_type]['Members']:
          if sub_member.name in all_members:
            raise DefinitionError(f"'{sub_member.name}' clashes with another member name in class '{classname}'"
                                  f" (defined in the component '{member.name}' and '{all_members[sub_member.name]}')")

          all_members[sub_member.name] = f"member '{member.name}'"

  @classmethod
  def _check_relations(cls, classname, definition, datamodel, upstream_edm):
    """Check the relations of a class."""
    def _valid_datatype(rel_type):
      if rel_type in datamodel.datatypes:
        return True
      if upstream_edm and rel_type in upstream_edm.datatypes:
        return True
      return False

    many_relations = definition.get("OneToManyRelations", [])
    for relation in many_relations:
      if not _valid_datatype(relation.full_type):
        raise DefinitionError(f"'{classname}' declares a non-allowed many-relation to '{relation.full_type}'")

    one_relations = definition.get("OneToOneRelations", [])
    for relation in one_relations:
      if not _valid_datatype(relation.full_type):
        raise DefinitionError(f"'{classname}' declares a non-allowed single-relation to '{relation.full_type}'")

    vector_members = definition.get("VectorMembers", [])
    for vecmem in vector_members:
      if not vecmem.is_builtin and not cls._check_comp(vecmem.full_type, datamodel.components, upstream_edm):
        raise DefinitionError(f"'{classname}' declares a non-allowed vector member of type '{vecmem.full_type}'")

  @classmethod
  def _check_keys(cls, classname, definition):
    """Check the keys of a datatype."""
    allowed_keys = cls.required_datatype_keys + cls.valid_datatype_member_keys + cls.valid_extra_datatype_keys
    # Give some more info for not yet implemented features
    invalid_keys = [k for k in definition.keys() if k not in allowed_keys]
    if invalid_keys:
      not_yet_impl = [k for k in invalid_keys if k in cls.not_yet_implemented_keys]
      if not_yet_impl:
        not_yet_impl = f' (not yet implemented: {not_yet_impl})'
      else:
        not_yet_impl = ''

      raise DefinitionError(f"'{classname}' defines invalid categories: {invalid_keys}{not_yet_impl}")

    for key in cls.required_datatype_keys:
      if key not in definition:
        raise DefinitionError(f"'{classname}' does not define '{key}'")

    if 'ExtraCode' in definition:
      extracode = definition['ExtraCode']
      invalid_keys = [k for k in extracode if k not in cls.valid_extra_code_keys]
      if invalid_keys:
        not_yet_impl = [k for k in invalid_keys if k in cls.not_yet_implemented_extra_code]
        if not_yet_impl:
          not_yet_impl = f' (not yet implemented: {not_yet_impl})'
        else:
          not_yet_impl = ''

        raise DefinitionError("{classname} defines invalid 'ExtraCode' categories: {invalid_keys}{not_yet_impl}")

  @classmethod
  def _fill_defaults(cls, definition):
    """Fill some of the fields with empty defaults in order to make it easier to
    handle them afterwards and not having to check everytime whether they exist.
    TODO: This is a rather ugly thing to do as it strongly couples all the
    components (reader, validation, generator) to each other. But currently the
    generator assumes that all these fields are present and would require a lot
    of changes to accomodate to optionally get these. Trying to at least
    encapsulate this into one place here, such that it makes it easier to remove
    once the generator is more robust against missing fields
    """
    for field in cls.valid_datatype_member_keys:
      if field not in definition:
        definition[field] = []

    for field in cls.valid_extra_datatype_keys:
      if field not in definition:
        definition[field] = {}


class PodioConfigReader:
  """Config reader that does basic parsing of the member definitions and puts
  everything into a somewhat uniform structure without doing any fancy
  validation
  """
  member_parser = MemberParser()
  # default options
  options = {
      # should getters / setters be prefixed with get / set?
      "getSyntax": False,
      # should POD members be exposed with getters/setters in classes that have them as members?
      "exposePODMembers": True,
      # use subfolder when including package header files
      "includeSubfolder": False,
      }

  @staticmethod
  def _handle_extracode(definition):
    """Handle the extra code definition. Currently simply returning a copy"""
    return copy.deepcopy(definition)

  @staticmethod
  def _read_component_old_definition(definition):
    """Read the component and put it into a similar structure as the datatypes, i.e.
    a dict with a 'Members' and an 'ExtraCode' field for easier handling
    afterwards
    """
    warning_text = """

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
    warnings.warn(warning_text, FutureWarning, stacklevel=3)
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
          raise DefinitionError(f"'{name}: {klass}' is not a valid member definition")

    return component

  @classmethod
  def _read_component(cls, definition):
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
      return cls._read_component_old_definition(definition)

    component = {}
    for name, category in definition.items():
      if name == 'Members':
        component['Members'] = []
        for member in definition[name]:
          # for components we do not require a description in the members
          component['Members'].append(cls.member_parser.parse(member, False))
      else:
        component[name] = copy.deepcopy(category)

    return component

  @classmethod
  def _read_datatype(cls, value):
    """Read the datatype and put it into an easily digestible format"""
    datatype = {}
    for category, definition in value.items():
      # special handling of the member types. Parse them here directly
      if category in ClassDefinitionValidator.valid_datatype_member_keys:
        members = []
        for member in definition:
          members.append(cls.member_parser.parse(member))
        datatype[category] = members
      else:
        datatype[category] = copy.deepcopy(definition)

    return datatype

  def parse_model(cls, model_dict, package_name, upstream_edm=None):
    """Parse a model from the dictionary, e.g. read from a yaml file."""

    if "schemaversion" in content:
      schema_version = content["schemaversion"]
    else:
      warnings.warn("Please provide a schemaversion entry. It will become mandatory. Setting it to 0 as default",
                    FutureWarning, stacklevel=3)
      schema_version = 0

    components = {}
    if "components" in model_dict:
      for klassname, value in model_dict["components"].items():
        components[klassname] = cls._read_component(value)

    datatypes = {}
    if "datatypes" in model_dict:
      for klassname, value in model_dict["datatypes"].items():
        datatypes[klassname] = cls._read_datatype(value)

    options = copy.deepcopy(cls.options)
    if "options" in model_dict:
      for option, value in model_dict["options"].items():
        options[option] = value

    # Normalize the includeSubfoler internally already here
    if options['includeSubfolder']:
      options['includeSubfolder'] = f'{package_name}/'
    else:
      options['includeSubfolder'] = ''

    # If this doesn't raise an exception everything should in principle work out
    validator = ClassDefinitionValidator()
    datamodel = DataModel(datatypes, components, options, schema_version)
    validator.validate(datamodel, upstream_edm)
    return datamodel

  @classmethod
  def read(cls, yamlfile, package_name, upstream_edm=None):
    """Read the datamodel definition from the yamlfile."""
    with open(yamlfile, "r", encoding='utf-8') as stream:
      content = yaml.load(stream, yaml.SafeLoader)

    return cls.parse_model(content, package_name, upstream_edm)

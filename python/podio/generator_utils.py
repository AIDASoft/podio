#!/usr/bin/env python3
"""
Module holding some generator utility functions
"""

import re
import json


def _get_namespace_class(full_type):
  """Get the namespace and the unqualified classname from the full type. Raise a
  DefinitionError if a nested namespace is found"""
  cnameparts = full_type.split('::')
  if len(cnameparts) > 2:
    raise DefinitionError(f"'{full_type}' is a type with a nested namespace. not supperted, yet.")
  if len(cnameparts) == 2:
    # If in std namespace, consider that to be part of the type instead and only
    # split namespace if that is not the case
    if cnameparts[0] != 'std':
      return cnameparts

  return "", full_type


def _prefix_name(name, prefix):
  """Prefix the name and capitalize the first letter if the prefix is not empty"""
  if prefix:
    return prefix + name[0].upper() + name[1:]
  return name


def get_julia_type(cpp_type=None, is_array=False, array_type=None, array_size=None):
  """Parse the given c++ type to a Julia type"""
  builtin_types_map = {"int": "Int32", "float": "Float32", "double": "Float64",
                       "bool": "Bool", "long": "Int32", "unsigned int": "UInt32",
                       "unsigned long": "UInt32", "char": "Char", "short": "Int16",
                       "long long": "Int64", "unsigned long long": "UInt64"}
  # is a global type as described in test_MemberParser.py #L121
  if cpp_type and cpp_type.startswith("::"):
    cpp_type = cpp_type[2:]
  if cpp_type in builtin_types_map:
    return builtin_types_map[cpp_type]

  if not is_array:
    if cpp_type.startswith('std::'):
      cpp_type = cpp_type[5:]
    if cpp_type in ALLOWED_FIXED_WIDTH_TYPES:
      regex_string = re.split("(u|)int(8|16|32|64)_t", cpp_type)
      cpp_type = regex_string[1].upper() + "Int" + regex_string[2]
      return cpp_type

  else:
    array_type = get_julia_type(cpp_type=array_type)
    if '::' in array_type:
      array_type = array_type.split('::')[1]
    if array_type not in builtin_types_map.values():
      array_type = 'Main.' + array_type + 'Struct'
    return f"MVector{{{array_size}, {array_type}}}"

  return cpp_type


class DefinitionError(Exception):
  """Exception raised by the ClassDefinitionValidator for invalid definitions.
  Mainly here to distinguish it from plain exceptions that are otherwise raised.
  In this way this makes it possible to selectively catch exceptions related to
  the datamodel definition without also catching all the rest which might point
  to another problem
  """


# Types considered to be builtin
BUILTIN_TYPES = ["int", "long", "float", "double",
                 "unsigned int", "unsigned", "unsigned long",
                 "char", "short", "bool", "long long",
                 "unsigned long long"]

# Fixed width types defined in <cstdint>. Omitting int8_t and uint8_t since they
# are often only aliases for signed char and unsigned char, which tends to break
# expectations towards the behavior of integer types. Also omitting the _fastN_t
# and leastN_t since those are probably already covered by the usual integer
# types.
ALLOWED_FIXED_WIDTH_TYPES = ["int16_t", "int32_t", "int64_t",
                             "uint16_t", "uint32_t", "uint64_t"]

# All fixed width integer types that may be defined in <cstdint>
ALL_FIXED_WIDTH_TYPES_RGX = re.compile(r'u?int(_(fast|least))?(8|16|32|64)_t')


def _is_fixed_width_type(type_name):
  """Check if the passed type is an fixed width type and that it is allowed"""
  # Remove the potentially present std:: namespace
  if type_name.startswith('std::'):
    type_name = type_name[5:]

  if ALL_FIXED_WIDTH_TYPES_RGX.match(type_name):
    return (True, type_name in ALLOWED_FIXED_WIDTH_TYPES)

  return (False, False)


class DataType:
  """Simple class to hold information about a datatype or component that is
  defined in the datamodel."""
  def __init__(self, klass):
    self.full_type = klass
    self.namespace, self.bare_type = _get_namespace_class(self.full_type)

  def __str__(self):
    if self.namespace:
      scoped_type = f'::{self.namespace}::{self.bare_type}'
    else:
      scoped_type = self.full_type

    return scoped_type


class MemberVariable:
  """Simple class to hold information about a member variable"""
  def __init__(self, name, **kwargs):
    self.name = name
    self.full_type = kwargs.pop('type', '')
    self.description = kwargs.pop('description', '')
    self.default_val = kwargs.pop('default_val', None)
    self.is_builtin = False
    self.is_builtin_array = False
    self.is_array = False
    # ensure that this will break somewhere if requested but not set
    self.namespace, self.bare_type = None, None
    self.julia_type = None
    self.array_namespace, self.array_bare_type = None, None

    self.array_type = kwargs.pop('array_type', None)
    self.array_size = kwargs.pop('array_size', None)

    self.includes = set()
    self.jl_imports = set()

    if kwargs:
      raise ValueError(f"Unused kwargs in MemberVariable: {list(kwargs.keys())}")

    if self.array_type is not None and self.array_size is not None:
      self.is_array = True
      self.is_builtin_array = self.array_type in BUILTIN_TYPES
      # We also have to check if this is a fixed width integer array
      if not self.is_builtin_array:
        is_fw, valid_fw = _is_fixed_width_type(self.array_type)
        if is_fw:
          if not valid_fw:
            raise DefinitionError(f'{self.full_type} is an array of a fixed width integer type that is not allowed')
          self.is_builtin_array = True
          if not self.array_type.startswith('std::'):
            self.array_type = f'std::{self.array_type}'
          self.includes.add('#include <cstdint>')

      self.full_type = rf'std::array<{self.array_type}, {self.array_size}>'
      self.includes.add('#include <array>')
      self.jl_imports.add('using StaticArrays')

    self.is_builtin = self.full_type in BUILTIN_TYPES

    # We still have to check if this type is a valid fixed width type that we
    # also consider to be builtin types
    if not self.is_builtin:
      is_fw, valid_fw = _is_fixed_width_type(self.full_type)
      if is_fw:
        if not valid_fw:
          raise DefinitionError(f'{self.full_type} is a fixed width integer type that is not allowed')

        self.is_builtin = True
        # "Normalize" the name by prepending it with the std namespace if necessary
        if not self.full_type.startswith('std::'):
          self.full_type = f'std::{self.full_type}'
        self.includes.add('#include <cstdint>')

    # For usage in constructor signatures
    self.signature = self.full_type + ' ' + self.name

    # Needed in case the PODs are exposed
    self.sub_members = None

    if self.is_array:
      self.array_namespace, self.array_bare_type = _get_namespace_class(self.array_type)
    else:
      self.namespace, self.bare_type = _get_namespace_class(self.full_type)

    self.julia_type = get_julia_type(cpp_type=self.bare_type, is_array=self.is_array,
                                     array_type=self.array_type, array_size=self.array_size)

  def __str__(self):
    """string representation"""
    # Make sure to include scope-operator if necessary
    if self.namespace:
      scoped_type = f'::{self.namespace}::{self.bare_type}'
    else:
      scoped_type = self.full_type

    if self.default_val:
      definition = rf'{scoped_type} {self.name}{{{self.default_val}}};'
    else:
      definition = rf'{scoped_type} {self.name}{{}};'

    if self.description:
      definition += rf' ///< {self.description}'
    return definition

  def getter_name(self, get_syntax):
    """Get the getter name of the variable"""
    if not get_syntax:
      return self.name
    return _prefix_name(self.name, "get")

  def setter_name(self, get_syntax, is_relation=False):
    """Get the setter name of the variable"""
    if is_relation:
      if not get_syntax:
        return 'add' + self.name
      return _prefix_name(self.name, 'addTo')

    if not get_syntax:
      return self.name
    return _prefix_name(self.name, 'set')

  def _to_json(self):
    """Return a string representation that can be parsed again."""
    # The __str__ method is geared towards c++ too much, so we have to build
    # things again here from available information
    def_val = f'{{{self.default_val}}}' if self.default_val else ''
    description = f' // {self.description}' if self.description else ''
    return f'{self.full_type} {self.name}{def_val}{description}'


class DataModel:  # pylint: disable=too-few-public-methods
  """A class for holding a complete datamodel read from a configuration file"""
  def __init__(self, datatypes=None, components=None, options=None, schema_version=None):
    self.options = options or {
        # should getters / setters be prefixed with get / set?
        "getSyntax": False,
        # should POD members be exposed with getters/setters in classes that have them as members?
        "exposePODMembers": True,
        # use subfolder when including package header files
        "includeSubfolder": False,
        }
    self.schema_version = schema_version
    self.components = components or {}
    self.datatypes = datatypes or {}

  def _to_json(self):
    """Return the dictionary, so that we can easily hook this into the pythons
    JSON ecosystem"""
    return self.__dict__


class DataModelJSONEncoder(json.JSONEncoder):
  """A JSON encoder for DataModels, resp. anything hat has a _to_json method."""

  def default(self, o):
    """The override for the default, first trying to call _to_json, otherwise
    handing off to the default JSONEncoder"""
    try:
      return o._to_json()  # pylint: disable=protected-access
    except AttributeError:
      return super().default(o)

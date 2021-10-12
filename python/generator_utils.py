#!/usr/bin/env python
"""
Module holding some generator utility functions
"""
from __future__ import unicode_literals, absolute_import, print_function

import re


def _get_namespace_class(full_type):
  """Get the namespace and the unqualified classname from the full type. Raise a
  DefinitionError if a nested namespace is found"""
  cnameparts = full_type.split('::')
  if len(cnameparts) > 2:
    raise DefinitionError("'{}' is a type with a nested namespace. not supperted, yet.".format(full_type))
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


class DefinitionError(Exception):
  """Exception raised by the ClassDefinitionValidator for invalid definitions.
  Mainly here to distinguish it from plain exceptions that are otherwise raised.
  In this way this makes it possible to selectively catch exceptions related to
  the datamodel definition without also catching all the rest which might point
  to another problem
  """
  pass


# Types considered to be builtin
BUILTIN_TYPES = ["int", "long", "float", "double",
                 "unsigned int", "unsigned", "unsigned long",
                 "char", "short", "bool", "long long",
                 "unsigned long long", "std::string"]

# Fixed width types defined in <cstdint>. Omitting int8_t and uint8_t since they
# are often only aliases for signed char and unsigned char, which tends to break
# expectations towards the behavior of integer types. Also omitting the _fastN_t
# and leastN_t since those are probably already covered by the usual integer
# types. These fixed width types should only be used when necessary (e.g. as
# basis for bit fields)
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


class DataType(object):
  """Simple class to hold information about a datatype or component that is
  defined in the datamodel."""
  def __init__(self, klass):
    self.full_type = klass
    self.namespace, self.bare_type = _get_namespace_class(self.full_type)

  def __str__(self):
    if self.namespace:
      scoped_type = '::{}::{}'.format(self.namespace, self.bare_type)
    else:
      scoped_type = self.full_type

    return scoped_type


class MemberVariable(object):
  """Simple class to hold information about a member variable"""
  def __init__(self, name, **kwargs):
    self.name = name
    self.full_type = kwargs.pop('type', '')
    self.description = kwargs.pop('description', '')
    self.is_builtin = False
    self.is_builtin_array = False
    self.is_array = False
    # ensure that this will break somewhere if requested but not set
    self.namespace, self.bare_type = None, None
    self.array_namespace, self.array_bare_type = None, None

    self.array_type = kwargs.pop('array_type', None)
    self.array_size = kwargs.pop('array_size', None)

    self.includes = set()

    if kwargs:
      raise ValueError("Unused kwargs in MemberVariable: {}".format(list(kwargs.keys())))

    if self.array_type is not None and self.array_size is not None:
      self.is_array = True
      self.full_type = r'std::array<{}, {}>'.format(self.array_type, self.array_size)
      self.is_builtin_array = self.array_type in BUILTIN_TYPES
      self.includes.add('#include <array>')

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
    # If used in a relation context. NOTE: The generator might still adapt this
    # depending on other criteria. Here it is just filled with a sane default
    # that works if none of these criteria are met
    self.relation_type = self.full_type

    # Needed in case the PODs are exposed
    self.sub_members = None

    if self.is_array:
      self.array_namespace, self.array_bare_type = _get_namespace_class(self.array_type)
    else:
      self.namespace, self.bare_type = _get_namespace_class(self.full_type)

  def __str__(self):
    """string representation"""
    # Make sure to include scope-operator if necessary
    # TODO: Make sure that this really does cover all use-cases
    if self.namespace:
      scoped_type = '::{}::{}'.format(self.namespace, self.bare_type)
    else:
      scoped_type = self.full_type

    definition = r'{} {}{{}};'.format(scoped_type, self.name)
    if self.description:
      definition += r' ///< {}'.format(self.description)
    return definition

  def as_const(self):
    """Get the Const name for the type without any namespace"""
    if self.is_array or self.is_builtin:
      raise ValueError('Trying to get the Const version of a builtin or array member')
    return 'Const{}'.format(self.bare_type)

  def as_qualified_const(self):
    """string representation for the ConstType including namespace"""
    if self.namespace:
      return '::{nsp}::{cls}'.format(nsp=self.namespace, cls=self.as_const())
    return self.as_const()

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
      else:
        return _prefix_name(self.name, 'addTo')

    else:
      if not get_syntax:
        return self.name
      else:
        return _prefix_name(self.name, 'set')

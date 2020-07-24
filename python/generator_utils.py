#!/usr/bin/env python
"""
Module holding some generator utility functions
"""
from __future__ import unicode_literals, absolute_import, print_function

from copy import deepcopy

from podio_templates import declarations, implementations


def _get_namespace_class(full_type):
  """Get the namespace and the unqualified classname from the full type. Raise a
  DefinitionError if a nested namespace is found"""
  cnameparts = full_type.split('::')
  if len(cnameparts) > 2:
    raise DefinitionError("'{}' is a type with a nested namespace. not supperted, yet.".format(full_type))
  if len(cnameparts) == 2:
    return cnameparts

  return "", full_type


def demangle_classname(classname):
  """Get the namespace, classname as well as opening and closing statements (for
  the namespace) from the full classname"""
  namespace, rawclassname = _get_namespace_class(classname)
  if namespace:
    namespace_open = "namespace {} {{".format(namespace)
    namespace_close = "}} // namespace {}".format(namespace)
  else:
    namespace_open, namespace_close = "", ""

  return namespace, rawclassname, namespace_open, namespace_close


def _prefix_name(name, prefix):
  """Prefix the name and capitalize the first letter if the prefix is not empty"""
  if prefix:
    return prefix + name[0].upper() + name[1:]
  return name


def _format(string, **replacements):
  """Do 'format'-like replacements of the passed key-value pairs in strings that
  also contain other curly-braces"""
  res_string = string
  for key, repl in replacements.items():
    res_string = res_string.replace('{' + key + '}', repl)
  return res_string


def get_extra_code(classname, definition):
  """Get the extra code from a datatype definition"""
  extra_code = {'decl': "", 'const_decl': "", 'impl': "", 'const_impl': "", 'includes': set()}

  extra = definition.get('ExtraCode', {})
  const_extra = definition.get('ConstExtraCode', {})
  if not extra and not const_extra:
    return extra_code

  extra_code['decl'] = _format(extra.get('declaration', ''), name=classname)
  extra_code['impl'] = _format(extra.get('implementation', ''), name=classname)

  extra_code['const_decl'] = _format(const_extra.get('declaration', ''), name='Const' + classname)
  extra_code['decl'] += '\n' + _format(const_extra.get('declaration', ''), name=classname)

  extra_code['const_impl'] = _format(const_extra.get('implementation', ''), name='Const' + classname)
  extra_code['impl'] += '\n' + _format(const_extra.get('implementation', ''), name=classname)

  # For the moment join the includes for both cases
  extra_code['includes'].update(extra.get('includes', '').split('\n'))
  extra_code['includes'].update(const_extra.get('includes', '').split('\n'))

  return extra_code


def _get_format_pattern_func(default_replacements):
  """Get a format function that has some default replacements and can additionally
  take some more replacements that override the defaults"""
  def _format_pattern(pattern_str, **replacements):
    """format the given string updating the default_replacements as necessary"""
    # make the arguments to this override the defaults if need be
    repls = deepcopy(default_replacements)
    repls.update(replacements)
    return pattern_str.format(**repls)

  return _format_pattern


def get_fmt_func(**replacements):
  """Wraper that works with kwargs"""
  return _get_format_pattern_func(replacements)


def _make_return_dict(getter_decls, getter_impls, setter_decls, setter_impls, const_getter_impls):
  """Put all the generation code into a dict to have some structure. Arguments can
  either be a list of strings or just plain strings"""
  return {
      'decl': {
          'get': ''.join(getter_decls),
          'set': ''.join(setter_decls)
          },
      'impl': {
          'get': ''.join(getter_impls),
          'set': ''.join(setter_impls),
          'const_get': ''.join(const_getter_impls)
          }
      }


def generate_get_set_member(member, classname, get_syntax, components=None):
  """Create the getters and setters for members of the class if components are
  passed the members of the components will also be exposed"""
  default_replacements = {
      'type': member.full_type,
      'name': member.name,
      'description': member.description,
      'classname': classname
      }
  _format_pattern = _get_format_pattern_func(default_replacements)
  getname, setname = member.getter_setter_names(get_syntax)

  getter_decls = []
  getter_impls = []
  setter_decls = []
  setter_impls = []
  const_getter_impls = []

  getter_decls.append(_format_pattern(declarations["member_getter"], fname=getname))
  getter_impls.append(_format_pattern(implementations["member_getter"], fname=getname))
  const_getter_impls.append(_format_pattern(implementations["const_member_getter"], fname=getname))

  if member.is_builtin or member.is_array:
    setter_decls.append(_format_pattern(declarations["member_builtin_setter"], fname=setname))
    setter_impls.append(_format_pattern(implementations["member_builtin_setter"], fname=setname))

    if member.is_array:
      # common replacements
      repls = {'type': member.array_type, 'fname': getname}
      getter_decls.append(_format_pattern(declarations["array_member_getter"], **repls))
      getter_impls.append(_format_pattern(implementations["array_member_getter"], **repls))
      const_getter_impls.append(_format_pattern(implementations["const_array_member_getter"], **repls))

      repls['fname'] = setname
      setter_decls.append(_format_pattern(declarations["array_builtin_setter"], **repls))
      setter_impls.append(_format_pattern(implementations["array_builtin_setter"], **repls))

  else:
    # To be compatible with previously generated code, the get/set naming scheme
    # is not used for getting the non-const references
    setter_decls.append(_format_pattern(declarations["member_class_refsetter"], fname=member.name))
    setter_impls.append(_format_pattern(implementations["member_class_refsetter"], fname=member.name))

    setter_decls.append(_format_pattern(declarations["member_class_setter"], fname=setname))
    setter_impls.append(_format_pattern(implementations["member_class_setter"], fname=setname))

    if components is not None:
      for sub_member in components[member.full_type]['Members']:
        getname, setname = sub_member.getter_setter_names(get_syntax)

        repls = {'type': sub_member.full_type, 'name': sub_member.name,
                 'fname': getname, 'compname': member.name}
        getter_decls.append(_format_pattern(declarations["pod_member_getter"], **repls))
        getter_impls.append(_format_pattern(implementations["pod_member_getter"], **repls))
        const_getter_impls.append(_format_pattern(implementations["const_pod_member_getter"], **repls))

        repls['fname'] = setname
        if sub_member.is_builtin:
          setter_decls.append(_format_pattern(declarations["pod_member_builtin_setter"], **repls))
          setter_impls.append(_format_pattern(implementations["pod_member_builtin_setter"], **repls))

        else:
          setter_decls.append(_format_pattern(declarations["pod_member_class_refsetter"], **repls))
          setter_impls.append(_format_pattern(implementations["pod_member_class_refsetter"], **repls))

          setter_decls.append(_format_pattern(declarations["pod_member_class_setter"], **repls))
          setter_impls.append(_format_pattern(implementations["pod_member_class_setter"], **repls))

  # give this thing a minimum amount structure
  return _make_return_dict(getter_decls, getter_impls, setter_decls, setter_impls, const_getter_impls)


def generate_get_set_relation(relation, classname, get_syntax):
  """Create getters and setters for 'OneToOneRelations'"""
  namespace, cls, _, _ = demangle_classname(relation.full_type)
  default_replacements = {
      'name': relation.name,
      'description': relation.description,
      'classname': classname,
      'namespace': namespace,
      'type': cls
      }
  _format_pattern = _get_format_pattern_func(default_replacements)

  getname, setname = relation.getter_setter_names(get_syntax)

  getter_decl = _format_pattern(declarations['one_rel_getter'], fname=getname)
  getter_impl = _format_pattern(implementations['one_rel_getter'], fname=getname)
  const_getter_impl = _format_pattern(implementations['const_one_rel_getter'], fname=getname)

  setter_decl = _format_pattern(declarations['one_rel_setter'], fname=setname)
  setter_impl = _format_pattern(implementations['one_rel_setter'], fname=setname)

  return _make_return_dict(getter_decl, getter_impl, setter_decl, setter_impl, const_getter_impl)


def constructor_destructor_collection(relations, references, vectormembers):
  """Generate the constructor and destructor bodys for a collection"""
  constructor = []
  destructor = []

  if relations or references:
    destructor.append('  for (auto& pointer : m_refCollections) { if (pointer) delete pointer; }')

  for relation in relations + references:
    destructor.append(implementations["destroy_relations"].format(name=relation.name))
    constructor.append('  m_refCollections.push_back(new std::vector<podio::ObjectID>());')

  for vecmem in vectormembers:
    destructor.append('  if (m_vec_{name}) delete m_vec_{name};'.format(name=vecmem.name))
    repls = {'type': vecmem.full_type, 'name': vecmem.name}
    constructor.append('  m_vecmem_info.emplace_back("{type}", &m_vec_{name});'.format(**repls))
    constructor.append('  m_vec_{name} = new std::vector<{type}>();'.format(**repls))

  return '\n'.join(constructor), '\n'.join(destructor)


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
                 "short", "bool", "long long",
                 "unsigned long long", "std::string"]


class MemberVariable(object):
  """Simple class to hold information about a member variable"""
  def __init__(self, **kwargs):
    self.name = kwargs.pop('name', '')
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

    if kwargs:
      raise ValueError("Unused kwargs in MemberVariable: {}".format(list(kwargs.keys())))

    if self.array_type is not None and self.array_size is not None:
      self.is_array = True
      self.full_type = r'std::array<{}, {}>'.format(self.array_type, self.array_size)
      self.is_builtin_array = self.array_type in BUILTIN_TYPES

    self.is_builtin = self.full_type in BUILTIN_TYPES

    if self.is_array:
      self.array_namespace, self.array_bare_type = _get_namespace_class(self.array_type)
    else:
      self.namespace, self.bare_type = _get_namespace_class(self.full_type)
      if self.namespace == 'std':
        self.namespace = ""
        self.bare_type = self.full_type

  def __str__(self):
    """string representation"""
    # Make sure to include scope-operator if necessary
    # TODO: Make sure that this really does cover all use-cases
    if self.namespace:
      scoped_type = '::{}::{}'.format(self.namespace, self.bare_type)
    else:
      scoped_type = self.full_type

    definition = r'{} {};'.format(scoped_type, self.name)
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

  def getter_setter_names(self, get_syntax, is_relation=False):
    """Get the names for the get/set functions, depending on whether a get/set
    should be prepended
    """
    return self.getter_name(get_syntax), self.setter_name(get_syntax, is_relation)

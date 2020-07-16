#!/usr/bin/env python
"""
Module holding some generator utility functions
"""
from __future__ import unicode_literals, absolute_import, print_function

from copy import deepcopy

from podio_templates import declarations, implementations

def demangle_classname(classname):
  """Get the namespace, classname as well as opening and closing statements (for
  the namespace) from the full classname"""
  namespace_open = ""
  namespace_close = ""
  namespace = ""
  rawclassname = ""
  if "::" in classname:
    cnameparts = classname.split("::")
    if len(cnameparts) > 2:
      raise Exception("'%s' defines a type with nested namespaces. Not supported, yet." % classname)
    namespace, rawclassname = cnameparts
    namespace_open = "namespace %s {" % namespace
    namespace_close = "} // namespace %s" % namespace
  else:
    rawclassname = classname
  return namespace, rawclassname, namespace_open, namespace_close


def remove_namespace(full_type):
  """Get the type after potentially removing the namespace from the name"""
  if '::'in full_type:
    return full_type.split('::')[1]
  return full_type


def get_set_names(name, use_get):
  """Get the names for the get/set functions, depending on whether a get/set
  should be prepended
  """
  if not use_get:
    return name, name

  caps_name = name[0].upper() + name[1:]
  return [s + caps_name for s in ["get", "set"]]


def get_set_relations(name, use_get):
  """Function names for get/setting of relations depending on whether 'get'-syntax is used"""
  if not use_get:
    return name, 'add' + name

  caps_name = name[0].upper() + name[1:]
  return ['get' + caps_name, 'addTo' + caps_name]


def _format(string, **replacements):
  """Do 'format'-like replacements of the passed key-value pairs in strings that
  also contain other curly-braces"""
  res_string = string
  for key, repl in replacements.items():
    res_string = res_string.replace('{' + key + '}', repl)
  return res_string


def get_extra_code(classname, definition):
  """Get the extra code from a datatype definition"""
  extra_code = {'decl': "", 'const_decl': "", 'code': "", 'const_code': "", 'includes': set()}

  extra = definition.get('ExtraCode', {})
  if not extra:
    return extra_code

  extra_code['decl'] = _format(extra.get('declaration', ''), name=classname)
  extra_code['code'] = _format(extra.get('implementation', ''), name=classname)

  if 'const_declaration' in extra:
    extra_code['const_decl'] = _format(extra['const_declaration'], name='Const' + classname)
    extra_code['decl'] += '\n' + _format(extra['const_declaration'], name=classname)

  if 'const_implementation' in extra:
    extra_code['const_code'] = _format(extra['const_implementation'], name='Const' + classname)
    extra_code['code'] += '\n' + _format(extra['const_implementation'], name=classname)

  extra_code['includes'].update(extra.get('includes', '').split('\n'))

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


def _make_return_dict(getter_decls, getter_impls, setter_decls, setter_impls, const_getter_impls):
  """Put all the generation code into a dict to have some structure. Arguments can
  either be a list of strings or just plain strings"""
  return {
    'decl': {
      'get': ''.join(getter_decls),
      'set': ''.join(setter_decls)
    }, 'impl': {
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
  getname, setname = get_set_names(member.name, get_syntax)

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
    setter_decls.append(_format_pattern(declarations["member_class_refsetter"], fname=setname))
    setter_impls.append(_format_pattern(implementations["member_class_refsetter"], fname=setname))

    setter_decls.append(_format_pattern(declarations["member_class_setter"], fname=setname))
    setter_impls.append(_format_pattern(implementations["member_class_setter"], fname=setname))

    if components is not None:
      for sub_member in components[member.full_type]['Members']:
        getname, setname = get_set_names(sub_member.name, get_syntax)

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

  getname, setname = get_set_names(relation.name, get_syntax)

  getter_decl = _format_pattern(declarations['one_rel_getter'], fname=getname)
  getter_impl = _format_pattern(implementations['one_rel_getter'], fname=getname)
  const_getter_impl = _format_pattern(implementations['const_one_rel_getter'], fname=getname)

  setter_decl = _format_pattern(declarations['one_rel_setter'], fname=setname)
  setter_impl = _format_pattern(implementations['one_rel_setter'], fname=setname)

  return _make_return_dict(getter_decl, getter_impl, setter_decl, setter_impl, const_getter_impl)


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

    self.array_type = kwargs.pop('array_type', None)
    self.array_size = kwargs.pop('array_size', None)
    if self.array_type is not None and self.array_size is not None:
      self.is_array = True
      self.full_type = r'std::array<{}, {}>'.format(self.array_type, self.array_size)
      self.is_builtin_array = self.array_type in BUILTIN_TYPES

    self.is_builtin = self.full_type in BUILTIN_TYPES

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

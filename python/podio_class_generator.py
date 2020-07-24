#!/usr/bin/env python

from __future__ import absolute_import, unicode_literals, print_function
from io import open
try:
  from itertools import zip_longest
except ImportError:
  from itertools import izip_longest as zip_longest

# for python2 compatible FileNotFoundError handling
import errno

from copy import deepcopy

import os
import string
import pickle
import subprocess
from podio_config_reader import PodioConfigReader, ClassDefinitionValidator
from podio_templates import declarations, implementations
from generator_utils import (
    demangle_classname, get_extra_code, generate_get_set_member, generate_get_set_relation,
    constructor_destructor_collection, get_fmt_func
    )

thisdir = os.path.dirname(os.path.abspath(__file__))

REPORT_TEXT = """
  PODIO Data Model
  ================
  Used
    {yamlfile}
  to create
    {nclasses} classes
  in
    {installdir}/
  Read instructions in
  the HOWTO.TXT to run
  your first example!
"""


def get_clang_format():
  """Check if clang format is available and if so get the list of arguments to
  invoke it via subprocess.Popen"""
  try:
    cformat_exe = subprocess.check_output(['which', 'clang-format']).strip()
    return [cformat_exe, "-style=file", "-fallback-style=llvm"]
  except subprocess.CalledProcessError:
    print("ERROR: Cannot find clang-format executable")
    print("       Please make sure it is in the PATH.")
    return []


def ostream_component(comp_members, classname, osname='o', valname='value'):
  """Define the inline ostream operator for component structs"""
  ostream = ['inline std::ostream& operator<<( std::ostream& {o}, const {classname}& {value} ) {{']

  for member in comp_members:
    if member.is_array:
      ostream.append('  for (int i = 0; i < {size}; ++i) {{{{'.format(size=member.array_size))
      ostream.append('    {{o}} << {{value}}.{name}[i] << "|";'.format(name=member.name))
      ostream.append('  }}')
      ostream.append('  {o} << " ";')
    else:
      ostream.append('  {{o}} << {{value}}.{name} << " ";'.format(name=member.name))

  ostream.append('  return {o};')
  ostream.append('}}\n')
  return '\n'.join(ostream).format(o=osname, classname=classname, value=valname)


class ClassGenerator(object):

  def __init__(self, yamlfile, install_dir, package_name,
               verbose=True, dryrun=False):

    self.yamlfile = yamlfile
    self.install_dir = install_dir
    self.package_name = package_name
    self.template_dir = os.path.join(thisdir, "templates")
    self.verbose = verbose
    self.created_classes = []
    self.requested_classes = []
    self.dryrun = dryrun

    self.reader = PodioConfigReader(yamlfile)
    self.reader.read()
    self.get_syntax = self.reader.options["getSyntax"]
    self.include_subfolder = self.reader.options["includeSubfolder"]
    self.expose_pod_members = self.reader.options["exposePODMembers"]
    self.warnings = self.reader.warnings

    self.clang_format = []
    self._template_cache = {}

  def process(self):
    self.process_components(self.reader.components)
    self.process_datatypes(self.reader.datatypes)
    self.create_selection_xml()
    self.print_report()

  def process_components(self, content):
    self.requested_classes += content.keys()
    for name, component in content.items():
      self.create_component(name, component)

  def process_datatypes(self, content):
    # First have to fill the requested classes before the actual generation can
    # start
    for name in content:
      self.requested_classes.append(name)
      self.requested_classes.append("%sData" % name)

    for name, components in content.items():
      datatype = self._process_datatype(components)

      self.create_data(name, components, datatype)
      self.create_class(name, components, datatype)
      self.create_collection(name, components)
      self.create_obj(name, components)
      # self.create_PrintInfo(name, components)

  def print_report(self):
    if not self.verbose:
      return

    text = REPORT_TEXT.format(yamlfile=self.yamlfile,
                              nclasses=len(self.created_classes),
                              installdir=self.install_dir)

    with open(os.path.join(thisdir, "figure.txt"), 'rb') as pkl:
      figure = pickle.load(pkl)

    print()
    for figline, summaryline in zip_longest(figure, text.splitlines(), fillvalue=''):
      print(figline + summaryline)
    print("     'Homage to the Square' - Josef Albers")
    print()

    for warning in self.warnings:
      print(warning)

  def _get_template(self, filename):
    """Get the template from the filename"""
    if filename not in self._template_cache:
      templatefile = os.path.join(self.template_dir, filename)
      with open(templatefile, 'r') as tempfile:
        self._template_cache[filename] = tempfile.read()

    return self._template_cache[filename]

  def create_selection_xml(self):
    content = ""
    for klass in self.created_classes:
      # if not klass.endswith("Collection") or klass.endswith("Data"):
      content += '          <class name="std::vector<%s>" />\n' % klass
      content += """
            <class name="%s">
              <field name="m_registry" transient="true"/>
              <field name="m_container" transient="true"/>
            </class>\n""" % klass

    templatefile = os.path.join(self.template_dir,
                                "selection.xml.template")
    template = self._get_template(templatefile)
    content = string.Template(template).substitute({"classes": content})
    self.write_file("selection.xml", content)

  def _process_datatype(self, definition):
    """Check whether all members are known and prepare include directories"""
    datatype_dict = {
        "description": definition["Description"],
        "author": definition["Author"],
        "includes": set(),
        "members": []
        }
    for member in definition["Members"]:
      datatype_dict["members"].append(str(member))
      klass = member.full_type

      if member.is_array:
        datatype_dict["includes"].add("#include <array>")
        if not member.is_builtin_array:
          datatype_dict["includes"].add(self._build_include(member.array_bare_type))

      for stl_type in ClassDefinitionValidator.allowed_stl_types:
        if klass.startswith('std::' + stl_type):
          datatype_dict["includes"].add('#include <{}>'.format(stl_type))

      if klass in self.requested_classes:
        datatype_dict["includes"].add(self._build_include(member.bare_type))

    # get rid of duplicates:
    return datatype_dict

  def create_data(self, classname, definition, datatype):
    """Create the Data"""
    data = deepcopy(datatype)  # avoid having outside side-effects
    # now handle the vector-members
    vectormembers = definition["VectorMembers"]
    for vectormember in vectormembers:
      name = vectormember.name
      data["members"].append("  unsigned int %s_begin;" % name)
      data["members"].append("  unsigned int %s_end;" % (name))

    # now handle the one-to-many relations
    refvectors = definition["OneToManyRelations"]
    for refvector in refvectors:
      name = refvector.name
      data["members"].append("  unsigned int %s_begin;" % (name))
      data["members"].append("  unsigned int %s_end;" % (name))

    _, rawclassname, namespace_open, namespace_close = demangle_classname(classname)
    substitutions = {"includes": self._join_set(data["includes"]),
                     "members": "\n".join(data["members"]),
                     "name": rawclassname,
                     "description": data["description"],
                     "author": data["author"],
                     "package_name": self.package_name,
                     "namespace_open": namespace_open,
                     "namespace_close": namespace_close
                     }
    self.fill_templates("Data", substitutions)
    self.created_classes.append(classname + "Data")

  @staticmethod
  def _concat_fwd_declarations(fwd_decls):
    """Concatenate all the forward declarations"""
    forward_declaration = []
    for nsp, classes in fwd_decls.items():
      nsp_fwd = []
      if nsp:
        nsp_fwd.append('namespace {} {{'.format(nsp))

      for cls in classes:
        nsp_fwd.append('class {};'.format(cls))

      if nsp:
        nsp_fwd.append('}\n')

      forward_declaration.append('\n'.join(nsp_fwd))

    return '\n'.join(forward_declaration)

  def _process_fwd_declarations(self, relations):
    """Process the forward declarations and includes for a oneToOneRelation"""
    fwd_decls = {"": []}
    includes = set()

    for member in relations:
      if member.full_type in self.requested_classes:
        if member.namespace not in fwd_decls:
          fwd_decls[member.namespace] = []
        fwd_decls[member.namespace].append(member.bare_type)
        fwd_decls[member.namespace].append('Const' + member.bare_type)
        includes.add(self._build_include(member.bare_type))

    forward_declaration = self._concat_fwd_declarations(fwd_decls)

    return includes, forward_declaration

  def _get_includes(self, member):
    """Get the additional includes for a given member"""
    includes = set()
    if member.full_type in self.requested_classes:
      includes.add(self._build_include(member.bare_type))

    elif member.is_array:
      includes.add('#include <array>')
      if not member.is_builtin_array:
        includes.add(self._build_include(member.array_bare_type))

    return includes

  def _ostream_class(self, class_members, multi_relations, single_relations, classname,
                     osname='o', valname='value'):
    """ostream operator overload declaration and implementation for a given class"""
    decl = "std::ostream& operator<<( std::ostream& {o}, const Const{classname}& {value} );\n"

    impl = [decl.replace(';\n', ' {{')]  # double brace for surviving .format
    impl.append('  {o} << " id: " << {value}.id() << \'\\n\';')

    for member in class_members:
      getname = member.getter_name(self.get_syntax)
      _fmt = get_fmt_func(name=member.name, getname=getname, size=member.array_size)
      if member.is_array:
        impl.append(_fmt('  {{o}} << " {name} : ";'))
        impl.append(_fmt('  for (size_t i = 0; i < {size}; ++i) {{{{'))  # have to survive format twice
        impl.append(_fmt('    {{o}} << {{value}}.{getname}()[i] << "|" ;'))
        impl.append('  }}')
        impl.append('  {o} << \'\\n\';')
      else:
        impl.append(_fmt('  {{o}} << " {name} : " << {{value}}.{getname}() << \'\\n\';'))

      if self.expose_pod_members and not member.is_builtin and not member.is_array:
        for sub_member in self.reader.components[member.full_type]['Members']:
          getname = sub_member.getter_name(self.get_syntax)
          _fmt = get_fmt_func(name=member.name, getname=getname)
          impl.append(_fmt('  {{o}} << " {name} : " << {{value}}.{getname}() << \'\\n\';'))

    for relation in single_relations:
      getname = relation.getter_name(self.get_syntax)
      impl.append('  {{o}} << " {name} : " << {{value}}.{getname}().id() << \'\\n\';'.format(
          name=relation.name, getname=getname))

    for relation in multi_relations:
      getname = relation.getter_name(self.get_syntax)
      _fmt = get_fmt_func(name=relation.name, getname=getname)
      impl.append(_fmt('  {{o}} << " {name} : " ;'))
      impl.append(_fmt('  for (unsigned i = 0; i < {{value}}.{name}_size(); ++i) {{{{'))
      # If the reference is of the same type as the class we have to avoid
      # running into a possible infinite loop when printing. Hence, print only
      # the id instead of the whole referenced object
      if relation.bare_type == classname:
        impl.append(_fmt('     {{o}} << {{value}}.{getname}(i).id() << " ";'))
      else:
        impl.append(_fmt('     {{o}} << {{value}}.{getname}(i) << " ";'))
      impl.append('  }}')
      impl.append('  {o} << \'\\n\';')

    impl.append('  return {o};')
    impl.append('}}')  # to survive .format
    return (decl.format(classname=classname, o=osname, value=valname),
            '\n'.join(impl).format(classname=classname, o=osname, value=valname))

  def _constructor_class(self, members, classname):
    """Generate the signature and body of the constructor for the given class (and
    its Const version)"""
    signature, body = [], []
    for member in members:
      signature.append('{type} {name}'.format(type=member.full_type, name=member.name))
      body.append('  m_obj->data.{name} = {name};'.format(name=member.name))

    decl, const_decl, impl, const_impl = '', '', '', ''
    if signature:
      signature = ', '.join(signature)
      decl = '{klass}({signature});'.format(klass=classname, signature=signature)
      const_decl = 'Const{klass}({signature});'.format(klass=classname, signature=signature)

      body = '\n'.join(body)
      substitutions = {
          'name': classname,
          'signature': signature,
          'constructor': body
          }
      impl = self.evaluate_template('Object.constructor.cc.template', substitutions)
      const_impl = self.evaluate_template('ConstObject.constructor.cc.template', substitutions)

    return {
        'decl': decl,
        'const_decl': const_decl,
        'impl': impl,
        'const_impl': const_impl
        }

  def _relation_handling_class(self, relations, classname):
    """Generate the code to do the relation handling of a class"""
    impl, decl, const_impl, const_decl = '', '', '', ''
    members = []

    for relation in relations:
      relationtype = relation.full_type
      if not relation.is_builtin and relation.full_type not in self.reader.components:
        relationtype = relation.namespace + '::Const' + relation.bare_type

      get_relation, set_relation = relation.getter_setter_names(self.get_syntax, is_relation=True)
      substitutions = {
          'relation': relation.name,
          'get_relation': get_relation,
          'add_relation': set_relation,
          'relationtype': relationtype,
          'classname': classname,
          'package_name': self.package_name
          }

      decl += self.evaluate_template('RefVector.h.template', substitutions)
      impl += self.evaluate_template('RefVector.cc.template', substitutions)
      const_decl += self.evaluate_template('ConstRefVector.h.template', substitutions)
      const_impl += self.evaluate_template('ConstRefVector.cc.template', substitutions)

      members.append('std::vector<{type}>* m_{name} ///< transient'
                     .format(type=relation.full_type, name=relation.name))

    return {
        'impl': impl,
        'decl': decl,
        'const_impl': const_impl,
        'const_decl': const_decl,
        'members': '\n'.join(members)
        }

  def create_class(self, classname, definition, datatype):
    """Create all files necessary for a given class"""
    datatype = deepcopy(datatype)  # avoid having outside side-effects
    namespace, rawclassname, namespace_open, namespace_close = demangle_classname(classname)

    datatype["includes"].add(self._build_include(rawclassname + "Data"))

    refvectors = definition["OneToManyRelations"]
    if refvectors:
      datatype["includes"].add("#include <vector>")
      datatype["includes"].add('#include "podio/RelationRange.h"')
    for item in refvectors:
      datatype["includes"].update(self._get_includes(item))

    getter_implementations = ""
    setter_implementations = ""
    getter_declarations = ""
    setter_declarations = ""
    ConstGetter_implementations = ""

    components = self.reader.components if self.expose_pod_members else None
    # handle standard members
    for member in definition["Members"]:
      getters_setters = generate_get_set_member(member, rawclassname, self.get_syntax, components)
      getter_declarations += getters_setters['decl']['get']
      getter_implementations += getters_setters['impl']['get']
      ConstGetter_implementations += getters_setters['impl']['const_get']

      setter_declarations += getters_setters['decl']['set']
      setter_implementations += getters_setters['impl']['set']

    # one-to-one relations
    for member in definition["OneToOneRelations"]:
      getters_setters = generate_get_set_relation(member, rawclassname, self.get_syntax)
      getter_declarations += getters_setters['decl']['get']
      getter_implementations += getters_setters['impl']['get']
      ConstGetter_implementations += getters_setters['impl']['const_get']

      setter_declarations += getters_setters['decl']['set']
      setter_implementations += getters_setters['impl']['set']

    # handle vector members
    vectormembers = definition["VectorMembers"]
    if vectormembers:
      datatype["includes"].add("#include <vector>")
      datatype["includes"].add('#include "podio/RelationRange.h"')
    for item in vectormembers:
      if item.full_type in self.reader.components:
        datatype["includes"].add(self._build_include(item.bare_type))

    constructor = self._constructor_class(definition['Members'], rawclassname)
    constructor_declaration = constructor['decl']
    constructor_implementation = constructor['impl']
    ConstConstructor_declaration = constructor['const_decl']
    ConstConstructor_implementation = constructor['const_impl']

    reference_code = self._relation_handling_class(refvectors + definition['VectorMembers'], rawclassname)
    references_members = reference_code['members']
    references_declarations = reference_code['decl']
    references = reference_code['impl']
    ConstReferences_declarations = reference_code['const_decl']
    ConstReferences = reference_code['const_impl']

    extra_code = get_extra_code(rawclassname, definition)
    extracode_declarations = extra_code['decl']
    extracode = extra_code['impl']
    constextracode_declarations = extra_code['const_decl']
    constextracode = extra_code['const_impl']
    datatype['includes'].update(extra_code['includes'])

    ostream_declaration, ostream_implementation = self._ostream_class(
        definition['Members'], refvectors + definition['VectorMembers'],
        definition['OneToOneRelations'], rawclassname)

    includes_cc, forward_declarations = self._process_fwd_declarations(definition["OneToOneRelations"])

    substitutions = {"includes": self._join_set(datatype["includes"]),
                     "includes_cc": self._join_set(includes_cc),
                     "forward_declarations": forward_declarations,
                     "getters": getter_implementations,
                     "getter_declarations": getter_declarations,
                     "setters": setter_implementations,
                     "setter_declarations": setter_declarations,
                     "constructor_declaration": constructor_declaration,
                     "constructor_implementation": constructor_implementation,
                     "extracode": extracode,
                     "extracode_declarations": extracode_declarations,
                     "name": rawclassname,
                     "description": datatype["description"],
                     "author": datatype["author"],
                     "relations": references,
                     "relation_declarations": references_declarations,
                     "relation_members": references_members,
                     "package_name": self.package_name,
                     "namespace_open": namespace_open,
                     "namespace_close": namespace_close,
                     "ostream_declaration": ostream_declaration,
                     "ostream_implementation": ostream_implementation
                     }
    self.fill_templates("Object", substitutions)
    self.created_classes.append(classname)

    substitutions["constructor_declaration"] = ConstConstructor_declaration
    substitutions["constructor_implementation"] = ConstConstructor_implementation
    substitutions["relation_declarations"] = ConstReferences_declarations
    substitutions["relations"] = ConstReferences
    substitutions["getters"] = ConstGetter_implementations
    substitutions["constextracode"] = constextracode
    substitutions["constextracode_declarations"] = constextracode_declarations
    self.fill_templates("ConstObject", substitutions)
    if "::" in classname:
      self.created_classes.append("%s::Const%s" % (namespace, rawclassname))
    else:
      self.created_classes.append("Const%s" % classname)

  def _ostream_collection(self, classname, members, relations, references, vectormembers,
                          osname='o', valname='v'):
    """Ostream operator implementation and declaration for collections"""
    # create colum based output for data members using scientific format
    decl = "std::ostream& operator<<( std::ostream& {o},const {classname}Collection& {value});\n"
    col_width = 12
    header_str = "{{:{width}}}".format(width=col_width).format('id:')

    impl = [decl.replace(';\n', '{{')]
    impl.append('  const std::ios::fmtflags old_flags = {o}.flags();')
    impl.append('  {o} << "{header_string}" << std::endl;')
    impl.append('  for (size_t i = 0; i < {value}.size(); ++i) {{')
    impl.append('    {o} << std::scientific << std::showpos')
    impl.append('      << std::setw({w}) << {{value}}[i].id() << " "'.format(w=col_width))

    for member in members:
      # TODO: Also handle array members properly
      if member.is_array:
        continue
      # column header
      col_w = col_width + 2
      comp_str = ""  # print component names if they are present
      if member.full_type in self.reader.components:
        comps = [c.name for c in self.reader.components[member.full_type]['Members']]
        comp_str = '[ {}]'.format(', '.join(comps))
        col_w *= len(comps)

      header_str += '{{:{width}}}'.format(width=col_w).format(member.name + ' :' + comp_str)

      getname = member.getter_name(self.get_syntax)
      impl.append('      << std::setw({w}) << {{value}}[i].{name}() << " "'.format(w=col_w, name=getname))

    impl.append('      << std::endl;\n')

    for relation in relations:
      getname = relation.getter_name(self.get_syntax)
      print_name = '{{:{width}}}'.format(width=col_width).format(relation.name)
      impl.append('    {{o}} << "{name} : ";'.format(name=print_name))
      impl.append('    for (unsigned j = 0, N = {{value}}[i].{name}_size(); j < N; ++j) {{{{'
                  .format(name=relation.name))
      impl.append('      {{o}} << {{value}}[i].{gname}(j).id() << " ";'.format(gname=getname))
      impl.append('    }}')
      impl.append('    {o} << std::endl;\n')

    for reference in references:
      getname = reference.getter_name(self.get_syntax)
      print_name = '{{:{width}}}'.format(width=col_width).format(reference.name)
      impl.append('    {{o}} << "{name} : ";'.format(name=print_name))
      impl.append('    {{o}} << {{value}}[i].{gname}().id() << std::endl;'.format(gname=getname))

    for vecmem in vectormembers:
      getname = vecmem.getter_name(self.get_syntax)
      print_name = '{{:{width}}}'.format(width=col_width).format(vecmem.name)
      impl.append('    {{o}} << "{name} : ";'.format(name=print_name))
      impl.append('    for (unsigned j = 0, N = {{value}}[i].{name}_size(); j < N; ++j) {{{{'.format(name=vecmem.name))
      impl.append('      {{o}} << {{value}}[i].{gname}(j) << " ";'.format(gname=getname))
      impl.append('    }}')
      impl.append('    {o} << std::endl;\n')

    impl.append('  }}')  # for loop

    impl.append('  {o}.flags(old_flags);')
    impl.append('  return {o};')
    impl.append('}}')
    return (decl.format(classname=classname, o=osname, value=valname),
            '\n'.join(impl).format(classname=classname, header_string=header_str,
                                   o=osname, value=valname))

  def create_collection(self, classname, definition):
    _, rawclassname, namespace_open, namespace_close = demangle_classname(classname)

    members = definition["Members"]
    prepareforwritinghead = ""
    prepareforwritingbody = ""
    setreferences = ""
    prepareafterread = ""
    includes = set()
    initializers = ""
    relations = ""
    create_relations = ""
    clear_relations = ""
    push_back_relations = ""
    prepareafterread_refmembers = ""
    prepareforwriting_refmembers = ""

    refmembers = definition["OneToOneRelations"]
    refvectors = definition["OneToManyRelations"]

    if refmembers or refvectors:
      clear_relations += "\tfor (auto& pointer : m_refCollections) { pointer->clear(); }\n"

    for irel, relation in enumerate(refvectors):
      includes.add(self._build_include(relation.bare_type + 'Collection'))

      substitutions = {
          'name': relation.name, 'type': relation.bare_type, 'namespace': relation.namespace,
          'class': relation.full_type, 'counter': irel
          }
      _fmt = get_fmt_func(**substitutions)

      relations += _fmt(declarations["relation"])
      relations += _fmt(declarations["relation_collection"])
      initializers += _fmt(implementations["ctor_list_relation"])

      create_relations += _fmt("\tm_rel_{name}_tmp.push_back(obj->m_{name});\n")
      # relation handling in ::clear
      clear_relations += _fmt(implementations["clear_relations_vec"])
      clear_relations += _fmt(implementations["clear_relations"])
      # relation handling in push_back
      push_back_relations += _fmt("\tm_rel_{name}_tmp.push_back(obj->m_{name});\n")
      # relation handling in ::prepareForWrite
      prepareforwritinghead += _fmt("\tint {name}_index =0;\n")
      prepareforwritingbody += self.evaluate_template("CollectionPrepareForWriting.cc.template", substitutions)
      # relation handling in ::settingReferences
      setreferences += self.evaluate_template("CollectionSetReferences.cc.template", substitutions)
      prepareafterread += _fmt("\t\tobj->m_{name} = m_rel_{name};")

    n_relations = len(refvectors)
    for iref, reference in enumerate(refmembers):
      includes.add(self._build_include(reference.bare_type + 'Collection'))

      substitutions = {
          'counter': n_relations + iref, 'i': n_relations + iref, 'rawclass': reference.bare_type,
          'name': reference.name, 'type': reference.bare_type, 'namespace': reference.namespace,
          'class': reference.full_type,
          }
      _fmt = get_fmt_func(**substitutions)

      # constructor call
      initializers += _fmt(implementations["ctor_list_relation"])
      # member
      relations += _fmt(declarations["relation"])
      # relation handling in ::clear
      clear_relations += _fmt(implementations["clear_relations"])
      # relation handling in ::prepareForWrite
      prepareforwriting_refmembers += _fmt(implementations["prep_writing_relations"])
      # relation handling in ::settingReferences
      prepareafterread_refmembers += self.evaluate_template("CollectionSetSingleReference.cc.template", substitutions)

    vectormembers = definition["VectorMembers"]
    vecmembers = ""
    if vectormembers:
      includes.add('#include <numeric>')
    for item in vectormembers:
      substitutions = {'name': item.name, 'type': item.full_type, 'rawclassname': rawclassname}
      _fmt = get_fmt_func(**substitutions)

      vecmembers += _fmt(declarations["vecmembers"])
      create_relations += _fmt("\tm_vecs_{name}.push_back(obj->m_{name});\n")
      clear_relations += _fmt("\tm_vec_{name}->clear();\n")
      clear_relations += _fmt("\tm_vecs_{name}.clear();\n")
      prepareforwritinghead += _fmt("\tint {name}_size = ")
      prepareforwritinghead += "std::accumulate( m_entries.begin(), m_entries.end(), 0, "
      prepareforwritinghead += _fmt("[](int sum, const {rawclassname}Obj*  obj)")
      prepareforwritinghead += _fmt("{{ return sum + obj->m_{name}->size();}} );\n")
      prepareforwritinghead += _fmt("\tm_vec_{name}->reserve( {name}_size );\n")
      prepareforwritinghead += _fmt("\tint {name}_index =0;\n")

      prepareforwritingbody += self.evaluate_template(
          "CollectionPrepareForWritingVecMember.cc.template", substitutions)
      push_back_relations += _fmt("\tm_vecs_{name}.push_back(obj->m_{name});\n")

      prepareafterread += _fmt("\t\tobj->m_{name} = m_vec_{name};\n")

    vectorized_access_decl, vectorized_access_impl = self.prepare_vectorized_access(rawclassname, members)

    ostream_declaration, ostream_implementation = self._ostream_collection(
        rawclassname, definition['Members'], definition['OneToManyRelations'],
        definition['OneToOneRelations'], definition['VectorMembers'])

    constructorbody, destructorbody = constructor_destructor_collection(
        definition['OneToManyRelations'], definition['OneToOneRelations'], definition['VectorMembers'])

    substitutions = {"name": rawclassname,
                     "classname": classname,
                     "constructorbody": constructorbody,
                     "destructorbody": destructorbody,
                     "prepareforwritinghead": prepareforwritinghead,
                     "prepareforwritingbody": prepareforwritingbody,
                     "prepareforwriting_refmembers": prepareforwriting_refmembers,
                     "setreferences": setreferences,
                     "prepareafterread": prepareafterread,
                     "prepareafterread_refmembers": prepareafterread_refmembers,
                     "includes": self._join_set(includes),
                     "initializers": initializers,
                     "relations": relations,
                     "create_relations": create_relations,
                     "clear_relations": clear_relations,
                     "push_back_relations": push_back_relations,
                     "vecmembers": vecmembers,
                     "package_name": self.package_name,
                     "vectorized_access_declaration": vectorized_access_decl,
                     "vectorized_access_implementation": vectorized_access_impl,
                     "namespace_open": namespace_open,
                     "namespace_close": namespace_close,
                     "ostream_declaration": ostream_declaration,
                     "ostream_implementation": ostream_implementation
                     }
    self.fill_templates("Collection", substitutions)
    self.created_classes.append("%sCollection" % classname)

  def create_PrintInfo(self, classname, definition):
    _, rawclassname, _, _ = demangle_classname(classname)

    toFormattingStrings = {"char": "3", "unsigned char": "3",
                           "long": "11", "longlong": "22", "bool": "1", "int": ""}
    formats = {"char": 3, "unsigned char": 3, "long": 11, "longlong": 22, "bool": 1, }

    outputSingleObject = 'o << "' + classname + ':" << std::endl; \n     o << '
    WidthIntegers = ""
    widthOthers = ""
    findMaximum = ""
    setFormats = ""
    formattedOutput = ""
    tableHeader = ""
    for member in definition["Members"]:
      name = member["name"]
      lengthName = len(name)
      klass = member["type"]
      if(klass in toFormattingStrings and not klass == "int"):
        if(formats[klass] > lengthName):
          widthOthers = widthOthers + "  int" + " " + name + "Width = " + toFormattingStrings[klass] + " ; \n"
        else:
          widthOthers = widthOthers + "  int" + " " + name + "Width = " + str(lengthName) + " ; \n"
      elif klass == "int":
        findMaximum += "\n    int " + name + "Max ; \n"
        findMaximum += "    " + name + "Width = 1 ; \n "
        findMaximum += "     for(int i = 0 ; i < value.size(); i++){ \n"
        findMaximum += "         if( value[i].get" + name[:1].upper() + name[1:] + "() > 0 ){ \n"
        findMaximum += "            if(" + name + \
            "Max <  value[i].get" + name[:1].upper() + name[1:] + "()){ \n"
        findMaximum += "               " + name + "Max = value[i].get" + name[:1].upper() + name[1:] + "();"
        findMaximum += "\n            } \n"
        findMaximum += "\n         } \n"
        findMaximum += "         else if( -" + name + \
            "Max / 10 > value[i].get" + name[:1].upper() + name[1:] + "()){ \n"
        findMaximum += "             " + name + \
            "Max = - value[i].get" + name[:1].upper() + name[1:] + "() * 10; "
        findMaximum += "\n         } \n"
        findMaximum += "     } \n"
        setFormats += "\n    while(" + name + "Max != 0){ \n"
        setFormats += "       " + name + "Width++; \n       " + name + "Max = " + name + "Max / 10; \n    } \n"
        setFormats += "   if(" + name + "Width < " + str(lengthName) + \
            "){ " + name + "Width = " + str(lengthName) + ";} \n"
        WidthIntegers = WidthIntegers + "  " + klass + " " + name + "Width = 1 ; \n"
      elif(klass == "double" or klass == "float"):
        if(lengthName > 12):
          widthOthers = widthOthers + "  int" + " " + name + "Width = " + str(lengthName) + " ; \n"
        else:
          widthOthers = widthOthers + "  int" + " " + name + "Width = 12 ; \n"
      elif(klass == "DoubleThree" or klass == "FloatThree"):
        if(lengthName > 38):
          widthOthers = widthOthers + "  int" + " " + name + "Width = " + str(lengthName) + " ; \n"
        else:
          widthOthers += "  int" + " " + name + "Width = 38 ; \n"
      elif(klass == "IntTwo"):
        if(lengthName > 24):
          widthOthers = widthOthers + "  int" + " " + name + "Width = " + str(lengthName) + " ; \n"
        else:
          widthOthers += "  int" + " " + name + "Width = 24 ; \n"
      else:
        widthOthers = widthOthers + "  int" + " " + name + "Width = 38 ; \n"
      if(klass != "StingVec"):
        tableHeader += 'std::setw(' + classname + "PrintInfo::instance()." + \
            name + 'Width) << "' + name + '" << "|" << '
        if(klass == "DoubleThree" or klass == "FloatThree" or klass == "StringVec"):
          formattedOutput += " value[i].get" + name[:1].upper() + name[1:] + '() << " "' + " << "
        else:
          formattedOutput += " std::setw(" + classname + "PrintInfo::instance()." + name + "Width)"
          formattedOutput += " << value[i].get" + name[:1].upper() + name[1:] + '() << " "' + " << "
          outputSingleObject += '"' + klass + '" << " " << "' + name + '" << " "' + \
              " << value.get" + name[:1].upper() + name[1:] + '() << " "' + " << "

    substitutions = {"name": rawclassname,
                     "widthIntegers": WidthIntegers,
                     "widthOthers": widthOthers,
                     "findMaximum": findMaximum,
                     "setFormats": setFormats,
                     "formattedOutput": formattedOutput,
                     "tableHeader": tableHeader,
                     "outputSingleObject": outputSingleObject
                     }

    # TODO: add loading of code from external files

    self.fill_templates("PrintInfo", substitutions)
    self.created_classes.append(classname)

  def create_component(self, classname, component):
    """ Create a component class to be used within the data types
        Components can only contain simple data types and no user
        defined ones
    """
    includes = set()

    for member in component['Members']:
      if member.full_type in self.reader.components:
        includes.add(self._build_include(member.bare_type))

      if member.is_array:
        includes.add('#include <array>')
        if member.array_type in self.reader.components:
          includes.add(self._build_include(member.bare_type))

    members_decl = '\n'.join(('  {}'.format(m) for m in component['Members']))

    if 'ExtraCode' in component:
      extracode = component['ExtraCode']
      extracode_declarations = extracode.get("declaration", "")
      includes.update(set(extracode.get("includes", "").split('\n')))
    else:
      extracode_declarations = ""

    _, rawclassname, namespace_open, namespace_close = demangle_classname(classname)
    substitutions = {"ostreamComponents": ostream_component(component['Members'], classname),
                     "includes": self._join_set(includes),
                     "members": members_decl,
                     "extracode_declarations": extracode_declarations,
                     "name": rawclassname,
                     "package_name": self.package_name,
                     "namespace_open": namespace_open,
                     "namespace_close": namespace_close
                     }
    self.fill_templates("Component", substitutions)
    self.created_classes.append(classname)

  def create_obj(self, classname, definition):
    """ Create an obj class containing all information
        relevant for a given object.
    """
    relations = ""
    includes_cc = set()
    forward_declarations_namespace = {"": []}
    initialize_relations = ""
    set_relations = ""
    deepcopy_relations = ""
    delete_relations = ""
    delete_singlerelations = ""

    # do includes and forward declarations for
    # oneToOneRelations and do proper cleanups
    for item in definition["OneToOneRelations"]:
      if not item.is_builtin:
        relations += '{}* m_{};\n'.format(item.as_qualified_const(), item.name)

        if item.full_type != classname:
          if item.namespace not in forward_declarations_namespace:
            forward_declarations_namespace[item.namespace] = []
          forward_declarations_namespace[item.namespace].append('Const' + item.bare_type)

          set_relations += implementations["set_relations"].format(name=item.name, klass=item.as_qualified_const())
          initialize_relations += ", m_%s(nullptr)\n" % (item.name)
          # for deep copy initialise as nullptr and set in copy ctor body
          # if copied object has non-trivial relation
          deepcopy_relations += ", m_%s(nullptr)" % (item.name)
          delete_singlerelations += "\t\tif (m_{name} != nullptr) delete m_{name};\n".format(name=item.name)

          includes_cc.add(self._build_include("%sConst" % item.bare_type))

    forward_declarations = self._concat_fwd_declarations(forward_declarations_namespace)

    _, rawclassname, namespace_open, namespace_close = demangle_classname(classname)
    includes = set()

    refvectors = definition["OneToManyRelations"]
    vecmembers = definition["VectorMembers"]
    if refvectors or vecmembers:
      includes.add("#include <vector>")

    for item in refvectors + vecmembers:
      qualified_class = item.full_type
      if not item.is_builtin:
        if item.full_type not in self.reader.components:
          qualified_class = item.as_qualified_const()

        if item.full_type == classname:
          includes_cc.add(self._build_include(rawclassname))
        else:
          includes.add(self._build_include(item.bare_type))

      _fmt = get_fmt_func(name=item.name, type=qualified_class)
      relations += _fmt("\tstd::vector<{type}>* m_{name};\n")
      initialize_relations += _fmt(", m_{name}(new std::vector<{type}>())")
      deepcopy_relations += _fmt(", m_{name}(new std::vector<{type}>(*(other.m_{name})))")

      delete_relations += "\t\tdelete m_%s;\n" % (item.name)

    substitutions = {"name": rawclassname,
                     "includes": self._join_set(includes),
                     "includes_cc": self._join_set(includes_cc),
                     "forward_declarations": forward_declarations,
                     "relations": relations,
                     "initialize_relations": initialize_relations,
                     "deepcopy_relations": deepcopy_relations,
                     "delete_relations": delete_relations,
                     "delete_singlerelations": delete_singlerelations,
                     "namespace_open": namespace_open,
                     "namespace_close": namespace_close,
                     "set_deepcopy_relations": set_relations
                     }
    self.fill_templates("Obj", substitutions)
    self.created_classes.append(classname + "Obj")

  def prepare_vectorized_access(self, classname, members):
    implementation = ""
    declaration = ""
    for member in members:
      name = member.name
      klass = member.full_type
      substitutions = {"classname": classname,
                       "member": name,
                       "type": klass
                       }
      if not member.is_builtin:
        substitutions["type"] = "class %s" % klass
      implementation += self.evaluate_template("CollectionReturnArray.cc.template", substitutions)
      declaration += "\ttemplate<size_t arraysize>\n\t" \
          "const std::array<%s,arraysize> %s() const;\n" % (klass, name)
    return declaration, implementation

  def write_file(self, name, content):
    # dispatch headers to header dir, the rest to /src
    # fullname = os.path.join(self.install_dir,self.package_name,name)
    if name.endswith("h"):
      fullname = os.path.join(self.install_dir, self.package_name, name)
    else:
      fullname = os.path.join(self.install_dir, "src", name)
    if not self.dryrun:
      if self.clang_format:
        cfproc = subprocess.Popen(self.clang_format, stdin=subprocess.PIPE, stdout=subprocess.PIPE)
        content = cfproc.communicate(input=content.encode())[0].decode()

      try:
        with open(fullname, 'r') as f:
          existing_content = f.read()
          changed = existing_content != content

      except EnvironmentError as e:
        # If we deprecate python2 support, FileNotFoundError becomes available
        # and this can be using it. For now we keep it compatible with both
        # versions
        if e.errno != errno.ENOENT:
          raise
        changed = True

      if changed:
        with open(fullname, 'w') as f:
          f.write(content)

  def evaluate_template(self, filename, substitutions):
    """ reads in a given template, evaluates it
        and returns result
    """
    template = self._get_template(filename)
    return string.Template(template).substitute(substitutions)

  def fill_templates(self, category, substitutions):
    # add common include subfolder if required
    substitutions['incfolder'] = '' if not self.include_subfolder else self.package_name + '/'
    substitutions['PACKAGE_NAME'] = self.package_name.upper()

    # depending on which category is passed different naming conventions apply
    # for the generated files. Additionally not all categories need source files.
    # Listing the special cases here
    fn_base = {
        'Data': 'Data',
        'Obj': 'Obj',
        'ConstObject': 'Const',
        'PrintInfo': 'PrintInfo',
        'Object': '',
        'Component': ''
        }.get(category, category)

    endings = {
        'Data': ('h',),
        'Component': ('h',),
        'PrintInfo': ('h',)
        }.get(category, ('h', 'cc'))

    for ending in endings:
      templatefile = "%s.%s.template" % (category, ending)
      content = self.evaluate_template(templatefile, substitutions)
      filename = "%s%s.%s" % (substitutions["name"], fn_base, ending)
      self.write_file(filename, content)

  def _build_include(self, classname):
    """Return the include statement."""
    if self.include_subfolder:
      classname = os.path.join(self.package_name, classname)
    return '#include "%s.h"' % classname

  @staticmethod
  def _join_set(aSet):
    """Return newline-joined sorted set."""
    return '\n'.join(sorted([inc.strip() for inc in aSet]))


##########################
if __name__ == "__main__":
  import argparse
  parser = argparse.ArgumentParser(description='Given a description yaml file this script generates '
                                   'the necessary c++ files in the target directory')

  parser.add_argument('description', help='yaml file describing the datamodel')
  parser.add_argument('targetdir', help='Target directory where the generated data classes will be put. '
                      'Header files will be put under <targetdir>/<packagename>/*.h. '
                      'Source files will be put under <targetdir>/src/*.cc')
  parser.add_argument('packagename', help='Name of the package.')

  parser.add_argument('-q', '--quiet', dest='verbose', action='store_false', default=True,
                      help='Don\'t write a report to screen')
  parser.add_argument('-d', '--dryrun', action='store_true', default=False,
                      help='Do not actually write datamodel files')
  parser.add_argument('-c', '--clangformat', action='store_true', default=False,
                      help='Apply clang-format when generating code (with -style=file)')

  args = parser.parse_args()

  install_path = args.targetdir
  project = args.packagename

  for sub_dir in ('src', project):
    directory = os.path.join(install_path, sub_dir)
    if not os.path.exists(directory):
      os.makedirs(directory)

  gen = ClassGenerator(args.description, args.targetdir, args.packagename,
                       verbose=args.verbose, dryrun=args.dryrun)
  if args.clangformat:
    gen.clang_format = get_clang_format()
  gen.process()

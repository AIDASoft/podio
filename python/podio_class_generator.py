#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Podio class generator script"""

import copy
import os
import sys
import subprocess
from copy import deepcopy
from enum import IntEnum

from collections.abc import Mapping
from collections import defaultdict

import jinja2

from podio_schema_evolution import DataModelComparator  # dealing with cyclic imports
from podio_schema_evolution import RenamedMember, root_filter, RootIoRule
from podio_gen.podio_config_reader import PodioConfigReader
from podio_gen.generator_utils import DataType, DefinitionError, DataModelJSONEncoder

THIS_DIR = os.path.dirname(os.path.abspath(__file__))
TEMPLATE_DIR = os.path.join(THIS_DIR, 'templates')

REPORT_TEXT = """
  PODIO Data Model
  ================
  Used {yamlfile} to create {nclasses} classes in {installdir}/
  Read instructions in the README.md to run your first example!
"""
REPORT_TEXT_JULIA = """
  Julia Code generation is an experimental feature.
  Warning: ExtraCode and MutableExtraCode will be ignored during julia code generation.
  PODIO Data Model
  ================
  Used {yamlfile} to create {nfiles} julia files in {installdir}/
  Read instructions in the README.md to run your first example!
"""


def has_clang_format():
  """Check if clang format is available and if so get the list of arguments to
  invoke it via subprocess.Popen"""
  try:
    # This one can raise if -fallback-style is not found
    out = subprocess.check_output(["clang-format", "-style=file", "-fallback-style=llvm", "--help"],
                                  stderr=subprocess.STDOUT)
    # This one doesn't raise
    out = subprocess.check_output('echo | clang-format -style=file ', stderr=subprocess.STDOUT, shell=True)
    if b'.clang-format' in out:
      return False
    return True
  except FileNotFoundError:
    print("ERROR: Cannot find clang-format executable")
    print("       Please make sure it is in the PATH.")
    return False
  except subprocess.CalledProcessError:
    print('ERROR: At least one argument was not recognized by clang-format')
    print('       Most likely the version you are using is old')
    return False


def clang_format_file(content, name):
  if name.endswith(".jl"):
    return content

  clang_format = ["clang-format", "-style=file", "-fallback-style=llvm"]
  with subprocess.Popen(clang_format, stdin=subprocess.PIPE, stdout=subprocess.PIPE) as cfproc:
    return cfproc.communicate(input=content.encode())[0].decode()


def write_file_if_changed(filename, content, force_write=False):
  """Write the file contents only if it has changed or if the file does not exist
  yet. Return whether the file has been written or not"""
  try:
    with open(filename, 'r', encoding='utf-8') as infile:
      existing_content = infile.read()
      changed = existing_content != content
  except FileNotFoundError:
    changed = True

  if changed or force_write:
    with open(filename, 'w', encoding='utf-8') as outfile:
      outfile.write(content)
    return True

  return False


class IncludeFrom(IntEnum):
  """Enum to signify if an include is needed and from where it should come"""
  NOWHERE = 0  # No include needed
  INTERNAL = 1  # include from within the datamodel
  EXTERNAL = 2  # include from an upstream datamodel


class ClassGenerator:
  """The entry point for reading a datamodel definition and generating the
  necessary source code from it."""
  # pylint: disable=too-many-arguments
  def __init__(self, yamlfile, install_dir, package_name, io_handlers, proglang, verbose, dryrun,
               upstream_edm, old_description, evolution_file):
    self.install_dir = install_dir
    self.package_name = package_name
    self.io_handlers = io_handlers
    self.proglang = proglang
    self.verbose = verbose
    self.dryrun = dryrun
    self.yamlfile = yamlfile
    # schema evolution specific code
    self.old_yamlfile = old_description
    self.evolution_file = evolution_file
    self.old_schema_version = None
    self.old_schema_version_int = None
    self.old_datamodel = None
    self.old_datamodels_components = set()
    self.old_datamodels_datatypes = set()
    self.root_schema_dict = {}  # containing the root relevant schema evolution per datatype
    # information to update the selection.xml
    self.root_schema_component_names = set()
    self.root_schema_datatype_names = set()
    self.root_schema_iorules = set()

    try:
      self.datamodel = PodioConfigReader.read(yamlfile, package_name, upstream_edm)
    except DefinitionError as err:
      print(f'Error while generating the datamodel: {err}')
      sys.exit(1)

    self.env = jinja2.Environment(loader=jinja2.FileSystemLoader(TEMPLATE_DIR),
                                  keep_trailing_newline=True,
                                  lstrip_blocks=True,
                                  trim_blocks=True)

    self.get_syntax = self.datamodel.options["getSyntax"]
    self.incfolder = self.datamodel.options['includeSubfolder']
    self.expose_pod_members = self.datamodel.options["exposePODMembers"]
    self.upstream_edm = upstream_edm

    self.formatter_func = None
    self.generated_files = []
    self.any_changes = False

  def _process_parent_module(self, datamodel):
    """Process parent module of julia that contains constructor definitions
    of components and datatypes"""
    self._fill_templates("ParentModule", datamodel)

  def process(self):
    """Run the actual generation"""
    self.process_schema_evolution()

    datamodel = {}
    datamodel['class'] = DataType(self.package_name.capitalize())
    datamodel['upstream_edm'] = self.upstream_edm
    datamodel['upstream_edm_name'] = ''
    if self.upstream_edm:
      datamodel['upstream_edm_name'] = self.upstream_edm.options["includeSubfolder"].split("/")[-2].capitalize()
    datamodel['components'] = []
    datamodel['datatypes'] = []
    for name, component in self.datamodel.components.items():
      datamodel['components'].append(self._process_component(name, component))

    for name, datatype in self.datamodel.datatypes.items():
      datamodel['datatypes'].append(self._process_datatype(name, datatype))

    datamodel['static_arrays_import'] = self._has_static_arrays_import(datamodel['components'] + datamodel['datatypes'])
    datamodel['includes'] = self._sort_components_and_datatypes(datamodel['components'] + datamodel['datatypes'])

    if self.proglang == "julia":
      self._process_parent_module(datamodel)

    if self.proglang == "cpp":
      self._write_edm_def_file()

      if 'ROOT' in self.io_handlers:
        self.prepare_iorules()
        self._create_selection_xml()

      self._write_cmake_lists_file()

    self.print_report()

  def process_schema_evolution(self):
    """Process the schema evolution"""
    # have to make all necessary comparisons
    # which are the ones that changed?
    # have to extend the selection xml file
    if self.old_yamlfile:
      comparator = DataModelComparator(self.yamlfile, self.old_yamlfile,
                                       evolution_file=self.evolution_file)
      comparator.read()
      comparator.compare()
      self.old_schema_version = f"v{comparator.datamodel_old.schema_version}"
      self.old_schema_version_int = comparator.datamodel_old.schema_version
      # some sanity checks
      if len(comparator.errors) > 0:
        print(f"The given datamodels '{self.yamlfile}' and '{self.old_yamlfile}' \
have unresolvable schema evolution incompatibilities:")
        for error in comparator.errors:
          print(error)
        sys.exit(-1)
      if len(comparator.warnings) > 0:
        print(f"The given datamodels '{self.yamlfile}' and '{self.old_yamlfile}' \
have resolvable schema evolution incompatibilities:")
        for warning in comparator.warnings:
          print(warning)
        sys.exit(-1)

      # now go through all the io_handlers and see what we have to do
      if 'ROOT' in self.io_handlers:
        for item in root_filter(comparator.schema_changes):
          # add whatever is relevant to our ROOT schema evolution
          self.root_schema_dict.setdefault(item.klassname, []).append(item)

  def print_report(self):
    """Print a summary report about the generated code"""
    if not self.verbose:
      return
    if self.proglang == "julia":
      nfiles = len(self.datamodel.datatypes) + len(self.datamodel.components) + 1
      text = REPORT_TEXT_JULIA.format(yamlfile=self.yamlfile,
                                      nfiles=nfiles,
                                      installdir=self.install_dir)
    if self.proglang == "cpp":
      nclasses = 5 * len(self.datamodel.datatypes) + len(self.datamodel.components)
      text = REPORT_TEXT.format(yamlfile=self.yamlfile,
                                nclasses=nclasses,
                                installdir=self.install_dir)

    for summaryline in text.splitlines():
      print(summaryline)
    print()

  def _eval_template(self, template, data, old_schema_data=None):
    """Fill the specified template"""
    # merge the info of data and the old schema into a single dict
    if old_schema_data:
      data['OneToOneRelations_old'] = old_schema_data['OneToOneRelations']
      data['OneToManyRelations_old'] = old_schema_data['OneToManyRelations']
      data['VectorMembers_old'] = old_schema_data['VectorMembers']

    return self.env.get_template(template).render(data)

  def _write_file(self, name, content):
    """Write the content to file. Dispatch to the correct directory depending on
    whether it is a header or a .cc file."""
    if name.endswith("h") or name.endswith("jl"):
      fullname = os.path.join(self.install_dir, self.package_name, name)
    else:
      fullname = os.path.join(self.install_dir, "src", name)
    if not self.dryrun:
      self.generated_files.append(fullname)
      if self.formatter_func:
        content = self.formatter_func(content, fullname)

      changed = write_file_if_changed(fullname, content)
      self.any_changes = changed or self.any_changes

  @staticmethod
  def _get_filenames_templates(template_base, name):
    """Get the list of output filenames and corresponding template names"""
    # depending on which category is passed different naming conventions apply
    # for the generated files. Additionally not all categories need source files.
    # Listing the special cases here
    def get_fn_format(tmpl):
      """Get a format string for the filename"""
      prefix = {'MutableObject': 'Mutable'}
      postfix = {'Data': 'Data',
                 'Obj': 'Obj',
                 'SIOBlock': 'SIOBlock',
                 'Collection': 'Collection',
                 'CollectionData': 'CollectionData',
                 'MutableStruct': 'Struct'
                 }

      return f'{prefix.get(tmpl, "")}{{name}}{postfix.get(tmpl, "")}.{{end}}'

    endings = {
        'Data': ('h',),
        'PrintInfo': ('h',),
        'MutableStruct': ('jl',),
        'ParentModule': ('jl',),
        }.get(template_base, ('h', 'cc'))

    fn_templates = []
    for ending in endings:
      template_name = f'{template_base}.{ending}.jinja2'
      filename = get_fn_format(template_base).format(name=name, end=ending)
      fn_templates.append((filename, template_name))

    return fn_templates

  def _fill_templates(self, template_base, data, old_schema_data=None):
    """Fill the template and write the results to file"""
    # Update the passed data with some global things that are the same for all
    # files
    data['package_name'] = self.package_name
    data['use_get_syntax'] = self.get_syntax
    data['incfolder'] = self.incfolder
    for filename, template in self._get_filenames_templates(template_base, data['class'].bare_type):
      self._write_file(filename, self._eval_template(template, data, old_schema_data))

  def _process_component(self, name, component):
    """Process one component"""
    # Make a copy here and add the preprocessing steps to that such that the
    # original definition can be left untouched
    # pylint: disable=too-many-nested-blocks
    component = deepcopy(component)
    includes = set()
    includes.update(*(m.includes for m in component['Members']))
    for member in component['Members']:
      if not (member.is_builtin or member.is_builtin_array):
        includes.add(self._build_include(member))

    includes.update(component.get("ExtraCode", {}).get("includes", "").split('\n'))

    component['includes'] = self._sort_includes(includes)
    component['class'] = DataType(name)
    component['upstream_edm'] = self.upstream_edm
    component['upstream_edm_name'] = ''
    if self.upstream_edm:
      component['upstream_edm_name'] = self.upstream_edm.options["includeSubfolder"].split("/")[-2].capitalize()

    if self.proglang == "cpp":
      self._fill_templates('Component', component)
      # Add potentially older schema for schema evolution
      # based on ROOT capabilities for now
      if name in self.root_schema_dict:
        schema_evolutions = self.root_schema_dict[name]
        component = copy.deepcopy(component)
        for schema_evolution in schema_evolutions:
          if isinstance(schema_evolution, RenamedMember):
            for member in component['Members']:
              if member.name == schema_evolution.member_name_new:
                member.name = schema_evolution.member_name_old
            component['class'] = DataType(name + self.old_schema_version)
          else:
            raise NotImplementedError
        self._fill_templates('Component', component)
        self.root_schema_component_names.add(name + self.old_schema_version)

    if self.proglang == "julia":
      self._fill_templates('MutableStruct', component)
    return component

  @staticmethod
  def _replace_component_in_paths(oldname, newname, paths):
    """Replace component name by another one in existing paths"""
    # strip the namespace
    shortoldname = oldname.split("::")[-1]
    shortnewname = newname.split("::")[-1]
    # and do the replace in place
    for index, thePath in enumerate(paths):
      if shortoldname in thePath:
        newPath = thePath.replace(shortoldname, shortnewname)
        paths[index] = newPath

  def _process_datatype(self, name, definition):
    """Process one datatype"""
    datatype = self._preprocess_datatype(name, definition)

    if self.proglang == "cpp":
      # ROOT schema evolution preparation
      # Compute and prepare the potential schema evolution parts
      schema_evolution_datatype = copy.deepcopy(datatype)
      needs_schema_evolution = False
      for member in schema_evolution_datatype['Members']:
        if member.is_array:
          if member.array_type in self.root_schema_dict:
            needs_schema_evolution = True
            self._replace_component_in_paths(member.array_type, member.array_type + self.old_schema_version,
                                             schema_evolution_datatype['includes_data'])
            member.full_type = member.full_type.replace(member.array_type, member.array_type + self.old_schema_version)
            member.array_type = member.array_type + self.old_schema_version

        else:
          if member.full_type in self.root_schema_dict:
            needs_schema_evolution = True
            # prepare the ROOT I/O rule
            self._replace_component_in_paths(member.full_type, member.full_type + self.old_schema_version,
                                             schema_evolution_datatype['includes_data'])
            member.full_type = member.full_type + self.old_schema_version
            member.bare_type = member.bare_type + self.old_schema_version

      if needs_schema_evolution:
        print(f"  Preparing explicit schema evolution for {name}")
        schema_evolution_datatype['class'].bare_type = schema_evolution_datatype['class'].bare_type + self.old_schema_version  # noqa
        schema_evolution_datatype["old_schema_version"] =self.old_schema_version_int
        self._fill_templates('Data', schema_evolution_datatype)
        self.root_schema_datatype_names.add(name + self.old_schema_version)
        self._fill_templates('Collection', datatype, schema_evolution_datatype)
      else:
        self._fill_templates('Collection', datatype)

      self._fill_templates('Data', datatype)
      self._fill_templates('Object', datatype)
      self._fill_templates('MutableObject', datatype)
      self._fill_templates('Obj', datatype)
      self._fill_templates('Collection', datatype)
      self._fill_templates('CollectionData', datatype)

      if 'SIO' in self.io_handlers:
        self._fill_templates('SIOBlock', datatype)

    if self.proglang == "julia":
      self._fill_templates('MutableStruct', datatype)
    return datatype

  def prepare_iorules(self):
    """Prepare the IORules to be put in the Reflex dictionary"""
    for type_name, schema_changes in self.root_schema_dict.items():
      for schema_change in schema_changes:
        if isinstance(schema_change, RenamedMember):
          # find out the type of the renamed member
          component = self.datamodel.components[type_name]
          for member in component["Members"]:
            if member.name == schema_change.member_name_new:
              member_type = member.full_type

          iorule = RootIoRule()
          iorule.sourceClass = type_name
          iorule.targetClass = type_name
          iorule.version = self.old_schema_version.lstrip("v")
          iorule.source = f'{member_type} {schema_change.member_name_old}'
          iorule.target = schema_change.member_name_new
          iorule.code = f'{iorule.target} = onfile.{schema_change.member_name_old};'
          self.root_schema_iorules.add(iorule)
        else:
          raise NotImplementedError(f"Schema evolution for {schema_change} not yet implemented.")

  @staticmethod
  def _get_julia_params(datatype):
    """Get the relations as parameteric types for MutableStructs"""
    params = set()
    for relation in datatype['OneToManyRelations'] + datatype['OneToOneRelations']:
      if not relation.is_builtin:
        params.add((relation.bare_type, relation.full_type))
    return list(params)

  def _preprocess_for_obj(self, datatype):
    """Do the preprocessing that is necessary for the Obj classes"""
    fwd_declarations = defaultdict(list)
    includes, includes_cc = set(), set()
    for relation in datatype['OneToOneRelations']:
      if relation.full_type != datatype['class'].full_type:
        fwd_declarations[relation.namespace].append(relation.bare_type)
        includes_cc.add(self._build_include(relation))

    if datatype['VectorMembers'] or datatype['OneToManyRelations']:
      includes.add('#include <vector>')

    for relation in datatype['VectorMembers'] + datatype['OneToManyRelations']:
      if not relation.is_builtin:
        if relation.full_type == datatype['class'].full_type:
          includes_cc.add(self._build_include(datatype['class']))
        else:
          includes.add(self._build_include(relation))

    datatype['forward_declarations_obj'] = fwd_declarations
    datatype['includes_obj'] = self._sort_includes(includes)
    datatype['includes_cc_obj'] = self._sort_includes(includes_cc)
    non_trivial_type = datatype['VectorMembers'] or datatype['OneToManyRelations'] or datatype['OneToOneRelations']
    datatype['is_trivial_type'] = not non_trivial_type

  def _preprocess_for_class(self, datatype):
    """Do the preprocessing that is necessary for the classes and Mutable classes"""
    includes = set(datatype['includes_data'])
    fwd_declarations = {}
    includes_cc = set()

    for member in datatype["Members"]:
      if self.expose_pod_members and not member.is_builtin and not member.is_array:
        member.sub_members = self.datamodel.components[member.full_type]['Members']

    for relation in datatype['OneToOneRelations']:
      if self._needs_include(relation.full_type):
        if relation.namespace not in fwd_declarations:
          fwd_declarations[relation.namespace] = []
        fwd_declarations[relation.namespace].append(relation.bare_type)
        fwd_declarations[relation.namespace].append('Mutable' + relation.bare_type)
        includes_cc.add(self._build_include(relation))

    if datatype['VectorMembers'] or datatype['OneToManyRelations']:
      includes.add('#include <vector>')
      includes.add('#include "podio/RelationRange.h"')

    for relation in datatype['OneToManyRelations']:
      if self._needs_include(relation.full_type):
        includes.add(self._build_include(relation))

    for vectormember in datatype['VectorMembers']:
      if vectormember.full_type in self.datamodel.components:
        includes.add(self._build_include(vectormember))

    includes.update(datatype.get('ExtraCode', {}).get('includes', '').split('\n'))
    # TODO: in principle only the mutable classes would need these includes!  # pylint: disable=fixme
    includes.update(datatype.get('MutableExtraCode', {}).get('includes', '').split('\n'))

    # When we have a relation to the same type we have the header that we are
    # just generating in the includes. This would lead to a circular include, so
    # remove "ourselves" again from the necessary includes
    try:
      includes.remove(self._build_include_for_class(datatype['class'].bare_type, IncludeFrom.INTERNAL))
    except KeyError:
      pass

    datatype['includes'] = self._sort_includes(includes)
    datatype['includes_cc'] = self._sort_includes(includes_cc)
    datatype['forward_declarations'] = fwd_declarations

  def _preprocess_for_collection(self, datatype):
    """Do the necessary preprocessing for the collection"""
    includes_cc, includes = set(), set()

    for relation in datatype['OneToManyRelations'] + datatype['OneToOneRelations']:
      if datatype['class'].bare_type != relation.bare_type:
        include_from = self._needs_include(relation.full_type)
        includes_cc.add(self._build_include_for_class(relation.bare_type + 'Collection', include_from))
        includes.add(self._build_include_for_class(relation.bare_type, include_from))

    if datatype['VectorMembers']:
      includes_cc.add('#include <numeric>')

    datatype['includes_coll_cc'] = self._sort_includes(includes_cc)
    datatype['includes_coll_data'] = self._sort_includes(includes)

    # the ostream operator needs a bit of help from the python side in the form
    # of some pre processing but also in the form of formatting, both are done
    # here.
    # TODO: also handle array members properly. These are currently simply  # pylint: disable=fixme
    # ignored
    header_contents = []
    for member in datatype['Members']:
      header = {'name': member.name}
      if member.full_type in self.datamodel.components:
        comps = [c.name for c in self.datamodel.components[member.full_type]['Members']]
        header['components'] = comps
      header_contents.append(header)

    def ostream_collection_header(member_header, col_width=12):
      """Custom filter for the jinja2 templates to handle the ostream header that is
      printed for the collections. Need this custom filter because it is easier
      to implement the content dependent width in python than in jinja2.
      """
      if not isinstance(member_header, Mapping):
        # Assume that we have a string and format it according to the width
        return f'{{:>{col_width}}}'.format(member_header)

      components = member_header.get('components', None)
      name = member_header['name']
      if components is None:
        return f'{{:>{col_width}}}'.format(name)

      n_comps = len(components)
      comp_str = f'[ {", ".join(components)}]'
      return f'{{:>{col_width * n_comps}}}'.format(name + ' ' + comp_str)

    datatype['ostream_collection_settings'] = {
        'header_contents': header_contents
        }
    # Register the custom filter for it to become available in the templates
    self.env.filters['ostream_collection_header'] = ostream_collection_header

  def _preprocess_datatype(self, name, definition):
    """Preprocess the datatype (building includes, etc.)"""
    # Make a copy here and add the preprocessing steps to that such that the
    # original definition can be left untouched
    data = deepcopy(definition)
    data['class'] = DataType(name)
    data['includes_data'] = self._get_member_includes(definition["Members"])
    data['params_jl'] = sorted(self._get_julia_params(data), key=lambda x: x[0])
    data['upstream_edm'] = self.upstream_edm
    data['upstream_edm_name'] = ''
    if self.upstream_edm:
      data['upstream_edm_name'] = self.upstream_edm.options["includeSubfolder"].split("/")[-2].capitalize()
    self._preprocess_for_class(data)
    self._preprocess_for_obj(data)
    self._preprocess_for_collection(data)
    return data

  def _write_edm_def_file(self):
    """Write the edm definition to a compile time string"""
    model_encoder = DataModelJSONEncoder()
    data = {
        'package_name': self.package_name,
        'edm_definition': model_encoder.encode(self.datamodel),
        'incfolder': self.incfolder,
        'schema_version': self.datamodel.schema_version,
        'datatypes': self.datamodel.datatypes,
        }

    def quoted_sv(string):
      return f"\"{string}\"sv"

    self.env.filters["quoted_sv"] = quoted_sv

    self._write_file('DatamodelDefinition.h',
                     self._eval_template('DatamodelDefinition.h.jinja2', data))

  def _get_member_includes(self, members):
    """Process all members and gather the necessary includes"""
    includes = set()
    includes.update(*(m.includes for m in members))
    for member in members:
      if member.is_array and not member.is_builtin_array:
        include_from = IncludeFrom.INTERNAL
        if self.upstream_edm and member.array_type in self.upstream_edm.components:
          include_from = IncludeFrom.EXTERNAL
        includes.add(self._build_include_for_class(member.array_bare_type, include_from))

      includes.add(self._build_include(member))

    return self._sort_includes(includes)

  def _write_cmake_lists_file(self):
    """Write the names of all generated header and src files into cmake lists"""
    header_files = (f for f in self.generated_files if f.endswith('.h'))
    src_files = (f for f in self.generated_files if f.endswith('.cc'))
    xml_files = (f for f in self.generated_files if f.endswith('.xml'))

    def _write_list(name, target_folder, files, comment):
      """Write all files into a cmake variable using the target_folder as path to the
      file"""
      list_cont = []

      list_cont.append(f'# {comment}')
      list_cont.append(f'SET({name}')
      for full_file in files:
        fname = os.path.basename(full_file)
        list_cont.append(f'  {os.path.join(target_folder, fname)}')

      list_cont.append(')')

      return '\n'.join(list_cont)

    full_contents = ['#-- AUTOMATICALLY GENERATED FILE - DO NOT EDIT -- \n']
    full_contents.append(_write_list('headers', r'${ARG_OUTPUT_FOLDER}/${datamodel}',
                                     header_files, 'Generated header files'))

    full_contents.append(_write_list('sources', r'${ARG_OUTPUT_FOLDER}/src',
                                     src_files, 'Generated source files'))

    full_contents.append(_write_list('selection_xml', r'${ARG_OUTPUT_FOLDER}/src',
                                     xml_files, 'Generated xml files'))

    write_file_if_changed(f'{self.install_dir}/podio_generated_files.cmake',
                          '\n'.join(full_contents),
                          self.any_changes)

  def _needs_include(self, classname) -> IncludeFrom:
    """Check whether the member needs an include from within the datamodel"""
    if classname in self.datamodel.components or classname in self.datamodel.datatypes:
      return IncludeFrom.INTERNAL

    if self.upstream_edm:
      if classname in self.upstream_edm.components or classname in self.upstream_edm.datatypes:
        return IncludeFrom.EXTERNAL

    return IncludeFrom.NOWHERE

  def _create_selection_xml(self):
    """Create the selection xml that is necessary for ROOT I/O"""
    data = {'version': self.datamodel.schema_version,
            'components': [DataType(c) for c in self.datamodel.components],
            'datatypes': [DataType(d) for d in self.datamodel.datatypes],
            'old_schema_components': [DataType(d) for d in
                                      self.root_schema_datatype_names | self.root_schema_component_names], # noqa
            'iorules': self.root_schema_iorules}

    self._write_file('selection.xml', self._eval_template('selection.xml.jinja2', data))

  def _build_include(self, member):
    """Return the include statment for the passed member."""
    return self._build_include_for_class(member.bare_type, self._needs_include(member.full_type))

  def _build_include_for_class(self, classname, include_from: IncludeFrom) -> str:
    """Return the include statement for the passed classname"""
    if include_from == IncludeFrom.INTERNAL:
      return f'#include "{self.datamodel.options["includeSubfolder"]}{classname}.h"'
    if include_from == IncludeFrom.EXTERNAL:
      return f'#include "{self.upstream_edm.options["includeSubfolder"]}{classname}.h"'

    # The empty string is filtered by _sort_includes (plus it doesn't hurt in
    # the generated code)
    return ''

  @staticmethod
  def _sort_components_and_datatypes(data):
    """Sorts a list of components and datatypes based on dependencies, ensuring that components and datatypes
    with no dependencies or dependencies on built-in types come first. The function performs
    topological sorting using Kahn's algorithm."""
    # Create a dictionary to store dependencies
    dependencies = {}
    bare_types_mapping = {}

    for component_data in data:
      full_type = component_data['class'].full_type
      bare_type = component_data['class'].bare_type
      bare_types_mapping[full_type] = bare_type
      dependencies[full_type] = set()

      # Check dependencies in 'Members'
      if 'Members' in component_data:
        for member_data in component_data['Members']:
          member_full_type = member_data.full_type
          if not member_data.is_builtin and not member_data.is_builtin_array:
            dependencies[full_type].add(member_full_type)

      # Check dependencies in 'VectorMembers'
      if 'VectorMembers' in component_data:
        for vector_member_data in component_data['VectorMembers']:
          vector_member_full_type = vector_member_data.full_type
          if not vector_member_data.is_builtin and not vector_member_data.is_builtin_array:
            dependencies[full_type].add(vector_member_full_type)

    # Perform topological sorting using Kahn's algorithm
    sorted_components = []
    while dependencies:
      ready = {component for component, deps in dependencies.items() if not deps}
      if not ready:
        sorted_components.extend(bare_types_mapping[component] for component in dependencies)
        break

      for component in ready:
        del dependencies[component]
        sorted_components.append(bare_types_mapping[component])

      for deps in dependencies.values():
        deps -= ready

    # Return the Sorted Components (bare_types)
    return sorted_components

  @staticmethod
  def _has_static_arrays_import(data):
    """Checks if any member within a list of components and datatypes contains the import statement
    'using StaticArrays' in its jl_imports. Returns True if found in any member, otherwise False."""
    for component_data in data:
      members_data = component_data.get('Members', [])
      for member_data in members_data:
        jl_imports = member_data.jl_imports
        if 'using StaticArrays' in jl_imports:
          return True
    return False

  def _sort_includes(self, includes):
    """Sort the includes in order to try to have the std includes at the bottom"""
    package_includes = sorted(i for i in includes if self.package_name in i)
    podio_includes = sorted(i for i in includes if 'podio' in i)
    stl_includes = sorted(i for i in includes if '<' in i and '>' in i)

    upstream_includes = []
    if self.upstream_edm:
      upstream_includes = sorted(i for i in includes if self.upstream_edm.options['includeSubfolder'] in i)

    # Are ther includes that fulfill more than one of the above conditions? Are
    # there includes that fulfill none?

    return package_includes + upstream_includes + podio_includes + stl_includes


def read_upstream_edm(name_path):
  """Read an upstream EDM yaml definition file to make the types that are defined
  in that available to the current EDM"""
  if name_path is None:
    return None

  try:
    name, path = name_path.split(':')
  except ValueError as err:
    raise argparse.ArgumentTypeError('upstream-edm argument needs to be the upstream package '
                                     'name and the upstream edm yaml file separated by a colon') from err

  if not os.path.isfile(path):
    raise argparse.ArgumentTypeError(f'{path} needs to be an EDM yaml file')

  try:
    return PodioConfigReader.read(path, name)
  except DefinitionError as err:
    raise argparse.ArgumentTypeError(f'{path} does not contain a valid datamodel definition') from err


if __name__ == "__main__":
  import argparse
  # pylint: disable=invalid-name # before 2.5.0 pylint is too strict with the naming here
  parser = argparse.ArgumentParser(description='Given a description yaml file this script generates '
                                   'the necessary c++ or julia files in the target directory')

  parser.add_argument('description', help='yaml file describing the datamodel')
  parser.add_argument('targetdir', help='Target directory where the generated data classes will be put. '
                      'Header files will be put under <targetdir>/<packagename>/*.h. '
                      'Source files will be put under <targetdir>/src/*.cc. '
                      'Julia files will be put under <targetdir>/<packagename>/*.jl.')
  parser.add_argument('packagename', help='Name of the package.')
  parser.add_argument('iohandlers', choices=['ROOT', 'SIO'], nargs='+',
                      help='The IO backend specific code that should be generated')
  parser.add_argument('-l', '--lang', choices=['cpp', 'julia'], default='cpp',
                      help='Specify the programming language (default: cpp)')
  parser.add_argument('-q', '--quiet', dest='verbose', action='store_false', default=True,
                      help='Don\'t write a report to screen')
  parser.add_argument('-d', '--dryrun', action='store_true', default=False,
                      help='Do not actually write datamodel files')
  parser.add_argument('-c', '--clangformat', action='store_true', default=False,
                      help='Apply clang-format when generating code (with -style=file)')
  parser.add_argument('--upstream-edm',
                      help='Make datatypes of this upstream EDM available to the current'
                      ' EDM. Format is \'<upstream-name>:<upstream.yaml>\'. '
                      'Note that only the code for the current EDM will be generated',
                      default=None, type=read_upstream_edm)
  parser.add_argument('--old-description',
                      help='Provide schema evolution relative to the old yaml file.',
                      default=None, action='store')
  parser.add_argument('-e', '--evolution_file', help='yaml file clarifying schema evolutions',
                      default=None, action='store')

  args = parser.parse_args()

  install_path = args.targetdir
  project = args.packagename

  for sub_dir in ('src', project):
    directory = os.path.join(install_path, sub_dir)
    if not os.path.exists(directory):
      os.makedirs(directory)

  gen = ClassGenerator(args.description, args.targetdir, args.packagename, args.iohandlers, proglang=args.lang,
                       verbose=args.verbose, dryrun=args.dryrun, upstream_edm=args.upstream_edm,
                       old_description=args.old_description, evolution_file=args.evolution_file)
  if args.clangformat and has_clang_format():
    gen.formatter_func = clang_format_file
  gen.process()

  # pylint: enable=invalid-name

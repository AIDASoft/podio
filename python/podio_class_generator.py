#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""Podio class generator script"""

import os
import sys
import subprocess
import pickle
from copy import deepcopy

from collections.abc import Mapping

from itertools import zip_longest

import jinja2

from podio_config_reader import PodioConfigReader, ClassDefinitionValidator
from generator_utils import DataType, DefinitionError

THIS_DIR = os.path.dirname(os.path.abspath(__file__))
TEMPLATE_DIR = os.path.join(THIS_DIR, 'templates')

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
  the README.md to run
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


class ClassGenerator:
  """The entry point for reading a datamodel definition and generating the
  necessary source code from it."""
  def __init__(self, yamlfile, install_dir, package_name, io_handlers, verbose, dryrun):
    self.install_dir = install_dir
    self.package_name = package_name
    self.io_handlers = io_handlers
    self.verbose = verbose
    self.dryrun = dryrun
    self.yamlfile = yamlfile

    self.reader = PodioConfigReader(yamlfile)
    try:
      self.reader.read()
    except DefinitionError as err:
      print(f'Error while generating the datamodel: {err}')
      sys.exit(1)

    self.include_subfolder = self.reader.options["includeSubfolder"]

    self.env = jinja2.Environment(loader=jinja2.FileSystemLoader(TEMPLATE_DIR),
                                  keep_trailing_newline=True,
                                  lstrip_blocks=True,
                                  trim_blocks=True)

    self.get_syntax = self.reader.options["getSyntax"]
    self.incfolder = self.package_name + "/" if self.reader.options["includeSubfolder"] else ""
    self.expose_pod_members = self.reader.options["exposePODMembers"]

    self.clang_format = []
    self.generated_files = []
    self.any_changes = False

  def process(self):
    """Run the actual generation"""
    for name, component in self.reader.components.items():
      self._process_component(name, component)

    for name, datatype in self.reader.datatypes.items():
      self._process_datatype(name, datatype)

    if 'ROOT' in self.io_handlers:
      self._create_selection_xml()
    self.print_report()

    self._write_cmake_lists_file()

  def print_report(self):
    """Print a summary report about the generated code"""
    if not self.verbose:
      return

    with open(os.path.join(THIS_DIR, "figure.txt"), 'rb') as pkl:
      figure = pickle.load(pkl)

    nclasses = 5 * len(self.reader.datatypes) + len(self.reader.components)
    text = REPORT_TEXT.format(yamlfile=self.yamlfile,
                              nclasses=nclasses,
                              installdir=self.install_dir)

    print()
    for figline, summaryline in zip_longest(figure, text.splitlines(), fillvalue=''):
      print(figline + summaryline)
    print("     'Homage to the Square' - Josef Albers")
    print()

    for warning in self.reader.warnings:
      print(warning)
    print()

  def _eval_template(self, template, data):
    """Fill the specified template"""
    return self.env.get_template(template).render(data)

  def _write_file(self, name, content):
    """Write the content to file. Dispatch to the correct directory depending on
    whether it is a header or a .cc file."""
    if name.endswith("h"):
      fullname = os.path.join(self.install_dir, self.package_name, name)
    else:
      fullname = os.path.join(self.install_dir, "src", name)
    if not self.dryrun:
      self.generated_files.append(fullname)
      if self.clang_format:
        with subprocess.Popen(self.clang_format, stdin=subprocess.PIPE, stdout=subprocess.PIPE) as cfproc:
          content = cfproc.communicate(input=content.encode())[0].decode()

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
                 'CollectionData': 'CollectionData'}

      return f'{prefix.get(tmpl, "")}{{name}}{postfix.get(tmpl, "")}.{{end}}'

    endings = {
        'Data': ('h',),
        'Component': ('h',),
        'PrintInfo': ('h',),
        }.get(template_base, ('h', 'cc'))

    fn_templates = []
    for ending in endings:
      template_name = f'{template_base}.{ending}.jinja2'
      filename = get_fn_format(template_base).format(name=name, end=ending)
      fn_templates.append((filename, template_name))

    return fn_templates

  def _fill_templates(self, template_base, data):
    """Fill the template and write the results to file"""
    # Update the passed data with some global things that are the same for all
    # files
    data['package_name'] = self.package_name
    data['use_get_syntax'] = self.get_syntax
    data['incfolder'] = self.incfolder

    for filename, template in self._get_filenames_templates(template_base, data['class'].bare_type):
      self._write_file(filename, self._eval_template(template, data))

  def _process_component(self, name, component):
    """Process one component"""
    includes = set()
    includes.update(*(m.includes for m in component['Members']))

    for member in component['Members']:
      if member.full_type in self.reader.components or member.array_type in self.reader.components:
        includes.add(self._build_include(member.bare_type))

    includes.update(component.get("ExtraCode", {}).get("includes", "").split('\n'))

    component['includes'] = self._sort_includes(includes)
    component['class'] = DataType(name)

    self._fill_templates('Component', component)

  def _process_datatype(self, name, definition):
    """Process one datatype"""
    datatype = self._preprocess_datatype(name, definition)
    self._fill_templates('Data', datatype)
    self._fill_templates('Object', datatype)
    self._fill_templates('MutableObject', datatype)
    self._fill_templates('Obj', datatype)
    self._fill_templates('Collection', datatype)
    self._fill_templates('CollectionData', datatype)

    if 'SIO' in self.io_handlers:
      self._fill_templates('SIOBlock', datatype)

  def _preprocess_for_obj(self, datatype):
    """Do the preprocessing that is necessary for the Obj classes"""
    fwd_declarations = {}
    includes, includes_cc, includes_jl = set(), set(), set()

    for relation in datatype['OneToOneRelations']:
      if relation.full_type != datatype['class'].full_type:
        if relation.namespace not in fwd_declarations:
          fwd_declarations[relation.namespace] = []
        fwd_declarations[relation.namespace].append(relation.bare_type)
        includes_cc.add(self._build_include(relation.bare_type))
        includes_jl.add(self._build_include(relation.bare_type, julia = True))

    if datatype['VectorMembers'] or datatype['OneToManyRelations']:
      includes.add('#include <vector>')

    for relation in datatype['VectorMembers'] + datatype['OneToManyRelations']:
      if not relation.is_builtin:
        if relation.full_type == datatype['class'].full_type:
          includes_cc.add(self._build_include(datatype['class'].bare_type))
          includes_jl.add(self._build_include(datatype['class'].bare_type, julia = True))
        else:
          includes.add(self._build_include(relation.bare_type))
          includes_jl.add(self._build_include(relation.bare_type, julia = True))

    datatype['forward_declarations_obj'] = fwd_declarations
    datatype['includes_obj'] = self._sort_includes(includes)
    datatype['includes_cc_obj'] = self._sort_includes(includes_cc)
    datatype['includes_jl'] += self._sort_includes(includes_jl)
    trivial_types = datatype['VectorMembers'] or datatype['OneToManyRelations'] or datatype['OneToOneRelations']
    datatype['is_trivial_type'] = trivial_types

  def _preprocess_for_class(self, datatype):
    """Do the preprocessing that is necessary for the classes and Mutable classes"""
    includes = set(datatype['includes_data'])
    fwd_declarations = {}
    includes_cc = set()
    includes_jl = set()

    for member in datatype["Members"]:
      if self.expose_pod_members and not member.is_builtin and not member.is_array:
        member.sub_members = self.reader.components[member.full_type]['Members']

    for relation in datatype['OneToOneRelations']:
      if self._needs_include(relation):
        if relation.namespace not in fwd_declarations:
          fwd_declarations[relation.namespace] = []
        fwd_declarations[relation.namespace].append(relation.bare_type)
        fwd_declarations[relation.namespace].append('Mutable' + relation.bare_type)
        includes_cc.add(self._build_include(relation.bare_type))
        includes_jl.add(self._build_include(relation.bare_type, julia = True))

    if datatype['VectorMembers'] or datatype['OneToManyRelations']:
      includes.add('#include <vector>')
      includes.add('#include "podio/RelationRange.h"')

    for relation in datatype['OneToManyRelations']:
      if self._needs_include(relation):
        includes.add(self._build_include(relation.bare_type))
        includes_jl.add(self._build_include(relation.bare_type, julia = True))

    for vectormember in datatype['VectorMembers']:
      if vectormember.full_type in self.reader.components:
        includes.add(self._build_include(vectormember.bare_type))
        includes_jl.add(self._build_include(vectormember.bare_type, julia = True))

    includes.update(datatype.get('ExtraCode', {}).get('includes', '').split('\n'))
    # TODO: in principle only the mutable classes would need these includes!  # pylint: disable=fixme
    includes.update(datatype.get('MutableExtraCode', {}).get('includes', '').split('\n'))

    # When we have a relation to the same type we have the header that we are
    # just generating in the includes. This would lead to a circular include, so
    # remove "ourselves" again from the necessary includes
    try:
      includes.remove(self._build_include(datatype['class'].bare_type))
    except KeyError:
      pass

    datatype['includes'] = self._sort_includes(includes)
    datatype['includes_cc'] = self._sort_includes(includes_cc)
    datatype['forward_declarations'] = fwd_declarations
    datatype['includes_jl'] = self._sort_includes(includes_jl)

  def _preprocess_for_collection(self, datatype):
    """Do the necessary preprocessing for the collection"""
    includes_cc, includes, includes_jl = set(), set(), set()

    for relation in datatype['OneToManyRelations'] + datatype['OneToOneRelations']:
      if datatype['class'].bare_type != relation.bare_type:
        includes_cc.add(self._build_include(relation.bare_type + 'Collection'))
        includes_jl.add(self._build_include(relation.bare_type + 'Collection', julia = True))
        includes.add(self._build_include(relation.bare_type))
        includes_jl.add(self._build_include(relation.bare_type, julia = True))

    if datatype['VectorMembers']:
      includes_cc.add('#include <numeric>')

    datatype['includes_coll_cc'] = self._sort_includes(includes_cc)
    datatype['includes_coll_data'] = self._sort_includes(includes)
    datatype['includes_jl'] += (self._sort_includes(includes_jl))

    # the ostream operator needs a bit of help from the python side in the form
    # of some pre processing but also in the form of formatting, both are done
    # here.
    # TODO: also handle array members properly. These are currently simply  # pylint: disable=fixme
    # ignored
    header_contents = []
    for member in datatype['Members']:
      header = {'name': member.name}
      if member.full_type in self.reader.components:
        comps = [c.name for c in self.reader.components[member.full_type]['Members']]
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
    data['includes_data_jl'] = self._get_member_includes(definition["Members"], julia=True)
    data['is_pod'] = self._is_pod_type(definition["Members"])
    self._preprocess_for_class(data)
    self._preprocess_for_obj(data)
    self._preprocess_for_collection(data)

    return data

  def _get_member_includes(self, members, julia = False):
    """Process all members and gather the necessary includes"""
    includes, includes_jl = set(),set()
    includes.update(*(m.includes for m in members))
    includes_jl.update(*(m.jl_imports for m in members))
    for member in members:
      if member.is_array and not member.is_builtin_array:
        includes.add(self._build_include(member.array_bare_type))
        includes_jl.add(self._build_include(member.array_bare_type, julia = True))

      for stl_type in ClassDefinitionValidator.allowed_stl_types:
        if member.full_type == 'std::' + stl_type:
          includes.add(f"#include <{stl_type}>")

      if self._needs_include(member):
        includes.add(self._build_include(member.bare_type))
        includes_jl.add(self._build_include(member.bare_type, julia = True))

    if not julia:
      return self._sort_includes(includes)
    else:
      return includes_jl

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
      list_cont.append(f'SET_PROPERTY(SOURCE ${{{name}}} PROPERTY GENERATED TRUE)\n')

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

  @staticmethod
  def _is_pod_type(members):
    """Check if the members of the class define a POD type"""
    for stl_type in ClassDefinitionValidator.allowed_stl_types:
      full_stl_type = 'std::' + stl_type
      if any(m.full_type.startswith(full_stl_type) for m in members):
        return False

    return True

  def _needs_include(self, member):
    """Check whether the member needs an include from within the datamodel"""
    return member.full_type in self.reader.components or member.full_type in self.reader.datatypes

  def _create_selection_xml(self):
    """Create the selection xml that is necessary for ROOT I/O"""
    data = {'components': [DataType(c) for c in self.reader.components],
            'datatypes': [DataType(d) for d in self.reader.datatypes]}
    self._write_file('selection.xml', self._eval_template('selection.xml.jinja2', data))

  def _build_include(self, classname, julia = False):
    """Return the include statement."""
    if self.include_subfolder:
      classname = os.path.join(self.package_name, classname)
    if not julia:
      return f'#include "{classname}.h"'
    else:
      return f'include("{classname}.jl")'

  def _sort_includes(self, includes):
    """Sort the includes in order to try to have the std includes at the bottom"""
    package_includes = sorted(i for i in includes if self.package_name in i)
    podio_includes = sorted(i for i in includes if 'podio' in i)
    stl_includes = sorted(i for i in includes if '<' in i and '>' in i)
    # Are ther includes that fulfill more than one of the above conditions? Are
    # there includes that fulfill none?

    return package_includes + podio_includes + stl_includes


def verify_io_handlers(handler):
  """Briefly verify that all arguments passed as handlers are indeed valid"""
  valid_handlers = (
      'ROOT',
      'SIO',
      )
  if handler in valid_handlers:
    return handler
  raise argparse.ArgumentTypeError(f'{handler} is not a valid io handler')


if __name__ == "__main__":
  import argparse
  # pylint: disable=invalid-name # before 2.5.0 pylint is too strict with the naming here
  parser = argparse.ArgumentParser(description='Given a description yaml file this script generates '
                                   'the necessary c++ files in the target directory')

  parser.add_argument('description', help='yaml file describing the datamodel')
  parser.add_argument('targetdir', help='Target directory where the generated data classes will be put. '
                      'Header files will be put under <targetdir>/<packagename>/*.h. '
                      'Source files will be put under <targetdir>/src/*.cc')
  parser.add_argument('packagename', help='Name of the package.')
  parser.add_argument('iohandlers', help='The IO backend specific code that should be generated',
                      type=verify_io_handlers, nargs='+')
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

  gen = ClassGenerator(args.description, args.targetdir, args.packagename, args.iohandlers,
                       verbose=args.verbose, dryrun=args.dryrun)
  if args.clangformat:
    gen.clang_format = get_clang_format()
  gen.process()

  # pylint: enable=invalid-name

#!/usr/bin/env python
# -*- coding: utf-8 -*-
from __future__ import unicode_literals, absolute_import, print_function

import os
import errno
import subprocess
from io import open
import pickle
from copy import deepcopy

# collections.abc not available for python2, so again some special care here
try:
  from collections.abc import Mapping
except ImportError:
  from collections import Mapping

try:
  from itertools import zip_longest
except ImportError:
  from itertools import izip_longest as zip_longest

import jinja2

from podio_config_reader import PodioConfigReader, ClassDefinitionValidator
from generator_utils import DataType

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


class ClassGenerator(object):
  def __init__(self, yamlfile, install_dir, package_name, verbose, dryrun):
    self.install_dir = install_dir
    self.package_name = package_name
    self.verbose = verbose
    self.dryrun = dryrun
    self.yamlfile = yamlfile

    self.reader = PodioConfigReader(yamlfile)
    self.reader.read()
    self.include_subfolder = self.reader.options["includeSubfolder"]

    self.env = jinja2.Environment(loader=jinja2.FileSystemLoader(TEMPLATE_DIR),
                                  keep_trailing_newline=True,
                                  lstrip_blocks=True,
                                  trim_blocks=True)

    self.get_syntax = self.reader.options["getSyntax"]
    self.incfolder = self.package_name + "/" if self.reader.options["includeSubfolder"] else ""
    self.expose_pod_members = self.reader.options["exposePODMembers"]

    self.clang_format = []

  def process(self):
    for name, component in self.reader.components.items():
      self._process_component(name, component)

    for name, datatype in self.reader.datatypes.items():
      self._process_datatype(name, datatype)

    self._create_selection_xml()
    self.print_report()

  def print_report(self):
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

  @staticmethod
  def _get_filenames_templates(template_base, name):
    """Get the list of output filenames and corresponding template names"""
    # depending on which category is passed different naming conventions apply
    # for the generated files. Additionally not all categories need source files.
    # Listing the special cases here
    fn_base = {
        'Data': 'Data',
        'Obj': 'Obj',
        'ConstObject': 'Const',
        'PrintInfo': 'PrintInfo',
        'Object': '',
        'Component': '',
        'SIOBlock': 'SIOBlock'
        }.get(template_base, template_base)

    endings = {
        'Data': ('h',),
        'Component': ('h',),
        'PrintInfo': ('h',)
        }.get(template_base, ('h', 'cc'))

    fn_templates = []
    for ending in endings:
      template_name = '{fn}.{end}.jinja2'.format(fn=template_base, end=ending)
      filename = '{name}{fn}.{end}'.format(fn=fn_base, name=name, end=ending)
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
    if any(m.is_array for m in component['Members']):
      includes.add('#include <array>')

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
    self._fill_templates('ConstObject', datatype)
    self._fill_templates('Obj', datatype)
    self._fill_templates('Collection', datatype)

    if self.reader.options["createSIOHandlers"]:
      self._fill_templates('SIOBlock', datatype)

  def _preprocess_for_obj(self, datatype):
    """Do the preprocessing that is necessary for the Obj classes"""
    fwd_declarations = {}
    includes, includes_cc = set(), set()

    for relation in datatype['OneToOneRelations']:
      if not relation.is_builtin:
        relation.relation_type = relation.as_qualified_const()

      if relation.full_type != datatype['class'].full_type:
        if relation.namespace not in fwd_declarations:
          fwd_declarations[relation.namespace] = []
        fwd_declarations[relation.namespace].append('Const' + relation.bare_type)
        includes_cc.add(self._build_include(relation.bare_type + 'Const'))

    if datatype['VectorMembers'] or datatype['OneToManyRelations']:
      includes.add('#include <vector>')
      includes.add('#include "podio/RelationRange.h"')

    for relation in datatype['VectorMembers'] + datatype['OneToManyRelations']:
      if not relation.is_builtin:
        if relation.full_type not in self.reader.components:
          relation.relation_type = relation.as_qualified_const()

        if relation.full_type == datatype['class'].full_type:
          includes_cc.add(self._build_include(datatype['class'].bare_type))
        else:
          includes.add(self._build_include(relation.bare_type))

    datatype['forward_declarations_obj'] = fwd_declarations
    datatype['includes_obj'] = self._sort_includes(includes)
    datatype['includes_cc_obj'] = self._sort_includes(includes_cc)

  def _preprocess_for_class(self, datatype):
    """Do the preprocessing that is necessary for the classes and Const classes"""
    includes = set(datatype['includes_data'])
    fwd_declarations = {}
    includes_cc = set()

    for member in datatype["Members"]:
      if self.expose_pod_members and not member.is_builtin and not member.is_array:
        member.sub_members = self.reader.components[member.full_type]['Members']

    for relation in datatype['OneToOneRelations']:
      if self._needs_include(relation):
        if relation.namespace not in fwd_declarations:
          fwd_declarations[relation.namespace] = []
        fwd_declarations[relation.namespace].append(relation.bare_type)
        fwd_declarations[relation.namespace].append('Const' + relation.bare_type)
        includes_cc.add(self._build_include(relation.bare_type))

    if datatype['VectorMembers'] or datatype['OneToManyRelations']:
      includes.add('#include <vector>')

    for relation in datatype['OneToManyRelations']:
      if self._needs_include(relation):
        includes.add(self._build_include(relation.bare_type))
      elif relation.is_array:
        includes.add('#include <array>')
        if not relation.is_builtin_array:
          includes.add(self._build_include(relation.array_bare_type))

    for vectormember in datatype['VectorMembers']:
      if vectormember.full_type in self.reader.components:
        includes.add(self._build_include(vectormember.bare_type))

    includes.update(datatype.get('ExtraCode', {}).get('includes', '').split('\n'))
    includes.update(datatype.get('ConstExtraCode', {}).get('includes', '').split('\n'))

    datatype['includes'] = self._sort_includes(includes)
    datatype['includes_cc'] = self._sort_includes(includes_cc)
    datatype['forward_declarations'] = fwd_declarations

  def _preprocess_for_collection(self, datatype):
    """Do the necessary preprocessing for the collection"""
    includes_cc = set()
    for relation in datatype['OneToManyRelations'] + datatype['OneToOneRelations']:
      includes_cc.add(self._build_include(relation.bare_type + 'Collection'))

    if datatype['VectorMembers']:
      includes_cc.add('#include <numeric>')

    datatype['includes_coll_cc'] = self._sort_includes(includes_cc)

    # the ostream operator needs a bit of help from the python side in the form
    # of some pre processing but also in the form of formatting, both are done
    # here.
    # TODO: also handle array members properly. These are currently simply
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
        return '{{:>{width}}}'.format(width=col_width).format(member_header)

      components = member_header.get('components', None)
      name = member_header['name']
      if components is None:
        return '{{:>{width}}}'.format(width=col_width).format(name)

      n_comps = len(components)
      comp_str = '[ {}]'.format(', '.join(components))
      return '{{:>{width}}}'.format(width=col_width * n_comps).format(name + ' ' + comp_str)

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
    data['is_pod'] = self._is_pod_type(definition["Members"])
    self._preprocess_for_class(data)
    self._preprocess_for_obj(data)
    self._preprocess_for_collection(data)

    return data

  def _get_member_includes(self, members):
    """Process all members and gather the necessary includes"""
    includes = set()
    for member in members:
      if member.is_array:
        includes.add("#include <array>")
        if not member.is_builtin_array:
          includes.add(self._build_include(member.array_bare_type))

      for stl_type in ClassDefinitionValidator.allowed_stl_types:
        if member.full_type == 'std::' + stl_type:
          includes.add("#include <{}>".format(stl_type))

      if self._needs_include(member):
        includes.add(self._build_include(member.bare_type))

    return self._sort_includes(includes)

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
    data = {'components': [DataType(c) for c in self.reader.components.keys()],
            'datatypes': [DataType(d) for d in self.reader.datatypes.keys()]}
    self._write_file('selection.xml', self._eval_template('selection.xml.jinja2', data))

  def _build_include(self, classname):
    """Return the include statement."""
    if self.include_subfolder:
      classname = os.path.join(self.package_name, classname)
    return '#include "%s.h"' % classname

  def _sort_includes(self, includes):
    """Sort the includes in order to try to have the std includes at the bottom"""
    package_includes = sorted(i for i in includes if self.package_name in i)
    podio_includes = sorted(i for i in includes if 'podio' in i)
    stl_includes = sorted(i for i in includes if '<' in i and '>' in i)
    # TODO: check whether there are includes that fulfill more than one of the
    # above conditions?

    return package_includes + podio_includes + stl_includes


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

#!/usr/bin/env python3
"""podio class / code generator base functionality"""

import os
import sys
from copy import deepcopy

import jinja2

from podio_gen.podio_config_reader import PodioConfigReader
from podio_gen.generator_utils import DefinitionError
from podio_gen.generator_utils import DataType


THIS_DIR = os.path.dirname(os.path.abspath(__file__))
PYTHONBASE_DIR = os.path.abspath(THIS_DIR + "/../")
TEMPLATE_DIR = os.path.join(PYTHONBASE_DIR, "templates")


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


class ClassGeneratorBaseMixin:
  """Base class for code generation providing common functionality and
  orchestration

  The base class takes care of initializing the common state that is necessary
  for code generation for the different languages. It reads and valiadates the
  datamodel and sets up the jinja2 environment. Furthermore it provides the
  functionality for filling templates and it also does the loop over all the
  components and datatypes in the datamodel offering hooks (see below) to
  augment the common processing with language specifics.

  The following members are initialized and accessible from inheriting classes
  - yamlfile (the path to the yamlfile)
  - install_dir (top level directory into which the code should be generated)
  - package_name (the name of the package)
  - verbose (whether to print some information about the code gen process)
  - dryrun (whether to actually generate the datamodel or to only run the
    processing without filling the contents)
  - upstream_edm (an optional upstream datamodel)
  - datamodel (the current datamodel read from the yamlfile)
  - get_syntax (whether to use get syntax or not)
  - incfolder (whether to create an includeSubfolder or not)
  - expose_pod_members (whether or not to expose the pod members)
  - formatter_func (an optional formatting function that is called after the
    jinja template evaluation but before writing the contents to disk)
  - generated_files (a list of files that have been generated)
  - any_changes (a boolean indicating whether the current run of the code
    generation led to any changes in the generated code wrt the one that is
    already present in the output directory)

  Inheriting classes need to implement the following (potentially empty) methods:

  pre_process() -> dict: does some global pre-processing for the datamodel
                         before any of the components or datatypes are
                         processed. Needs to return a (potentially) empty
                         dictionary

  do_process_component(name: str, component: dict): do some language specific
                       processing for a component populating the component
                       dictionary further. When called only the "class" key will
                       be populated. This function also has to to take care of
                       filling the necessary templates!

  do_process_datatype(name: str, datatype: dict): do some language specific
                      processing for a datatype populating the datatype
                      dictionary further. When called only the "class" key will
                      be populated. This function also has to take care of
                      filling the necessary templates!

  post_process(datamodel: dict): do some global post processing for which all
               components and datatypes need to have been processed already.
               Gets called with the dictionary that has been created in
               pre_proces and filled during the processing. The process
               components and datatypes are accessible via the "components" and
               "datatypes" keys respectively.

  print_report(): prints a report summarizing what has been generated
  """
  def __init__(self, yamlfile, install_dir, package_name, verbose, dryrun, upstream_edm):
    self.yamlfile = yamlfile
    self.install_dir = install_dir
    self.package_name = package_name
    self.verbose = verbose
    self.dryrun = dryrun
    self.upstream_edm = upstream_edm

    try:
      self.datamodel = PodioConfigReader.read(yamlfile, package_name, upstream_edm)
    except DefinitionError as err:
      print(f"Error while generating the datamodel: {err}")
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

  def process(self):
    """Run the actual generation"""
    datamodel = self.pre_process()

    datamodel['components'] = []
    datamodel['datatypes'] = []

    for name, component in self.datamodel.components.items():
      datamodel["components"].append(self._process_component(name, component))

    for name, datatype in self.datamodel.datatypes.items():
      datamodel["datatypes"].append(self._process_datatype(name, datatype))

    self.post_process(datamodel)
    if self.verbose:
      self.print_report()

  def _process_component(self, name, component):
    """Process a single component into a dictionary that can be used in jinja2
    templates and return that"""
    # Make a copy here and add the preprocessing steps to that such that the
    # original definition can be left untouched
    component = deepcopy(component)
    component['class'] = DataType(name)

    self.do_process_component(name, component)
    return component

  def _process_datatype(self, name, datatype):
    """Process a single datatype into a dictionary that can be used in jinja2
    templates and return that"""
    datatype = deepcopy(datatype)
    datatype["class"] = DataType(name)

    self.do_process_datatype(name, datatype)

    return datatype

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
      if self.formatter_func is not None:
        content = self.formatter_func(content, fullname)

      changed = write_file_if_changed(fullname, content)
      self.any_changes = changed or self.any_changes

  def _fill_templates(self, template_base, data, old_schema_data=None):
    """Fill the template and write the results to file"""
    # Update the passed data with some global things that are the same for all
    # files
    data['package_name'] = self.package_name
    data['use_get_syntax'] = self.get_syntax
    data['incfolder'] = self.incfolder
    for filename, template in self._get_filenames_templates(template_base, data['class'].bare_type):
      self._write_file(filename, self._eval_template(template, data, old_schema_data))

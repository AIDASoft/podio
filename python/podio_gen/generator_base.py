#!/usr/bin/env python3

import os
import sys

import jinja2

from podio_gen.podio_config_reader import PodioConfigReader
from podio_gen.generator_utils import DefinitionError


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


class ClassGeneratorBase:
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
      if self.formatter_func:
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

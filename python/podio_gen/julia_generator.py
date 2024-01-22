#!/usr/bin/env python3
"""podio Julia class / code generator"""

from podio_gen.generator_base import ClassGeneratorBaseMixin
from podio_gen.generator_utils import DataType

REPORT_TEXT_JULIA = """
  Julia Code generation is an experimental feature.
  Warning: ExtraCode and MutableExtraCode will be ignored during julia code generation.
  PODIO Data Model
  ================
  Used {yamlfile} to create {nfiles} julia files in {installdir}/
  Read instructions in the README.md to run your first example!
"""


class JuliaClassGenerator(ClassGeneratorBaseMixin):
  """The julia class / code generator for podio"""
  def print_report(self):
    """Print a summary of the generated code"""
    nfiles = len(self.datamodel.datatypes) + len(self.datamodel.components) + 1
    text = REPORT_TEXT_JULIA.format(yamlfile=self.yamlfile,
                                    nfiles=nfiles,
                                    installdir=self.install_dir)

    for summaryline in text.splitlines():
      print(summaryline)
    print()

  def pre_process(self):
    """The necessary specific pre-processing for julia code generation"""
    datamodel = {}
    datamodel['class'] = DataType(self.package_name.capitalize())
    datamodel['upstream_edm'] = self.upstream_edm
    datamodel['upstream_edm_name'] = self.get_upstream_name()
    return datamodel

  def post_process(self, datamodel):
    """The necessary julia specific post processing"""
    datamodel['static_arrays_import'] = self._has_static_arrays_import(datamodel['components'] + datamodel['datatypes'])
    datamodel['includes'] = self._sort_components_and_datatypes(datamodel['components'] + datamodel['datatypes'])
    self._fill_templates("ParentModule", datamodel)

  def do_process_component(self, _, component):
    """Do the julia specific processing of a component"""
    component['upstream_edm'] = self.upstream_edm
    component['upstream_edm_name'] = self.get_upstream_name()
    self._fill_templates("MutableStruct", component)
    return component

  def do_process_datatype(self, _, datatype):
    """Do the julia specific processing for a datatype"""
    if any(self._is_interface(r.full_type) for r in datatype["OneToOneRelations"] + datatype["OneToManyRelations"]):
      # Julia doesn't support any interfaces yet, so we have to also sort out
      # all the datatypes that use them
      return None

    datatype["params_jl"] = sorted(self._get_julia_params(datatype), key=lambda x: x[0])
    datatype["upstream_edm"] = self.upstream_edm
    datatype["upstream_edm_name"] = self.get_upstream_name()

    self._fill_templates("MutableStruct", datatype)
    return datatype

  def do_process_interface(self, _, __):
    """Julia does not support interface types yet, so this does nothing"""
    return None

  def get_upstream_name(self):
    """Get the name of the upstream datamodel if any"""
    if self.upstream_edm:
      return self.upstream_edm.options["includeSubfolder"].split("/")[-2].capitalize()
    return ""

  @staticmethod
  def _get_julia_params(datatype):
    """Get the relations as parametric types for MutableStructs"""
    params = set()
    for relation in datatype['OneToManyRelations'] + datatype['OneToOneRelations']:
      if not relation.is_builtin:
        params.add((relation.bare_type, relation.full_type))
    return list(params)

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

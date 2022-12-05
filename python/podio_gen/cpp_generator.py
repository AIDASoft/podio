#!/usr/bin/env python3
"""Podio C++ class / code generator"""

import sys
import os
from copy import deepcopy
from enum import IntEnum
from collections import defaultdict
from collections.abc import Mapping
from itertools import combinations_with_replacement, product

from podio_schema_evolution import DataModelComparator
from podio_schema_evolution import RenamedMember, root_filter, RootIoRule
from podio_gen.generator_base import ClassGeneratorBaseMixin, write_file_if_changed
from podio_gen.generator_utils import DataType, DataModelJSONEncoder

REPORT_TEXT = """
  PODIO Data Model
  ================
  Used {yamlfile} to create {nclasses} classes in {installdir}/
  Read instructions in the README.md to run your first example!
"""


def replace_component_in_paths(oldname, newname, paths):
    """Replace component name by another one in existing paths"""
    # strip the namespace
    shortoldname = oldname.split("::")[-1]
    shortnewname = newname.split("::")[-1]
    # and do the replace in place
    for index, thePath in enumerate(paths):
        if shortoldname in thePath:
            newPath = thePath.replace(shortoldname, shortnewname)
            paths[index] = newPath


class IncludeFrom(IntEnum):
    """Enum to signify if an include is needed and from where it should come"""

    NOWHERE = 0  # No include needed
    INTERNAL = 1  # include from within the datamodel
    EXTERNAL = 2  # include from an upstream datamodel


class CPPClassGenerator(ClassGeneratorBaseMixin):
    """The c++ class / code generator for podio"""

    def __init__(  # pylint: disable=too-many-arguments
        self,
        yamlfile,
        install_dir,
        package_name,
        io_handlers,
        verbose,
        dryrun,
        upstream_edm,
        old_description,
        evolution_file,
        datamodel_version=None,
    ):
        super().__init__(
            yamlfile,
            install_dir,
            package_name,
            verbose,
            dryrun,
            upstream_edm,
            datamodel_version=datamodel_version,
        )
        self.io_handlers = io_handlers

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
        # a map of datatypes that are used in interfaces populated by pre_process
        self.types_in_interfaces = {}

    def pre_process(self):
        """The necessary specific pre-processing for cpp code generation"""
        self._pre_process_schema_evolution()
        self.types_in_interfaces = self._invert_interfaces()

        return {}

    def post_process(self, _):
        """Do the cpp specific post processing"""
        self._write_edm_def_file()

        # all possible associations
        assocs = self._instantiate_associations()

        if "ROOT" in self.io_handlers:
            self._prepare_iorules()
            self._create_selection_xml(assocs)

        self._write_all_collections_header()
        self._write_cmake_lists_file()

    def do_process_component(self, name, component):
        """Handle everything cpp specific after the common processing of a component"""
        includes = set()
        includes.update(*(m.includes for m in component["Members"]))
        for member in component["Members"]:
            if not (member.is_builtin or member.is_builtin_array):
                includes.add(self._build_include(member))

        includes.update(component.get("ExtraCode", {}).get("includes", "").split("\n"))

        component["includes"] = self._sort_includes(includes)

        self._fill_templates("Component", component)
        # Add potentially older schema for schema evolution
        # based on ROOT capabilities for now
        if name in self.root_schema_dict:
            schema_evolutions = self.root_schema_dict[name]
            component = deepcopy(component)
            for schema_evolution in schema_evolutions:
                if isinstance(schema_evolution, RenamedMember):
                    for member in component["Members"]:
                        if member.name == schema_evolution.member_name_new:
                            member.name = schema_evolution.member_name_old
                    component["class"] = DataType(name + self.old_schema_version)
                else:
                    raise NotImplementedError
            self._fill_templates("Component", component)
            self.root_schema_component_names.add(name + self.old_schema_version)

        return component

    def do_process_datatype(self, name, datatype):
        """Do the cpp specific processing of a datatype"""
        datatype["includes_data"] = self._get_member_includes(datatype["Members"])
        datatype["using_interface_types"] = self.types_in_interfaces.get(name, [])
        self._preprocess_for_class(datatype)
        self._preprocess_for_obj(datatype)
        self._preprocess_for_collection(datatype)

        # ROOT schema evolution preparation
        # Compute and prepare the potential schema evolution parts
        schema_evolution_datatype = deepcopy(datatype)
        needs_schema_evolution = False
        for member in schema_evolution_datatype["Members"]:
            if member.is_array:
                if member.array_type in self.root_schema_dict:
                    needs_schema_evolution = True
                    replace_component_in_paths(
                        member.array_type,
                        member.array_type + self.old_schema_version,
                        schema_evolution_datatype["includes_data"],
                    )
                    member.full_type = member.full_type.replace(
                        member.array_type, member.array_type + self.old_schema_version
                    )
                    member.array_type = member.array_type + self.old_schema_version

            else:
                if member.full_type in self.root_schema_dict:
                    needs_schema_evolution = True
                    # prepare the ROOT I/O rule
                    replace_component_in_paths(
                        member.full_type,
                        member.full_type + self.old_schema_version,
                        schema_evolution_datatype["includes_data"],
                    )
                    member.full_type = member.full_type + self.old_schema_version
                    member.bare_type = member.bare_type + self.old_schema_version

        if needs_schema_evolution:
            print(f"  Preparing explicit schema evolution for {name}")
            schema_evolution_datatype["class"].bare_type = (
                schema_evolution_datatype["class"].bare_type + self.old_schema_version
            )  # noqa
            schema_evolution_datatype["old_schema_version"] = self.old_schema_version_int
            self._fill_templates("Data", schema_evolution_datatype)
            self.root_schema_datatype_names.add(name + self.old_schema_version)
            self._fill_templates("Collection", datatype, schema_evolution_datatype)
        else:
            self._fill_templates("Collection", datatype)

        self._fill_templates("Data", datatype)
        self._fill_templates("Object", datatype)
        self._fill_templates("MutableObject", datatype)
        self._fill_templates("Obj", datatype)
        self._fill_templates("Collection", datatype)
        self._fill_templates("CollectionData", datatype)

        if "SIO" in self.io_handlers:
            self._fill_templates("SIOBlock", datatype)

        return datatype

    def do_process_interface(self, _, interface):
        """Process an interface definition and generate the necessary code"""
        interface["include_types"] = [self._build_include(t) for t in interface["Types"]]

        self._fill_templates("Interface", interface)
        return interface

    def print_report(self):
        """Print a summary report about the generated code"""
        if not self.verbose:
            return
        nclasses = 5 * len(self.datamodel.datatypes) + len(self.datamodel.components)
        text = REPORT_TEXT.format(
            yamlfile=self.yamlfile, nclasses=nclasses, installdir=self.install_dir
        )

        for summaryline in text.splitlines():
            print(summaryline)
        print()

    def _preprocess_for_class(self, datatype):
        """Do the preprocessing that is necessary for the classes and Mutable classes"""
        includes = set(datatype["includes_data"])
        fwd_declarations = defaultdict(list)
        fwd_declarations[datatype["class"].namespace] = [
            f"{datatype['class'].bare_type}Collection"
        ]
        includes_cc = set()

        for member in datatype["Members"]:
            if self.expose_pod_members and not member.is_builtin and not member.is_array:
                member.sub_members = self.datamodel.components[member.full_type]["Members"]

        for relation in datatype["OneToOneRelations"]:
            if self._is_interface(relation.full_type):
                relation.interface_types = self.datamodel.interfaces[relation.full_type]["Types"]
            if self._needs_include(relation.full_type):
                fwd_declarations[relation.namespace].append(relation.bare_type)
                fwd_declarations[relation.namespace].append(f"Mutable{relation.bare_type}")
                includes_cc.add(self._build_include(relation))

        if datatype["VectorMembers"] or datatype["OneToManyRelations"]:
            includes.add("#include <vector>")
            includes.add('#include "podio/RelationRange.h"')

        for relation in datatype["OneToManyRelations"]:
            if self._is_interface(relation.full_type):
                relation.interface_types = self.datamodel.interfaces[relation.full_type]["Types"]
            if self._needs_include(relation.full_type):
                includes.add(self._build_include(relation))

        for vectormember in datatype["VectorMembers"]:
            if vectormember.full_type in self.datamodel.components:
                includes.add(self._build_include(vectormember))

        includes.update(datatype.get("ExtraCode", {}).get("includes", "").split("\n"))
        # TODO: in principle only the mutable classes need these includes!  # pylint: disable=fixme
        includes.update(datatype.get("MutableExtraCode", {}).get("includes", "").split("\n"))

        # When we have a relation to the same type we have the header that we are
        # just generating in the includes. This would lead to a circular include, so
        # remove "ourselves" again from the necessary includes
        try:
            includes.remove(
                self._build_include_for_class(datatype["class"].bare_type, IncludeFrom.INTERNAL)
            )
        except KeyError:
            pass

        # Make sure that all using interface types are properly forward declared
        # to make it possible to declare them as friends so that they can access
        # internals more easily
        for interface in datatype["using_interface_types"]:
            if_type = DataType(interface)
            fwd_declarations[if_type.namespace].append(if_type.bare_type)

        datatype["includes"] = self._sort_includes(includes)
        datatype["includes_cc"] = self._sort_includes(includes_cc)
        datatype["forward_declarations"] = fwd_declarations

    def _preprocess_for_obj(self, datatype):
        """Do the preprocessing that is necessary for the Obj classes"""
        fwd_declarations = defaultdict(list)
        includes, includes_cc = set(), set()
        for relation in datatype["OneToOneRelations"]:
            if relation.full_type != datatype["class"].full_type:
                fwd_declarations[relation.namespace].append(relation.bare_type)
                includes_cc.add(self._build_include(relation))

        if datatype["VectorMembers"] or datatype["OneToManyRelations"]:
            includes.add("#include <vector>")

        for relation in datatype["VectorMembers"] + datatype["OneToManyRelations"]:
            if not relation.is_builtin:
                if relation.full_type == datatype["class"].full_type:
                    includes_cc.add(self._build_include(datatype["class"]))
                else:
                    includes.add(self._build_include(relation))

        datatype["forward_declarations_obj"] = fwd_declarations
        datatype["includes_obj"] = self._sort_includes(includes)
        datatype["includes_cc_obj"] = self._sort_includes(includes_cc)
        non_trivial_type = (
            datatype["VectorMembers"]
            or datatype["OneToManyRelations"]
            or datatype["OneToOneRelations"]
        )
        datatype["is_trivial_type"] = not non_trivial_type

    def _preprocess_for_collection(self, datatype):
        """Do the necessary preprocessing for the collection"""
        includes_cc, includes = set(), set()

        for relation in datatype["OneToManyRelations"] + datatype["OneToOneRelations"]:
            if datatype["class"].bare_type != relation.bare_type:
                include_from = self._needs_include(relation.full_type)
                if self._is_interface(relation.full_type):
                    includes_cc.add(
                        self._build_include_for_class(relation.bare_type, include_from)
                    )
                    for int_type in relation.interface_types:
                        includes_cc.add(
                            self._build_include_for_class(
                                int_type.bare_type + "Collection", include_from
                            )
                        )
                else:
                    includes_cc.add(
                        self._build_include_for_class(
                            relation.bare_type + "Collection", include_from
                        )
                    )
                includes.add(self._build_include_for_class(relation.bare_type, include_from))

        if datatype["VectorMembers"]:
            includes_cc.add("#include <numeric>")

        datatype["includes_coll_cc"] = self._sort_includes(includes_cc)
        datatype["includes_coll_data"] = self._sort_includes(includes)

        # the ostream operator needs a bit of help from the python side in the form
        # of some pre processing but also in the form of formatting, both are done
        # here.
        # TODO: handle array members properly. These are currently ignored  # pylint: disable=fixme
        header_contents = []
        for member in datatype["Members"]:
            header = {"name": member.name}
            if member.full_type in self.datamodel.components:
                comps = [c.name for c in self.datamodel.components[member.full_type]["Members"]]
                header["components"] = comps
            header_contents.append(header)

        def ostream_collection_header(member_header, col_width=12):
            """Custom filter for the jinja2 templates to handle the ostream header that is
            printed for the collections. Need this custom filter because it is easier
            to implement the content dependent width in python than in jinja2.
            """
            if not isinstance(member_header, Mapping):
                # Assume that we have a string and format it according to the width
                return f"{{:>{col_width}}}".format(member_header)

            components = member_header.get("components", None)
            name = member_header["name"]
            if components is None:
                return f"{{:>{col_width}}}".format(name)

            n_comps = len(components)
            comp_str = f'[ {", ".join(components)}]'
            return f"{{:>{col_width * n_comps}}}".format(name + " " + comp_str)

        datatype["ostream_collection_settings"] = {"header_contents": header_contents}
        # Register the custom filter for it to become available in the templates
        self.env.filters["ostream_collection_header"] = ostream_collection_header

    def _pre_process_schema_evolution(self):
        """Process the schema evolution"""
        # have to make all necessary comparisons
        # which are the ones that changed?
        # have to extend the selection xml file
        if self.old_yamlfile:
            comparator = DataModelComparator(
                self.yamlfile, self.old_yamlfile, evolution_file=self.evolution_file
            )
            comparator.read()
            comparator.compare()
            self.old_schema_version = f"v{comparator.datamodel_old.schema_version}"
            self.old_schema_version_int = comparator.datamodel_old.schema_version
            # some sanity checks
            if len(comparator.errors) > 0:
                print(
                    f"The given datamodels '{self.yamlfile}' and '{self.old_yamlfile}' \
have unresolvable schema evolution incompatibilities:"
                )
                for error in comparator.errors:
                    print(error)
                sys.exit(-1)
            if len(comparator.warnings) > 0:
                print(
                    f"The given datamodels '{self.yamlfile}' and '{self.old_yamlfile}' \
have resolvable schema evolution incompatibilities:"
                )
                for warning in comparator.warnings:
                    print(warning)
                sys.exit(-1)

            # now go through all the io_handlers and see what we have to do
            if "ROOT" in self.io_handlers:
                for item in root_filter(comparator.schema_changes):
                    # add whatever is relevant to our ROOT schema evolution
                    self.root_schema_dict.setdefault(item.klassname, []).append(item)

    def _invert_interfaces(self):
        """'Invert' the interfaces to have a mapping of types and their usage in
        interfaces.

        This is necessary to declare the interface types as friends of the
        classes they wrap in order to more easily access some internals.
        """
        types_in_interfaces = defaultdict(list)
        for name, interface in self.datamodel.interfaces.items():
            for if_type in interface["Types"]:
                types_in_interfaces[if_type.full_type].append(name)

        return types_in_interfaces

    def _prepare_iorules(self):
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
                    iorule.source = f"{member_type} {schema_change.member_name_old}"
                    iorule.target = schema_change.member_name_new
                    iorule.code = f"{iorule.target} = onfile.{schema_change.member_name_old};"
                    self.root_schema_iorules.add(iorule)
                else:
                    raise NotImplementedError(
                        f"Schema evolution for {schema_change} not yet implemented."
                    )

    def _write_cmake_lists_file(self):
        """Write the names of all generated header and src files into cmake lists"""
        header_files = (f for f in self.generated_files if f.endswith(".h"))
        src_files = (f for f in self.generated_files if f.endswith(".cc"))
        xml_files = (f for f in self.generated_files if f.endswith(".xml"))

        def _write_list(name, target_folder, files, comment):
            """Write all files into a cmake variable using the target_folder as path to the
            file"""
            list_cont = []

            list_cont.append(f"# {comment}")
            list_cont.append(f"SET({name}")
            for full_file in files:
                fname = os.path.basename(full_file)
                list_cont.append(f"  {os.path.join(target_folder, fname)}")

            list_cont.append(")")

            return "\n".join(list_cont)

        full_contents = ["#-- AUTOMATICALLY GENERATED FILE - DO NOT EDIT -- \n"]
        full_contents.append(
            _write_list(
                "headers",
                r"${ARG_OUTPUT_FOLDER}/${datamodel}",
                header_files,
                "Generated header files",
            )
        )

        full_contents.append(
            _write_list(
                "sources",
                r"${ARG_OUTPUT_FOLDER}/src",
                src_files,
                "Generated source files",
            )
        )

        full_contents.append(
            _write_list(
                "selection_xml",
                r"${ARG_OUTPUT_FOLDER}/src",
                xml_files,
                "Generated xml files",
            )
        )

        write_file_if_changed(
            f"{self.install_dir}/podio_generated_files.cmake",
            "\n".join(full_contents),
            self.any_changes,
        )

    def _write_all_collections_header(self):
        """Write a header file that includes all collection headers"""

        collection_files = (x.split("::")[-1] + "Collection.h" for x in self.datamodel.datatypes)
        self._write_file(
            os.path.join(self.install_dir, self.package_name, f"{self.package_name}.h"),
            self._eval_template(
                "datamodel.h.jinja2",
                {
                    "includes": collection_files,
                    "incfolder": self.incfolder,
                    "package_name": self.package_name,
                },
            ),
        )

    def _write_edm_def_file(self):
        """Write the edm definition to a compile time string"""
        model_encoder = DataModelJSONEncoder()
        data = {
            "package_name": self.package_name,
            "edm_definition": model_encoder.encode(self.datamodel),
            "incfolder": self.incfolder,
            "schema_version": self.datamodel.schema_version,
            "datatypes": self.datamodel.datatypes,
            "datamodel_version": self.datamodel_version,
        }

        def quoted_sv(string):
            return f'"{string}"sv'

        self.env.filters["quoted_sv"] = quoted_sv

        self._write_file(
            "DatamodelDefinition.h",
            self._eval_template("DatamodelDefinition.h.jinja2", data),
        )

    def _create_selection_xml(self, assoc_combinations):
        """Create the selection xml that is necessary for ROOT I/O"""
        data = {
            "version": self.datamodel.schema_version,
            "components": [DataType(c) for c in self.datamodel.components],
            "datatypes": [DataType(d) for d in self.datamodel.datatypes],
            "old_schema_components": [
                DataType(d)
                for d in self.root_schema_datatype_names | self.root_schema_component_names
            ],  # noqa
            "iorules": self.root_schema_iorules,
            "associations": assoc_combinations,
        }

        self._write_file("selection.xml", self._eval_template("selection.xml.jinja2", data))

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

    def _instantiate_associations(self):
        """Instantiate all the associations in a dedicated .cc file and return
        the combination of all instantiated things
        """
        datatypes = [DataType(d) for d in self.datamodel.datatypes]
        includes = [
            self._build_include_for_class(f"{d.bare_type}Collection", IncludeFrom.INTERNAL)
            for d in datatypes
        ]

        # We want all combinations of our datamodel
        combinations = tuple(combinations_with_replacement(datatypes, 2))

        if self.upstream_edm:
            ext_datatypes = [DataType(d) for d in self.upstream_edm.datatypes]
            includes.extend(
                [
                    self._build_include_for_class(f"{d.bare_type}Collection", IncludeFrom.EXTERNAL)
                    for d in ext_datatypes
                ]
            )

            combinations += tuple(product(ext_datatypes, datatypes))

        self._write_file(
            "Associations.h",
            self._eval_template(
                "Associations.h.jinja2",
                {
                    "package_name": self.package_name,
                    "includes": includes,
                    "combinations": combinations,
                },
            ),
        )

        return combinations

    def _needs_include(self, classname) -> IncludeFrom:
        """Check whether the member needs an include from within the datamodel"""
        if (
            classname in self.datamodel.components
            or classname in self.datamodel.datatypes
            or classname in self.datamodel.interfaces
        ):
            return IncludeFrom.INTERNAL

        if self.upstream_edm:
            if (
                classname in self.upstream_edm.components
                or classname in self.upstream_edm.datatypes
                or classname in self.upstream_edm.interfaces
            ):
                return IncludeFrom.EXTERNAL

        return IncludeFrom.NOWHERE

    def _build_include(self, member):
        """Return the include statement for the passed member."""
        return self._build_include_for_class(
            member.bare_type, self._needs_include(member.full_type)
        )

    def _build_include_for_class(self, classname, include_from: IncludeFrom) -> str:
        """Return the include statement for the passed classname"""
        if include_from == IncludeFrom.INTERNAL:
            return f'#include "{self.datamodel.options["includeSubfolder"]}{classname}.h"'
        if include_from == IncludeFrom.EXTERNAL:
            return f'#include "{self.upstream_edm.options["includeSubfolder"]}{classname}.h"'

        # The empty string is filtered by _sort_includes (plus it doesn't hurt in
        # the generated code)
        return ""

    def _sort_includes(self, includes):
        """Sort the includes in order to try to have the std includes at the bottom"""
        package_includes = []
        podio_includes = []
        stl_includes = []
        upstream_includes = []
        for include in (inc.strip() for inc in includes if inc.strip()):
            if self.package_name in include:
                package_includes.append(include)
            elif "podio" in include:
                podio_includes.append(include)
            elif "<" in include and ">" in include:
                stl_includes.append(include)
            elif self.upstream_edm and self.upstream_edm.options["includeSubfolder"] in include:
                upstream_includes.append(include)
            else:
                print(f"Warning: unable to include '{include}'")

        package_includes.sort()
        podio_includes.sort()
        stl_includes.sort()
        upstream_includes.sort()
        return package_includes + upstream_includes + podio_includes + stl_includes

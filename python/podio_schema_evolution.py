#!/usr/bin/env python
"""
Provides infrastructure for analyzing schema definitions for schema evolution
"""

import sys
from dataclasses import dataclass
from typing import List
import yaml

from podio_gen.podio_config_reader import PodioConfigReader
from podio_gen.generator_utils import DataModel


# @TODO: not really a good class model here
# this is a remnant from previous more-sophisticated setups


class SchemaChange:
    """The base class for all schema changes providing a brief description as representation"""

    def __init__(self, description):
        self.description = description

    def __str__(self) -> str:
        return self.description

    def __repr__(self) -> str:
        return self.description


class AddedComponent(SchemaChange):
    """Class representing an added component"""

    def __init__(self, component, name):
        self.component = component
        self.klassname = name
        super().__init__(f"'{self.klassname}' has been added")


class DroppedComponent(SchemaChange):
    """Class representing a dropped component"""

    def __init__(self, component, name):
        self.component = component
        self.klassname = name
        super().__init__(f"'{self.klassname}' has been dropped")


class AddedDatatype(SchemaChange):
    """Class representing an added datatype"""

    def __init__(self, datatype, name):
        self.datatype = datatype
        self.klassname = name
        super().__init__(f"'{self.klassname}' has been added")


class DroppedDatatype(SchemaChange):
    """Class representing a dropped datatype"""

    def __init__(self, datatype, name):
        self.datatype = datatype
        self.klassname = name
        super().__init__(f"'{self.klassname}' has been dropped")


class RenamedDataType(SchemaChange):
    """Class representing a renamed datatype"""

    def __init__(self, name_old, name_new):
        self.name_old = name_old
        self.name_new = name_new
        super().__init__(
            f"'{self.name_new}': datatype '{self.name_old}' renamed to '{self.name_new}'."
        )


class AddedMember(SchemaChange):
    """Class representing an added member"""

    def __init__(self, member, definition_name):
        self.member = member
        self.klassname = definition_name
        super().__init__(f"'{self.klassname}' has an added member '{self.member.name}'")


class DroppedMember(SchemaChange):
    """Class representing a dropped member"""

    def __init__(self, member, definition_name):
        self.member = member
        self.klassname = definition_name
        super().__init__(f"'{self.klassname}' has a dropped member '{self.member.name}")


class ChangedMemberType(SchemaChange):
    """Class representing a type change in a member"""

    def __init__(self, name, old_member, new_member):
        self.old_member = old_member
        self.new_member = new_member
        self.klassname = name
        super().__init__(
            f"'{self.klassname}.{self.old_member.name}' changed type from "
            + f"{self.old_member.full_type} to {self.new_member.full_type}"
        )


class RenamedMember(SchemaChange):
    """Class representing a renamed member"""

    def __init__(self, name, member_name_old, member_name_new):
        self.member_name_old = member_name_old
        self.member_name_new = member_name_new
        self.klassname = name
        super().__init__(
            f"'{self.klassname}': member '{self.member_name_old}' renamed to "
            f"'{self.member_name_new}'."
        )


class AddedVectorMember(SchemaChange):
    """Class representing an added VectorMember"""

    def __init__(self, member, datatype):
        self.member = member
        self.klassname = datatype
        super().__init__(f"'{self.klassname}' has added a VectorMember '{self.member}'")


class DroppedVectorMember(SchemaChange):
    """Class representing a dropped VectorMember"""

    def __init__(self, member, datatype):
        self.member = member
        self.klassname = datatype
        super().__init__(f"'{self.klassname}' has a dropped VectorMember '{self.member.name}")


class AddedSingleRelation(SchemaChange):
    """Class representing an added OneToOneRelation"""

    def __init__(self, member, datatype):
        self.member = member
        self.klassname = datatype
        super().__init__(f"'{self.klassname}' has added a OneToOneRelation '{self.member.name}'")


class DroppedSingleRelation(SchemaChange):
    """Class representing a dropped OneToOneRelation"""

    def __init__(self, member, datatype):
        self.member = member
        self.klassname = datatype
        super().__init__(f"'{self.klassname}' has dropped a OneToOneRelation '{self.member.name}'")


class AddedMultiRelation(SchemaChange):
    """Class representing an added OneToManyRelation"""

    def __init__(self, member, datatype):
        self.member = member
        self.klassname = datatype
        super().__init__(f"'{self.klassname}' has added a OneToManyRelation '{self.member.name}'")


class DroppedMultiRelation(SchemaChange):
    """Class representing a dropped OneToManyRelation"""

    def __init__(self, member, datatype):
        self.member = member
        self.klassname = datatype
        super().__init__(
            f"'{self.klassname}' has dropped a OneToManyRelation '{self.member.name}'"
        )


@dataclass
class RootIoRule:
    """Class collecting all necessary information to create an I/O rule for ROOT"""

    sourceClass: str
    targetClass: str
    version: int
    code: str
    target: str
    source: str = ""


@dataclass
class ComparisonResults:
    """Class grouping the results of a datamodel comparison"""

    old_datamodel: DataModel
    warnings: List[str]
    errors: List[str]
    schema_changes: List[SchemaChange] = None


def sio_filter(schema_changes):
    """
    Checks what is required/supported for the SIO backend

    At this point in time all schema changes have to be handled on PODIO side

    """
    return schema_changes


def root_filter(schema_changes):
    """
    Checks what is required/supported for the ROOT backend

    At this point in time we are only interested in renames.
    Everything else will be done by ROOT automatically
    """
    relevant_schema_changes = []
    for schema_change in schema_changes:
        if isinstance(schema_change, RenamedMember):
            relevant_schema_changes.append(schema_change)
    return relevant_schema_changes


class SchemaEvolutionJudge:
    """
    Analyzes detected schema changes and applies heuristics to determine
    what changes are supported, need user input, or are forbidden
    """

    unsupported_changes = (
        AddedVectorMember,
        DroppedVectorMember,
        AddedSingleRelation,
        DroppedSingleRelation,
        AddedMultiRelation,
        DroppedMultiRelation,
    )

    def __init__(self, new_datamodel, evolution_file=None):
        self.datamodel_new = new_datamodel
        self.evolution_file = evolution_file

    def judge(self, old_datamodel, detected_schema_changes) -> ComparisonResults:
        """Apply heuristics to detected schema changes and return analysis results"""
        read_schema_changes = []
        warnings = []
        errors = []

        if self.evolution_file:
            read_schema_changes = self._read_evolution_file(old_datamodel)

        schema_changes, warnings, errors = self._heuristics(
            old_datamodel, detected_schema_changes, read_schema_changes, warnings, errors
        )

        return ComparisonResults(
            old_datamodel=old_datamodel,
            warnings=warnings,
            errors=errors,
            schema_changes=schema_changes,
        )

    def check_rename(self, added_member, dropped_member, schema_changes, read_schema_changes):
        """Check whether this pair of addition / removal could be a rename and
        return True if it is found in the renamings and false otherwise"""
        if added_member.member.full_type != dropped_member.member.full_type:
            # Different types cannot be a simple renaming
            return False

        for schema_change in read_schema_changes:
            if (
                isinstance(schema_change, RenamedMember)
                and (schema_change.klassname == dropped_member.klassname)
                and (schema_change.member_name_old == dropped_member.member.name)
                and (schema_change.member_name_new == added_member.member.name)
            ):
                # remove the dropping/adding from the schema changes
                # and replace it by the rename
                schema_changes.remove(dropped_member)
                schema_changes.remove(added_member)
                schema_changes.append(schema_change)
                return True

        return False

    def filter_types_with_adds_and_drops(self, added_members, dropped_members):
        """Filter all additions and removals and return pairs of additions /
        removals that happen on the same datatype"""
        filtered_list = []
        for dropped_member in dropped_members:
            for added_member in added_members:
                if dropped_member.klassname == added_member.klassname:
                    filtered_list.append((added_member, dropped_member))

        return filtered_list

    def heuristics_members(
        self, added_members, dropped_members, schema_changes, read_schema_changes, warnings
    ):
        """make analysis of member changes in a given data type"""
        same_type_adds_drops = self.filter_types_with_adds_and_drops(
            added_members, dropped_members
        )
        for added_member, dropped_member in same_type_adds_drops:
            if not self.check_rename(
                added_member, dropped_member, schema_changes, read_schema_changes
            ):
                warnings.append(
                    f"Definition '{dropped_member.klassname}' has a potential "
                    f"rename: '{dropped_member.member.name}' -> "
                    f"'{added_member.member.name}' of type "
                    f"'{dropped_member.member.full_type}'."
                )

        return warnings

    def _heuristics(
        self, old_datamodel, detected_schema_changes, read_schema_changes, warnings, errors
    ):
        """make an analysis of the data model changes:
        - check which can be auto-resolved
        - check which need extra information from the user
        - check which one are plain forbidden/impossible
        """
        # let's analyse the changes in more detail
        # make a copy that can be altered along the way
        schema_changes = detected_schema_changes.copy()
        # are there dropped/added member pairs that could be interpreted as rename?
        dropped_members = [
            change for change in schema_changes if isinstance(change, DroppedMember)
        ]
        added_members = [change for change in schema_changes if isinstance(change, AddedMember)]
        warnings = self.heuristics_members(
            added_members, dropped_members, schema_changes, read_schema_changes, warnings
        )

        for change in (c for c in schema_changes if isinstance(c, self.unsupported_changes)):
            errors.append(f"Unsupported schema change: {change}")

        # are the member changes actually supported/supportable?
        changed_members = [
            change for change in schema_changes if isinstance(change, ChangedMemberType)
        ]
        for change in changed_members:
            # changes between arrays and basic types are forbidden
            if change.old_member.is_array != change.new_member.is_array:
                errors.append(
                    f"Forbidden schema change in '{change.klassname}' for "
                    f"'{change.old_member.name}' from '{change.old_member.full_type}'"
                    f" to '{change.new_member.full_type}'"
                )
            # changing from one component type to another component type is forbidden
            elif change.old_member.full_type in old_datamodel.components:
                errors.append(
                    f"Forbidden schema change in '{change.klassname}' for "
                    f"'{change.old_member.name}' from '{change.old_member.full_type}'"
                    f" to '{change.new_member.full_type}'"
                )

        # are there dropped/added datatype pairs that could be interpreted as rename?
        # for now assuming no change to the individual datatype definition
        # I do not think more complicated heuristics are needed at this point in time
        dropped_datatypes = [
            change for change in schema_changes if isinstance(change, DroppedDatatype)
        ]
        added_datatypes = [
            change for change in schema_changes if isinstance(change, AddedDatatype)
        ]

        for dropped in dropped_datatypes:
            dropped_members = {member.name: member for member in dropped.datatype["Members"]}
            is_known_evolution = False
            for added in added_datatypes:
                added_members = {member.name: member for member in added.datatype["Members"]}
                if set(dropped_members.keys()) == set(added_members.keys()):
                    for schema_change in read_schema_changes:
                        if isinstance(schema_change, RenamedDataType) and (
                            schema_change.name_old == dropped.klassname
                            and schema_change.name_new == added.klassname
                        ):
                            schema_changes.remove(dropped)
                            schema_changes.remove(added)
                            schema_changes.append(schema_change)
                            is_known_evolution = True
                    if not is_known_evolution:
                        warnings.append(
                            f"Potential rename of '{dropped.klassname}' into '{added.klassname}'."
                        )

        # are there dropped/added component pairs that could be interpreted as rename?
        dropped_components = [
            change for change in schema_changes if isinstance(change, DroppedComponent)
        ]
        added_components = [
            change for change in schema_changes if isinstance(change, AddedComponent)
        ]

        for dropped in dropped_components:
            dropped_members = {member.name: member for member in dropped.component["Members"]}
            for added in added_components:
                added_members = {member.name: member for member in added.component["Members"]}
                if set(dropped_members.keys()) == set(added_members.keys()):
                    warnings.append(f"Potential rename of '{dropped.name}' into '{added.name}'.")

        return schema_changes, warnings, errors

    def _read_evolution_file(self, old_datamodel):
        """read and parse evolution file"""
        read_schema_changes = []
        supported_operations = ("member_rename", "class_renamed_to")
        with open(self.evolution_file, "r", encoding="utf-8") as stream:
            content = yaml.load(stream, yaml.SafeLoader)
            from_schema_version = content["from_schema_version"]
            to_schema_version = content["to_schema_version"]
            if (from_schema_version != old_datamodel.schema_version) or (
                to_schema_version != self.datamodel_new.schema_version
            ):
                raise BaseException(
                    "Versions in schema evolution file do not match versions in "
                    "data model descriptions."
                )

            if "evolutions" in content:
                for klassname, value in content["evolutions"].items():
                    # now let's go through the various supported evolutions
                    for operation, details in value.items():
                        if operation not in supported_operations:
                            raise BaseException(
                                f"Schema evolution operation {operation} in {klassname} unknown"
                                " or not supported"
                            )
                        if operation == "member_rename":
                            schema_change = RenamedMember(klassname, details[0], details[1])
                            read_schema_changes.append(schema_change)
                        elif operation == "class_renamed_to":
                            schema_change = RenamedDataType(klassname, details)
                            read_schema_changes.append(schema_change)

        return read_schema_changes


class DataModelComparator:
    """
    Compares two datamodels and detects schema changes
    """

    def __init__(self, new_datamodel: DataModel) -> None:
        self.datamodel_new = new_datamodel

    def compare(self, old_datamodel: DataModel) -> List[SchemaChange]:
        """execute the comparison between the new datamodel and the old one"""

        detected_schema_changes = []
        detected_schema_changes.extend(self._compare_components(old_datamodel))
        detected_schema_changes.extend(self._compare_datatypes(old_datamodel))

        return detected_schema_changes

    def _compare_components(self, old_datamodel):
        """compare component definitions of old and new datamodel"""
        detected_schema_changes = []
        # first check for dropped, added and kept components
        added_components, dropped_components, kept_components = self._compare_keys(
            self.datamodel_new.components.keys(), old_datamodel.components.keys()
        )
        detected_schema_changes.extend(
            [
                AddedComponent(self.datamodel_new.components[name], name)
                for name in added_components
            ]
        )
        detected_schema_changes.extend(
            [DroppedComponent(old_datamodel.components[name], name) for name in dropped_components]
        )

        detected_schema_changes.extend(
            self._compare_members(
                kept_components,
                self.datamodel_new.components,
                old_datamodel.components,
                "Members",
            )
        )

        return detected_schema_changes

    def _compare_datatypes(self, old_datamodel):
        """compare datatype definitions of old and new datamodel"""
        detected_schema_changes = []
        # first check for dropped, added and kept components
        added_types, dropped_types, kept_types = self._compare_keys(
            self.datamodel_new.datatypes.keys(), old_datamodel.datatypes.keys()
        )
        detected_schema_changes.extend(
            [AddedDatatype(self.datamodel_new.datatypes[name], name) for name in added_types]
        )
        detected_schema_changes.extend(
            [DroppedDatatype(old_datamodel.datatypes[name], name) for name in dropped_types]
        )

        detected_schema_changes.extend(
            self._compare_members(
                kept_types,
                self.datamodel_new.datatypes,
                old_datamodel.datatypes,
                "Members",
            )
        )

        detected_schema_changes.extend(
            self._compare_members(
                kept_types,
                self.datamodel_new.datatypes,
                old_datamodel.datatypes,
                "VectorMembers",
                AddedVectorMember,
                DroppedVectorMember,
            )
        )

        detected_schema_changes.extend(
            self._compare_members(
                kept_types,
                self.datamodel_new.datatypes,
                old_datamodel.datatypes,
                "OneToOneRelations",
                AddedSingleRelation,
                DroppedSingleRelation,
            )
        )

        detected_schema_changes.extend(
            self._compare_members(
                kept_types,
                self.datamodel_new.datatypes,
                old_datamodel.datatypes,
                "OneToManyRelations",
                AddedMultiRelation,
                DroppedMultiRelation,
            )
        )

        return detected_schema_changes

    def _compare_members(
        self,
        definitions,
        first,
        second,
        category,
        added_change=AddedMember,
        dropped_change=DroppedMember,
    ):
        """compare member definitions in old and new datamodel"""
        detected_schema_changes = []
        for name in definitions:
            # we are only interested in members not the extracode
            members1 = {member.name: member for member in first[name][category]}
            members2 = {member.name: member for member in second[name][category]}
            added_members, dropped_members, kept_members = self._compare_keys(
                members1.keys(), members2.keys()
            )
            detected_schema_changes.extend(
                [added_change(members1[member], name) for member in added_members]
            )
            detected_schema_changes.extend(
                [dropped_change(members2[member], name) for member in dropped_members]
            )

            # now let's compare old and new for the kept members
            for member_name in kept_members:
                new = members1[member_name]
                old = members2[member_name]
                if old.full_type != new.full_type:
                    detected_schema_changes.append(ChangedMemberType(name, old, new))

        return detected_schema_changes

    @staticmethod
    def _compare_keys(keys1, keys2):
        """compare keys of two given dicts. return added, dropped and overlapping keys"""
        added = set(keys1).difference(keys2)
        dropped = set(keys2).difference(keys1)
        kept = set(keys1).intersection(keys2)
        return added, dropped, kept

    def get_changed_schemata(self, results, schema_filter=None):
        """return the schemata which actually changed"""
        if schema_filter:
            schema_changes = schema_filter(results.schema_changes)
        else:
            schema_changes = results.schema_changes
        changed_klasses = {}
        for schema_change in schema_changes:
            changed_klass = changed_klasses.setdefault(schema_change.klassname, [])
            changed_klass.append(schema_change)
        return changed_klasses

    def print_comparison(self, results):
        """print the result of the datamodel comparison"""
        print(
            f"Comparing datamodel versions {self.datamodel_new.schema_version}"
            f" and {results.old_datamodel.schema_version}"
        )

        print(f"Detected {len(results.schema_changes)} schema changes:")
        for change in results.schema_changes:
            print(f" - {change}")

        if len(results.warnings) > 0:
            print("Warnings:")
            for warning in results.warnings:
                print(f" - {warning}")

        if len(results.errors) > 0:
            print("ERRORS:")
            for error in results.errors:
                print(f" - {error}")
            return False

        return True


##########################
if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(
        description="Given two yaml files this script analyzes "
        "the difference of the two datamodels"
    )

    parser.add_argument("new", help="yaml file describing the new datamodel")
    parser.add_argument("old", help="yaml file describing the old datamodel")
    parser.add_argument(
        "-e", "--evo", help="yaml file clarifying schema evolutions", action="store"
    )
    args = parser.parse_args()

    reader = PodioConfigReader()
    datamodel_new = reader.read(args.new, package_name="new")
    datamodel_old = reader.read(args.old, package_name="old")
    comparator = DataModelComparator(datamodel_new)
    detected_changes = comparator.compare(datamodel_old)

    judge = SchemaEvolutionJudge(comparator.datamodel_new, evolution_file=args.evo)
    comparison_results = judge.judge(datamodel_old, detected_changes)

    if not comparator.print_comparison(comparison_results):
        sys.exit(1)
    # print(comparator.get_changed_schemata(comparison_results, schema_filter=root_filter))

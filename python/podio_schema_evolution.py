#!/usr/bin/env python

from podio_config_reader import PodioConfigReader

# @TODO: not really a good class model here
# this is a remnant from previous more-sophisticated setups


class SchemaChange:
    def __init__(self, description):
        self.description = description

    def __str__(self) -> str:
        return self.description

    def __repr__(self) -> str:
        return self.description


class AddedComponent(SchemaChange):
    def __init__(self, component):
        self.component = component

    def __str__(self) -> str:
        return "'%s' has been added" % self.component.name

    def __repr__(self) -> str:
        return "'%s' has been added" % self.component.name


class DroppedComponent(SchemaChange):
    def __init__(self, component, name):
        self.component = component
        self.name = name

    def __str__(self) -> str:
        return "'%s' has been dropped" % self.name

    def __repr__(self) -> str:
        return "'%s' has been dropped" % self.name


class AddedDatatype(SchemaChange):
    def __init__(self, datatype, name):
        self.datatype = datatype
        self.name = name

    def __str__(self) -> str:
        return "'%s' has been added" % self.name

    def __repr__(self) -> str:
        return "'%s' has been added" % self.name


class DroppedDatatype(SchemaChange):
    def __init__(self, datatype, name):
        self.datatype = datatype
        self.name = name

    def __str__(self) -> str:
        return "'%s' has been dropped" % self.name

    def __repr__(self) -> str:
        return "'%s' has been dropped" % self.name


class AddedMember(SchemaChange):
    def __init__(self, member, definition_name):
        self.member = member
        self.definition_name = definition_name

    def __str__(self) -> str:
        return "'%s' has an addded member '%s'" % (self.definition_name,
                                                   self.member.name)

    def __repr__(self) -> str:
        return "'%s' has an addded member '%s'" % (self.definition_name,
                                                   self.member.name)


class ChangedMember(SchemaChange):
    def __init__(self, name, member_name, old_member, new_member):
        self.name = name
        self.member_name = member_name
        self.old_member = old_member
        self.new_member = new_member

    def __str__(self) -> str:
        return "'%s.%s' changed type from %s to %s" % (self.name, self.member_name,
                                                       self.old_member.full_type, self.new_member.full_type)

    def __repr__(self) -> str:
        return "'%s.%s' changed type from %s to %s" % (self.name, self.member_name,
                                                       self.old_member.full_type, self.new_member.full_type)


class DroppedMember(SchemaChange):
    def __init__(self, member, definition_name):
        self.member = member
        self.definition_name = definition_name

    def __str__(self) -> str:
        return "'%s' has a dropped member '%s'" % (self.definition_name,
                                                   self.member.name)

    def __repr__(self) -> str:
        return "'%s' has a dropped member '%s'" % (self.definition_name,
                                                   self.member.name)


class DataModelComparator:
    def __init__(self, yamlfile_new, yamlfile_old) -> None:
        self.yamlfile_new = yamlfile_new
        self.yamlfile_old = yamlfile_old
        self.reader = PodioConfigReader()
        self.schema_changes = []
        self.warnings = []
        self.errors = []

    def compare(self) -> None:
        self._compare_components()
        self._compare_datatypes()
        self.heuristics()

    def _compare_components(self) -> None:
        # first check for dropped, added and kept components
        added_components, dropped_components, kept_components = self._compare_keys(self.components1.keys(),
                                                                                   self.components2.keys())
        # Make findings known globally
        self.schema_changes.extend([AddedComponent(self.components1[name], name) for name in added_components])
        self.schema_changes.extend([DroppedComponent(self.components2[name], name) for name in dropped_components])

        self._compare_definitions(kept_components, self.components1, self.components2, "Members")

    def _compare_datatypes(self) -> None:
        # first check for dropped, added and kept components
        added_types, dropped_types, kept_types = self._compare_keys(self.datatypes1.keys(),
                                                                    self.datatypes2.keys())
        # Make findings known globally
        self.schema_changes.extend([AddedDatatype(self.datatypes1[name], name) for name in added_types])
        self.schema_changes.extend([DroppedDatatype(self.datatypes2[name], name) for name in dropped_types])

        self._compare_definitions(kept_types, self.datatypes1, self.datatypes2, "Members")

    def _compare_definitions(self, definitions, first, second, category) -> None:
        for name in definitions:
            # we are only interested in members not the extracode
            members1 = {member.name: member for member in first[name][category]}
            members2 = {member.name: member for member in second[name][category]}
            added_members, dropped_members, kept_members = self._compare_keys(members1.keys(),
                                                                              members2.keys())
            # Make findings known globally
            self.schema_changes.extend([AddedMember(members1[member], name) for member in added_members])
            self.schema_changes.extend([DroppedMember(members2[member], name) for member in dropped_members])

            # now let's compare old and new for the kept members
            for member_name in kept_members:
                new = members1[member_name]
                old = members2[member_name]
                if (old.full_type != new.full_type):
                    self.schema_changes.append(
                        ChangedMember(name, member_name, old, new))

    def _compare_keys(self, keys1, keys2):
        added = set(keys1).difference(keys2)
        dropped = set(keys2).difference(keys1)
        kept = set(keys1).intersection(keys2)
        return added, dropped, kept

    def heuristics(self):
        # let's analyse the changes in more detail
        schema_changes = self.schema_changes

        # are there dropped/added member pairs that could be interpreted as rename?
        dropped_members = [change for change in schema_changes if type(change) == DroppedMember]
        added_members = [change for change in schema_changes if type(change) == AddedMember]
        for dropped_member in dropped_members:
            added_members_in_same_definition = [member for member in added_members if
                                                dropped_member.definition_name == member.definition_name]

            for added_member in added_members_in_same_definition:
                if added_member.member.full_type == dropped_member.member.full_type:
                    self.warnings.append("Definition '%s' has a potential member rename '%s' -> '%s' of type '%s'." %
                                         (dropped_member.definition_name, dropped_member.member.name,
                                          added_member.member.name, dropped_member.member.full_type))

        # are the member changes actually supported/supportable?
        changed_members = [change for change in schema_changes if type(change) == ChangedMember]
        for change in changed_members:
          # changes between arrays and basic types are forbidden
          if change.old_member.is_array != change.new_member.is_array:
              self.errors.append("Forbidden schema change in '%s' for '%s' from '%s' to '%s' " %
                                 (change.name, change.member_name,
                                  change.old_member.full_type, change.new_member.full_type))

        # are there dropped/added datatype pairs that could be interpreted as rename?
        # for now assuming no change to the individual datatype definition
        # I do not think more complicated heuristics are needed at this point in time
        dropped_datatypes = [change for change in schema_changes if type(change) == DroppedDatatype]
        added_datatypes = [change for change in schema_changes if type(change) == AddedDatatype]

        for dropped in dropped_datatypes:
            dropped_members = {member.name: member for member in dropped.datatype["Members"]}
            for added in added_datatypes:
                added_members = {member.name: member for member in added.datatype["Members"]}
                if set(dropped_members.keys()) == set(added_members.keys()):
                    self.warnings.append("Potential rename of '%s' into '%s'." % (dropped.name, added.name))

        # are there dropped/added component pairs that could be interpreted as rename?
        dropped_components = [change for change in schema_changes if type(change) == DroppedComponent]
        added_components = [change for change in schema_changes if type(change) == AddedComponent]

        for dropped in dropped_components:
            dropped_members = {member.name: member for member in dropped.component["Members"]}
            for added in added_components:
                added_members = {member.name: member for member in added.component["Members"]}
                if set(dropped_members.keys()) == set(added_members.keys()):
                    self.warnings.append("Potential rename of '%s' into '%s'." % (dropped.name, added.name))

    def print(self):
        print("Comparing datamodel versions %s and %s"
              % (self.datamodel_new.schema_version,
                 self.datamodel_old.schema_version)
              )
        print("Found %i schema changes:" % len(self.schema_changes))
        for change in self.schema_changes:
            print(" - %s" % change)

        if len(self.warnings) > 0:
            print("Warnings:")
            for warning in self.warnings:
                print(" - %s" % warning)

        if len(self.errors) > 0:
            print("ERRORS:")
            for error in self.errors:
                print(" - %s" % error)

    def read(self) -> None:
        self.datamodel_new = self.reader.read(self.yamlfile_new, package_name="new")
        self.datamodel_old = self.reader.read(self.yamlfile_old, package_name="old")
        self.components1 = self.datamodel_new.components
        self.components2 = self.datamodel_old.components
        self.datatypes1 = self.datamodel_new.datatypes
        self.datatypes2 = self.datamodel_old.datatypes


##########################
if __name__ == "__main__":
  import argparse
  parser = argparse.ArgumentParser(description='Given two yaml files this script analyzes '
                                   'the difference of the two datamodels')

  parser.add_argument('new', help='yaml file describing the new datamodel')
  parser.add_argument('old', help='yaml file describing the old datamodel')
  args = parser.parse_args()

  comparator = DataModelComparator(args.new, args.old)
  comparator.read()
  comparator.compare()
  comparator.print()

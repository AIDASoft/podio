#!/usr/bin/env python3
"""Unit tests for the schema evolution module."""

import unittest
import tempfile
import os
from textwrap import dedent

from podio_gen.schema_evolution import (
    SchemaMigration,
    SchemaMigrationReader,
    ChangeType,
)


class TestSchemaMigration(unittest.TestCase):
    """Unit tests for SchemaMigration class."""

    def test_valid_migration_creation(self):
        """Test creating a valid SchemaMigration."""
        migration = SchemaMigration(
            from_version=1,
            to_version=2,
            type=ChangeType.RENAME_MEMBER,
            details={"from": "old_name", "to": "new_name"},
        )
        self.assertEqual(migration.from_version, 1)
        self.assertEqual(migration.to_version, 2)
        self.assertEqual(migration.type, ChangeType.RENAME_MEMBER)
        self.assertEqual(migration.details["from"], "old_name")
        self.assertEqual(migration.details["to"], "new_name")

    def test_invalid_version_order(self):
        """Test that from_version must be less than to_version."""
        with self.assertRaises(ValueError) as cm:
            SchemaMigration(
                from_version=2,
                to_version=1,
                type=ChangeType.RENAME_MEMBER,
                details={"from": "old_name", "to": "new_name"},
            )
        self.assertIn("from_version (2) must be less than to_version (1)", str(cm.exception))

    def test_equal_versions(self):
        """Test that from_version cannot equal to_version."""
        with self.assertRaises(ValueError) as cm:
            SchemaMigration(
                from_version=1,
                to_version=1,
                type=ChangeType.RENAME_MEMBER,
                details={"from": "old_name", "to": "new_name"},
            )
        self.assertIn("from_version (1) must be less than to_version (1)", str(cm.exception))

    def test_missing_required_fields_rename_member(self):
        """Test that RENAME_MEMBER requires 'from' and 'to' fields."""
        with self.assertRaises(ValueError) as cm:
            SchemaMigration(
                from_version=1,
                to_version=2,
                type=ChangeType.RENAME_MEMBER,
                details={"from": "old_name"},  # missing 'to'
            )
        self.assertIn("missing required fields: ['to']", str(cm.exception))

        with self.assertRaises(ValueError) as cm:
            SchemaMigration(
                from_version=1,
                to_version=2,
                type=ChangeType.RENAME_MEMBER,
                details={"to": "new_name"},  # missing 'from'
            )
        self.assertIn("missing required fields: ['from']", str(cm.exception))

        with self.assertRaises(ValueError) as cm:
            SchemaMigration(
                from_version=1,
                to_version=2,
                type=ChangeType.RENAME_MEMBER,
                details={},  # missing both
            )
        self.assertIn("missing required fields: ['from', 'to']", str(cm.exception))


class TestSchemaMigrationReader(unittest.TestCase):
    """Unit tests for SchemaMigrationReader class."""

    def _create_temp_yaml_file(self, yaml_content):
        """Helper method to create a temporary YAML file with the given content."""
        with tempfile.NamedTemporaryFile(mode="w", suffix=".yaml", delete=False) as temp_file:
            temp_file.write(yaml_content)
            return temp_file.name

    def _cleanup_temp_file(self, temp_file):
        """Helper method to clean up a temporary file."""
        os.unlink(temp_file)

    def test_read_valid_yaml_file(self):
        """Test reading a valid YAML file."""
        yaml_content = """\
        migrations:
          ExampleHit:
            - from_version: 1
              to_version: 2
              rename_member:
                from: old_field
                to: new_field
            - from_version: 2
              to_version: 3
              rename_member:
                from: another_old
                to: another_new
          AnotherType:
             - from_version: 1
               to_version: 2
               rename_member:
                 from: x
                 to: y
        """
        temp_file = self._create_temp_yaml_file(dedent(yaml_content))

        try:
            result = SchemaMigrationReader.read_yaml(temp_file)
        finally:
            self._cleanup_temp_file(temp_file)

        self.assertIn("ExampleHit", result)
        self.assertIn("AnotherType", result)
        self.assertEqual(len(result["ExampleHit"]), 2)
        self.assertEqual(len(result["AnotherType"]), 1)

        # Check first migration
        migration = result["ExampleHit"][0]
        self.assertEqual(migration.from_version, 1)
        self.assertEqual(migration.to_version, 2)
        self.assertEqual(migration.type, ChangeType.RENAME_MEMBER)
        self.assertEqual(migration.details["from"], "old_field")
        self.assertEqual(migration.details["to"], "new_field")

    def test_missing_migrations_key(self):
        """Test that YAML must contain 'migrations' key."""
        yaml_content = """\
        some_other_key:
          value: test
        """
        temp_file = self._create_temp_yaml_file(dedent(yaml_content))

        try:
            with self.assertRaises(ValueError) as cm:
                SchemaMigrationReader.read_yaml(temp_file)
            self.assertIn("YAML must contain a 'migrations' key", str(cm.exception))
        finally:
            self._cleanup_temp_file(temp_file)

    def test_migrations_not_dict(self):
        """Test that 'migrations' must be a dictionary."""
        yaml_content = """\
        migrations:
         - not_a_dict
        """
        temp_file = self._create_temp_yaml_file(dedent(yaml_content))

        try:
            with self.assertRaises(ValueError) as cm:
                SchemaMigrationReader.read_yaml(temp_file)
            self.assertIn("'migrations' must be a dictionary", str(cm.exception))
        finally:
            self._cleanup_temp_file(temp_file)

    def test_migration_list_not_list(self):
        """Test that migrations for a datatype must be a list."""
        yaml_content = """\
        migrations:
          ExampleHit: not_a_list
        """
        temp_file = self._create_temp_yaml_file(dedent(yaml_content))

        try:
            with self.assertRaises(ValueError) as cm:
                SchemaMigrationReader.read_yaml(temp_file)
            self.assertIn("Migrations for datatype 'ExampleHit' must be a list", str(cm.exception))
        finally:
            self._cleanup_temp_file(temp_file)

    def test_missing_required_migration_keys(self):
        """Test that migrations must have required keys."""
        yaml_content = """\
        migrations:
         ExampleHit:
           - from_version: 1
           # missing to_version and change type
        """
        temp_file = self._create_temp_yaml_file(dedent(yaml_content))

        try:
            with self.assertRaises(ValueError) as cm:
                SchemaMigrationReader.read_yaml(temp_file)
            self.assertIn("Migration missing required key: 'to_version'", str(cm.exception))
        finally:
            self._cleanup_temp_file(temp_file)

    def test_invalid_version_types(self):
        """Test that versions must be integers."""
        yaml_content = """\
        migrations:
          ExampleHit:
            - from_version: "1"
              to_version: 2
              rename_member:
                from: old
                to: new
        """
        temp_file = self._create_temp_yaml_file(dedent(yaml_content))

        try:
            with self.assertRaises(ValueError) as cm:
                SchemaMigrationReader.read_yaml(temp_file)
            self.assertIn("from_version and to_version must be integers", str(cm.exception))
        finally:
            self._cleanup_temp_file(temp_file)

    def test_change_details_not_dict(self):
        """Test that change details must be a dictionary."""
        yaml_content = """\
        migrations:
          ExampleHit:
            - from_version: 1
              to_version: 2
              rename_member: not_a_dict
        """
        temp_file = self._create_temp_yaml_file(dedent(yaml_content))

        try:
            with self.assertRaises(ValueError) as cm:
                SchemaMigrationReader.read_yaml(temp_file)
            self.assertIn(
                "Change type 'rename_member' missing required fields:",
                str(cm.exception),
            )
        finally:
            self._cleanup_temp_file(temp_file)

    def test_missing_change_type(self):
        """Test that migration must contain a change type."""
        yaml_content = """\
        migrations:
          ExampleHit:
            - from_version: 1
              to_version: 2
              # missing change type
        """
        temp_file = self._create_temp_yaml_file(dedent(yaml_content))

        try:
            with self.assertRaises(ValueError) as cm:
                SchemaMigrationReader.read_yaml(temp_file)
            self.assertIn("Migration must contain one change type", str(cm.exception))
        finally:
            self._cleanup_temp_file(temp_file)

    def test_multiple_change_types(self):
        """Test that migration cannot have multiple change types."""
        yaml_content = """\
        migrations:
          ExampleHit:
            - from_version: 1
              to_version: 2
              rename_member:
                from: old
                to: new
              another_type:
                some: value
        """
        temp_file = self._create_temp_yaml_file(dedent(yaml_content))

        try:
            with self.assertRaises(ValueError) as cm:
                SchemaMigrationReader.read_yaml(temp_file)
            self.assertIn("Migration can only have one change type", str(cm.exception))
        finally:
            self._cleanup_temp_file(temp_file)

    def test_invalid_change_type(self):
        """Test that change type must be valid."""
        yaml_content = """\
        migrations:
          ExampleHit:
            - from_version: 1
              to_version: 2
              invalid_type:
                from: old
                to: new
        """
        temp_file = self._create_temp_yaml_file(dedent(yaml_content))

        try:
            with self.assertRaises(ValueError) as cm:
                SchemaMigrationReader.read_yaml(temp_file)
            self.assertIn("Invalid change type 'invalid_type'", str(cm.exception))
            self.assertIn("Valid types: ['rename_member']", str(cm.exception))
        finally:
            self._cleanup_temp_file(temp_file)

    def test_migrations_sorted_by_version(self):
        """Test that migrations are sorted by from_version."""
        yaml_content = """\
        migrations:
          ExampleHit:
            - from_version: 3
              to_version: 4
              rename_member:
                from: c
                to: d
            - from_version: 1
              to_version: 2
              rename_member:
                from: a
                to: b
            - from_version: 2
              to_version: 3
              rename_member:
                from: b
                to: c
        """
        temp_file = self._create_temp_yaml_file(dedent(yaml_content))

        try:
            result = SchemaMigrationReader.read_yaml(temp_file)
            migrations = result["ExampleHit"]

            # Should be sorted by from_version
            self.assertEqual(migrations[0].from_version, 1)
            self.assertEqual(migrations[1].from_version, 2)
            self.assertEqual(migrations[2].from_version, 3)
        finally:
            self._cleanup_temp_file(temp_file)

    def test_read_yaml_file(self):
        """Test reading from an actual file."""
        yaml_content = """\
        migrations:
          TestType:
            - from_version: 1
              to_version: 2
              rename_member:
                from: old
                to: new
        """
        temp_file = self._create_temp_yaml_file(dedent(yaml_content))

        try:
            result = SchemaMigrationReader.read_yaml(temp_file)
            self.assertIn("TestType", result)
            self.assertEqual(len(result["TestType"]), 1)
            migration = result["TestType"][0]
            self.assertEqual(migration.from_version, 1)
            self.assertEqual(migration.to_version, 2)
        finally:
            self._cleanup_temp_file(temp_file)


if __name__ == "__main__":
    unittest.main()

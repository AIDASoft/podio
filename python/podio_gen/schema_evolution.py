#!/usr/bin/env python3

"""Schema evolution utilities for PODIO data types.

This module provides classes and utilities for reading and validating
schema migration configurations from YAML files.
"""

from typing import Dict, List, Any
from dataclasses import dataclass
from enum import Enum

import yaml


class ChangeType(Enum):
    """Enumeration of supported schema change types."""

    RENAME_MEMBER = "rename_member"


@dataclass
class SchemaMigration:
    """Represents a single schema migration between two versions."""

    from_version: int
    to_version: int
    type: ChangeType
    details: Dict[str, Any]

    def __post_init__(self):
        if self.from_version >= self.to_version:
            raise ValueError(
                f"from_version ({self.from_version}) must be less than "
                f"to_version ({self.to_version})"
            )
        self._validate_change()

    def _validate_change(self):
        """Validate that the change has required fields based on its type."""
        required_fields = {
            ChangeType.RENAME_MEMBER: ["from", "to"],
        }

        required = required_fields.get(self.type, [])
        missing = [field for field in required if field not in self.details]

        if missing:
            raise ValueError(f"Change type {self.type.value} missing required fields: {missing}")


class SchemaMigrationReader:
    """Reader and validator for schema migration YAML files."""

    @staticmethod
    def read_yaml(file_path: str) -> Dict[str, List[SchemaMigration]]:
        """Read and validate a schema migration YAML file."""
        with open(file_path, "r", encoding="utf-8") as file:
            data = yaml.safe_load(file)

        return SchemaMigrationReader._parse_migrations(data)

    @staticmethod
    def _parse_migrations(data: Dict[str, Any]) -> Dict[str, List[SchemaMigration]]:
        """Parse and validate the migrations section."""
        if "migrations" not in data:
            raise ValueError("YAML must contain a 'migrations' key")

        migrations_data = data["migrations"]
        if not isinstance(migrations_data, dict):
            raise ValueError("'migrations' must be a dictionary")

        result = {}

        for datatype, migration_list in migrations_data.items():
            if not isinstance(migration_list, list):
                raise ValueError(f"Migrations for datatype '{datatype}' must be a list")

            parsed_migrations = []
            for i, migration_dict in enumerate(migration_list):
                try:
                    migration = SchemaMigrationReader._parse_single_migration(migration_dict)
                    parsed_migrations.append(migration)
                except Exception as e:
                    raise ValueError(
                        f"Error parsing migration {i} for datatype '{datatype}': {e}"
                    ) from e

            # Sort migrations by from_version for consistency
            parsed_migrations.sort(key=lambda m: (m.from_version, m.to_version))
            result[datatype] = parsed_migrations

        return result

    @staticmethod
    def _parse_single_migration(migration_dict: Dict[str, Any]) -> SchemaMigration:
        """Parse a single migration dictionary."""
        required_keys = ["from_version", "to_version", "change"]
        missing_keys = [key for key in required_keys if key not in migration_dict]

        if missing_keys:
            raise ValueError(f"Migration missing required keys: {missing_keys}")

        from_version = migration_dict["from_version"]
        to_version = migration_dict["to_version"]
        change_dict = migration_dict["change"]

        if not isinstance(from_version, int) or not isinstance(to_version, int):
            raise ValueError("from_version and to_version must be integers")

        if not isinstance(change_dict, dict):
            raise ValueError("'change' must be a dictionary")

        if "type" not in change_dict:
            raise ValueError("Change dictionary must contain a 'type' field")

        type_value = change_dict.pop("type")
        try:
            change_type = ChangeType(type_value)
        except ValueError as exc:
            valid_types = [ct.value for ct in ChangeType]
            raise ValueError(
                f"Invalid change type '{type_value}'. Valid types: {valid_types}"
            ) from exc

        return SchemaMigration(
            from_version=from_version,
            to_version=to_version,
            type=change_type,
            details=change_dict,
        )

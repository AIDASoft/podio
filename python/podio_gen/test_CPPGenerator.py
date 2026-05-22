#!/usr/bin/env python3
"""Unit tests for the C++ code generator."""

import os
import tempfile
import unittest

from podio_gen.cpp_generator import CPPClassGenerator

# Resolve paths relative to the podio repository root, regardless of current working directory
_PODIO_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
_DATALAYOUT_YAML = os.path.join(_PODIO_ROOT, "tests", "datalayout.yaml")


class TestCPPClassGenerator(unittest.TestCase):
    """Unit tests for CPPClassGenerator."""

    def test_arrow_mapper_generation(self):
        """Test that the ARROW I/O handler creates an Arrow schema mapper."""
        with tempfile.TemporaryDirectory() as tmpdir:
            os.makedirs(os.path.join(tmpdir, "src"))
            os.makedirs(os.path.join(tmpdir, "datamodel"))

            generator = CPPClassGenerator(
                _DATALAYOUT_YAML,
                tmpdir,
                "datamodel",
                ["ARROW"],
                verbose=False,
                dryrun=False,
                upstream_edm=None,
                old_descriptions=[],
                evolution_file=None,
            )
            generator.process()

            with open(
                os.path.join(tmpdir, "datamodel", "ArrowMapper.h"), encoding="utf-8"
            ) as mapper:
                contents = mapper.read()

        self.assertIn('arrow::field("ExampleHit", arrow::list(arrow::struct_({', contents)
        self.assertIn('arrow::field("cellID", arrow::uint64())', contents)
        self.assertIn('arrow::field("x", arrow::float64())', contents)
        self.assertIn('arrow::field("count", arrow::list(arrow::int32()))', contents)
        # one-to-one relation: non-nullable (PODIO uses sentinel values, not Arrow null)
        self.assertIn('arrow::field("cluster", objectRefType())', contents)
        # one-to-many relation: list of non-nullable items
        self.assertIn('arrow::field("parents", arrow::list(objectRefType()))', contents)
        self.assertIn(
            'arrow::field("myArray", arrow::fixed_size_list(arrow::int32(), 4))', contents
        )
        self.assertIn(
            'arrow::field("structArray", arrow::fixed_size_list(arrow::struct_({', contents
        )
        self.assertIn('arrow::field("frame_parameters", frameParametersType())', contents)
        self.assertIn(
            'arrow::field("int_params", arrow::map(arrow::utf8(), arrow::list(arrow::int32())))',
            contents,
        )
        self.assertIn(
            'arrow::field("float_params", arrow::map(arrow::utf8(), arrow::list(arrow::float32())))',
            contents,
        )
        self.assertIn(
            'arrow::field("double_params", arrow::map(arrow::utf8(), arrow::list(arrow::float64())))',
            contents,
        )
        self.assertIn(
            'arrow::field("string_params", arrow::map(arrow::utf8(), arrow::list(arrow::utf8())))',
            contents,
        )
        self.assertIn("schemaWithMetadata", contents)
        self.assertIn("arrow::KeyValueMetadata", contents)


if __name__ == "__main__":
    unittest.main()

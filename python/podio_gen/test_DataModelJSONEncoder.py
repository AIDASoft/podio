#!/usr/bin/env python3
"""Unit tests for the JSON encoding of data models"""

import unittest

from podio_gen.generator_utils import DataModelJSONEncoder
from podio_gen.podio_config_reader import MemberParser


def get_member_var_json(string):
    """Get a MemberVariable encoded as JSON from the passed string.

    Passes through the whole chain of parsing and JSON encoding, as it is done
    during data model encoding.

    Args:
        string (str): The member variable definition as a string. NOTE: here it is
            assumed that this is a valid string that can be parsed.

    Returns:
        str: The json encoded member variable
    """
    parser = MemberParser()
    member_var = parser.parse(string, False)  # be lenient with missing descriptions
    return DataModelJSONEncoder().encode(member_var).strip('"')  # strip quotes from JSON


class DataModelJSONEncoderTest(unittest.TestCase):
    """Unit tests for the DataModelJSONEncoder and the utility functionality in MemberVariable"""

    def test_encode_only_types(self):
        """Test that encoding works for type declarations only"""
        for mdef in (
            r"float someFloat",
            r"ArbitraryType name",
            r"std::int16_t fixedWidth",
            r"namespace::Type type",
        ):
            self.assertEqual(get_member_var_json(mdef), mdef)

        # Fixed with without std are encoded with std namespace
        fixed_w = r"int32_t fixedWidth"
        self.assertEqual(get_member_var_json(fixed_w), f"std::{fixed_w}")

    def test_encode_array_types(self):
        """Test that encoding array member variable declarations work"""
        for mdef in (
            r"std::array<float, 5> anArray",
            r"std::array<std::uint32_t, 2> fwArr",
            r"std::array<AType, 42> typeArr",
            r"std::array<nsp::Type, 3> namespacedTypeArr",
        ):
            self.assertEqual(get_member_var_json(mdef), mdef)

    def test_encode_default_vals(self):
        """Test that encoding definitions with default values works"""
        for mdef in (
            r"int i{42}",
            r"std::uint32_t uint{64}",
            r"ArbType a{123}",
            r"namespace::Type t{whatever}",
            r"std::array<float, 2> fs{3.14f, 6.28f}",
            r"std::array<ns::T, 42> typeArr{1, 2, 3}",
        ):
            self.assertEqual(get_member_var_json(mdef), mdef)

    def test_encode_with_description(self):
        """Test that encoding definitions that contain a description works"""
        for mdef in (
            r"int i // an uninitialized int",
            r"std::uint32_t ui{42} // an initialized unsigned int",
            r"std::array<float, 3> fs // a float array",
            r"std::array<nsp::T, 32> tA{1, 2, 3} // an initialized array of namespaced types",
            r"AType type // a very special type",
            r"nsp::Type nspT // a namespaced type",
            r"nsp::Type nspT{with init} // an initialized namespaced type",
            r"ArbitraryType arbT{42} // an initialized type",
        ):
            self.assertEqual(get_member_var_json(mdef), mdef)

#!/usr/bin/env python3
"""Unit tests for methods of the MemberVariable"""

import unittest

from podio_gen.podio_config_reader import MemberParser


class MemberVariableTest(unittest.TestCase):
    """Basic tests for the MemberVariable focusing mainly on the formatting part
    since the parsing part is covered by the MemberParser"""

    def test_signature_property(self):
        """Test signature property for various member variable types"""

        # Test cases: (declaration, expected_signature)
        test_cases = [
            # Builtin types (passed by value)
            ("int count", "const int count"),
            ("double energy", "const double energy"),
            ("float mass", "const float mass"),
            ("bool flag", "const bool flag"),
            ("uint64_t timestamp", "const std::uint64_t timestamp"),
            # Non-builtin types (passed by const reference)
            ("Vector3d position", "const Vector3d& position"),
            (
                "edm4hep::Vector3f momentum",
                "const edm4hep::Vector3f& momentum",
            ),
            ("CustomType data", "const CustomType& data"),
            # Array types (always passed by const reference)
            (
                "std::array<float, 3> coords",
                "const std::array<float, 3>& coords",
            ),
            (
                "std::array<Vertex, 4> vertices",
                "const std::array<Vertex, 4>& vertices",
            ),
            (
                "std::array<int, 2> indices",
                "const std::array<int, 2>& indices",
            ),
        ]

        parser = MemberParser()
        for declaration, expected_signature in test_cases:
            with self.subTest(declaration=declaration):
                member = parser.parse(declaration, False)
                self.assertEqual(member.signature, expected_signature)

    def test_str_representation(self):
        """Test __str__ representation for various member variable types"""

        # Test cases: (declaration, expected_str)
        test_cases = [
            # Basic types without description
            ("int count", "int count{};"),
            ("double energy", "double energy{};"),
            ("float mass", "float mass{};"),
            ("bool flag", "bool flag{};"),
            ("uint64_t timestamp", "std::uint64_t timestamp{};"),
            # Types with description
            ("int count // particle count", "int count{}; ///< particle count"),
            ("double energy // energy in GeV", "double energy{}; ///< energy in GeV"),
            (
                "Vector3d position // position vector",
                "Vector3d position{}; ///< position vector",
            ),
            # Types with default values
            ("int count{42}", "int count{42};"),
            ("float mass{0.5f}", "float mass{0.5f};"),
            ("bool flag{true}", "bool flag{true};"),
            # Types with default values and description
            ("int count{42} // particle count", "int count{42}; ///< particle count"),
            ("float mass{0.5f} // mass in GeV", "float mass{0.5f}; ///< mass in GeV"),
            # Array types
            ("std::array<float, 3> coords", "std::array<float, 3> coords{};"),
            ("std::array<int, 2> indices{1, 2}", "std::array<int, 2> indices{1, 2};"),
            (
                "std::array<double, 4> matrix // transformation matrix",
                "std::array<double, 4> matrix{}; ///< transformation matrix",
            ),
            # Namespaced types
            ("edm4hep::Vector3f momentum", "::edm4hep::Vector3f momentum{};"),
            # Types with units
            (
                "double energy [GeV] // particle energy",
                "double energy{}; ///< particle energy [GeV]",
            ),
            (
                "float mass{0.5f} [GeV] // particle mass",
                "float mass{0.5f}; ///< particle mass [GeV]",
            ),
        ]

        parser = MemberParser()
        for declaration, expected_str in test_cases:
            with self.subTest(declaration=declaration):
                member = parser.parse(declaration, False)
                self.assertEqual(str(member), expected_str)

    def test_qualified_as_version(self):
        """Test qualified_as_version method for various member variable types"""

        # Test cases: (declaration, version, expected_qualified_str)
        test_cases = [
            # Builtin types should remain unchanged (not versionable)
            ("int count", 1, "int count{};"),
            ("double energy", 2, "double energy{};"),
            ("float mass{0.5f}", 1, "float mass{0.5f};"),
            ("bool flag{true} // a flag", 3, "bool flag{true}; ///< a flag"),
            ("uint64_t timestamp", 1, "std::uint64_t timestamp{};"),
            # Builtin arrays should remain unchanged
            ("std::array<int, 3> coords", 1, "std::array<int, 3> coords{};"),
            (
                "std::array<float, 2> values{1.0f, 2.0f}",
                1,
                "std::array<float, 2> values{1.0f, 2.0f};",
            ),
            # Non-builtin types should get version namespace
            ("Vector3d position", 1, "::v1::Vector3d position{};"),
            ("CustomType data", 2, "::v2::CustomType data{};"),
            (
                "SomeClass obj{42} // description",
                3,
                "::v3::SomeClass obj{42}; ///< description",
            ),
            # Namespaced non-builtin types should get version namespace
            ("edm4hep::Vector3f momentum", 1, "::edm4hep::v1::Vector3f momentum{};"),
            ("ns::MyType value", 2, "::ns::v2::MyType value{};"),
            # Non-builtin arrays should get version namespace for array type
            (
                "std::array<Vector3d, 4> vertices",
                1,
                "std::array<::v1::Vector3d, 4> vertices{};",
            ),
            (
                "std::array<edm4hep::Particle, 2> particles",
                2,
                "std::array<::edm4hep::v2::Particle, 2> particles{};",
            ),
            (
                "std::array<CustomType, 3> objects // array of objects",
                1,
                "std::array<::v1::CustomType, 3> objects{}; ///< array of objects",
            ),
            # Types with units
            (
                "Vector3d position [mm] // position vector",
                1,
                "::v1::Vector3d position{}; ///< position vector [mm]",
            ),
            (
                "CustomType data{42} [unit] // some data",
                2,
                "::v2::CustomType data{42}; ///< some data [unit]",
            ),
        ]

        parser = MemberParser()
        for declaration, version, expected_qualified_str in test_cases:
            with self.subTest(declaration=declaration, version=version):
                member = parser.parse(declaration, False)
                self.assertEqual(member.qualified_as_version(version), expected_qualified_str)

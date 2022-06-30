#!/usr/bin/env python
"""
Tests for the MemberParser highlighting what it is currently capable of and
what trips it up
"""

import unittest

from podio_config_reader import MemberParser, DefinitionError


class MemberParserTest(unittest.TestCase):
  """Unit tests for the MemberParser"""

  def test_parse_valid(self):  # pylint: disable=too-many-statements
    """Test if valid member definitions pass"""
    parser = MemberParser()

    parsed = parser.parse(r'float someFloat // with an additional comment')
    self.assertEqual(parsed.full_type, r'float')
    self.assertEqual(parsed.name, r'someFloat')
    self.assertEqual(parsed.description, r'with an additional comment')
    self.assertEqual(parsed.julia_type, r'Float32')

    parsed = parser.parse(r'float float2 // with numbers')
    self.assertEqual(parsed.full_type, r'float')
    self.assertEqual(parsed.name, r'float2')
    self.assertEqual(parsed.description, r'with numbers')
    self.assertEqual(parsed.julia_type, r'Float32')

    parsed = parser.parse(r'  float   spacefloat    //    whitespace everywhere   ')
    self.assertEqual(parsed.full_type, r'float')
    self.assertEqual(parsed.name, r'spacefloat')
    self.assertEqual(parsed.description, 'whitespace everywhere')
    self.assertEqual(parsed.julia_type, r'Float32')

    parsed = parser.parse(r'int snake_case // snake case')
    self.assertEqual(parsed.full_type, r'int')
    self.assertEqual(parsed.name, r'snake_case')
    self.assertEqual(parsed.description, r'snake case')
    self.assertEqual(parsed.julia_type, r'Int32')

    parsed = parser.parse(r'std::string mixed_UglyCase_12 // who wants this')
    self.assertEqual(parsed.full_type, r'std::string')
    self.assertEqual(parsed.name, r'mixed_UglyCase_12')
    self.assertEqual(parsed.description, r'who wants this')

    # Check some of the trickier builtin types
    parsed = parser.parse(r'unsigned long long uVar // an unsigned long variable')
    self.assertEqual(parsed.full_type, r'unsigned long long')
    self.assertEqual(parsed.name, r'uVar')
    self.assertEqual(parsed.description, r'an unsigned long variable')
    self.assertEqual(parsed.julia_type, r'UInt64')

    parsed = parser.parse(r'unsigned int uInt // an unsigned integer')
    self.assertEqual(parsed.full_type, r'unsigned int')
    self.assertEqual(parsed.name, r'uInt')
    self.assertEqual(parsed.description, r'an unsigned integer')
    self.assertEqual(parsed.julia_type, r'UInt32')

    # Fixed width integers in their various forms that they can be spelled out
    # and be considered valid in our case
    parsed = parser.parse(r'std::int16_t qualified // qualified fixed width ints work')
    self.assertEqual(parsed.full_type, r'std::int16_t')
    self.assertEqual(parsed.name, r'qualified')
    self.assertEqual(parsed.description, r'qualified fixed width ints work')
    self.assertTrue(parsed.is_builtin)
    self.assertEqual(parsed.julia_type, r'Int16')

    parsed = parser.parse(r'std::uint64_t bits // fixed width integer types should work')
    self.assertEqual(parsed.full_type, r'std::uint64_t')
    self.assertEqual(parsed.name, r'bits')
    self.assertEqual(parsed.description, r'fixed width integer types should work')
    self.assertTrue(parsed.is_builtin)
    self.assertEqual(parsed.julia_type, r'UInt64')

    parsed = parser.parse(r'int32_t fixedInt // fixed width signed integer should work')
    self.assertEqual(parsed.full_type, r'std::int32_t')
    self.assertEqual(parsed.name, r'fixedInt')
    self.assertEqual(parsed.description, r'fixed width signed integer should work')
    self.assertTrue(parsed.is_builtin)
    self.assertEqual(parsed.julia_type, r'Int32')

    parsed = parser.parse(r'uint16_t fixedUInt // fixed width unsigned int with 16 bits')
    self.assertEqual(parsed.full_type, r'std::uint16_t')
    self.assertEqual(parsed.name, r'fixedUInt')
    self.assertEqual(parsed.description, r'fixed width unsigned int with 16 bits')
    self.assertTrue(parsed.is_builtin)
    self.assertEqual(parsed.julia_type, r'UInt16')

    # an array definition with space everywhere it is allowed
    parsed = parser.parse(r'  std::array < double , 4 >   someArray   // a comment  ')
    self.assertEqual(parsed.full_type, r'std::array<double, 4>')
    self.assertEqual(parsed.name, r'someArray')
    self.assertEqual(parsed.description, r'a comment')
    self.assertTrue(not parsed.is_builtin)
    self.assertTrue(parsed.is_builtin_array)
    self.assertEqual(int(parsed.array_size), 4)
    self.assertEqual(parsed.array_type, r'double')
    self.assertEqual(parsed.julia_type, r'MVector{4, Float64}')

    # an array definition as terse as possible
    parsed = parser.parse(r'std::array<int,2>anArray//with a comment')
    self.assertEqual(parsed.full_type, r'std::array<int, 2>')
    self.assertEqual(parsed.name, r'anArray')
    self.assertEqual(parsed.description, r'with a comment')
    self.assertEqual(parsed.julia_type, r'MVector{2, Int32}')

    parsed = parser.parse('::TopLevelNamespaceType aValidType // hopefully')
    self.assertEqual(parsed.full_type, '::TopLevelNamespaceType')
    self.assertEqual(parsed.name, r'aValidType')
    self.assertEqual(parsed.description, 'hopefully')
    self.assertEqual(parsed.julia_type, r'TopLevelNamespaceType')

    parsed = parser.parse(r'std::array<::GlobalType, 1> anArray // with a top level type')
    self.assertEqual(parsed.full_type, r'std::array<::GlobalType, 1>')
    self.assertEqual(parsed.name, r'anArray')
    self.assertEqual(parsed.description, r'with a top level type')
    self.assertTrue(not parsed.is_builtin_array)
    self.assertEqual(parsed.array_type, r'::GlobalType')
    self.assertEqual(parsed.julia_type, r'MVector{1, GlobalType}')

    parsed = parser.parse(r'std::array<std::int16_t, 42> fixedWidthArray // a fixed width type array')
    self.assertEqual(parsed.full_type, r'std::array<std::int16_t, 42>')
    self.assertEqual(parsed.name, r'fixedWidthArray')
    self.assertEqual(parsed.description, r'a fixed width type array')
    self.assertTrue(parsed.is_builtin_array)
    self.assertEqual(parsed.array_type, r'std::int16_t')
    self.assertEqual(parsed.julia_type, r'MVector{42, Int16}')

    parsed = parser.parse(r'std::array<uint32_t, 42> fixedWidthArray // a fixed width type array without namespace')
    self.assertEqual(parsed.full_type, r'std::array<std::uint32_t, 42>')
    self.assertEqual(parsed.name, r'fixedWidthArray')
    self.assertEqual(parsed.description, r'a fixed width type array without namespace')
    self.assertTrue(parsed.is_builtin_array)
    self.assertEqual(parsed.array_type, r'std::uint32_t')
    self.assertEqual(parsed.julia_type, r'MVector{42, UInt32}')

  def test_parse_invalid(self):
    """Test that invalid member variable definitions indeed fail during parsing"""
    # setup an empty parser
    parser = MemberParser()

    invalid_inputs = [
        r'int // a type without name',
        r'int anIntWithoutDescription',
        r'__someType name // an illformed type',
        r'double 1WrongNamedDouble // an invalid name',
        r'std::array<double, 3>',  # array without name and description
        r'std::array<double, 2> // an array without a name',
        r'std::array<int, 2> anArrayWithoutDescription',
        r'std::array<__foo, 3> anArray // with invalid type',
        r'std::array<double, N> array // with invalid size',
        r'int another ill formed name // some comment'

        # Some examples of valid c++ that are rejected by the validation
        r'unsigned long int uLongInt // technically valid c++, but not in our builtin list',
        r'::std::array<float, 2> a // technically valid c++, but breaks class generation',
        r':: std :: array<int, 3> arr // also technically valid c++ but not in our case',
        r'int8_t disallowed // fixed width ints with 8 bits are often aliased to signed char',
        r'uint8_t disallowed // fixed width unsigned ints with 8 bits are often aliased to unsigned char',
        r'int_least32_t disallowed // only allow fixed width integers with exact widths',
        r'uint_fast64_t disallowed // only allow fixed width integers with exact widths',
        r'std::int_least16_t disallowed // also adding a std namespace here does not make these allowed',
        r'std::uint_fast16_t disallowed // also adding a std namespace here does not make these allowed',
        r'std::array<uint_fast16_t, 42> disallowedArray // arrays should not accept disallowed fixed width types'
        ]

    for inp in invalid_inputs:
      with self.assertRaises(DefinitionError):
        parser.parse(inp)

  def test_parse_valid_no_description(self):
    """Test that member variable definitions are OK without description"""
    parser = MemberParser()

    parsed = parser.parse('unsigned long long aLongWithoutDescription', False)
    self.assertEqual(parsed.full_type, 'unsigned long long')
    self.assertEqual(parsed.name, 'aLongWithoutDescription')
    self.assertEqual(parsed.julia_type, r'UInt64')

    parsed = parser.parse('std::array<unsigned long, 123> unDescribedArray', False)
    self.assertEqual(parsed.full_type, 'std::array<unsigned long, 123>')
    self.assertEqual(parsed.name, 'unDescribedArray')
    self.assertEqual(parsed.array_type, 'unsigned long')
    self.assertTrue(parsed.is_builtin_array)
    self.assertEqual(parsed.julia_type, r'MVector{123, UInt32}')

    parsed = parser.parse('unsigned long longWithReallyStupidName', False)
    self.assertEqual(parsed.full_type, 'unsigned long')
    self.assertEqual(parsed.name, 'longWithReallyStupidName')
    self.assertEqual(parsed.julia_type, r'UInt32')

    parsed = parser.parse('NonBuiltIn aType // descriptions are not ignored even though they are not required', False)
    self.assertEqual(parsed.full_type, 'NonBuiltIn')
    self.assertEqual(parsed.name, 'aType')
    self.assertEqual(parsed.description, 'descriptions are not ignored even though they are not required')
    self.assertTrue(not parsed.is_builtin)
    self.assertEqual(parsed.julia_type, r'NonBuiltIn')

  def test_string_representation(self):
    """Test that the string representation that is used in the jinja2 templates
    includes the default initialization"""
    parser = MemberParser()

    parsed = parser.parse('unsigned long long var // description')
    self.assertEqual(str(parsed), r'unsigned long long var{}; ///< description')

    # Also works without a description
    parsed = parser.parse('SomeType memberVar', False)
    self.assertEqual(str(parsed), r'SomeType memberVar{};')

    parsed = parser.parse('Type var//with very close comment')
    self.assertEqual(str(parsed), r'Type var{}; ///< with very close comment')


if __name__ == '__main__':
  unittest.main()

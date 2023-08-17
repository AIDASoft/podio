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
    self.assertTrue(parsed.default_val is None)

    parsed = parser.parse(r'float float2 // with numbers')
    self.assertEqual(parsed.full_type, r'float')
    self.assertEqual(parsed.name, r'float2')
    self.assertEqual(parsed.description, r'with numbers')

    parsed = parser.parse(r'  float   spacefloat    //    whitespace everywhere   ')
    self.assertEqual(parsed.full_type, r'float')
    self.assertEqual(parsed.name, r'spacefloat')
    self.assertEqual(parsed.description, 'whitespace everywhere')

    parsed = parser.parse(r'int snake_case // snake case')
    self.assertEqual(parsed.full_type, r'int')
    self.assertEqual(parsed.name, r'snake_case')
    self.assertEqual(parsed.description, r'snake case')

    parsed = parser.parse(r'std::string mixed_UglyCase_12 // who wants this')
    self.assertEqual(parsed.full_type, r'std::string')
    self.assertEqual(parsed.name, r'mixed_UglyCase_12')
    self.assertEqual(parsed.description, r'who wants this')

    # Check some of the trickier builtin types
    parsed = parser.parse(r'unsigned long long uVar // an unsigned long variable')
    self.assertEqual(parsed.full_type, r'unsigned long long')
    self.assertEqual(parsed.name, r'uVar')
    self.assertEqual(parsed.description, r'an unsigned long variable')

    parsed = parser.parse(r'unsigned int uInt // an unsigned integer')
    self.assertEqual(parsed.full_type, r'unsigned int')
    self.assertEqual(parsed.name, r'uInt')
    self.assertEqual(parsed.description, r'an unsigned integer')

    # Fixed width integers in their various forms that they can be spelled out
    # and be considered valid in our case
    parsed = parser.parse(r'std::int16_t qualified // qualified fixed width ints work')
    self.assertEqual(parsed.full_type, r'std::int16_t')
    self.assertEqual(parsed.name, r'qualified')
    self.assertEqual(parsed.description, r'qualified fixed width ints work')
    self.assertTrue(parsed.is_builtin)

    parsed = parser.parse(r'std::uint64_t bits // fixed width integer types should work')
    self.assertEqual(parsed.full_type, r'std::uint64_t')
    self.assertEqual(parsed.name, r'bits')
    self.assertEqual(parsed.description, r'fixed width integer types should work')
    self.assertTrue(parsed.is_builtin)

    parsed = parser.parse(r'int32_t fixedInt // fixed width signed integer should work')
    self.assertEqual(parsed.full_type, r'std::int32_t')
    self.assertEqual(parsed.name, r'fixedInt')
    self.assertEqual(parsed.description, r'fixed width signed integer should work')
    self.assertTrue(parsed.is_builtin)

    parsed = parser.parse(r'uint16_t fixedUInt // fixed width unsigned int with 16 bits')
    self.assertEqual(parsed.full_type, r'std::uint16_t')
    self.assertEqual(parsed.name, r'fixedUInt')
    self.assertEqual(parsed.description, r'fixed width unsigned int with 16 bits')
    self.assertTrue(parsed.is_builtin)

    # an array definition with space everywhere it is allowed
    parsed = parser.parse(r'  std::array < double , 4 >   someArray   // a comment  ')
    self.assertEqual(parsed.full_type, r'std::array<double, 4>')
    self.assertEqual(parsed.name, r'someArray')
    self.assertEqual(parsed.description, r'a comment')
    self.assertTrue(not parsed.is_builtin)
    self.assertTrue(parsed.is_builtin_array)
    self.assertEqual(int(parsed.array_size), 4)
    self.assertEqual(parsed.array_type, r'double')
    self.assertTrue(parsed.default_val is None)

    # an array definition as terse as possible
    parsed = parser.parse(r'std::array<int,2>anArray//with a comment')
    self.assertEqual(parsed.full_type, r'std::array<int, 2>')
    self.assertEqual(parsed.name, r'anArray')
    self.assertEqual(parsed.description, r'with a comment')

    parsed = parser.parse('::TopLevelNamespaceType aValidType // hopefully')
    self.assertEqual(parsed.full_type, '::TopLevelNamespaceType')
    self.assertEqual(parsed.name, r'aValidType')
    self.assertEqual(parsed.description, 'hopefully')

    parsed = parser.parse(r'std::array<::GlobalType, 1> anArray // with a top level type')
    self.assertEqual(parsed.full_type, r'std::array<::GlobalType, 1>')
    self.assertEqual(parsed.name, r'anArray')
    self.assertEqual(parsed.description, r'with a top level type')
    self.assertTrue(not parsed.is_builtin_array)
    self.assertEqual(parsed.array_type, r'::GlobalType')

    parsed = parser.parse(r'std::array<std::int16_t, 42> fixedWidthArray // a fixed width type array')
    self.assertEqual(parsed.full_type, r'std::array<std::int16_t, 42>')
    self.assertEqual(parsed.name, r'fixedWidthArray')
    self.assertEqual(parsed.description, r'a fixed width type array')
    self.assertTrue(parsed.is_builtin_array)
    self.assertEqual(parsed.array_type, r'std::int16_t')

    parsed = parser.parse(r'std::array<uint32_t, 42> fixedWidthArray // a fixed width type array without namespace')
    self.assertEqual(parsed.full_type, r'std::array<std::uint32_t, 42>')
    self.assertEqual(parsed.name, r'fixedWidthArray')
    self.assertEqual(parsed.description, r'a fixed width type array without namespace')
    self.assertTrue(parsed.is_builtin_array)
    self.assertEqual(parsed.array_type, r'std::uint32_t')

  def test_parse_valid_default_value(self):
    """Test that member variables can be parsed correctly if they have a user
    defined default value"""
    parser = MemberParser()

    parsed = parser.parse(r'int fortyTwo{43} // default values can lie')
    self.assertEqual(parsed.full_type, r'int')
    self.assertEqual(parsed.name, r'fortyTwo')
    self.assertEqual(parsed.description, 'default values can lie')
    self.assertEqual(parsed.default_val, r'43')
    self.assertEqual(str(parsed), 'int fortyTwo{43}; ///< default values can lie')

    parsed = parser.parse(r'float f{3.14f}', require_description=False)
    self.assertEqual(parsed.full_type, 'float')
    self.assertEqual(parsed.name, 'f')
    self.assertEqual(parsed.default_val, '3.14f')

    parsed = parser.parse(r'std::array<int, 3> array{1, 2, 3} // arrays can be initialized')
    self.assertEqual(parsed.full_type, r'std::array<int, 3>')
    self.assertEqual(parsed.default_val, '1, 2, 3')
    self.assertEqual(parsed.name, 'array')

    parsed = parser.parse(r'std::array<int, 25> array{1, 2, 3} // we do not have to init the complete array')
    self.assertEqual(parsed.full_type, r'std::array<int, 25>')
    self.assertEqual(parsed.default_val, r'1, 2, 3')

    # These are cases where we cannot really decide whether the initialization
    # is valid just from the member declaration. We let them pass here
    parsed = parser.parse('nsp::SomeValue val {42} // default values can have space')
    self.assertEqual(parsed.full_type, 'nsp::SomeValue')
    self.assertEqual(parsed.name, 'val')
    self.assertEqual(parsed.default_val, '42')
    self.assertEqual(parsed.namespace, 'nsp')
    self.assertEqual(parsed.bare_type, 'SomeValue')

    parsed = parser.parse(r'edm4hep::Vector3d v{1, 2, 3, 4} // for aggregates we do not validate init values')
    self.assertEqual(parsed.full_type, 'edm4hep::Vector3d')
    self.assertEqual(parsed.default_val, '1, 2, 3, 4')

    # There are cases where we could technically validate this via a syntax
    # check by the compiler but we don't do that because it is too costly and
    # this is considered an expert feature. The generated code will not compile
    parsed = parser.parse(r'AggType bogusInit{here, we can even put invalid c++}', False)
    self.assertEqual(parsed.default_val, 'here, we can even put invalid c++')

    parsed = parser.parse(r'std::array<int, 1> array{1, 2, 3} // too many values provided')
    self.assertEqual(parsed.default_val, '1, 2, 3')

    # Invalid user default value initialization
    parsed = parser.parse(r'int weirdDefault{whatever, even space} // invalid c++ is not caught')
    self.assertEqual(parsed.default_val, 'whatever, even space')

    parsed = parser.parse(r'int floatInit{3.14f} // implicit conversions are not allowed in aggregate initialization')
    self.assertEqual(parsed.default_val, '3.14f')

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
        r'int another ill formed name // some comment',
        r'float illFormedDefault {',

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
        r'std::array<uint_fast16_t, 42> disallowedArray // arrays should not accept disallowed fixed width types',

        # Default values cannot be empty
        r'int emptyDefault{} // valid c++, but we want an explicit default value here',

        ]

    for inp in invalid_inputs:
      try:
        self.assertRaises(DefinitionError, parser.parse, inp)
      except AssertionError:
        raise AssertionError(f"'{inp}' should raise a DefinitionError from the MemberParser")

  def test_parse_valid_no_description(self):
    """Test that member variable definitions are OK without description"""
    parser = MemberParser()

    parsed = parser.parse('unsigned long long aLongWithoutDescription', False)
    self.assertEqual(parsed.full_type, 'unsigned long long')
    self.assertEqual(parsed.name, 'aLongWithoutDescription')

    parsed = parser.parse('std::array<unsigned long, 123> unDescribedArray', False)
    self.assertEqual(parsed.full_type, 'std::array<unsigned long, 123>')
    self.assertEqual(parsed.name, 'unDescribedArray')
    self.assertEqual(parsed.array_type, 'unsigned long')
    self.assertTrue(parsed.is_builtin_array)

    parsed = parser.parse('unsigned long longWithReallyStupidName', False)
    self.assertEqual(parsed.full_type, 'unsigned long')
    self.assertEqual(parsed.name, 'longWithReallyStupidName')

    parsed = parser.parse('NonBuiltIn aType // descriptions are not ignored even though they are not required', False)
    self.assertEqual(parsed.full_type, 'NonBuiltIn')
    self.assertEqual(parsed.name, 'aType')
    self.assertEqual(parsed.description, 'descriptions are not ignored even though they are not required')
    self.assertTrue(not parsed.is_builtin)

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

#!/usr/bin/env python

"""
Check some cases that should fail the class definition validator
"""

from __future__ import print_function, absolute_import, unicode_literals
import unittest
from copy import deepcopy

from podio_config_reader import ClassDefinitionValidator, MemberVariable, DefinitionError


def make_dm(components, datatypes):
  """Small helper function to turn things into a datamodel dict as expected by
  the validator"""
  return {
      'components': components,
      'datatypes': datatypes,
      }


class ClassDefinitionValidatorTest(unittest.TestCase):
  def setUp(self):
    valid_component_members = [
        MemberVariable(type='int', name='anInt'),
        MemberVariable(type='float', name='aFloat'),
        MemberVariable(array_type='int', array_size='4', name='anArray')
        ]

    self.valid_component = {
        'Component': {
            'Members': valid_component_members,
            'ExtraCode': {
                'includes': '#include "someFancyHeader.h"',
                'declaration': 'we do not validate if this is valid c++'
                }
            }
        }

    valid_datatype_members = [
        MemberVariable(type='float', name='energy', description='energy [GeV]'),
        MemberVariable(array_type='int', array_size='5', name='anArray', description='some Array')
        ]

    self.valid_datatype = {
        'DataType': {
            'Author': 'Mr. Bean',
            'Description': 'I am merely here for a test',
            'Members': valid_datatype_members,
            'ExtraCode': {
                'includes': '#include <whatever> not checking for valid c++',
                'declaration': 'not necessarily valid c++',
                'implementation': 'still not checked for c++ validity',
                },
            'ConstExtraCode': {
                'declaration': 'also not checked for valid c++',
                'implementation': 'nothing has changed',
                'includes': '#include <something_else> this will appear in both includes!'
                }
            }
        }

    self.validator = ClassDefinitionValidator()
    self.validate = self.validator.validate

  def _assert_no_exception(self, exceptions, message, func, *args, **kwargs):
    """Helper function to assert a function does not raise any of the specific exceptions"""
    try:
      func(*args, **kwargs)
    except exceptions:
      self.fail(message.format(func.__name__))

  def test_component_invalid_extra_code(self):
    component = deepcopy(self.valid_component)
    component['Component']['ExtraCode']['const_declaration'] = '// not even valid c++ passes here'
    with self.assertRaises(DefinitionError):
      self.validate(make_dm(component, {}), False)

    component = deepcopy(self.valid_component)
    component['Component']['ExtraCode']['const_implementation'] = '// it does not either here'
    with self.assertRaises(DefinitionError):
      self.validate(make_dm(component, {}), False)

  def test_component_invalid_member(self):
    # non-builin type
    component = deepcopy(self.valid_component)
    component['Component']['Members'].append(MemberVariable(type='NonBuiltinType', name='foo'))
    with self.assertRaises(DefinitionError):
      self.validate(make_dm(component, {}), False)

    # non-builtin array that is also not in another component
    component = deepcopy(self.valid_component)
    component['Component']['Members'].append(
        MemberVariable(array_type='NonBuiltinType', array_size=3, name='complexArray'))
    with self.assertRaises(DefinitionError):
      self.validate(make_dm(component, {}), False)

  def test_component_valid_members(self):
    self._assert_no_exception(DefinitionError, '{} should not raise for a valid component',
                              self.validate, make_dm(self.valid_component, {}), False)

    self.valid_component['SecondComponent'] = {
        'Members': [MemberVariable(array_type='Component', array_size='10', name='referToOtheComponent')]
        }
    self._assert_no_exception(DefinitionError, '{} should allow for component members in components',
                              self.validate, make_dm(self.valid_component, {}), False)

  def test_component_invalid_field(self):
    self.valid_component['Component']['Author'] = 'An invalid field for a component'
    with self.assertRaises(DefinitionError):
      self.validate(make_dm(self.valid_component, {}), False)

  def test_datatype_valid_members(self):
    self._assert_no_exception(DefinitionError, '{} should not raise for a valid datatype',
                              self.validate, make_dm({}, self.valid_datatype), False)

    # things should still work if we add a component member
    self.valid_datatype['DataType']['Members'].append(MemberVariable(type='Component', name='comp'))
    self._assert_no_exception(DefinitionError, '{} should allow for members that are components',
                              self.validate,
                              make_dm(self.valid_component, self.valid_datatype), False)

    # also when we add an array of components
    self.valid_datatype['DataType']['Members'].append(MemberVariable(array_type='Component',
                                                                     array_size='3',
                                                                     name='arrComp'))
    self._assert_no_exception(DefinitionError, '{} should allow for arrays of components as members',
                              self.validate,
                              make_dm(self.valid_component, self.valid_datatype), False)

    # pod members can be redefined if they are note exposed
    self.valid_datatype['DataType']['Members'].append(MemberVariable(type='double', name='aFloat'))
    self._assert_no_exception(DefinitionError,
                              '{} should allow for re-use of component names if the components are not exposed',
                              self.validate,
                              make_dm(self.valid_component, self.valid_datatype), False)

    datatype = {
        'DataTypeWithoutMembers': {
            'Author': 'Anonymous', 'Description': 'A pretty useless Datatype as it is'
            }
        }
    self._assert_no_exception(DefinitionError, '{} should allow for almost empty datatypes',
                              self.validate, make_dm({}, datatype), False)

  def test_datatype_invalid_definitions(self):
    for required in ('Author', 'Description'):
      datatype = deepcopy(self.valid_datatype)
      del datatype['DataType'][required]
      with self.assertRaises(DefinitionError):
        self.validate(make_dm({}, datatype), False)

    datatype = deepcopy(self.valid_datatype)
    datatype['DataType']['ExtraCode']['invalid_extracode'] = 'an invalid entry to the ExtraCode'
    with self.assertRaises(DefinitionError):
      self.validate(make_dm({}, datatype), False)

    datatype = deepcopy(self.valid_datatype)
    datatype['InvalidCategory'] = {'key': 'invalid value'}
    with self.assertRaises(DefinitionError):
      self.validate(make_dm({}, datatype), False)

  def test_datatype_invalid_members(self):
    datatype = deepcopy(self.valid_datatype)
    datatype['DataType']['Members'].append(MemberVariable(type='NonDeclaredType', name='foo'))
    with self.assertRaises(DefinitionError):
      self.validate(make_dm({}, datatype), False)

    datatype = deepcopy(self.valid_datatype)
    datatype['DataType']['Members'].append(MemberVariable(type='float', name='definedTwice'))
    datatype['DataType']['Members'].append(MemberVariable(type='int', name='definedTwice'))
    with self.assertRaises(DefinitionError):
      self.validate(make_dm({}, datatype), False)

    # Re-definition of a member present in a component and pod members are exposed
    datatype = deepcopy(self.valid_datatype)
    datatype['DataType']['Members'].append(MemberVariable(type='Component', name='aComponent'))
    datatype['DataType']['Members'].append(MemberVariable(type='float', name='aFloat'))
    with self.assertRaises(DefinitionError):
      self.validate(make_dm(self.valid_component, datatype), True)


  def _test_datatype_valid_relations(self, rel_type):
    self.valid_datatype['DataType'][rel_type] = [
        MemberVariable(type='DataType', name='selfRelation')
        ]
    self._assert_no_exception(DefinitionError,
                              '{} should allow for relations of datatypes to themselves',
                              self.validate, make_dm({}, self.valid_datatype), False)

    self.valid_datatype['BlackKnight'] = {
        'Author': 'John Cleese',
        'Description': 'Tis but a scratch',
        'Members': [MemberVariable(type='int', name='counter', description='number of arms')],
        rel_type: [MemberVariable(type='DataType', name='relation', description='soo many relations')]
        }

    self._assert_no_exception(DefinitionError, '{} should validate a valid relation',
                              self.validate, make_dm(self.valid_component, self.valid_datatype), False)

  def test_datatype_valid_many_relations(self):
    self._test_datatype_valid_relations('OneToManyRelations')

  def test_datatype_valid_single_relations(self):
    self._test_datatype_valid_relations('OneToOneRelations')

  def _test_datatype_invalid_relations(self, rel_type):
    datatype = deepcopy(self.valid_datatype)
    datatype['DataType'][rel_type] = [MemberVariable(type='NonExistentDataType',
                                                     name='aName')]
    with self.assertRaises(DefinitionError):
      self.validate(make_dm({}, datatype), False)

    datatype = deepcopy(self.valid_datatype)
    datatype['DataType'][rel_type] = [MemberVariable(type='Component',
                                                     name='componentRelation')]
    with self.assertRaises(DefinitionError):
      self.validate(make_dm(self.valid_component, datatype), False)

    datatype = deepcopy(self.valid_datatype)
    datatype['DataType'][rel_type] = [
        MemberVariable(array_type='int', array_size='42', name='arrayRelation')
        ]
    with self.assertRaises(DefinitionError):
      self.validate(make_dm({}, datatype), False)

  def test_datatype_invalid_many_relations(self):
    self._test_datatype_invalid_relations('OneToManyRelations')

  def test_datatype_invalid_single_relations(self):
    self._test_datatype_invalid_relations('OneToOneRelations')

  def test_datatype_valid_vector_members(self):
    self.valid_datatype['DataType']['VectorMembers'] = [MemberVariable(type='int', name='someInt')]
    self._assert_no_exception(DefinitionError,
                              '{} should validate builtin VectorMembers',
                              self.validate, make_dm({}, self.valid_datatype), False)

    self.valid_datatype['DataType']['VectorMembers'] = [MemberVariable(type='Component', name='components')]
    self._assert_no_exception(DefinitionError,
                              '{} should validate component VectorMembers',
                              self.validate, make_dm(self.valid_component, self.valid_datatype), False)

  def test_datatype_invalid_vector_members(self):
    datatype = deepcopy(self.valid_datatype)
    datatype['DataType']['VectorMembers'] = [MemberVariable(type='DataType', name='invalid')]
    with self.assertRaises(DefinitionError):
      self.validate(make_dm({}, datatype), False)

    datatype['Brian'] = {
        'Author': 'Graham Chapman',
        'Description': 'Not the messiah, a very naughty boy',
        'VectorMembers': [
            MemberVariable(type='DataType', name='invalid',
                           description='also non-self relations are not allowed')
            ]
        }
    with self.assertRaises(DefinitionError):
      self.validate(make_dm({}, datatype), False)

    datatype = deepcopy(self.valid_datatype)
    datatype['DataType']['VectorMembers'] = [
        MemberVariable(type='Component', name='component',
                       description='not working because component will not be part of the datamodel we pass')]
    with self.assertRaises(DefinitionError):
      self.validate(make_dm({}, datatype), False)


if __name__ == '__main__':
  unittest.main()

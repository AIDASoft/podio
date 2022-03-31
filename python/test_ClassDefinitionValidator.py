#!/usr/bin/env python

"""
Check some cases that should fail the class definition validator
"""

from __future__ import print_function, absolute_import, unicode_literals
import unittest
from copy import deepcopy

from podio_config_reader import ClassDefinitionValidator, MemberVariable, DefinitionError
from generator_utils import DataModel


def make_dm(components, datatypes, options=None):
  """Small helper function to turn things into a datamodel dict as expected by
  the validator"""
  return DataModel(datatypes, components, options)


class ClassDefinitionValidatorTest(unittest.TestCase):
  """Unit tests for the ClassDefinitionValidator"""
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
            'MutableExtraCode': {
                'declaration': 'also not checked for valid c++',
                'implementation': 'nothing has changed',
                'includes': '#include <something_else> this will appear in both includes!'
                }
            }
        }

    # The default options that should be used for validation
    self.def_opts = {'exposePODMembers': False}

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

    components = deepcopy(self.valid_component)
    components['SecondComponent'] = {
        'Members': [MemberVariable(array_type='Component', array_size='10', name='referToOtheComponent')]
        }
    self._assert_no_exception(DefinitionError, '{} should allow for component members in components',
                              self.validate, make_dm(components, {}), False)

  def test_component_invalid_field(self):
    self.valid_component['Component']['Author'] = 'An invalid field for a component'
    with self.assertRaises(DefinitionError):
      self.validate(make_dm(self.valid_component, {}), False)

  def test_datatype_valid_members(self):
    self._assert_no_exception(DefinitionError, '{} should not raise for a valid datatype',
                              self.validate, make_dm({}, self.valid_datatype, self.def_opts))

    # things should still work if we add a component member
    self.valid_datatype['DataType']['Members'].append(MemberVariable(type='Component', name='comp'))
    self._assert_no_exception(DefinitionError, '{} should allow for members that are components',
                              self.validate,
                              make_dm(self.valid_component, self.valid_datatype, self.def_opts))

    # also when we add an array of components
    self.valid_datatype['DataType']['Members'].append(MemberVariable(array_type='Component',
                                                                     array_size='3',
                                                                     name='arrComp'))
    self._assert_no_exception(DefinitionError, '{} should allow for arrays of components as members',
                              self.validate,
                              make_dm(self.valid_component, self.valid_datatype, self.def_opts))

    # pod members can be redefined if they are note exposed
    self.valid_datatype['DataType']['Members'].append(MemberVariable(type='double', name='aFloat'))
    self._assert_no_exception(DefinitionError,
                              '{} should allow for re-use of component names if the components are not exposed',
                              self.validate,
                              make_dm(self.valid_component, self.valid_datatype, self.def_opts))

    datatype = {
        'DataTypeWithoutMembers': {
            'Author': 'Anonymous', 'Description': 'A pretty useless Datatype as it is'
            }
        }
    self._assert_no_exception(DefinitionError, '{} should allow for almost empty datatypes',
                              self.validate, make_dm({}, datatype, self.def_opts))

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
      self.validate(make_dm({}, datatype, self.def_opts))

    datatype = deepcopy(self.valid_datatype)
    datatype['DataType']['Members'].append(MemberVariable(type='float', name='definedTwice'))
    datatype['DataType']['Members'].append(MemberVariable(type='int', name='definedTwice'))
    with self.assertRaises(DefinitionError):
      self.validate(make_dm({}, datatype, self.def_opts))

    # Re-definition of a member present in a component and pod members are exposed
    datatype = deepcopy(self.valid_datatype)
    datatype['DataType']['Members'].append(MemberVariable(type='Component', name='aComponent'))
    datatype['DataType']['Members'].append(MemberVariable(type='float', name='aFloat'))
    with self.assertRaises(DefinitionError):
      self.validate(make_dm(self.valid_component, datatype, {'exposePODMembers': True}))

    datatype = deepcopy(self.valid_datatype)
    datatype['AnotherType'] = {
        'Author': 'Avril L.',
        'Description': 'I\'m just a datatype',
        }
    datatype['DataType']['Members'].append(
        MemberVariable(type='AnotherType', name='impossibleType',
                       description='Another datatype cannot be a member'))
    with self.assertRaises(DefinitionError):
      self.validate(make_dm(self.valid_component, datatype, self.def_opts))

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

  def test_component_valid_upstream(self):
    """Test that a component from an upstream datamodel passes here"""
    component = {
        'DownstreamComponent': {
            'Members': [
                MemberVariable(type='Component', name='UpstreamComponent'),
                MemberVariable(array_type='Component', array_size='42', name='upstreamArray')
                ]
            }
        }

    upstream_dm = make_dm(self.valid_component, {})

    self._assert_no_exception(DefinitionError, '{} should allow to use upstream components in components',
                              self.validate, make_dm(component, {}, self.def_opts), upstream_dm)

  def test_component_invalid_upstream(self):
    """Test that a component does not pass if it is not available upstream or in the
    current definition"""
    # Valid non-array component, invalid array
    component = {
        'DownstreamComponent': {
            'Members': [
                MemberVariable(type='Component', name='UpstreamComponent'),
                MemberVariable(array_type='NotAvailComponent', array_size='42', name='upstreamArray')
                ]
            }
        }

    upstream_dm = make_dm(self.valid_component, {})

    with self.assertRaises(DefinitionError):
      self.validate(make_dm(component, {}, self.def_opts), upstream_dm)

    # invalid non-array component, valid array
    component = {
        'DownstreamComponent': {
            'Members': [
                MemberVariable(type='NotAvailComponent', name='UpstreamComponent'),
                MemberVariable(array_type='Component', array_size='42', name='upstreamArray')
                ]
            }
        }

    with self.assertRaises(DefinitionError):
      self.validate(make_dm(component, {}, self.def_opts), upstream_dm)

  def test_datatype_valid_upstream(self):
    """Test that a datatype from an upstream datamodel passes here"""
    datatype = {
        'DownstreamDatatype': {
            'Members': [
                MemberVariable(type='Component', name='UpstreamComponent', description='upstream component')
                ],
            'Description': 'A datatype with upstream components and relations',
            'Author': 'Sophisticated datamodel authors',
            'OneToOneRelations': [
                MemberVariable(type='DataType', name='upSingleRel', description='upstream single relation')
                ],
            'OneToManyRelations': [
                MemberVariable(type='DataType', name='upManyRel', description='upstream many relation')
                ],
            'VectorMembers': [
                MemberVariable(type='Component', name='upVector', description='upstream component as vector member')
                ]
            }
        }

    upstream_dm = make_dm(self.valid_component, self.valid_datatype, self.def_opts)
    self._assert_no_exception(DefinitionError, '{} should allow to use to use upstream datatypes and components',
                              self.validate, make_dm({}, datatype, self.def_opts), upstream_dm)

  def test_datatype_invalid_upstream(self):
    """Test that datatypes that are not from upstream cannot be used"""
    basetype = {
        'DsType': {
            'Author': 'Less sophisticated datamodel authors',
            'Description': 'A datatype trying to use non-existant upstream content'
            }
        }

    upstream_dm = make_dm(self.valid_component, self.valid_datatype, self.def_opts)

    # Check for invalid members
    dtype = deepcopy(basetype)
    dtype['DsType']['Members'] = [MemberVariable(type='InvalidType', name='foo', description='non existant upstream')]
    with self.assertRaises(DefinitionError):
      self.validate(make_dm({}, dtype, self.def_opts), upstream_dm)

    # Check relations
    dtype = deepcopy(basetype)
    dtype['DsType']['OneToOneRelations'] = [MemberVariable(type='InvalidType', name='foo', description='invalid')]
    with self.assertRaises(DefinitionError):
      self.validate(make_dm({}, dtype, self.def_opts), upstream_dm)

    dtype = deepcopy(basetype)
    dtype['DsType']['OneToManyRelations'] = [MemberVariable(type='InvalidType', name='foo', description='invalid')]
    with self.assertRaises(DefinitionError):
      self.validate(make_dm({}, dtype, self.def_opts), upstream_dm)

    # vector members
    dtype = deepcopy(basetype)
    dtype['DsType']['VectorMembers'] = [MemberVariable(type='InvalidType', name='foo', description='invalid')]
    with self.assertRaises(DefinitionError):
      self.validate(make_dm({}, dtype, self.def_opts), upstream_dm)


if __name__ == '__main__':
  unittest.main()

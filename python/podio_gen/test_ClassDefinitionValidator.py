#!/usr/bin/env python

"""
Check some cases that should fail the class definition validator
"""

from __future__ import print_function, absolute_import, unicode_literals
import unittest
from copy import deepcopy

from podio_gen.podio_config_reader import (
    ClassDefinitionValidator,
    MemberVariable,
    DefinitionError,
)
from podio_gen.generator_utils import DataModel, DataType


def make_dm(components, datatypes, interfaces=None, links=None, options=None):
    """Small helper function to turn things into a datamodel dict as expected by
    the validator"""
    return DataModel(datatypes, components, interfaces, links, options)


class ClassDefinitionValidatorTest(unittest.TestCase):  # pylint: disable=too-many-public-methods
    """Unit tests for the ClassDefinitionValidator"""

    def setUp(self):
        valid_component_members = [
            MemberVariable(type="int", name="anInt"),
            MemberVariable(type="float", name="aFloat"),
            MemberVariable(array_type="int", array_size="4", name="anArray"),
        ]

        self.valid_component = {
            "Component": {
                "Members": valid_component_members,
                "ExtraCode": {
                    "includes": '#include "someFancyHeader.h"',
                    "declaration": "we do not validate if this is valid c++",
                },
            }
        }

        valid_datatype_members = [
            MemberVariable(type="float", name="energy", description="energy [GeV]"),
            MemberVariable(
                array_type="int",
                array_size="5",
                name="anArray",
                description="some Array",
            ),
        ]

        self.valid_datatype = {
            "DataType": {
                "Author": "Mr. Bean",
                "Description": "I am merely here for a test",
                "Members": valid_datatype_members,
                "ExtraCode": {
                    "includes": "#include <whatever> not checking for valid c++",
                    "declaration": "not necessarily valid c++",
                    "implementation": "still not checked for c++ validity",
                },
                "MutableExtraCode": {
                    "declaration": "also not checked for valid c++",
                    "implementation": "nothing has changed",
                    "includes": "#include <something_else> this will appear in both includes!",
                },
            }
        }

        self.valid_interface = {
            "InterfaceType": {
                "Author": "Karma Chameleon",
                "Description": "I can be many things but only one at a time",
                "Members": valid_datatype_members,
                "Types": [DataType("DataType")],
            }
        }

        self.valid_link = {
            "LinkType": {
                "Author": "Princess Zelda",
                "Description": "Because we know our lore",
                "From": DataType("DataType"),
                "To": DataType("DataType"),
            }
        }

        # The default options that should be used for validation
        self.def_opts = {"exposePODMembers": False}

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
        component["Component"]["ExtraCode"][
            "const_declaration"
        ] = "// not even valid c++ passes here"
        with self.assertRaises(DefinitionError):
            self.validate(make_dm(component, {}), False)

        component = deepcopy(self.valid_component)
        component["Component"]["ExtraCode"]["const_implementation"] = "// it does not either here"
        with self.assertRaises(DefinitionError):
            self.validate(make_dm(component, {}), False)

    def test_component_invalid_member(self):
        # non-builtin type
        component = deepcopy(self.valid_component)
        component["Component"]["Members"].append(MemberVariable(type="NonBuiltinType", name="foo"))
        with self.assertRaises(DefinitionError):
            self.validate(make_dm(component, {}), False)

        # non-builtin array that is also not in another component
        component = deepcopy(self.valid_component)
        component["Component"]["Members"].append(
            MemberVariable(array_type="NonBuiltinType", array_size=3, name="complexArray")
        )
        with self.assertRaises(DefinitionError):
            self.validate(make_dm(component, {}), False)

    def test_component_valid_members(self):
        self._assert_no_exception(
            DefinitionError,
            "{} should not raise for a valid component",
            self.validate,
            make_dm(self.valid_component, {}),
            False,
        )

        components = deepcopy(self.valid_component)
        components["SecondComponent"] = {
            "Members": [
                MemberVariable(
                    array_type="Component", array_size="10", name="referToOtheComponent"
                )
            ]
        }
        self._assert_no_exception(
            DefinitionError,
            "{} should allow for component members in components",
            self.validate,
            make_dm(components, {}),
            False,
        )

    def test_datatype_valid_members(self):
        self._assert_no_exception(
            DefinitionError,
            "{} should not raise for a valid datatype",
            self.validate,
            make_dm({}, self.valid_datatype, options=self.def_opts),
        )

        # things should still work if we add a component member
        self.valid_datatype["DataType"]["Members"].append(
            MemberVariable(type="Component", name="comp")
        )
        self._assert_no_exception(
            DefinitionError,
            "{} should allow for members that are components",
            self.validate,
            make_dm(self.valid_component, self.valid_datatype, options=self.def_opts),
        )

        # also when we add an array of components
        self.valid_datatype["DataType"]["Members"].append(
            MemberVariable(array_type="Component", array_size="3", name="arrComp")
        )
        self._assert_no_exception(
            DefinitionError,
            "{} should allow for arrays of components as members",
            self.validate,
            make_dm(self.valid_component, self.valid_datatype, options=self.def_opts),
        )

        # pod members can be redefined if they are note exposed
        self.valid_datatype["DataType"]["Members"].append(
            MemberVariable(type="double", name="aFloat")
        )
        self._assert_no_exception(
            DefinitionError,
            "{} should allow for reuse of component names if the components are not exposed",
            self.validate,
            make_dm(self.valid_component, self.valid_datatype, options=self.def_opts),
        )

        datatype = {
            "DataTypeWithoutMembers": {
                "Author": "Anonymous",
                "Description": "A pretty useless Datatype as it is",
            }
        }
        self._assert_no_exception(
            DefinitionError,
            "{} should allow for almost empty datatypes",
            self.validate,
            make_dm({}, datatype, options=self.def_opts),
        )

    def test_datatype_invalid_definitions(self):
        for required in ("Author", "Description"):
            datatype = deepcopy(self.valid_datatype)
            del datatype["DataType"][required]
            with self.assertRaises(DefinitionError):
                self.validate(make_dm({}, datatype), False)

        datatype = deepcopy(self.valid_datatype)
        datatype["DataType"]["ExtraCode"][
            "invalid_extracode"
        ] = "an invalid entry to the ExtraCode"
        with self.assertRaises(DefinitionError):
            self.validate(make_dm({}, datatype), False)

        datatype = deepcopy(self.valid_datatype)
        datatype["InvalidCategory"] = {"key": "invalid value"}
        with self.assertRaises(DefinitionError):
            self.validate(make_dm({}, datatype), False)

    def test_datatype_invalid_members(self):
        datatype = deepcopy(self.valid_datatype)
        datatype["DataType"]["Members"].append(MemberVariable(type="NonDeclaredType", name="foo"))
        with self.assertRaises(DefinitionError):
            self.validate(make_dm({}, datatype, self.def_opts))

        datatype = deepcopy(self.valid_datatype)
        datatype["DataType"]["Members"].append(MemberVariable(type="float", name="definedTwice"))
        datatype["DataType"]["Members"].append(MemberVariable(type="int", name="definedTwice"))
        with self.assertRaises(DefinitionError):
            self.validate(make_dm({}, datatype, self.def_opts))

        # Re-definition of a member present in a component and pod members are exposed
        datatype = deepcopy(self.valid_datatype)
        datatype["DataType"]["Members"].append(MemberVariable(type="Component", name="aComponent"))
        datatype["DataType"]["Members"].append(MemberVariable(type="float", name="aFloat"))
        with self.assertRaises(DefinitionError):
            self.validate(make_dm(self.valid_component, datatype, {"exposePODMembers": True}))

        datatype = deepcopy(self.valid_datatype)
        datatype["AnotherType"] = {
            "Author": "Avril L.",
            "Description": "I'm just a datatype",
        }
        datatype["DataType"]["Members"].append(
            MemberVariable(
                type="AnotherType",
                name="impossibleType",
                description="Another datatype cannot be a member",
            )
        )
        with self.assertRaises(DefinitionError):
            self.validate(make_dm(self.valid_component, datatype, self.def_opts))

    def _test_datatype_valid_relations(self, rel_type):
        self.valid_datatype["DataType"][rel_type] = [
            MemberVariable(type="DataType", name="selfRelation")
        ]
        self._assert_no_exception(
            DefinitionError,
            "{} should allow for relations of datatypes to themselves",
            self.validate,
            make_dm({}, self.valid_datatype),
            False,
        )

        self.valid_datatype["BlackKnight"] = {
            "Author": "John Cleese",
            "Description": "Tis but a scratch",
            "Members": [MemberVariable(type="int", name="counter", description="number of arms")],
            rel_type: [
                MemberVariable(type="DataType", name="relation", description="soo many relations")
            ],
        }

        self._assert_no_exception(
            DefinitionError,
            "{} should validate a valid relation",
            self.validate,
            make_dm(self.valid_component, self.valid_datatype),
            False,
        )

    def test_datatype_valid_many_relations(self):
        self._test_datatype_valid_relations("OneToManyRelations")

    def test_datatype_valid_single_relations(self):
        self._test_datatype_valid_relations("OneToOneRelations")

    def _test_datatype_invalid_relations(self, rel_type):
        datatype = deepcopy(self.valid_datatype)
        datatype["DataType"][rel_type] = [MemberVariable(type="NonExistentDataType", name="aName")]
        with self.assertRaises(DefinitionError):
            self.validate(make_dm({}, datatype), False)

        datatype = deepcopy(self.valid_datatype)
        datatype["DataType"][rel_type] = [
            MemberVariable(type="Component", name="componentRelation")
        ]
        with self.assertRaises(DefinitionError):
            self.validate(make_dm(self.valid_component, datatype), False)

        datatype = deepcopy(self.valid_datatype)
        datatype["DataType"][rel_type] = [
            MemberVariable(array_type="int", array_size="42", name="arrayRelation")
        ]
        with self.assertRaises(DefinitionError):
            self.validate(make_dm({}, datatype), False)

    def test_datatype_invalid_many_relations(self):
        self._test_datatype_invalid_relations("OneToManyRelations")

    def test_datatype_invalid_single_relations(self):
        self._test_datatype_invalid_relations("OneToOneRelations")

    def test_datatype_valid_vector_members(self):
        self.valid_datatype["DataType"]["VectorMembers"] = [
            MemberVariable(type="int", name="someInt")
        ]
        self._assert_no_exception(
            DefinitionError,
            "{} should validate builtin VectorMembers",
            self.validate,
            make_dm({}, self.valid_datatype),
            False,
        )

        self.valid_datatype["DataType"]["VectorMembers"] = [
            MemberVariable(type="Component", name="components")
        ]
        self._assert_no_exception(
            DefinitionError,
            "{} should validate component VectorMembers",
            self.validate,
            make_dm(self.valid_component, self.valid_datatype),
            False,
        )

    def test_datatype_invalid_vector_members(self):
        datatype = deepcopy(self.valid_datatype)
        datatype["DataType"]["VectorMembers"] = [MemberVariable(type="DataType", name="invalid")]
        with self.assertRaises(DefinitionError):
            self.validate(make_dm({}, datatype), False)

        datatype["Brian"] = {
            "Author": "Graham Chapman",
            "Description": "Not the messiah, a very naughty boy",
            "VectorMembers": [
                MemberVariable(
                    type="DataType",
                    name="invalid",
                    description="also non-self relations are not allowed",
                )
            ],
        }
        with self.assertRaises(DefinitionError):
            self.validate(make_dm({}, datatype), False)

        datatype = deepcopy(self.valid_datatype)
        datatype["DataType"]["VectorMembers"] = [
            MemberVariable(
                type="Component",
                name="component",
                description="not working because component is not part of the datamodel we pass",
            )
        ]
        with self.assertRaises(DefinitionError):
            self.validate(make_dm({}, datatype), False)

    def test_component_valid_upstream(self):
        """Test that a component from an upstream datamodel passes here"""
        component = {
            "DownstreamComponent": {
                "Members": [
                    MemberVariable(type="Component", name="UpstreamComponent"),
                    MemberVariable(array_type="Component", array_size="42", name="upstreamArray"),
                ]
            }
        }

        upstream_dm = make_dm(self.valid_component, {})

        self._assert_no_exception(
            DefinitionError,
            "{} should allow to use upstream components in components",
            self.validate,
            make_dm(component, {}, options=self.def_opts),
            upstream_dm,
        )

    def test_component_invalid_upstream(self):
        """Test that a component does not pass if it is not available upstream or in the
        current definition"""
        # Valid non-array component, invalid array
        component = {
            "DownstreamComponent": {
                "Members": [
                    MemberVariable(type="Component", name="UpstreamComponent"),
                    MemberVariable(
                        array_type="NotAvailComponent",
                        array_size="42",
                        name="upstreamArray",
                    ),
                ]
            }
        }

        upstream_dm = make_dm(self.valid_component, {})

        with self.assertRaises(DefinitionError):
            self.validate(make_dm(component, {}, self.def_opts), upstream_dm)

        # invalid non-array component, valid array
        component = {
            "DownstreamComponent": {
                "Members": [
                    MemberVariable(type="NotAvailComponent", name="UpstreamComponent"),
                    MemberVariable(array_type="Component", array_size="42", name="upstreamArray"),
                ]
            }
        }

        with self.assertRaises(DefinitionError):
            self.validate(make_dm(component, {}, self.def_opts), upstream_dm)

    def test_datatype_valid_upstream(self):
        """Test that a datatype from an upstream datamodel passes here"""
        datatype = {
            "DownstreamDatatype": {
                "Members": [
                    MemberVariable(
                        type="Component",
                        name="UpstreamComponent",
                        description="upstream component",
                    )
                ],
                "Description": "A datatype with upstream components and relations",
                "Author": "Sophisticated datamodel authors",
                "OneToOneRelations": [
                    MemberVariable(
                        type="DataType",
                        name="upSingleRel",
                        description="upstream single relation",
                    )
                ],
                "OneToManyRelations": [
                    MemberVariable(
                        type="DataType",
                        name="upManyRel",
                        description="upstream many relation",
                    )
                ],
                "VectorMembers": [
                    MemberVariable(
                        type="Component",
                        name="upVector",
                        description="upstream component as vector member",
                    )
                ],
            }
        }

        upstream_dm = make_dm(self.valid_component, self.valid_datatype, self.def_opts)
        self._assert_no_exception(
            DefinitionError,
            "{} should allow to use to use upstream datatypes and components",
            self.validate,
            make_dm({}, datatype, {}, options=self.def_opts),
            upstream_dm,
        )

    def test_datatype_invalid_upstream(self):
        """Test that datatypes that are not from upstream cannot be used"""
        basetype = {
            "DsType": {
                "Author": "Less sophisticated datamodel authors",
                "Description": "A datatype trying to use non-existant upstream content",
            }
        }

        upstream_dm = make_dm(self.valid_component, self.valid_datatype, {}, self.def_opts)

        # Check for invalid members
        dtype = deepcopy(basetype)
        dtype["DsType"]["Members"] = [
            MemberVariable(type="InvalidType", name="foo", description="non existant upstream")
        ]
        with self.assertRaises(DefinitionError):
            self.validate(make_dm({}, dtype, options=self.def_opts), upstream_dm)

        # Check relations
        dtype = deepcopy(basetype)
        dtype["DsType"]["OneToOneRelations"] = [
            MemberVariable(type="InvalidType", name="foo", description="invalid")
        ]
        with self.assertRaises(DefinitionError):
            self.validate(make_dm({}, dtype, options=self.def_opts), upstream_dm)

        dtype = deepcopy(basetype)
        dtype["DsType"]["OneToManyRelations"] = [
            MemberVariable(type="InvalidType", name="foo", description="invalid")
        ]
        with self.assertRaises(DefinitionError):
            self.validate(make_dm({}, dtype, options=self.def_opts), upstream_dm)

        # vector members
        dtype = deepcopy(basetype)
        dtype["DsType"]["VectorMembers"] = [
            MemberVariable(type="InvalidType", name="foo", description="invalid")
        ]
        with self.assertRaises(DefinitionError):
            self.validate(make_dm({}, dtype, options=self.def_opts), upstream_dm)

    def test_interface_valid_def(self):
        """Make sure that a valid interface definition inside a valid datamodel
        passes without exceptions"""
        self._assert_no_exception(
            DefinitionError,
            "{} should not raise for a valid interface type",
            self.validate,
            make_dm({}, self.valid_datatype, self.valid_interface),
            False,
        )

    def test_interface_invalid_fields(self):
        """Make sure that interface definitions do not contain any superfluous fields"""
        for inv_field in ["OneToManyRelations", "VectorMembers", "OneToOneRelations"]:
            interface = deepcopy(self.valid_interface)
            interface["InterfaceType"][inv_field] = ["An invalid field"]
            with self.assertRaises(DefinitionError):
                self.validate(make_dm({}, self.valid_datatype, interface))

    def test_interface_missing_fields(self):
        """Make sure that interfaces have all the required types when they pass validation"""
        for req in ("Author", "Description", "Members", "Types"):
            int_type = deepcopy(self.valid_interface)
            del int_type["InterfaceType"][req]
            with self.assertRaises(DefinitionError):
                self.validate(make_dm({}, self.valid_datatype, int_type), False)

    def test_interface_only_defined_datatypes(self):
        """Make sure that the interface only uses defined datatypes"""
        int_type = deepcopy(self.valid_interface)
        int_type["InterfaceType"]["Types"].append(DataType("UndefinedType"))
        with self.assertRaises(DefinitionError):
            self.validate(make_dm({}, self.valid_datatype, int_type), False)

        int_type = deepcopy(self.valid_interface)
        int_type["InterfaceType"]["Types"].append(DataType("Component"))

        int_type = deepcopy(self.valid_interface)
        int_type["InterfaceType"]["Types"].append(DataType("float"))
        with self.assertRaises(DefinitionError):
            self.validate(make_dm({}, self.valid_datatype, int_type), False)

    def test_interface_no_redefining_datatype(self):
        """Make sure that there is no datatype already with the same name"""
        int_type = {
            "DataType": {
                "Author": "Copycat",
                "Description": "I shall not redefine datatypes as interfaces",
                "Members": [],
                "Types": [],
            }
        }
        with self.assertRaises(DefinitionError):
            self.validate(make_dm({}, self.valid_datatype, int_type), False)

    def test_datatype_uses_interface_type(self):
        """Make sure that a data type can use a valid interface definition"""
        datatype = deepcopy(self.valid_datatype)
        datatype["DataType"]["OneToManyRelations"] = [
            MemberVariable(type="InterfaceType", name="interfaceRelation")
        ]
        self._assert_no_exception(
            DefinitionError,
            "{} should allow to use relations to interface types",
            self.validate,
            make_dm({}, datatype, self.valid_interface),
            False,
        )

    def test_interface_valid_upstream(self):
        """Make sure that we can use interface definitions from upstream models"""
        # Create an upstream datamodel that contains the interface type
        upstream_dm = make_dm({}, self.valid_datatype, self.valid_interface)

        # Make a downstream model datatype that uses the interface from the upstream
        # but doesn't bring along its own interface definitions
        datatype = {"DownstreamType": deepcopy(self.valid_datatype["DataType"])}
        datatype["DownstreamType"]["OneToOneRelations"] = [
            MemberVariable(type="InterfaceType", name="interfaceRelation")
        ]
        self._assert_no_exception(
            DefinitionError,
            "{} should allow to use interface types from an upstream datamodel",
            self.validate,
            make_dm({}, datatype),
            upstream_dm,
        )

    def test_link_valid_def(self):
        """Make sure that a valid link definition inside a valid datamodel
        passes"""
        self._assert_no_exception(
            DefinitionError,
            "{} should not raise for a valid link type",
            self.validate,
            make_dm({}, self.valid_datatype, links=self.valid_link),
        )

        links = deepcopy(self.valid_link)
        links["LinkType"]["From"] = DataType("InterfaceType")
        self._assert_no_exception(
            DefinitionError,
            "{} should not raise for a valid link type",
            self.validate,
            make_dm({}, self.valid_datatype, self.valid_interface, links),
        )

    def test_link_invalid_fields(self):
        """Make sure that link definitions cannot contain any superfluous fields"""
        for inv_field in ("Members", "OneToOneRelations", "OneToManyRelations", "VectorMembers"):
            link = deepcopy(self.valid_link)
            link["LinkType"][inv_field] = ["A value that does not matter"]
            with self.assertRaises(DefinitionError):
                self.validate(make_dm({}, self.valid_datatype, links=link))

    def test_link_missing_fields(self):
        """Make sure that links need to have all the required fields"""
        for req in ClassDefinitionValidator.required_link_keys:
            link_type = deepcopy(self.valid_link)
            del link_type["LinkType"][req]
            with self.assertRaises(DefinitionError):
                self.validate(make_dm({}, self.valid_datatype, links=link_type))

    def test_link_only_defined_datatypes(self):
        """Make sure links can only use defined datatypes"""
        link_type = deepcopy(self.valid_link)
        link_type["LinkType"]["From"] = DataType("NonExistantType")
        with self.assertRaises(DefinitionError):
            self.validate(make_dm({}, self.valid_datatype, links=link_type))

        link_type = deepcopy(self.valid_link)
        link_type["LinkType"]["To"] = DataType("NonExistantType")
        with self.assertRaises(DefinitionError):
            self.validate(make_dm({}, self.valid_datatype, links=link_type))

    def test_link_no_redefining_datatypes(self):
        """Make sure that a link cannot shadow / redeclare an existing datatype"""
        link_type = {
            "DataType": {
                "Author": "T.B. Lee",
                "Description": "Redefining datatypes is bad",
                "From": DataType("DataType"),
                "To": DataType("DataType"),
            }
        }
        with self.assertRaises(DefinitionError):
            self.validate(make_dm({}, self.valid_datatype, links=link_type))

        # We can also not redefine interface types
        link_type["InterfaceType"] = link_type.pop("DataType")
        with self.assertRaises(DefinitionError):
            self.validate(make_dm({}, self.valid_datatype, self.valid_interface, link_type))

    def test_link_valid_upstream(self):
        """Check that links can use upstream datatypes"""
        upstream_dm = make_dm({}, self.valid_datatype, self.valid_interface)

        link_type = {
            "Century": {
                "Author": "Link",
                "Description": "Redefining datatypes is bad",
                "From": DataType("DataType"),
                "To": DataType("InterfaceType"),
            }
        }
        self._assert_no_exception(
            DefinitionError,
            "{} links should be able to use upstream datatypes",
            self.validate,
            make_dm({}, {}, links=link_type),
            upstream_dm,
        )


if __name__ == "__main__":
    unittest.main()

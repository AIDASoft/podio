#!/usr/bin/env python3
"""cppyy python binding tests using opt-in pythonizations"""

import unittest
from ROOT import ex2
from pythonizations import load_pythonizations  # pylint: disable=import-error

# load all available pythonizations to the classes in a namespace
# loading pythonizations changes the state of cppyy backend shared by all the tests in a process
# NOTE: We need to load the versioned namespace here for components as the
# typedef we have doe s not cover it
load_pythonizations("ex2::v2")


class AttributeCreationTest(unittest.TestCase):
    """Setting new attributes test should be disabled by freeze class pythonization"""

    def test_disable_new_attribute_creation(self):
        component = ex2.NamespaceStruct()
        self.assertEqual(component.x, 0)
        component.x = 1
        self.assertEqual(component.x, 1)
        with self.assertRaises(AttributeError):
            component.not_existing_attribute = 0

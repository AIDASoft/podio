#!/usr/bin/env python3
"""cppyy python binding tests"""

import unittest
import ROOT
from ROOT import ExampleMCCollection, MutableExampleMC
from ROOT import nsp
from pythonizations import load_pythonizations  # pylint: disable=import-error

# load all available pythonizations to the classes in a namespace
# loading pythonizations changes the state of cppyy backend shared by all the tests in a process
load_pythonizations("nsp")


class ObjectConversionsTest(unittest.TestCase):
    """Object conversion binding tests"""

    def test_conversion_mutable_to_immutable(self):
        ROOT.gInterpreter.Declare(
            """
            void test_accepts_immutable(ExampleMC) {}
            """
        )
        accepts_immutable = ROOT.test_accepts_immutable
        mutable_obj = MutableExampleMC()
        accepts_immutable(mutable_obj)


class OneToManyRelationsTest(unittest.TestCase):
    """OneToManyRelations binding tests"""

    def test_add(self):
        particles = ExampleMCCollection()
        parent_particle = particles.create()
        daughter_particle = particles.create()

        self.assertEqual(len(daughter_particle.parents()), 0)
        daughter_particle.addparents(parent_particle)
        self.assertEqual(len(daughter_particle.parents()), 1)


class CollectionSubscriptTest(unittest.TestCase):
    """Collection subscript test"""

    def test_bound_check(self):
        collection = nsp.EnergyInNamespaceCollection()
        _ = collection.create()
        self.assertEqual(len(collection), 1)
        with self.assertRaises(IndexError):
            _ = collection[20]
        _ = collection[0]


class AttributeCreationTest(unittest.TestCase):
    """Setting new attributes test"""

    def test_disable_new_attribute_creation(self):
        component = nsp.AnotherNamespaceStruct()
        self.assertEqual(component.x, 0)
        component.x = 1
        self.assertEqual(component.x, 1)
        with self.assertRaises(AttributeError):
            component.not_existing_attribute = 0

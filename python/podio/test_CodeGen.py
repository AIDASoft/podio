#!/usr/bin/env python3
"""cppyy python binding tests with pythonizations"""

import unittest
import ROOT
import cppyy
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
        """Test that bound checking for collection subscript is enabled"""
        collection = nsp.EnergyInNamespaceCollection()
        _ = collection.create()
        self.assertEqual(len(collection), 1)
        self.assertRaises(IndexError, lambda: collection[20])
        _ = collection[0]

    def test_getitem_return_type(self):
        """Test that collection subscript returns an instance of podio immutable datatype"""
        collection = nsp.EnergyInNamespaceCollection()
        obj = collection.create()
        self.assertIsInstance(obj, nsp.MutableEnergyInNamespace)
        self.assertIsInstance(collection[0], nsp.EnergyInNamespace)


class AttributeCreationTest(unittest.TestCase):
    """Setting new attributes test"""

    def test_disable_new_attribute_creation(self):
        component = nsp.AnotherNamespaceStruct()
        self.assertEqual(component.x, 0)
        component.x = 1
        self.assertEqual(component.x, 1)
        with self.assertRaises(AttributeError):
            component.not_existing_attribute = 0


class HashTest(unittest.TestCase):
    """Hash object test"""

    def test_hash(self):
        collection = nsp.EnergyInNamespaceCollection()
        obj1 = collection.create()
        hash1 = hash(obj1)
        # same object same hash
        self.assertEqual(hash(obj1), hash1)
        obj2 = obj1
        # same object same hash
        self.assertEqual(obj2, obj1)
        self.assertEqual(hash(obj2), hash(obj1))
        # different objects different hash
        obj_different = collection.create()
        self.assertNotEqual(obj_different, obj1)
        self.assertNotEqual(hash(obj_different), hash(obj1))
        # changing the object does not change the hash
        obj1.energy = 3.14
        self.assertEqual(hash(obj1), hash1)
        # mutable and immutable objects have same hashes
        obj_immutable = nsp.EnergyInNamespace(obj1)
        self.assertEqual(obj_immutable, obj1)
        self.assertEqual(hash(obj_immutable), hash(obj1))
        # python hash is same as cpp hash
        self.assertEqual(hash(obj1), cppyy.gbl.std.hash[nsp.EnergyInNamespace]()(obj1))

    def test_sets_and_dicts(self):
        collection = nsp.EnergyInNamespaceCollection()
        obj1 = collection.create()
        obj2 = collection.create()
        obj3 = collection.create()
        # test set
        test_set = {obj1, obj2, obj3, obj3, obj3}
        self.assertEqual(len(test_set), 3)
        self.assertIn(obj1, test_set)
        self.assertIn(obj2, test_set)
        self.assertIn(obj3, test_set)
        # test dictionary
        test_dict = {obj1: 1, obj2: 2, obj3: 3}
        self.assertEqual(len(test_dict), 3)
        self.assertEqual(test_dict[obj1], 1)
        self.assertEqual(test_dict[obj2], 2)
        self.assertEqual(test_dict[obj3], 3)

#!/usr/bin/env python3
"""cppyy python binding tests"""

import unittest
import ROOT
from ROOT import ExampleMCCollection, MutableExampleMC


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

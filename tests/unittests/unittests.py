#!/usr/bin/env python3
"""cppyy python binding tests"""

import unittest
import ROOT

res = ROOT.gSystem.Load("libTestDataModel.so")
if res < 0:
    raise RuntimeError("Failed to load libTestDataModel.so")

from ROOT import MutableExampleMC  # pylint: disable=wrong-import-position # noqa: E402
from ROOT import ExampleMCCollection  # pylint: disable=wrong-import-position # noqa: E402


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


if __name__ == "__main__":
    unittest.main()

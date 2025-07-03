#!/usr/bin/env python3
"""cppyy python binding tests without pythonizations"""

import unittest

from ROOT import ExampleHitCollection, ExampleHit, nsp


class TestBracketOperator(unittest.TestCase):
    """Check that operator[] returns a non-mutable object"""

    def test_bracket_operator(self):
        coll = ExampleHitCollection()
        coll.create()
        self.assertEqual(type(coll[0]), type(ExampleHit()))
        coll = nsp.EnergyInNamespaceCollection()
        coll.create()
        self.assertEqual(type(coll[0]), type(nsp.EnergyInNamespace()))

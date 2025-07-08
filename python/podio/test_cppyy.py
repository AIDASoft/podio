#!/usr/bin/env python3
"""cppyy python binding tests without pythonizations"""

import unittest
import cppyy

from ROOT import ExampleHitCollection, ExampleHit, nsp


class TestBracketOperator(unittest.TestCase):
    """Check that operator[] returns a non-mutable object
    and raises an exception (from cppyy) for out-of-range access."""

    def test_bracket_operator(self):
        coll = ExampleHitCollection()
        coll.create()
        self.assertEqual(type(coll[0]), type(ExampleHit()))
        self.assertRaises(cppyy.gbl.std.out_of_range, lambda: coll[1000])
        coll = nsp.EnergyInNamespaceCollection()
        coll.create()
        self.assertEqual(type(coll[0]), type(nsp.EnergyInNamespace()))

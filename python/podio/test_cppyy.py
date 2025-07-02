#!/usr/bin/env python3

import unittest

from ROOT import ExampleHitCollection, ExampleHit
import podio

class TestBracketOperator(unittest.TestCase):
    """Check that operator[] returns a non-mutable object"""

    def test_bracket_operator(self):
        coll = ExampleHitCollection()
        coll.create()
        frame = podio.Frame()
        frame.put(coll, "hits_from_python")
        coll = frame.get("hits_from_python")
        self.assertEqual(type(coll[0]), type(ExampleHit()))

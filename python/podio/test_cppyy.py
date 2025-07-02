#!/usr/bin/env python3

import unittest

from ROOT import ExampleHitCollection, ExampleHit


class TestBracketOperator(unittest.TestCase):
    """Check that operator[] returns a non-mutable object"""

    def test_bracket_operator(self):
        coll = ExampleHitCollection()
        coll.create()
        self.assertEqual(type(coll[0]), type(ExampleHit()))

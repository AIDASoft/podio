#!/usr/bin/env python3
"""Unittests for python version module"""

import unittest

from podio import version
from podio import __version__


class TestVersion(unittest.TestCase):
    """General unittests for the python bindings of Version"""

    def test_parse(self):
        """Make sure that parse works as expected"""
        vv = version.parse(1, 2, 3)
        self.assertEqual(vv.major, 1)
        self.assertEqual(vv.minor, 2)
        self.assertEqual(vv.patch, 3)

        vv = version.parse(0, 2)
        self.assertEqual(vv.major, 0)
        self.assertEqual(vv.minor, 2)
        self.assertEqual(vv.patch, 0)

        vv = version.parse("42.0")
        self.assertEqual(vv.major, 42)
        self.assertEqual(vv.minor, 0)
        self.assertEqual(vv.patch, 0)

        vv = version.Version()
        self.assertEqual(vv.major, 0)
        self.assertEqual(vv.minor, 0)
        self.assertEqual(vv.patch, 0)

    def test_build_version(self):
        """Make sure that the build version is set consistently (i.e. configured
        correctly)"""
        self.assertEqual(version.build_version, version.parse(__version__))

    def test_version_comparison(self):
        """Make sure that version comparisons work"""
        v1 = version.parse(1, 2, 3)
        v2 = version.parse("0.4.2")
        self.assertTrue(v1 > v2)

        v3 = version.parse("1.2.3")
        self.assertEqual(v3, v1)

    def test_string_representation(self):
        """Make sure the string representation is OK"""
        self.assertEqual(f"{version.build_version}", __version__)

        vv = version.parse(42)
        self.assertEqual(f"{vv}", "42.0.0")

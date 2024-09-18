#!/usr/bin/env python3
"""Unittests for python version module"""

import unittest

from podio import version
from podio import __version__


class TestVersion(unittest.TestCase):
    """General unittests for the python bindings of Version"""

    def test_build_version(self):
        """Make sure that the build version is set consistently (i.e. configured
        correctly)"""
        self.assertEqual(version.build_version, version.Version(__version__))

    def test_version_comparison(self):
        """Make sure that version comparisons work"""
        v1 = version.Version(1, 2, 3)
        v2 = version.Version("0.4.2")
        self.assertTrue(v1 > v2)

        v3 = version.Version("1.2.3")
        self.assertEqual(v3, v1)

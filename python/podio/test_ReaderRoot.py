#!/usr/bin/env python3
"""Python unit tests for the ROOT backend (using Frames)"""

import unittest

# pylint: disable-next=import-error
from test_Reader import (
    ReaderTestCaseMixin,
    LegacyReaderTestCaseMixin,
)
from podio.test_utils import get_legacy_input

from podio.root_io import Reader, LegacyReader


class RootReaderTestCase(ReaderTestCaseMixin, unittest.TestCase):
    """Test cases for root input files"""

    def setUp(self):
        """Setup the corresponding reader"""
        self.reader = Reader("root_io/example_frame.root")


class RootLegacyReaderTestCase(LegacyReaderTestCaseMixin, unittest.TestCase):
    """Test cases for the legacy root input files and reader."""

    def setUp(self):
        """Setup a reader, reading from the example files"""
        self.reader = LegacyReader(get_legacy_input("v00-16-06-example.root"))

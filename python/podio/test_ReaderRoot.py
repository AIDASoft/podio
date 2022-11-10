#!/usr/bin/env python3
"""Python unit tests for the ROOT backend (using Frames)"""

import unittest

from podio.root_io import Reader, LegacyReader
from podio.test_Reader import ReaderTestCaseMixin, LegacyReaderTestCaseMixin


class RootReaderTestCase(ReaderTestCaseMixin, unittest.TestCase):
  """Test cases for root input files"""
  def setUp(self):
    """Setup the corresponding reader"""
    self.reader = Reader('example_frame.root')


class RootLegacyReaderTestCase(LegacyReaderTestCaseMixin, unittest.TestCase):
  """Test cases for the legacy root input files and reader."""
  def setUp(self):
    """Setup a reader, reading from the example files"""
    self.reader = LegacyReader('example.root')

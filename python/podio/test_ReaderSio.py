#!/usr/bin/env python3
"""Python unit tests for the SIO backend (using Frames)"""

import unittest

try:
  from podio.sio_io import Reader, LegacyReader
except ImportError:
  print('Unable to load SIO for this unit test, aborting the test...')
from podio.test_Reader import ReaderTestCaseMixin, LegacyReaderTestCaseMixin
from podio.test_utils import SKIP_SIO_TESTS


@unittest.skipIf(SKIP_SIO_TESTS, "no SIO support")
class SioReaderTestCase(ReaderTestCaseMixin, unittest.TestCase):
  """Test cases for root input files"""
  def setUp(self):
    """Setup the corresponding reader"""
    self.reader = Reader('example_frame.sio')


@unittest.skipIf(SKIP_SIO_TESTS, "no SIO support")
class SIOLegacyReaderTestCase(LegacyReaderTestCaseMixin, unittest.TestCase):
  """Test cases for the legacy root input files and reader."""
  def setUp(self):
    """Setup a reader, reading from the example files"""
    self.reader = LegacyReader('example.sio')

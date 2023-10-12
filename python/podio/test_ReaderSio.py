#!/usr/bin/env python3
"""Python unit tests for the SIO backend (using Frames)"""

import os
import unittest

from test_Reader import ReaderTestCaseMixin, LegacyReaderTestCaseMixin  # pylint: disable=import-error
from test_utils import SKIP_SIO_TESTS  # pylint: disable=import-error


@unittest.skipIf(SKIP_SIO_TESTS, "no SIO support")
class SioReaderTestCase(ReaderTestCaseMixin, unittest.TestCase):
  """Test cases for root input files"""
  def setUp(self):
    """Setup the corresponding reader"""
    from podio.sio_io import Reader  # pylint: disable=import-outside-toplevel
    self.reader = Reader('sio_io/example_frame.sio')


@unittest.skipIf(SKIP_SIO_TESTS, "no SIO support")
class SIOLegacyReaderTestCase(LegacyReaderTestCaseMixin, unittest.TestCase):
  """Test cases for the legacy root input files and reader."""
  def setUp(self):
    """Setup a reader, reading from the example files"""
    from podio.sio_io import LegacyReader  # pylint: disable=import-outside-toplevel
    self.reader = LegacyReader(os.path.join(os.environ["PODIO_TEST_INPUT_DATA_DIR"], "v00-16-06", "example.sio"))

#!/usr/bin/env python3
"""Python unit tests for the SIO backend (using Frames)"""

import unittest

from podio.sio_io import Reader
from podio.test_Reader import ReaderTestCaseMixin
from podio.test_utils import SKIP_SIO_TESTS


@unittest.skipIf(SKIP_SIO_TESTS, "no SIO support")
class SioReaderTestCase(ReaderTestCaseMixin, unittest.TestCase):
  """Test cases for root input files"""
  def setUp(self):
    """Setup the corresponding reader"""
    self.reader = Reader('example_frame.sio')

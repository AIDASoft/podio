#!/usr/bin/env python3
"""Python unit tests for the ROOT backend (using Frames)"""

import unittest

from podio.root_io import Reader
from podio.test_Reader import ReaderTestCaseMixin


class RootReaderTestCase(ReaderTestCaseMixin, unittest.TestCase):
  """Test cases for root input files"""
  def setUp(self):
    """Setup the corresponding reader"""
    self.reader = Reader('example_frame.root')

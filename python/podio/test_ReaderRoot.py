#!/usr/bin/env python3
"""Python unit tests for the ROOT backend (using Frames)"""

import unittest

from podio.root_io import Reader, LegacyReader
from podio.test_Reader import ReaderTestCaseMixin


class RootReaderTestCase(ReaderTestCaseMixin, unittest.TestCase):
  """Test cases for root input files"""
  def setUp(self):
    """Setup the corresponding reader"""
    self.reader = Reader('example_frame.root')


class RootLegacyReaderTestCase(unittest.TestCase):
  """Test cases for the legacy root input files and reader.

  NOTE: Since also the LegacyReader uses the BaseReaderMixin many invalid access
  test cases are already covered by the ReaderTestCaseMixin and here we focus on
  the happy paths as this is slightly different.
  """
  def setUp(self):
    """Setup a reader, reading from the example files"""
    self.reader = LegacyReader('example.root')

  def test_categories(self):
    """Make sure the legacy reader returns only one category"""
    cats = self.reader.categories
    self.assertEqual(("events",), cats)

  def test_frame_iterator(self):
    """Make sure the FrameIterator works."""
    frames = self.reader.get('events')
    self.assertEqual(len(frames), 2000)

    for i, frame in enumerate(frames):
      # Rudimentary check here only to see whether we got the right frame
      self.assertEqual(frame.get_parameter('UserEventName'), f' event_number_{i}')
      # Only check a few Frames here
      if i > 10:
        break

    # Index based access
    frame = frames[123]
    self.assertEqual(frame.get_parameter('UserEventName'), ' event_number_123')

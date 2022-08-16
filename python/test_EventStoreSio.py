#!/usr/bin/env python3

import unittest
import os

from EventStore import EventStore
from test_EventStore import EventStoreBaseTestCase


SKIP_SIO_TESTS = os.environ.get('SKIP_SIO_TESTS', '1') == '1'


@unittest.skipIf(SKIP_SIO_TESTS, "no SIO support")
class EventStoreSioTestCase(EventStoreBaseTestCase, unittest.TestCase):
  """Test cases for root input files"""
  def setUp(self):
    print(SKIP_SIO_TESTS)
    self.filename = 'example.sio'
    self.assertTrue(os.path.isfile(self.filename))
    self.store = EventStore([self.filename])

  def test_no_file(self):
    '''Test that non-accessible files are gracefully handled.'''
    with self.assertRaises(ValueError):
      self.store = EventStore('foo.sio')


if __name__ == '__main__':
  # NOTE: These tests are really not intended to be run directly as they depend
  # on quite some environment setup as well as externally produced inputs.
  # See the CMakeLists.txt file in the tests folder for the specifics of that
  # environment and the inputs
  unittest.main()

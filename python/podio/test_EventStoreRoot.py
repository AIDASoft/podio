#!/usr/bin/env python3
"""Python unit tests for the ROOT backend"""

import unittest
import os

from ROOT import TFile

from podio.EventStore import EventStore
from podio.test_EventStore import EventStoreBaseTestCaseMixin


class EventStoreRootTestCase(EventStoreBaseTestCaseMixin, unittest.TestCase):
  """Test cases for root input files"""
  def setUp(self):
    """Setup an EventStore reading from a ROOT file"""
    self.filename = 'example.root'
    self.assertTrue(os.path.isfile(self.filename))
    self.store = EventStore(['example.root'])

  def test_chain(self):
    self.store = EventStore([self.filename,
                             self.filename])
    rootfile = TFile(self.filename)
    events = rootfile.Get(str('events'))
    numbers = []
    for iev, _ in enumerate(self.store):
      evinfo = self.store.get("info")
      numbers.append(evinfo[0].Number())
    self.assertEqual(iev + 1, 2 * events.GetEntries())  # pylint: disable=undefined-loop-variable
    # testing that numbers is [0, .. 1999, 0, .. 1999]
    self.assertEqual(numbers, list(range(events.GetEntries())) * 2)
    # trying to go to an event beyond the last one
    self.assertRaises(ValueError, self.store.__getitem__, 4001)
    # this is in the first event in the second file,
    # so its event number should be 0.
    self.assertEqual(self.store[2000].get("info")[0].Number(), 0)

  def test_no_file(self):
    '''Test that non-accessible files are gracefully handled.'''
    with self.assertRaises(ValueError):
      self.store = EventStore('foo.root')


if __name__ == '__main__':
  # NOTE: These tests are really not intended to be run directly as they depend
  # on quite some environment setup as well as externally produced inputs.
  # See the CMakeLists.txt file in the tests folder for the specifics of that
  # environment and the inputs
  unittest.main()

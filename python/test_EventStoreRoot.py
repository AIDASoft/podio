#!/usr/bin/env python3

from __future__ import absolute_import

import unittest
import os

from ROOT import TFile

from EventStore import EventStore
from test_EventStore import EventStoreBaseTestCase


class EventStoreRootTestCase(EventStoreBaseTestCase, unittest.TestCase):
  """Test cases for root input files"""
  def setUp(self):
    self.filename = 'example.root'
    self.assertTrue(os.path.isfile(self.filename))
    self.store = EventStore(['example.root'])

  def test_chain(self):
    self.store = EventStore([self.filename,
                             self.filename])
    rootfile = TFile(self.filename)
    events = rootfile.Get(str('events'))
    numbers = []
    for iev, event in enumerate(self.store):
      evinfo = self.store.get("info")
      numbers.append(evinfo[0].Number())
    self.assertEqual(iev + 1, 2 * events.GetEntries())
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

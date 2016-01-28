#!/usr/bin/env python
import os
import unittest
from EventStore import EventStore

class EventStoreTestCase(unittest.TestCase):

    def setUp(self):
        self.filename = 'example.root'
        self.assertTrue( os.path.isfile(self.filename) )
        self.store = EventStore(self.filename)

    def test_eventloop(self):
        self.assertTrue( self.store.getEntries() >= 0 )
        self.assertEqual( self.store.getEntries(), len(self.store) )
        for iev, event in enumerate(self.store):
            self.assertTrue( True )
            if iev>5:
                break
        event0 = self.store[0]
        self.assertEqual( event0.__class__, self.store.__class__)

    def test_collections(self):
        evinfo = self.store.get("clusters")
        self.assertTrue( len(evinfo)>0 )
        particles = self.store.get("CollectionNotThere")
        self.assertFalse(particles)

##########################
if __name__ == "__main__":
  unittest.main()

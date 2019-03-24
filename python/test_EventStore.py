import unittest
from EventStore import EventStore
import os

from ROOT import TFile
from ROOT import ExampleHit, ConstExampleHit


class EventStoreTestCase(unittest.TestCase):

    def setUp(self):
        self.filename = 'example1.root'
        self.assertTrue(os.path.isfile(self.filename))
        self.store = EventStore([self.filename])

    def test_eventloop(self):
        self.assertTrue(len(self.store) >= 0)
        self.assertEqual(self.store.current_store.getEntries(),
                         len(self.store))
        for iev, event in enumerate(self.store):
            self.assertTrue(True)
            if iev > 5:
                break

    def test_navigation(self):
        event0 = self.store[0]
        self.assertEqual(event0.__class__, self.store.__class__)

    def test_collections(self):
        evinfo = self.store.get("info")
        self.assertTrue(len(evinfo) > 0)
        particles = self.store.get("CollectionNotThere")
        self.assertFalse(particles)

    def test_read_only(self):
        hits = self.store.get("hits")
        # testing that one can't modify attributes in
        # read-only pods
        self.assertEqual(hits[0].energy(), 23.)
        hits[0].energy(10)
        self.assertEqual(hits[0].energy(), 10)  # oops
        # self.assertEqual(type(hits[0]), ConstExampleHit) # should be True

    def test_one_to_many(self):
        clusters = self.store.get("clusters")
        ref_hits = []
        # testing that cluster hits can be accessed and make sense
        for cluster in clusters:
            sume = 0
            for ihit in range(cluster.Hits_size()):
                hit = cluster.Hits(ihit)
                ref_hits.append(hit)
                sume += hit.energy()
            self.assertEqual(cluster.energy(), sume)
        hits = self.store.get("hits")
        # testing that the hits stored as a one to many relation
        # in the cluster can be found in the hit collection
        for hit in ref_hits:
            self.assertTrue(hit in hits)

    def test_hash(self):
        clusters = self.store.get("clusters")
        ref_hits = []
        # testing that cluster hits can be accessed and make sense
        for cluster in clusters:
            sume = 0
            for ihit in range(cluster.Hits_size()):
                hit = cluster.Hits(ihit)
                ref_hits.append(hit)
                sume += hit.energy()
            self.assertEqual(cluster.energy(), sume)
        hits = self.store.get("hits")
        if hits[0] == ref_hits[0]:
            self.assertEqual(hits[0].getObjectID().index,
                             ref_hits[0].getObjectID().index)
            self.assertEqual(hits[0].getObjectID().collectionID,
                             ref_hits[0].getObjectID().collectionID)
            self.assertEqual(hits[0].getObjectID(), ref_hits[0].getObjectID())
        # testing that the hits stored as a one to many relation
        # import pdb; pdb.set_trace()

    def test_chain(self):
        self.store = EventStore([self.filename,
                                 self.filename])
        rootfile = TFile(self.filename)
        events = rootfile.Get('events')
        numbers = []
        for iev, event in enumerate(self.store):
            evinfo = self.store.get("info")
            numbers.append(evinfo[0].Number())
        self.assertEqual(iev+1, 2*events.GetEntries())
        # testing that numbers is [0, .. 1999, 0, .. 1999]
        self.assertEqual(numbers, range(events.GetEntries())*2)
        # trying to go to an event beyond the last one
        self.assertRaises(ValueError, self.store.__getitem__,
                          4001)
        # this is in the first event in the second file,
        # so its event number should be 0.
        self.assertEqual(self.store[2000].get("info")[0].Number(), 0)

    def test_context_managers(self):
        with EventStore([self.filename]) as store:
            self.assertTrue(len(store) >= 0)
            self.assertTrue(store.isValid())

    def test_no_file(self):
        '''Test that non-accessible files are gracefully handled.'''
        with self.assertRaises(ValueError):
            self.store = EventStore('foo.root')
        
if __name__ == "__main__":
    from ROOT import gSystem
    from subprocess import call
    import os
    gSystem.Load("libTestDataModel")
    # creating example file for the tests
    if not os.path.isfile('example1.root'):
        write = '{podio}/tests/write'.format(podio=os.environ['PODIO'])
        print write
        call(write)
    unittest.main()

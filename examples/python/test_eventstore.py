import unittest
from eventstore import EventStore
import os

class EventStoreTestCase(unittest.TestCase):

    def setUp(self):
        self.filename = '../../build/example.root'
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
        particles = self.store.get("GenParticle")
        self.assertTrue( len(particles)>0 )
        self.assertIsNotNone( particles[0] )
        self.assertTrue( particles[0].P4().Pt > 0)
        jets = self.store.get("GenJet")
        self.assertTrue( len(jets)>0 )
        self.assertIsNotNone( jets[0] )
        self.assertTrue( jets[0].P4().Pt > 0)

    def test_handles(self):
        assocs = self.store.get("GenJetParticle")
        particles = self.store.get("GenParticle")
        jets = self.store.get("GenJet")
        self.assertTrue( len(assocs)>0 )
        jet = assocs[0].Jet()
        ptc = assocs[0].Particle()
        self.assertIsNotNone( jet )
        self.assertIsNotNone( ptc )
        self.assertTrue( jet in jets )
        self.assertTrue( ptc in particles )
        self.assertFalse( jet in particles )
        self.assertFalse( ptc in jets )
        ptcsInJet0 = []
        for assoc in assocs:
            jet = assoc.Jet()
            if jet == jets[0]:
                ptcsInJet0.append( assoc.Particle() )
        self.assertTrue( len(ptcsInJet0) > 0)
                
            

if __name__ == "__main__":
    unittest.main()

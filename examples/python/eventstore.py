from ROOT import gSystem
gSystem.Load("libDatamodelExample")
from ROOT import ParticleCollection
from ROOT import PyEventStore

def iterator(self):
    '''dynamically added iterator'''
    entries = self._getBuffer().size()
    for entry in xrange(entries):
        yield self.get(entry)

def addIterator(obj):
    obj.__class__.__iter__ = iterator
    return obj 

class EventStore(object):
    '''Interface to events in an albers root file.

    Example of use:

    events = EventStore("example.root")
    for iev, store in islice(enumerate(events), 0, 2): 
        particles = store.get("GenParticle");
        for i, p in islice(enumerate(particles), 0, 5):
            print "particle ", i, p.ID(), p.P4().Pt
    '''
    def __init__(self, filename):
        '''Create an event list from the albers root file.

        Parameters:
           filename: path to the root file
        '''
        self.store = PyEventStore(filename)

    def get(self, name):
        '''Returns a collection.

        Parameters:
           name: name of the collection in the albers root file.
        '''
        coll = self.store.get(name)
        collection = addIterator(coll)
        return collection

    def __getattr__(self, name):
        '''missing attributes are taken from self.store'''
        return getattr(self.store, name)

    def __iter__(self):
        '''iterate on events in the tree.
        '''
        for entry in xrange(self.getEntries()):
            yield self
            self.endOfEvent()

if __name__ == "__main__":
    
    import sys
    from itertools import islice

    filename = sys.argv[1]
    events = EventStore(filename)

    for iev, store in islice(enumerate(events), 0, 2): 
        particles = store.get("GenParticle");
        for i, p in islice(enumerate(particles), 0, 5):
            print "particle ", i, p.ID(), p.P4().Pt
        jets = store.get("GenJet")
        for i, jet in enumerate(jets):
            print "jet ", jet.P4().Pt 
        assocs = store.get("GenJetParticle")
        for assoc in assocs:
            print assoc.Jet().P4().Pt,  assoc.Particle().P4().Pt

        

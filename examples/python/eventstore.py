from ROOT import gSystem
gSystem.Load("libDatamodelExample")
from ROOT import ParticleCollection
from ROOT import PyEventStore

def iterator(self):
    '''dynamically added iterator'''
    entries = self._getBuffer().size()
    for entry in xrange(entries):
        yield self.get(entry)

def len(self):
    return self._getBuffer().size()
        
def getitem(self, key):
    return self.get(key)


class EventStore(object):
    '''Interface to events in an albers root file.

    Example of use:

    events = EventStore("example.root")
    for iev, store in islice(enumerate(events), 0, 2): 
        particles = store.get("GenParticle");
        for i, p in islice(enumerate(particles), 0, 5):
            print "particle ", i, p.ID(), p.P4().Pt
    '''
    def __init__(self, filename, treename=None):
        '''Create an event list from the albers root file.

        Parameters:
           filename: path to the root file
           treename: not used at the moment
        '''
        # COLIN what to do with chains? 
        if hasattr(filename, '__getitem__'):
            filename = filename[0] 
        self.store = PyEventStore(filename)

    def get(self, name):
        '''Returns a collection.

        Parameters:
           name: name of the collection in the albers root file.
        '''
        coll = self.store.get(name)
        # adding iterator generator to be able to loop on the collection
        coll.__class__.__iter__ = iterator
        # adding length function
        coll.__class__.__len__ = len
        # enabling the use of [] notation on the collection
        coll.__class__.__getitem__ = getitem
        return coll

    def __getattr__(self, name):
        '''missing attributes are taken from self.store'''
        return getattr(self.store, name)

    def __iter__(self):
        '''iterate on events in the tree.
        '''
        for entry in xrange(self.getEntries()):
            yield self
            self.endOfEvent()

    def __getitem__(self, evnum):
        self.goToEvent( evnum )
        return self
            
    def __len__(self):
        return self.store.getEntries()
            
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

        

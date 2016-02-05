from ROOT import gSystem
gSystem.Load("libpodio")
from ROOT import podio

def iterator(self):
    '''dynamically added iterator'''
    entries = self.size()
    for entry in xrange(entries):
        yield self.at(entry)

def len(self):
    return self.size()

def getitem(self, key):
    return self.at(key)


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
        '''Create an event list from the podio root file.
        Parameters:
           filename: path to the root file
           treename: not used at the moment
        '''
        # COLIN what to do with chains?
        # if hasattr(filename, '__getitem__'):
        #     filename = filename[0]
        self.store = podio.PythonEventStore(filename)

    def get(self, name):
        '''Returns a collection.
        Parameters:
           name: name of the collection in the albers root file.
        '''
        coll = self.store.get(name)
        # adding iterator generator to be able to loop on the collection
        coll.__iter__ = iterator
        # adding length function
        coll.__len__ = len
        # enabling the use of [] notation on the collection
        coll.__getitem__ = getitem
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

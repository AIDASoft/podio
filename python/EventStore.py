

from __future__ import absolute_import, unicode_literals, print_function
from six.moves import range

from ROOT import gSystem
gSystem.Load("libpodioRootIO")
from ROOT import podio


def iterator(self):
    '''dynamically added iterator'''
    entries = self.size()
    for entry in range(entries):
        yield self.at(entry)


def size(self):
    return self.size()


def getitem(self, key):
    return self.at(key)


class EventStore(object):
    '''Interface to events in an podio root file.
    Example of use:
    events = EventStore(["example.root", "example1.root"])
    for iev, store in islice(enumerate(events), 0, 2):
        particles = store.get("GenParticle");
        for i, p in islice(enumerate(particles), 0, 5):
            print "particle ", i, p.ID(), p.P4().Pt
    '''
    def __init__(self, filenames, treename=None):
        '''Create an event list from the podio root file.
        Parameters:
           filenames: list of root files
                      you can of course provide a list containing a single
                      root file. you could use the glob module to get all
                      files matching a wildcard pattern.
           treename: not used at the moment
        '''
        if isinstance(filenames,str):
            filenames = (filenames,)
        self.files = filenames
        self.stores = []
        self.current_store = None
        for fname in self.files:
            store = podio.PythonEventStore(fname)
            if store.isZombie():
                raise ValueError(fname + ' does not exist.')
            store.name = fname
            if self.current_store is None:
                self.current_store = store
            self.stores.append((store.getEntries(), store))

    def __str__(self):
        result = "Content:"
        for item in self.current_store.getCollectionNames():
            result += "\n\t%s" %item
        return result

    def get(self, name):
        '''Returns a collection.
        Parameters:
           name: name of the collection in the podio root file.
        '''
        coll = self.current_store.get(name)
        # adding iterator generator to be able to loop on the collection
        coll.__iter__ = iterator
        # adding length function
        coll.__len__ = size
        # enabling the use of [] notation on the collection
        coll.__getitem__ = getitem
        return coll

    def isValid(self):
        return self.current_store is not None and self.current_store.isValid()

    # def __getattr__(self, name):
    #     '''missing attributes are taken from self.current_store'''
    #     if name != 'current_store':
    #         return getattr(self.current_store, name)
    #     else:
    #         return None

    def current_filename(self):
        '''Returns the name of the current file.'''
        if self.current_store is None:
            return None
        else:
            return self.current_store.fname
    
    def __enter__(self):
        return self

    def __exit__(self, exception_type, exception_val, trace):
        for store in self.stores:
            store[1].close()

    def __iter__(self):
        '''iterate on events in the tree.
        '''
        for nev, store in self.stores:
            self.current_store = store
            for entry in range(store.getEntries()):
                yield store
                store.endOfEvent()

    def __getitem__(self, evnum):
        '''Get event number evnum'''
        current_store = None
        rel_evnum = evnum
        for nev, store in self.stores:
            if rel_evnum < nev:
                current_store = store
                break
            rel_evnum -= nev
        if current_store is None:
            raise ValueError('event number too large: ' + str(evnum))
        self.current_store = current_store
        self.current_store.goToEvent(rel_evnum)
        return self

    def __len__(self):
        '''Returns the total number of events in all files.'''
        nevts_all_files = 0
        for nev, store in self.stores:
            nevts_all_files += nev
        return nevts_all_files

from ROOT import gSystem
gSystem.Load("libpodio")
from ROOT import podio

def iterator(self):
    '''dynamically added iterator'''
    entries = self.size()
    for entry in xrange(entries):
        yield self.at(entry)

def size(self):
    return self.size()

def getitem(self, key):
    return self.at(key)


class EventStore(object):
    '''Interface to events in an podio root file.
    Example of use:
    events = EventStore("example.root")
    for iev, store in islice(enumerate(events), 0, 2):
        particles = store.get("GenParticle");
        for i, p in islice(enumerate(particles), 0, 5):
            print "particle ", i, p.ID(), p.P4().Pt
    '''
    def __init__(self, filenames, treename=None):
        '''Create an event list from the podio root file.
        Parameters:
           filename: path to the root file
           treename: not used at the moment
        '''
        self.files = filenames
        self.ifile = 0
        self.store = self.next_file()

    def next_file(self):
        if self.ifile == len(self.files):
            return False
        fname = self.files[self.ifile]
        self.store = podio.PythonEventStore(fname)
        self.ifile += 1    
        return self.store
        
    def get(self, name):
        '''Returns a collection.
        Parameters:
           name: name of the collection in the podio root file.
        '''
        coll = self.store.get(name)
        # adding iterator generator to be able to loop on the collection
        coll.__iter__ = iterator
        # adding length function
        coll.__len__ = size
        # enabling the use of [] notation on the collection
        coll.__getitem__ = getitem
        return coll

    def __getattr__(self, name):
        '''missing attributes are taken from self.store'''
        return getattr(self.store, name)

    def __iter__(self):
        '''iterate on events in the tree.
        '''
        while 1:
            for entry in xrange(self.getEntries()):
                yield self
                self.endOfEvent()        
            if not self.next_file():
                break

    def __getitem__(self, evnum):
        if evnum > len(self):
            details = None
            if len(self.files)>1:
                details = '''
                direct event navigation is not yet implemented. 
                Please accept our apologies for any inconvenience, 
                and in the meanwhile, just loop. 
                '''
            elif len(self.files)==1:
                details = '''the event number that you have provided 
                is too large as your input file contains only {length} events
                '''.format(length=len(self))
            err = '''Cannot navigate to event {evnum}

            {details}
            '''.format(evnum=evnum, details=details)
            raise ValueError(err)
        self.goToEvent( evnum )
        return self

    def __len__(self):
        return self.store.getEntries()

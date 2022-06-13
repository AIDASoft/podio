"""Python EventStore for reading files with podio generated datamodels"""


from ROOT import gSystem
gSystem.Load("libpodioRootIO")  # noqa: E402
from ROOT import podio  # noqa: E402 # pylint: disable=wrong-import-position


def size(self):
  """Override size function that can be attached as __len__ method to
  collections"""
  return self.size()


def getitem(self, key):
  """Override getitem function that can be attached as __getitem__ method to
  collections (see below why this is necessary sometimes)"""
  return self.at(key)


class EventStore:
  '''Interface to events in an podio root file.
  Example of use:
  events = EventStore(["example.root", "example1.root"])
  for iev, store in islice(enumerate(events), 0, 2):
      particles = store.get("GenParticle");
      for i, p in islice(enumerate(particles), 0, 5):
          print "particle ", i, p.ID(), p.P4().Pt
  '''

  def __init__(self, filenames):
    '''Create an event list from the podio root file.
    Parameters:
       filenames: list of root files
                  you can of course provide a list containing a single
                  root file. you could use the glob module to get all
                  files matching a wildcard pattern.
    '''
    if isinstance(filenames, str):
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
      result += f"\n\t{item}"
    return result

  def get(self, name):
    '''Returns a collection.
    Parameters:
       name: name of the collection in the podio root file.
    '''
    coll = self.current_store.get(name)
    # adding length function
    coll.__len__ = size
    # enabling the use of [] notation on the collection
    # cppyy defines the __getitem__ method if the underlying c++ class has an operator[]
    # method. For some reason they do not conform to the usual signature and only
    # pass one argument to the function they call. Here we simply check if we have to
    # define the __getitem__ for the collection.
    if not hasattr(coll, '__getitem__'):
      coll.__getitem__ = getitem
    return coll

  def isValid(self):
    """Check if the EventStore is in a valid state"""
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
    return self.current_store.fname

  def __enter__(self):
    return self

  def __exit__(self, exception_type, exception_val, trace):
    for store in self.stores:
      store[1].close()

  def __iter__(self):
    '''iterate on events in the tree.
    '''
    for _, store in self.stores:
      self.current_store = store
      for _ in range(store.getEntries()):
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
    for nev, _ in self.stores:
      nevts_all_files += nev
    return nevts_all_files

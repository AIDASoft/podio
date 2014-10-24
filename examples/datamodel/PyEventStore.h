#ifndef ALBERS_PYEVENTSTORE_H
#define ALBERS_PYEVENTSTORE_H



namespace albers {
  class CollectionBase;
  class EventStore; 
  class Reader; 
}

class PyEventStore {

 public:

  PyEventStore(const char* filename);

  /// access a collection.
  albers::CollectionBase* get(const char* name);

  /// signify end of event
  void endOfEvent();
  
  /// go to a given event
  void goToEvent(unsigned ievent);

  /// get number of entries in the tree
  unsigned getEntries() const;

 private:
  albers::EventStore* m_store; 
  albers::Reader* m_reader; 
    
};

#endif

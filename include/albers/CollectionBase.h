#ifndef COLLECTIONBASE_H
#define COLLECTIONBASE_H

#include <string>
#include <utility>
#include <vector>

#include "albers/ObjectID.h"

namespace albers {
  // forward declarations
  class ObjectID;
  class ICollectionProvider;
  class CollectionBase;

  typedef std::vector<std::pair<std::string,albers::CollectionBase*>> CollRegistry;
  typedef std::vector<std::vector<albers::ObjectID>*> CollRefCollection;

  //class CollectionBuffer {
  //public:
  //  void* data;
  //  CollRefCollection* references;
  //};

  class CollectionBase {
  public:
    virtual void  prepareForWrite() = 0;
    //virtual void  write(CollectionBuffer& buffer) = 0;
    //virtual void  read(CollectionBuffer& buffer) = 0;
    virtual void  prepareAfterRead() = 0;
    virtual bool  setReferences(const ICollectionProvider* collectionProvider) = 0;
    virtual void  setID(unsigned id) = 0;
    virtual void  setBuffer(void*) = 0;
    virtual void* getBufferAddress() = 0;
    virtual ~CollectionBase(){};
    virtual void clear() = 0 ;
    virtual CollRefCollection* referenceCollections() = 0;
  };

} // namespace

#endif

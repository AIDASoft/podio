#ifndef COLLECTIONBASE_H
#define COLLECTIONBASE_H

#include <string>
#include <utility>
#include <vector>

#include "podio/ObjectID.h"

namespace podio {
  // forward declarations
  class ObjectID;
  class ICollectionProvider;
  class CollectionBase;

  using CollRefCollection = std::vector<std::vector<podio::ObjectID>*>;
  using VectorMembersInfo = std::vector<std::pair<std::string, void*>>;

  class CollectionBase {
  public:
    /// prepare buffers for serialization
    virtual void  prepareForWrite() = 0;

    //virtual void  write(CollectionBuffer& buffer) = 0;
    //virtual void  read(CollectionBuffer& buffer) = 0;
    
    /// re-create collection from buffers after read
    virtual void  prepareAfterRead() = 0;

    /// initialize references after read
    virtual bool setReferences(const ICollectionProvider* collectionProvider) = 0;

    /// set collection ID
    virtual void setID(unsigned id) = 0;

    /// get collection ID
    virtual unsigned getID() const = 0;

    /// set I/O buffer
    virtual void  setBuffer(void*) = 0;

    /// get address of the pointer to the I/O buffer
    virtual void* getBufferAddress() = 0;

    /// check for validity of the container after read
    virtual bool isValid() const = 0;

    /// number of elements in the collection
    virtual size_t size() const = 0;

    /// fully qualified type name of elements - with namespace
    virtual std::string getValueTypeName() const = 0;

    /// destructor
    virtual ~CollectionBase(){};

    /// clear the collection and all internal states
    virtual void clear() = 0 ;

    /// return the buffers containing the object-relation information
    virtual CollRefCollection* referenceCollections() = 0;

    /// return pointers to vector members
    virtual VectorMembersInfo* vectorMembers() = 0;
  };

} // namespace

#endif

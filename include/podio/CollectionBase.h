#ifndef COLLECTIONBASE_H
#define COLLECTIONBASE_H

#include "podio/ObjectID.h"
#include "podio/CollectionBuffers.h"

#include <string>
#include <utility>
#include <vector>

namespace podio {
  // forward declarations
  class ICollectionProvider;

  class CollectionBase {
  public:
    /// prepare buffers for serialization
    virtual void prepareForWrite() = 0;

    /// re-create collection from buffers after read
    virtual void  prepareAfterRead() = 0;

    /// initialize references after read
    virtual bool setReferences(const ICollectionProvider* collectionProvider) = 0;

    /// set collection ID
    virtual void setID(unsigned id) = 0;

    /// get collection ID
    virtual unsigned getID() const = 0;

    /// Get the collection buffers for this collection
    virtual podio::CollectionBuffers getBuffers() = 0;

    /// check for validity of the container after read
    virtual bool isValid() const = 0;

    /// number of elements in the collection
    virtual size_t size() const = 0;

    /// fully qualified type name
    virtual std::string getTypeName() const = 0;
    /// fully qualified type name of elements - with namespace
    virtual std::string getValueTypeName() const = 0;
    /// fully qualified type name of stored POD elements - with namespace
    virtual std::string getDataTypeName() const = 0;

    /// destructor
    virtual ~CollectionBase() = default;

    /// clear the collection and all internal states
    virtual void clear() = 0 ;

    /// check if this collection is a subset collection
    virtual bool isSubsetCollection() const = 0;

    /// declare this collection to be a subset collection
    virtual void setSubsetCollection(bool setSubset=true) = 0;
  };

} // namespace

#endif

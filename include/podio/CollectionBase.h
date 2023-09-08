#ifndef PODIO_COLLECTIONBASE_H
#define PODIO_COLLECTIONBASE_H

#include "podio/CollectionBuffers.h"
#include "podio/ObjectID.h"
#include "podio/SchemaEvolution.h"

#include <iostream>
#include <string_view>
#include <utility>
#include <vector>

namespace podio {
// forward declarations
class ICollectionProvider;

struct RelationNames;

class CollectionBase {
protected:
  /// default constructor
  CollectionBase() = default;
  /// Move constructor
  CollectionBase(CollectionBase&&) = default;
  /// Move assignment
  CollectionBase& operator=(CollectionBase&&) = default;

public:
  /// No copy c'tor because collections are move-only
  CollectionBase(const CollectionBase&) = delete;
  /// No copy assignment because collections are move-only
  CollectionBase& operator=(const CollectionBase&) = delete;

  /// prepare buffers for serialization
  virtual void prepareForWrite() const = 0;

  /// re-create collection from buffers after read
  virtual void prepareAfterRead() = 0;

  /// initialize references after read
  virtual bool setReferences(const ICollectionProvider* collectionProvider) = 0;

  /// set collection ID
  virtual void setID(uint32_t id) = 0;

  /// get collection ID
  virtual uint32_t getID() const = 0;

  /// Get the collection buffers for this collection
  virtual podio::CollectionWriteBuffers getBuffers() = 0;

  /// check for validity of the container after read
  virtual bool isValid() const = 0;

  /// number of elements in the collection
  virtual size_t size() const = 0;

  /// Is the collection empty
  virtual bool empty() const = 0;

  /// fully qualified type name
  virtual const std::string_view getTypeName() const = 0;
  /// fully qualified type name of elements - with namespace
  virtual const std::string_view getValueTypeName() const = 0;
  /// fully qualified type name of stored POD elements - with namespace
  virtual const std::string_view getDataTypeName() const = 0;
  /// schema version of the collection
  virtual SchemaVersionT getSchemaVersion() const = 0;

  /// destructor
  virtual ~CollectionBase() = default;

  /// clear the collection and all internal states
  virtual void clear() = 0;

  /// check if this collection is a subset collection
  virtual bool isSubsetCollection() const = 0;

  /// declare this collection to be a subset collection
  virtual void setSubsetCollection(bool setSubset = true) = 0;

  /// print this collection to the passed stream
  virtual void print(std::ostream& os = std::cout, bool flush = true) const = 0;

  /// Get the index in the DatatypeRegistry of the EDM this collection belongs to
  virtual size_t getDatamodelRegistryIndex() const = 0;
};

} // namespace podio

#endif

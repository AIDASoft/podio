#ifndef COLLECTIONBASE_H
#define COLLECTIONBASE_H

#include <string>
#include <utility>
#include <vector>
// forward declarations

namespace albers {
  class Registry;
  class CollectionBase;

  typedef std::vector<std::pair<std::string,albers::CollectionBase*>> CollRegistry;

  class CollectionBase {
  public:
    virtual void  prepareForWrite(const Registry* registry) = 0;
    virtual void  prepareAfterRead(Registry* registry) = 0;
    virtual void  setPODsAddress(const void*) = 0;
    virtual void* _getRawBuffer() = 0;
    virtual ~CollectionBase(){};
    virtual void clear() = 0 ;
    virtual CollRegistry& referenceCollections() { return m_referencingCollections; };

  private:
    CollRegistry m_referencingCollections;
    virtual void print() const = 0;
  };

} // namespace

#endif

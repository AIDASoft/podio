#ifndef COLLECTIONBASE_H
#define COLLECTIONBASE_H

// forward declarations

namespace albers {
  class Registry;

  class CollectionBase {
  public:
    virtual void  prepareForWrite(const Registry* registry) = 0;
    virtual void  prepareAfterRead(Registry* registry) = 0;
    virtual void  setPODsAddress(const void*) = 0;
    virtual void* _getRawBuffer() = 0;
    virtual ~CollectionBase(){};
    virtual void clear() = 0 ;
  };

} // namespace

#endif

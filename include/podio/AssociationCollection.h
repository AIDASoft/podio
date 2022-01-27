#ifndef PODIO_ASSOCIATIONCOLLECTION_H
#define PODIO_ASSOCIATIONCOLLECTION_H

#include "podio/Association.h"
#include "podio/AssociationFwd.h"
#include "podio/AssociationObj.h"
#include "podio/CollectionBase.h"
#include "podio/CollectionBuffers.h"
#include "podio/ICollectionProvider.h"

namespace podio {

template <typename FromT, typename ToT>
class AssociationCollection : public podio::CollectionBase {
  static_assert(std::is_same_v<FromT, detail::GetDefT<FromT>>,
                "Associations need to be instantiated with the default types!");
  static_assert(std::is_same_v<ToT, detail::GetDefT<ToT>>,
                "Associations need to be instantiated with the default types!");

public:
  size_t size() const override {
    // TODO
  }

  void clear() override {
    // TODO
  }

  bool isValid() const override {
    // TODO
  }

  podio::CollectionBuffers getBuffers() override {
    // TOOD
  }

  std::string getTypeName() const override {
    return std::string("podio::AssociationCollection<") + FromT::TypeName + "," + ToT::TypeName + ">";
  }

  std::string getValueTypeName() const override {
    return std::string("podio::Association<") + FromT::TypeName + "," + ToT::TypeName + ">";
  }

  std::string getDataTypeName() const override {
    return "float";
  }

  bool isSubsetCollection() const override {
    // TODO
  }

  void setSubsetCollection(bool) override {
    // TODO
  }

  void setID(unsigned id) override {
    m_collectionID = id;
  }

  unsigned getID() const override {
    return m_collectionID;
  }

  void prepareForWrite() override {
    // TODO
  }

  void prepareAfterRead() override {
    // TODO
  }

  bool setReferences(const ICollectionProvider*) override {
    // TODO
  }

private:
  unsigned m_collectionID{};
};

} // namespace podio

#endif // PODIO_ASSOCIATIONCOLLECTION_H

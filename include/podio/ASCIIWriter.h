#ifndef PODIO_ASCIIWRITER_H
#define PODIO_ASCIIWRITER_H

#include "podio/EventStore.h"
#include "podio/SchemaEvolution.h"
#include "podio/utilities/Deprecated.h"

#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <vector>

namespace podio {

class CollectionBase;

struct ColWriterBase {
  virtual void writeCollection(CollectionBase*, std::ostream&) = 0;
  virtual ~ColWriterBase() = default;
};

template <class T>
struct ColWriter : public ColWriterBase {
  void writeCollection(CollectionBase* c, std::ostream& o) override {
    T* col = static_cast<T*>(c);
    o << col->size() << std::endl;
    o << *col << std::endl;
  }
};

typedef std::map<std::string, ColWriterBase*> FunMap;

class DEPR_EVTSTORE ASCIIWriter {

public:
  ASCIIWriter(const std::string& filename, EventStore* store);
  ~ASCIIWriter();

  // non-copyable
  ASCIIWriter(const ASCIIWriter&) = delete;
  ASCIIWriter& operator=(const ASCIIWriter&) = delete;

  template <typename T>
  bool registerForWrite(const std::string& name);
  void writeEvent();
  void finish();

private:
  template <typename T>
  void writeCollection(const std::string& name);
  // members
  std::string m_filename;
  EventStore* m_store;

  std::ofstream* m_file;

  std::vector<CollectionBase*> m_storedCollections{};
  std::vector<std::string> m_collectionNames{};
  FunMap m_map{};
};

template <typename T>
bool ASCIIWriter::registerForWrite(const std::string& name) {
  const T* tmp_coll(nullptr);
  if (!m_store->get(name, tmp_coll)) {
    std::cerr << "no such collection to write, throw exception." << std::endl;
    return false;
  }
  T* coll = const_cast<T*>(tmp_coll);

  m_storedCollections.emplace_back(coll);
  m_collectionNames.emplace_back(name);
  m_map[name] = new ColWriter<T>;
  return true;
}

} // namespace podio
#endif

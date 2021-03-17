// podio specific includes
#include "podio/ASCIIWriter.h"
#include "podio/CollectionBase.h"
#include "podio/EventStore.h"

namespace podio {

ASCIIWriter::ASCIIWriter(const std::string& filename, EventStore* store) :
    m_filename(filename), m_store(store), m_file(new std::ofstream) {

  m_file->open(filename, std::ofstream::binary);
}

ASCIIWriter::~ASCIIWriter() { delete m_file; }

void ASCIIWriter::writeEvent() {

  unsigned i = 0;
  for (auto& coll : m_storedCollections) {
    coll->prepareForWrite();

    const std::string& name = m_collectionNames[i++];
    std::cout << " writing collection " << name << std::endl;

    *m_file << name << "  ";

    ColWriterBase* wrt = m_map[name];

    wrt->writeCollection(coll, *m_file);
  }
}

void ASCIIWriter::finish() { m_file->close(); }

} // namespace podio

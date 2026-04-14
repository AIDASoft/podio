#include "podio/RNTupleReader.h"
#include "podio/CollectionBufferFactory.h"
#include "podio/CollectionBuffers.h"
#include "podio/DatamodelRegistry.h"
#include "podio/GenericParameters.h"
#include "podio/utilities/RootHelpers.h"
#include "rntuple_utils.h"
#include "rootUtils.h"

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

namespace podio {

bool RNTupleReader::initCategory(std::string_view category) {
  return initCategoryCommon(category, m_collectionInfo[category], m_idTables[category]);
}

void RNTupleReader::openFile(const std::string& filename) {
  openFiles({filename});
}

void RNTupleReader::openFiles(const std::vector<std::string>& filenames) {
  openMetaData(filenames, m_fileVersion, m_datamodelHolder);

  // Pre-fill the entries map
  for (const auto& category : m_availableCategories) {
    m_readerEntries[category].reserve(m_filenames.size() + 1);
    m_readerEntries[category].push_back(0);
    for (const auto& filename : m_filenames) {
      try {
#if ROOT_VERSION_CODE >= ROOT_VERSION(6, 36, 0)
        ROOT::RNTupleDescriptor::RCreateModelOptions options;
        // Read unknown types (like deleted ones) without errors
        options.SetEmulateUnknownTypes(true);
        m_readers[category].emplace_back(root_compat::RNTupleReader::Open(options, category, filename));
#else
        m_readers[category].emplace_back(root_compat::RNTupleReader::Open(category, filename));
#endif
        m_readerEntries[category].push_back(m_readerEntries[category].back() +
                                            m_readers[category].back()->GetNEntries());
      } catch (const RException&) {
        std::cout << "Category " << category << " not found in file " << filename << std::endl;
      }
    }
    m_totalEntries[category] = m_readerEntries[category].back();
    // The last entry is not needed since it's the total number of entries
    m_readerEntries[category].pop_back();
  }
}

unsigned RNTupleReader::getEntries(std::string_view name) const {
  if (const auto it = m_totalEntries.find(name); it != m_totalEntries.end()) {
    return it->second;
  }
  return 0;
}

std::unique_ptr<ROOTFrameData> RNTupleReader::readNextEntry(std::string_view category,
                                                            const std::vector<std::string>& collsToRead) {
  return readEntry(category, m_entries[category], collsToRead);
}

std::unique_ptr<ROOTFrameData> RNTupleReader::readEntry(std::string_view category, const unsigned entNum,
                                                        const std::vector<std::string>& collsToRead) {
  if (m_collectionInfo.find(category) == m_collectionInfo.end()) {
    if (!initCategory(category)) {
      return nullptr;
    }
  }
  const auto maxCatEvents = getEntries(category);
  if (entNum >= maxCatEvents) {
    return nullptr;
  }

  const auto& collInfo = m_collectionInfo[category];
  // Make sure to not silently ignore non-existant but requested collections
  if (!collsToRead.empty()) {
    for (const auto& name : collsToRead) {
      if (std::ranges::find(collInfo, name, &root_utils::CollectionWriteInfo::name) == collInfo.end()) {
        throw std::invalid_argument(name + " is not available from Frame");
      }
    }
  }

  m_entries[category] = entNum + 1;

  // m_readerEntries contains the accumulated entries for all the readers
  // therefore, the first number that is lower or equal to the entry number
  // is at the index of the reader that contains the entry
  const auto& readerEntries = m_readerEntries[category];
  const auto upper = std::ranges::upper_bound(readerEntries, entNum);
  const auto localEntry = entNum - *(upper - 1);
  const auto readerIndex = upper - 1 - readerEntries.begin();
  const auto& reader = m_readers[category][readerIndex];

  ROOTFrameData::BufferMap buffers;
  // We need to create a non-bare entry here, because the entries for the
  // parameters are not explicitly (re)set and we need them default initialized.
  // In principle we would only need a bare entry for the collection data, since
  // we set all the fields there in any case.
  const auto dentry = reader->GetModel().CreateEntry();

  for (const auto& coll : collInfo) {
    if (!collsToRead.empty() && std::ranges::find(collsToRead, coll.name) == collsToRead.end()) {
      continue;
    }
    const auto& collType = coll.dataType;
    const auto& bufferFactory = podio::CollectionBufferFactory::instance();
    auto maybeBuffers = bufferFactory.createBuffers(collType, coll.schemaVersion, coll.isSubset);

    if (!maybeBuffers) {
      std::cerr << "WARNING: Buffers couldn't be created for collection " << coll.name << " of type " << coll.dataType
                << " and schema version " << coll.schemaVersion << std::endl;
      continue;
    }
    auto& collBuffers = maybeBuffers.value();

    if (!rntuple_utils::bindCollectionToEntry(dentry.get(), collBuffers, coll)) {
      continue;
    }

    buffers.emplace(coll.name, std::move(collBuffers));
  }

  reader->LoadEntry(localEntry, *dentry);

  auto parameters = readEventMetaData(reader.get(), localEntry);

  auto idTable = m_idTables[category];
  return std::make_unique<ROOTFrameData>(std::move(buffers), std::move(idTable), std::move(parameters));
}

} // namespace podio

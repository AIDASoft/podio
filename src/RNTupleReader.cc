#include "podio/RNTupleReader.h"
#include "podio/CollectionBufferFactory.h"
#include "podio/CollectionBuffers.h"
#include "podio/DatamodelRegistry.h"
#include "podio/GenericParameters.h"
#include "podio/ReadOptions.h"
#include "podio/utilities/RootHelpers.h"
#include "rootUtils.h"

#include <ROOT/RError.hxx>

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

// Adjust for the move of this out of ROOT v7 in
// https://github.com/root-project/root/pull/17281
#if ROOT_VERSION_CODE >= ROOT_VERSION(6, 35, 0)
using ROOT::RException;
#else
using ROOT::Experimental::RException;
#endif

namespace podio {

template <typename T>
void RNTupleReader::readParams(const std::string& name, const unsigned localEntry, const unsigned readerIndex,
                               GenericParameters& params) {
  auto keyView = m_readers[name][readerIndex]->GetView<std::vector<std::string>>(root_utils::getGPKeyName<T>());
  auto valueView = m_readers[name][readerIndex]->GetView<std::vector<std::vector<T>>>(root_utils::getGPValueName<T>());

  params.loadFrom(keyView(localEntry), valueView(localEntry));
}

GenericParameters RNTupleReader::readEventMetaData(const std::string& name, const unsigned localEntry,
                                                   const unsigned readerIndex) {
  GenericParameters params;

  readParams<int>(name, localEntry, readerIndex, params);
  readParams<float>(name, localEntry, readerIndex, params);
  readParams<double>(name, localEntry, readerIndex, params);
  readParams<std::string>(name, localEntry, readerIndex, params);

  return params;
}

bool RNTupleReader::initCategory(const std::string& category) {
  if (std::ranges::find(m_availableCategories, category) == m_availableCategories.end()) {
    return false;
  }
  // Assume that the metadata is the same in all files
  const auto& filename = m_filenames[0];

  auto collInfo = m_metadata_readers[filename]->GetView<std::vector<root_utils::CollectionWriteInfo>>(
      {root_utils::collInfoName(category)});

  m_collectionInfo[category] = collInfo(0);
  m_idTables[category] = root_utils::makeCollIdTable(collInfo(0));

  return true;
}

void RNTupleReader::openFile(const std::string& filename) {
  openFiles({filename});
}

void RNTupleReader::openFiles(const std::vector<std::string>& filenames) {

  m_filenames.insert(m_filenames.end(), filenames.begin(), filenames.end());
  for (const auto& filename : filenames) {
    m_metadata_readers.try_emplace(filename, root_compat::RNTupleReader::Open(root_utils::metaTreeName, filename));
  }

  m_metadata = root_compat::RNTupleReader::Open(root_utils::metaTreeName, filenames[0]);

  auto versionView = m_metadata->GetView<std::vector<uint16_t>>(root_utils::versionBranchName);
  const auto version = versionView(0);

  m_fileVersion = podio::version::Version{version[0], version[1], version[2]};

  auto edmView = m_metadata->GetView<std::vector<std::tuple<std::string, std::string>>>(root_utils::edmDefBranchName);
  auto edm = edmView(0);
  DatamodelDefinitionHolder::VersionList edmVersions{};
  for (const auto& [name, _] : edm) {
    try {
      auto edmVersionView = m_metadata->GetView<std::vector<uint16_t>>(root_utils::edmVersionBranchName(name));
      const auto edmVersion = edmVersionView(0);
      edmVersions.emplace_back(name, podio::version::Version{edmVersion[0], edmVersion[1], edmVersion[2]});
    } catch (const RException&) {
    }
  }
  m_datamodelHolder = DatamodelDefinitionHolder(std::move(edm), std::move(edmVersions));

  auto availableCategoriesField = m_metadata->GetView<std::vector<std::string>>(root_utils::availableCategories);
  m_availableCategories = availableCategoriesField(0);

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

unsigned RNTupleReader::getEntries(const std::string& name) const {
  if (const auto it = m_totalEntries.find(name); it != m_totalEntries.end()) {
    return it->second;
  }
  return 0;
}

std::vector<std::string_view> RNTupleReader::getAvailableCategories() const {
  std::vector<std::string_view> cats;
  cats.reserve(m_availableCategories.size());
  for (const auto& cat : m_availableCategories) {
    cats.emplace_back(cat);
  }
  return cats;
}

std::unique_ptr<ROOTFrameData> RNTupleReader::readNextEntry(const std::string& name,
                                                            const std::vector<std::string>& collsToRead) {
  return readEntry(name, m_entries[name], {collsToRead, false});
}

std::unique_ptr<podio::ROOTFrameData> RNTupleReader::readNextEntry(const std::string& name,
                                                                   const podio::ReadOptions& readOptions) {
  return readEntry(name, m_entries[name], readOptions);
}

std::unique_ptr<ROOTFrameData> RNTupleReader::readEntry(const std::string& category, const unsigned entNum,
                                                        const std::vector<std::string>& collsToRead) {
  return readEntry(category, entNum, {collsToRead, false});
}

std::unique_ptr<ROOTFrameData> RNTupleReader::readEntry(const std::string& category, const unsigned entNum,
                                                        const podio::ReadOptions& readOptions) {
  if (m_totalEntries.find(category) == m_totalEntries.end()) {
    getEntries(category);
  }
  if (entNum >= m_totalEntries[category]) {
    return nullptr;
  }

  if (m_collectionInfo.find(category) == m_collectionInfo.end()) {
    if (!initCategory(category)) {
      return nullptr;
    }
  }

  const auto& collInfo = m_collectionInfo[category];
  // Make sure to not silently ignore non-existant but requested collections
  if (!readOptions.collsToRead.empty()) {
    for (const auto& name : readOptions.collsToRead) {
      if (std::ranges::find(collInfo, name, &root_utils::CollectionWriteInfo::name) == collInfo.end()) {
        throw std::invalid_argument(name + " is not available from Frame");
      }
    }
  }

  m_entries[category] = entNum + 1;

  // m_readerEntries contains the accumulated entries for all the readers
  // therefore, the first number that is lower or equal to the entry number
  // is at the index of the reader that contains the entry
  const auto upper = std::ranges::upper_bound(m_readerEntries[category], entNum);
  const auto localEntry = entNum - *(upper - 1);
  const auto readerIndex = upper - 1 - m_readerEntries[category].begin();

  ROOTFrameData::BufferMap buffers;
  // We need to create a non-bare entry here, because the entries for the
  // parameters are not explicitly (re)set and we need them default initialized.
  // In principle we would only need a bare entry for the collection data, since
  // we set all the fields there in any case.
  const auto dentry = m_readers[category][readerIndex]->GetModel().CreateEntry();

  for (const auto& coll : collInfo) {
    if (!readOptions.collsToRead.empty() &&
        std::ranges::find(readOptions.collsToRead, coll.name) == readOptions.collsToRead.end()) {
      continue;
    }
    const auto& collType = coll.dataType;
    const auto& bufferFactory = podio::CollectionBufferFactory::instance();
    const auto maybeBuffers = bufferFactory.createBuffers(collType, coll.schemaVersion, coll.isSubset);

    if (!maybeBuffers) {
      std::cout << "WARNING: Buffers couldn't be created for collection " << coll.name << " of type " << coll.dataType
                << " and schema version " << coll.schemaVersion << std::endl;
      if (readOptions.skipUnreadable) {
        continue;
      } else {
        return nullptr;
      }
    }
    const auto& collBuffers = maybeBuffers.value();

    if (coll.isSubset) {
      const auto brName = root_utils::subsetBranch(coll.name);
      const auto vec = new std::vector<podio::ObjectID>;
      dentry->BindRawPtr(brName, vec);
      collBuffers.references->at(0) = std::unique_ptr<std::vector<podio::ObjectID>>(vec);
    } else {
      dentry->BindRawPtr(coll.name, collBuffers.data);

      const auto relVecNames = podio::DatamodelRegistry::instance().getRelationNames(collType);
      for (size_t j = 0; j < relVecNames.relations.size(); ++j) {
        const auto relName = relVecNames.relations[j];
        const auto vec = new std::vector<podio::ObjectID>;
        const auto brName = root_utils::refBranch(coll.name, relName);
        dentry->BindRawPtr(brName, vec);
        collBuffers.references->at(j) = std::unique_ptr<std::vector<podio::ObjectID>>(vec);
      }

      for (size_t j = 0; j < relVecNames.vectorMembers.size(); ++j) {
        const auto vecName = relVecNames.vectorMembers[j];
        const auto brName = root_utils::vecBranch(coll.name, vecName);
        dentry->BindRawPtr(brName, collBuffers.vectorMembers->at(j).second);
      }
    }

    buffers.emplace(coll.name, std::move(collBuffers));
  }

  m_readers[category][readerIndex]->LoadEntry(localEntry, *dentry);

  auto parameters = readEventMetaData(category, localEntry, readerIndex);

  return std::make_unique<ROOTFrameData>(std::move(buffers), m_idTables[category], std::move(parameters));
}

} // namespace podio

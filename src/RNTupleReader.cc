#include "podio/RNTupleReader.h"
#include "podio/CollectionBufferFactory.h"
#include "podio/CollectionBuffers.h"
#include "podio/DatamodelRegistry.h"
#include "podio/GenericParameters.h"
#include "podio/ObjectID.h"
#include "podio/podioVersion.h"
#include "podio/utilities/DatamodelRegistryIOHelpers.h"
#include "podio/utilities/MiscHelpers.h"
#include "podio/utilities/RootHelpers.h"
#include "rootUtils.h"

#include <ROOT/RError.hxx>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
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
RNTupleReader::ParameterViews<T>::ParameterViews(root_compat::RNTupleReader& reader) :
    keys(reader.GetView<std::vector<std::string>>(root_utils::getGPKeyName<T>())),
    values(reader.GetView<std::vector<std::vector<T>>>(root_utils::getGPValueName<T>())) {
}

RNTupleReader::FrameParameterViews::FrameParameterViews(root_compat::RNTupleReader& reader) :
    ints(reader), floats(reader), doubles(reader), strings(reader) {
}

namespace {
  void readParams(auto& views, const unsigned localEntry, GenericParameters& params) {
    params.loadFrom(views.keys(localEntry), views.values(localEntry));
  }
} // namespace

GenericParameters RNTupleReader::readEventMetaData(FrameParameterViews& views, const unsigned localEntry) {
  GenericParameters params;

  readParams(views.ints, localEntry, params);
  readParams(views.floats, localEntry, params);
  readParams(views.doubles, localEntry, params);
  readParams(views.strings, localEntry, params);

  return params;
}

bool RNTupleReader::initCategory(std::string_view category) {
  const auto it = std::ranges::find(m_availableCategories, category);
  if (it == m_availableCategories.end()) {
    return false;
  }
  const std::string_view stableCategory = *it; // points into m_availableCategories
  // Assume that the metadata is the same in all files
  const auto& filename = m_filenames[0];

  auto collInfo = m_metadata_readers[filename]->GetView<std::vector<root_utils::CollectionWriteInfo>>(
      {root_utils::collInfoName(stableCategory)});

  m_collectionInfo[stableCategory] = collInfo(0);
  m_idTables[stableCategory] = root_utils::makeCollIdTable(collInfo(0));

  return true;
}

void RNTupleReader::openFile(const std::string& filename) {
  openFiles({filename});
}

void RNTupleReader::openFiles(const std::vector<std::string>& filenames) {
  if (m_metadata) {
    throw std::logic_error("RNTupleReader: files have already been opened with this reader");
  }

  decltype(m_metadata_readers) metadataReaders{};
  for (const auto& filename : filenames) {
    metadataReaders.try_emplace(filename, root_compat::RNTupleReader::Open(root_utils::metaTreeName, filename));
  }

  auto metadata = root_compat::RNTupleReader::Open(root_utils::metaTreeName, filenames[0]);

  auto versionView = metadata->GetView<std::vector<uint16_t>>(root_utils::versionBranchName);
  const auto version = versionView(0);

  m_fileVersion = podio::version::Version{version[0], version[1], version[2]};

  auto edmView = metadata->GetView<std::vector<std::tuple<std::string, std::string>>>(root_utils::edmDefBranchName);
  auto edm = edmView(0);
  DatamodelDefinitionHolder::VersionList edmVersions{};
  for (const auto& [name, _] : edm) {
    try {
      auto edmVersionView = metadata->GetView<std::vector<uint16_t>>(root_utils::edmVersionBranchName(name));
      const auto edmVersion = edmVersionView(0);
      edmVersions.emplace_back(name, podio::version::Version{edmVersion[0], edmVersion[1], edmVersion[2]});
    } catch (const RException&) {
    }
  }
  m_datamodelHolder = DatamodelDefinitionHolder(std::move(edm), std::move(edmVersions));

  auto availableCategoriesField = metadata->GetView<std::vector<std::string>>(root_utils::availableCategories);
  auto availableCategories = availableCategoriesField(0);
  std::ranges::sort(availableCategories);

  m_metadata_readers = std::move(metadataReaders);
  m_filenames = filenames;
  m_availableCategories = std::move(availableCategories);

  // Pre-fill the entries map
  for (const auto& category : m_availableCategories) {
    m_readerEntries[category].reserve(m_filenames.size() + 1);
    m_readerEntries[category].push_back(0);
    for (const auto& filename : m_filenames) {
      try {
        // Models are not needed: collection fields and parameters are read via
        // views, so unknown types in skipped collections need not be emulated.
        m_readers[category].push_back({root_compat::RNTupleReader::Open(category, filename), {}});
        m_readerEntries[category].push_back(m_readerEntries[category].back() +
                                            m_readers[category].back().reader->GetNEntries());
      } catch (const RException&) {
        std::cout << "Category " << category << " not found in file " << filename << std::endl;
      }
    }
    m_totalEntries[category] = m_readerEntries[category].back();
    // The last entry is not needed since it's the total number of entries
    m_readerEntries[category].pop_back();
  }

  // Only set this at the very end, since it marks this reader as having opened
  // files and a failed attempt should not do that
  m_metadata = std::move(metadata);
}

unsigned RNTupleReader::getEntries(std::string_view name) const {
  if (const auto it = m_totalEntries.find(name); it != m_totalEntries.end()) {
    return it->second;
  }
  return 0;
}

std::unique_ptr<ROOTFrameData> RNTupleReader::readNextEntry(std::string_view category,
                                                            const std::vector<std::string>& collsToRead) {
  // m_totalEntries is keyed by stable string_views (into m_availableCategories),
  // so a single O(1) lookup both validates the category and yields the stable key.
  const auto it = m_totalEntries.find(category);
  if (it == m_totalEntries.end()) {
    return nullptr;
  }
  const std::string_view stableCategory = it->first;
  return readEntry(stableCategory, m_entries[stableCategory], collsToRead);
}

std::unique_ptr<ROOTFrameData> RNTupleReader::readEntry(std::string_view category, const unsigned entNum,
                                                        const std::vector<std::string>& collsToRead) {
  // Deduplicate collections to read
  const auto dedupedCollsToRead =
      collsToRead.empty() ? std::vector<std::string>{} : podio::utils::sortAndDeduplicate(collsToRead, "collsToRead");

  auto collInfoIt = m_collectionInfo.find(category);
  if (collInfoIt == m_collectionInfo.end()) {
    if (!initCategory(category)) {
      return nullptr;
    }
    collInfoIt = m_collectionInfo.find(category);
  }
  // Use the map key (which points into m_availableCategories) as the stable
  // string_view to avoid dangling keys when the caller's backing string is gone.
  const std::string_view stableCategory = collInfoIt->first;

  const auto maxCatEvents = getEntries(stableCategory);
  if (entNum >= maxCatEvents) {
    return nullptr;
  }

  const auto& collInfo = m_collectionInfo[stableCategory];
  // Make sure to not silently ignore non-existant but requested collections
  if (!dedupedCollsToRead.empty()) {
    for (const auto& name : dedupedCollsToRead) {
      if (std::ranges::find(collInfo, name, &root_utils::CollectionWriteInfo::name) == collInfo.end()) {
        throw std::invalid_argument(name + " is not available from Frame");
      }
    }
  }

  m_entries[stableCategory] = entNum + 1;

  // m_readerEntries contains the accumulated entries for all the readers
  // therefore, the first number that is lower or equal to the entry number
  // is at the index of the reader that contains the entry
  const auto& readerEntries = m_readerEntries[stableCategory];
  const auto upper = std::ranges::upper_bound(readerEntries, entNum);
  const auto localEntry = entNum - *(upper - 1);
  const auto readerIndex = upper - 1 - readerEntries.begin();
  auto& readerInfo = m_readers[stableCategory][readerIndex];
  const auto& reader = readerInfo.reader;

  ROOTFrameData::BufferMap buffers;
  for (const auto& coll : collInfo) {
    if (!dedupedCollsToRead.empty() && std::ranges::find(dedupedCollsToRead, coll.name) == dedupedCollsToRead.end()) {
      continue;
    }
    auto viewsIt = readerInfo.collectionViews.find(coll.name);
    if (viewsIt != readerInfo.collectionViews.end() && !viewsIt->second) {
      // Binding already failed for this file, skip without creating buffers
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

    if (viewsIt == readerInfo.collectionViews.end()) {
      // Only connect fields of this collection. Cache the views once all fields
      // bind successfully, so a failure cannot leave partially connected views.
      CollectionViews views;
      try {
        if (coll.isSubset) {
          const auto brName = root_utils::subsetBranch(coll.name);
          views.references.emplace_back(0, reader->GetView(brName, collBuffers.references->at(0).get()));
        } else {
          views.data.emplace_back(reader->GetView<void>(coll.name, collBuffers.data));

          const auto relVecNames = podio::DatamodelRegistry::instance().getRelationNames(collType);
          for (size_t j = 0; j < relVecNames.relations.size(); ++j) {
            // Relations that are not stored in this schema version have no view.
            if (!collBuffers.references->at(j)) {
              continue;
            }
            const auto brName = root_utils::refBranch(coll.name, relVecNames.relations[j]);
            views.references.emplace_back(j, reader->GetView(brName, collBuffers.references->at(j).get()));
          }

          for (size_t j = 0; j < relVecNames.vectorMembers.size(); ++j) {
            const auto brName = root_utils::vecBranch(coll.name, relVecNames.vectorMembers[j]);
            views.data.emplace_back(reader->GetView<void>(brName, collBuffers.vectorMembers->at(j).second));
          }
        }
      } catch (const RException&) {
        // No fields have been read yet. Discard the views before collBuffers
        // cleans up, without reading any part of this collection, and remember
        // the failure to avoid retrying the binding for every entry.
        readerInfo.collectionViews.emplace(coll.name, std::nullopt);
        continue;
      }
      viewsIt = readerInfo.collectionViews.emplace(coll.name, std::move(views)).first;
    }

    // Missing relations in an older schema still need empty buffers in every
    // event, including when reusing views connected for an earlier event.
    if (collBuffers.references) {
      for (auto& ref : *collBuffers.references) {
        if (!ref) {
          ref = std::make_unique<std::vector<podio::ObjectID>>();
        }
      }
    }

    auto& views = *viewsIt->second;
    if (!coll.isSubset) {
      views.data.front().BindRawPtr(collBuffers.data);
      for (size_t j = 1; j < views.data.size(); ++j) {
        views.data[j].BindRawPtr(collBuffers.vectorMembers->at(j - 1).second);
      }
    }
    for (auto& [index, view] : views.references) {
      view.BindRawPtr(collBuffers.references->at(index).get());
    }

    // Keep read errors outside the binding catch: a collection that was bound
    // successfully must not be silently dropped if deserialization fails.
    // Cached views borrow these buffers; they are rebound before the next read.
    for (auto& view : views.data) {
      view(localEntry);
    }
    for (auto& [index, view] : views.references) {
      view(localEntry);
    }

    buffers.emplace(coll.name, std::move(collBuffers));
  }

  if (!readerInfo.parameterViews) {
    readerInfo.parameterViews.emplace(*reader);
  }
  auto parameters = readEventMetaData(*readerInfo.parameterViews, localEntry);

  return std::make_unique<ROOTFrameData>(std::move(buffers), m_idTables[category], std::move(parameters));
}

} // namespace podio

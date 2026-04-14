#include "podio/RNTupleLazyReader.h"
#include "podio/GenericParameters.h"
#include "podio/utilities/RootHelpers.h"

#include "RNTupleLazyCategoryState.h"
#include "rntuple_utils.h"
#include "rootUtils.h"

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

namespace podio {

bool RNTupleLazyReader::initCategory(std::string_view category) {
  auto& state = m_categoryStates[category];
  if (!state) {
    return false;
  }
  return initCategoryCommon(category, state->collectionInfo, state->idTable);
}

void RNTupleLazyReader::openFile(const std::string& filename) {
  openFiles({filename});
}

void RNTupleLazyReader::openFiles(const std::vector<std::string>& filenames) {
  openMetaData(filenames, m_fileVersion, m_datamodelHolder);

  // For each category, create a shared state and open one full reader per file
  for (const auto& category : m_availableCategories) {
    auto state = std::make_shared<RNTupleCategoryState>();
    state->category = category;
    state->readerEntries.reserve(m_filenames.size() + 1);
    state->readerEntries.push_back(0);

    for (const auto& filename : m_filenames) {
      try {
#if ROOT_VERSION_CODE >= ROOT_VERSION(6, 36, 0)
        ROOT::RNTupleDescriptor::RCreateModelOptions options;
        options.SetEmulateUnknownTypes(true);
        state->readers.emplace_back(root_compat::RNTupleReader::Open(options, category, filename));
#else
        state->readers.emplace_back(root_compat::RNTupleReader::Open(category, filename));
#endif
        state->filenames.emplace_back(filename);
        state->readerEntries.push_back(state->readerEntries.back() + state->readers.back()->GetNEntries());
      } catch (const RException&) {
        std::cout << "Category " << category << " not found in file " << filename << std::endl;
      }
    }

    state->totalEntries = state->readerEntries.back();
    // The last element is the total; drop it so the vector only contains start offsets
    state->readerEntries.pop_back();

    m_categoryStates.try_emplace(category, std::move(state));
  }
}

unsigned RNTupleLazyReader::getEntries(std::string_view name) const {
  if (const auto it = m_categoryStates.find(name); it != m_categoryStates.end()) {
    return it->second->totalEntries;
  }
  return 0;
}

std::unique_ptr<RNTupleLazyFrameData> RNTupleLazyReader::readNextEntry(std::string_view name,
                                                                       const std::vector<std::string>& collsToRead) {
  if (const auto it = m_categoryStates.find(name); it != m_categoryStates.end()) {
    return readEntry(name, it->second->entry, collsToRead);
  }
  return nullptr;
}

std::unique_ptr<RNTupleLazyFrameData> RNTupleLazyReader::readEntry(std::string_view category, const unsigned entNum,
                                                                   const std::vector<std::string>& collsToRead) {
  // Lazy-initialize collection info on first access for this category
  const auto stateIt = m_categoryStates.find(category);
  if (stateIt == m_categoryStates.end()) {
    return nullptr;
  }
  auto& state = stateIt->second;

  if (state->collectionInfo.empty()) {
    if (!initCategory(category)) {
      return nullptr;
    }
  }

  if (entNum >= state->totalEntries) {
    return nullptr;
  }

  const auto& collInfo = state->collectionInfo;
  if (!collsToRead.empty()) {
    for (const auto& name : collsToRead) {
      if (std::ranges::find(collInfo, name, &root_utils::CollectionWriteInfo::name) == collInfo.end()) {
        throw std::invalid_argument(name + " is not available from Frame");
      }
    }
  }

  // Multi-file dispatch to find the right reader and local entry
  const auto& readerEntries = state->readerEntries;
  const auto upper = std::ranges::upper_bound(readerEntries, entNum);
  const auto localEntry = entNum - *(upper - 1);
  const auto readerIndex = static_cast<size_t>(upper - 1 - readerEntries.begin());

  // Read parameters eagerly (they are small and almost always needed)
  GenericParameters parameters;
  {
    std::lock_guard lock{state->mutex};
    parameters = readEventMetaData(state->readers[readerIndex].get(), localEntry);
  }

  // Build available collections map: name -> index in collectionInfo
  std::unordered_map<std::string, size_t> availableCollections;
  for (size_t i = 0; i < collInfo.size(); ++i) {
    if (!collsToRead.empty() && std::ranges::find(collsToRead, collInfo[i].name) == collsToRead.end()) {
      continue;
    }
    availableCollections.emplace(collInfo[i].name, i);
  }

  state->entry = entNum + 1;

  return std::make_unique<RNTupleLazyFrameData>(state, entNum, std::move(availableCollections), state->idTable,
                                               std::move(parameters));
}

} // namespace podio

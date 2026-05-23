#include "podio/ROOTLazyReader.h"
#include "podio/GenericParameters.h"
#include "podio/utilities/RootHelpers.h"

#include "ROOTLazyCategoryState.h"
#include "rootUtils.h"

// ROOT specific includes
#include "TChain.h"

#include <algorithm>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace podio {

std::unique_ptr<ROOTLazyFrameData> ROOTLazyReader::readNextEntry(std::string_view name,
                                                                 const std::vector<std::string>& collsToRead) {
  auto& catState = getCategoryState(name);
  if (!catState) {
    return nullptr;
  }
  auto result = readEntry(catState, catState->entry, collsToRead);
  if (result) {
    catState->entry++;
  }
  return result;
}

std::unique_ptr<ROOTLazyFrameData> ROOTLazyReader::readEntry(std::string_view name, const unsigned entry,
                                                             const std::vector<std::string>& collsToRead) {
  auto& catState = getCategoryState(name);
  auto result = readEntry(catState, entry, collsToRead);
  if (result) {
    catState->entry = entry + 1;
  }
  return result;
}

std::unique_ptr<ROOTLazyFrameData> ROOTLazyReader::readEntry(std::shared_ptr<CategoryState>& catState, unsigned entry,
                                                             const std::vector<std::string>& collsToRead) {
  if (!catState || !catState->chain) {
    return nullptr;
  }
  if (entry >= catState->chain->GetEntries()) {
    return nullptr;
  }

  // Validate requested collections
  if (!collsToRead.empty()) {
    for (const auto& name : collsToRead) {
      if (std::ranges::find(catState->storedClasses, name, &NamedCollInfo::name) == catState->storedClasses.end()) {
        throw std::invalid_argument(name + " is not available from Frame");
      }
    }
  }

  podio::GenericParameters parameters;
  {
    std::lock_guard lock{catState->mutex};

    const auto localEntry = catState->chain->LoadTree(entry);

    // Read parameters eagerly
    parameters = readEntryParameters(catState->paramBranches, catState->chain.get(), m_fileVersion,
                                     /*reloadBranches=*/true, localEntry);
  }

  // Build available collections map (no lock needed, just reading metadata)
  std::unordered_map<std::string, size_t> availableCollections;
  for (size_t i = 0; i < catState->storedClasses.size(); ++i) {
    if (!collsToRead.empty() && std::ranges::find(collsToRead, catState->storedClasses[i].name) == collsToRead.end()) {
      continue;
    }
    availableCollections.emplace(catState->storedClasses[i].name, i);
  }

  return std::make_unique<ROOTLazyFrameData>(catState, entry, std::move(availableCollections), catState->table,
                                             std::move(parameters));
}

std::shared_ptr<CategoryState>& ROOTLazyReader::getCategoryState(std::string_view category) {
  if (auto it = m_categoryStates.find(category); it != m_categoryStates.end()) {
    // Use branches as proxy to check whether this category has been initialized
    if (it->second->branches.empty()) {
      root_utils::initCategory(*it->second, m_metaChain.get(), category, m_fileVersion);
    }
    return it->second;
  }

  // Return a static nullptr for unknown categories
  static auto invalidState = std::shared_ptr<CategoryState>(nullptr);
  return invalidState;
}

void ROOTLazyReader::openFile(const std::string& filename) {
  openFiles({filename});
}

void ROOTLazyReader::openFiles(const std::vector<std::string>& filenames) {
  openMetaChain(filenames, m_fileVersion, m_datamodelHolder);

  // Set up categories and their chains
  for (const auto& cat : m_availCategories) {
    auto state = std::make_shared<CategoryState>();
    state->chain = std::make_unique<TChain>(cat.c_str());
    for (const auto& fn : filenames) {
      state->chain->Add(fn.c_str());
    }
    m_categoryStates.try_emplace(cat, std::move(state));
  }
}

unsigned ROOTLazyReader::getEntries(std::string_view name) const {
  if (const auto it = m_categoryStates.find(name); it != m_categoryStates.end()) {
    return it->second->chain->GetEntries();
  }
  return 0;
}

std::vector<std::string_view> ROOTLazyReader::getAvailableCategories() const {
  std::vector<std::string_view> cats;
  cats.reserve(m_categoryStates.size());
  for (const auto& [cat, _] : m_categoryStates) {
    cats.emplace_back(cat);
  }
  return cats;
}

} // namespace podio

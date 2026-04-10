#include "podio/ROOTLazyFrameData.h"
#include "podio/CollectionBufferFactory.h"
#include "podio/CollectionBuffers.h"
#include "podio/CollectionIDTable.h"
#include "podio/GenericParameters.h"

#include "ROOTLazyCategoryState.h"
#include "rootUtils.h"

#include "TChain.h"

#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace podio {

ROOTLazyFrameData::ROOTLazyFrameData(std::shared_ptr<CategoryState> state, unsigned entry,
                                     std::unordered_map<std::string, size_t>&& availableCollections, CollIDPtr idTable,
                                     podio::GenericParameters&& params) :
    m_state(std::move(state)),
    m_entry(entry),
    m_availableCollections(std::move(availableCollections)),
    m_idTable(std::move(idTable)),
    m_parameters(std::move(params)) {
}

std::optional<podio::CollectionReadBuffers> ROOTLazyFrameData::getCollectionBuffers(const std::string& name) {
  const auto it = m_availableCollections.find(name);
  if (it == m_availableCollections.end()) {
    return std::nullopt;
  }

  const auto collIndex = it->second;
  const auto& [collType, isSubsetColl, schemaVersion, branchIndex] = m_state->storedClasses[collIndex].info;
  auto& branches = m_state->branches[branchIndex];

  const auto& bufferFactory = podio::CollectionBufferFactory::instance();
  auto maybeBuffers = bufferFactory.createBuffers(collType, schemaVersion, isSubsetColl);
  if (!maybeBuffers) {
    std::cerr << "WARNING: Buffers couldn't be created for collection " << name << " of type " << collType
              << " and schema version " << schemaVersion << std::endl;
    return std::nullopt;
  }
  auto& collBuffers = maybeBuffers.value();

  {
    std::lock_guard lock{m_state->mutex};

    const auto localEntry = m_state->chain->LoadTree(m_entry);

    // Always refresh branch pointers for this collection since any intervening
    // LoadTree call (from another ROOTLazyFrameData) may have invalidated them
    root_utils::resetBranches(m_state->chain.get(), branches, m_state->storedClasses[collIndex].name);

    if (!root_utils::setCollectionAddressesReader(collBuffers, branches)) {
      return std::nullopt;
    }
    root_utils::readBranchesData(branches, localEntry);
  }

  m_availableCollections.erase(it);
  return {std::move(collBuffers)};
}

podio::CollectionIDTable ROOTLazyFrameData::getIDTable() const {
  return {m_idTable->ids(), m_idTable->names()};
}

std::unique_ptr<podio::GenericParameters> ROOTLazyFrameData::getParameters() {
  return std::make_unique<podio::GenericParameters>(std::move(m_parameters));
}

std::vector<std::string> ROOTLazyFrameData::getAvailableCollections() const {
  std::vector<std::string> collections;
  collections.reserve(m_availableCollections.size());
  for (const auto& [name, _] : m_availableCollections) {
    collections.push_back(name);
  }
  return collections;
}

} // namespace podio

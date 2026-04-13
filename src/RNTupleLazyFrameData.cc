#include "podio/RNTupleLazyFrameData.h"
#include "podio/CollectionBufferFactory.h"
#include "podio/CollectionBuffers.h"
#include "podio/CollectionIDTable.h"
#include "podio/DatamodelRegistry.h"
#include "podio/GenericParameters.h"

#include "RNTupleLazyCategoryState.h"
#include "rootUtils.h"

#include <ROOT/RCreateFieldOptions.hxx>
#include <ROOT/RError.hxx>
#include <ROOT/RNTupleDescriptor.hxx>
#include <ROOT/RNTupleModel.hxx>
#include <RVersion.h>

#include <algorithm>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
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

RNTupleLazyFrameData::RNTupleLazyFrameData(std::shared_ptr<RNTupleCategoryState> state, unsigned entry,
                                           std::unordered_map<std::string, size_t>&& availableCollections,
                                           CollIDPtr idTable, podio::GenericParameters&& params) :
    m_state(std::move(state)),
    m_entry(entry),
    m_availableCollections(std::move(availableCollections)),
    m_idTable(std::move(idTable)),
    m_parameters(std::move(params)) {
}

std::optional<podio::CollectionReadBuffers> RNTupleLazyFrameData::getCollectionBuffers(const std::string& name) {
  const auto it = m_availableCollections.find(name);
  if (it == m_availableCollections.end()) {
    return std::nullopt;
  }

  const auto collIndex = it->second;
  const auto& coll = m_state->collectionInfo[collIndex];
  const auto& collType = coll.dataType;

  const auto& bufferFactory = podio::CollectionBufferFactory::instance();
  auto maybeBuffers = bufferFactory.createBuffers(collType, coll.schemaVersion, coll.isSubset);
  if (!maybeBuffers) {
    std::cerr << "WARNING: Buffers couldn't be created for collection " << name << " of type " << collType
              << " and schema version " << coll.schemaVersion << std::endl;
    return std::nullopt;
  }
  auto& collBuffers = maybeBuffers.value();

  {
    std::lock_guard lock{m_state->mutex};

    // Multi-file dispatch: find which reader contains this entry and what the
    // local entry index is within that reader
    const auto& readerEntries = m_state->readerEntries;
    const auto upper = std::ranges::upper_bound(readerEntries, m_entry);
    const auto localEntry = m_entry - *(upper - 1);
    const auto readerIndex = static_cast<size_t>(upper - 1 - readerEntries.begin());

    // Get or create a partial reader for (readerIndex, collectionName).
    // Each partial reader has a minimal model with only the fields for one
    // collection, so LoadEntry() only reads that collection's data from disk.
    const auto cacheKey = std::make_pair(readerIndex, name);
    auto partialIt = m_state->partialReaders.find(cacheKey);
    if (partialIt == m_state->partialReaders.end()) {
      // Compute the RNTuple field names needed for this collection
      std::vector<std::string> neededFieldNames;
      if (coll.isSubset) {
        neededFieldNames.emplace_back(root_utils::subsetBranch(coll.name));
      } else {
        neededFieldNames.emplace_back(coll.name);
        const auto relVecNames = podio::DatamodelRegistry::instance().getRelationNames(collType);
        for (const auto& relName : relVecNames.relations) {
          neededFieldNames.emplace_back(root_utils::refBranch(coll.name, relName));
        }
        for (const auto& vecName : relVecNames.vectorMembers) {
          neededFieldNames.emplace_back(root_utils::vecBranch(coll.name, vecName));
        }
      }

      // Build a minimal RNTupleModel from the full reader's descriptor,
      // containing only the fields needed for this collection
      auto& fullReader = *m_state->readers[readerIndex];
      const auto& desc = fullReader.GetDescriptor();

      ROOT::RCreateFieldOptions fieldOpts;
      fieldOpts.SetEmulateUnknownTypes(true);
      fieldOpts.SetReturnInvalidOnError(true);

      auto smallModel = ROOT::RNTupleModel::CreateBare();
      const auto& topFieldDesc = desc.GetFieldDescriptor(desc.GetFieldZeroId());
      for (const auto& fieldDesc : desc.GetFieldIterable(topFieldDesc)) {
        const auto& fn = fieldDesc.GetFieldName();
        if (std::ranges::find(neededFieldNames, fn) != neededFieldNames.end()) {
          auto field = fieldDesc.CreateField(desc, fieldOpts);
          if (field) {
            smallModel->AddField(std::move(field));
          }
        }
      }
      smallModel->Freeze();

      const auto& filename = m_state->filenames[readerIndex];
      auto partialReader = root_compat::RNTupleReader::Open(std::move(smallModel), m_state->category, filename);
      auto [insertIt, _] = m_state->partialReaders.emplace(cacheKey, std::move(partialReader));
      partialIt = insertIt;
    }

    auto& partialReader = *partialIt->second;
    const auto dentry = partialReader.GetModel().CreateEntry();

    try {
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
    } catch (const RException&) {
      // Disable automatic cleanup to avoid double-free if ROOT partially
      // cleaned up during a failed BindRawPtr
      collBuffers.deleteBuffers = {};
      return std::nullopt;
    }

    partialReader.LoadEntry(localEntry, *dentry);
  } // mutex released here

  m_availableCollections.erase(it);
  return {std::move(collBuffers)};
}

podio::CollectionIDTable RNTupleLazyFrameData::getIDTable() const {
  return {m_idTable->ids(), m_idTable->names()};
}

std::unique_ptr<podio::GenericParameters> RNTupleLazyFrameData::getParameters() {
  return std::make_unique<podio::GenericParameters>(std::move(m_parameters));
}

std::vector<std::string> RNTupleLazyFrameData::getAvailableCollections() const {
  std::vector<std::string> collections;
  collections.reserve(m_availableCollections.size());
  for (const auto& [name, _] : m_availableCollections) {
    collections.push_back(name);
  }
  return collections;
}

} // namespace podio

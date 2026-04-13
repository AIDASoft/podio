#include "podio/utilities/RootHelpers.h"
#include "podio/CollectionIDTable.h"

#include "rootUtils.h"

#include "TChain.h"

namespace podio::root_utils {
GenericParameters
loadParamsFrom(ROOT::VecOps::RVec<std::string> intKeys, ROOT::VecOps::RVec<std::vector<int>> intValues,
               ROOT::VecOps::RVec<std::string> floatKeys, ROOT::VecOps::RVec<std::vector<float>> floatValues,
               ROOT::VecOps::RVec<std::string> doubleKeys, ROOT::VecOps::RVec<std::vector<double>> doubleValues,
               ROOT::VecOps::RVec<std::string> stringKeys, ROOT::VecOps::RVec<std::vector<std::string>> stringValues) {
  GenericParameters params{};
  params.loadFrom(std::move(intKeys), std::move(intValues));
  params.loadFrom(std::move(floatKeys), std::move(floatValues));
  params.loadFrom(std::move(doubleKeys), std::move(doubleValues));
  params.loadFrom(std::move(stringKeys), std::move(stringValues));
  return params;
}

std::tuple<std::vector<root_utils::CollectionBranches>, std::vector<TTreeReaderCommon::NamedCollInfo>>
TTreeReaderCommon::createCollectionBranches(TChain* chain, const podio::CollectionIDTable& idTable,
                                            const std::vector<root_utils::CollectionWriteInfo>& collInfo) {
  size_t collectionIndex{0};
  std::vector<root_utils::CollectionBranches> collBranches;
  collBranches.reserve(collInfo.size() + 1);
  std::vector<NamedCollInfo> storedClasses;
  storedClasses.reserve(collInfo.size());

  for (const auto& [collID, collType, isSubsetColl, collSchemaVersion, _, __] : collInfo) {
    // We only write collections that are in the collectionIDTable, so no need
    // to check here
    const auto name = idTable.name(collID).value();

    root_utils::CollectionBranches branches{};
    if (isSubsetColl) {
      // Only one branch will exist and we can trivially get its name
      const auto brName = root_utils::subsetBranch(name);
      branches.refs.push_back(root_utils::getBranch(chain, brName.c_str()));
      branches.refNames.emplace_back(std::move(brName));
    } else {
      // This branch is guaranteed to exist since only collections that are
      // also written to file are in the info metadata that we work with here
      branches.data = root_utils::getBranch(chain, name.c_str());

      const auto relVecNames = podio::DatamodelRegistry::instance().getRelationNames(collType);
      for (const auto& relName : relVecNames.relations) {
        const auto brName = root_utils::refBranch(name, relName);
        branches.refs.push_back(root_utils::getBranch(chain, brName.c_str()));
        branches.refNames.emplace_back(std::move(brName));
      }
      for (const auto& vecName : relVecNames.vectorMembers) {
        const auto brName = root_utils::refBranch(name, vecName);
        branches.vecs.push_back(root_utils::getBranch(chain, brName.c_str()));
        branches.vecNames.emplace_back(std::move(brName));
      }
    }

    storedClasses.emplace_back(name, std::make_tuple(collType, isSubsetColl, collSchemaVersion, collectionIndex++));
    collBranches.emplace_back(std::move(branches));
  }

  return {std::move(collBranches), storedClasses};
}

std::tuple<std::vector<podio::root_utils::CollectionBranches>, std::vector<TTreeReaderCommon::NamedCollInfo>>
TTreeReaderCommon::createCollectionBranchesIndexBased(
    TChain* chain, const podio::CollectionIDTable& idTable,
    const std::vector<podio::root_utils::CollectionWriteInfo>& collInfo) {
  size_t collectionIndex{0};
  std::vector<root_utils::CollectionBranches> collBranches;
  collBranches.reserve(collInfo.size() + 1);
  std::vector<TTreeReaderCommon::NamedCollInfo> storedClasses;
  storedClasses.reserve(collInfo.size());

  for (const auto& [collID, collType, isSubsetColl, collSchemaVersion, _, __] : collInfo) {
    // We only write collections that are in the collectionIDTable, so no need
    // to check here
    const auto name = idTable.name(collID).value();

    const auto collectionClass = TClass::GetClass(collType.c_str());
    // Need the collection here to setup all the branches. Have to manage the
    // temporary collection ourselves
    const auto collection =
        std::unique_ptr<podio::CollectionBase>(static_cast<podio::CollectionBase*>(collectionClass->New()));
    root_utils::CollectionBranches branches{};
    if (isSubsetColl) {
      // Only one branch will exist and we can trivially get its name
      const auto brName = root_utils::refBranch(name, 0);
      branches.refs.push_back(root_utils::getBranch(chain, brName.c_str()));
      branches.refNames.emplace_back(std::move(brName));
    } else {
      // This branch is guaranteed to exist since only collections that are
      // also written to file are in the info metadata that we work with here
      branches.data = root_utils::getBranch(chain, name.c_str());

      const auto buffers = collection->getBuffers();
      for (size_t i = 0; i < buffers.references->size(); ++i) {
        const auto brName = root_utils::refBranch(name, i);
        branches.refs.push_back(root_utils::getBranch(chain, brName.c_str()));
        branches.refNames.emplace_back(std::move(brName));
      }

      for (size_t i = 0; i < buffers.vectorMembers->size(); ++i) {
        const auto brName = root_utils::vecBranch(name, i);
        branches.vecs.push_back(root_utils::getBranch(chain, brName.c_str()));
        branches.vecNames.emplace_back(std::move(brName));
      }
    }

    storedClasses.emplace_back(name, std::make_tuple(collType, isSubsetColl, collSchemaVersion, collectionIndex++));
    collBranches.emplace_back(std::move(branches));
  }

  return {std::move(collBranches), storedClasses};
}

} // namespace podio::root_utils

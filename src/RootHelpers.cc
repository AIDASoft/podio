#include "podio/utilities/RootHelpers.h"
#include "podio/CollectionIDTable.h"
#include "podio/utilities/DatamodelRegistryIOHelpers.h"

#include "rootUtils.h"

#include "TChain.h"

#include <stdexcept>

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

void TTreeReaderCommon::openMetaChain(const std::vector<std::string>& filenames,
                                      podio::version::Version& fileVersion,
                                      podio::DatamodelDefinitionHolder& datamodelHolder) {
  m_metaChain = std::make_unique<TChain>(root_utils::metaTreeName);
  // NOTE: We simply assume that the meta data doesn't change throughout the
  // chain! This essentially boils down to the assumption that all files that
  // are read this way were written with the same settings.
  // Reading all files is done to check that all file exists
  for (const auto& filename : filenames) {
    if (!m_metaChain->Add(filename.c_str(), -1)) {
      throw std::runtime_error("File " + filename + " couldn't be found or the \"" +
                               std::string(root_utils::metaTreeName) + "\" tree couldn't be read.");
    }
  }

  auto* versionPtr = &fileVersion;
  if (auto* versionBranch = root_utils::getBranch(m_metaChain.get(), root_utils::versionBranchName)) {
    versionBranch->SetAddress(&versionPtr);
    versionBranch->GetEntry(0);
  }

  if (auto* edmDefBranch = root_utils::getBranch(m_metaChain.get(), root_utils::edmDefBranchName)) {
    auto datamodelDefs = DatamodelDefinitionHolder::MapType{};
    auto* datamodelDefsPtr = &datamodelDefs;
    edmDefBranch->SetAddress(&datamodelDefsPtr);
    edmDefBranch->GetEntry(0);

    DatamodelDefinitionHolder::VersionList edmVersions{};
    for (const auto& [name, _] : datamodelDefs) {
      if (auto* edmVersionBranch =
              root_utils::getBranch(m_metaChain.get(), root_utils::edmVersionBranchName(name))) {
        const auto edmVersion = podio::version::Version{};
        auto* tmpPtr = &edmVersion;
        edmVersionBranch->SetAddress(&tmpPtr);
        edmVersionBranch->GetEntry(0);
        edmVersions.emplace_back(name, edmVersion);
      }
    }

    datamodelHolder = DatamodelDefinitionHolder(std::move(datamodelDefs), std::move(edmVersions));
  }

  m_availCategories = root_utils::getAvailableCategories(m_metaChain.get());
}

TTreeReaderCommon::CategoryInitResult
TTreeReaderCommon::initCategoryCommon(TChain* categoryChain, std::string_view category,
                                      const podio::version::Version& fileVersion) {
  auto* collInfoBranch = root_utils::getBranch(m_metaChain.get(), root_utils::collInfoName(category));

  auto collInfo = std::vector<root_utils::CollectionWriteInfo>();
  auto* collInfoPtr = &collInfo;
  if (fileVersion >= podio::version::Version{1, 2, 999}) {
    collInfoBranch->SetAddress(&collInfoPtr);
    collInfoBranch->GetEntry(0);
  } else {
    auto collInfoOld = std::vector<root_utils::CollectionWriteInfoT>();
    if (fileVersion < podio::version::Version{0, 16, 4}) {
      auto collInfoReallyOld = std::vector<root_utils::CollectionInfoWithoutSchemaT>();
      auto* tmpPtr = &collInfoReallyOld;
      collInfoBranch->SetAddress(&tmpPtr);
      collInfoBranch->GetEntry(0);
      collInfoOld.reserve(collInfoReallyOld.size());
      for (const auto& [collID, collType, isSubsetColl] : collInfoReallyOld) {
        // Manually set the schema version to 1
        collInfo.emplace_back(collID, std::move(collType), isSubsetColl, 1u);
      }
    } else {
      auto* tmpPtr = &collInfoOld;
      collInfoBranch->SetAddress(&tmpPtr);
      collInfoBranch->GetEntry(0);
    }
    // "Convert" to new style
    collInfo.reserve(collInfoOld.size());
    for (const auto& [id, typeName, isSubsetColl, schemaVersion] : collInfoOld) {
      collInfo.emplace_back(id, std::move(typeName), isSubsetColl, schemaVersion);
    }
  }

  CategoryInitResult result{};

  // Recreate the idTable from the collection info if necessary, otherwise read
  // it directly
  if (fileVersion >= podio::version::Version{1, 2, 999}) {
    result.table = root_utils::makeCollIdTable(collInfo);
  } else {
    result.table = std::make_shared<podio::CollectionIDTable>();
    const auto* table = result.table.get();
    auto* tableBranch = root_utils::getBranch(m_metaChain.get(), root_utils::idTableName(category));
    tableBranch->SetAddress(&table);
    tableBranch->GetEntry(0);
  }

  // For backwards compatibility make it possible to read the index based files
  // from older versions
  if (fileVersion < podio::version::Version{0, 16, 99}) {
    std::tie(result.branches, result.storedClasses) =
        createCollectionBranchesIndexBased(categoryChain, *result.table, collInfo);
  } else {
    std::tie(result.branches, result.storedClasses) =
        createCollectionBranches(categoryChain, *result.table, collInfo);
  }

  // Finally set up the branches for the parameters
  if (fileVersion < podio::version::Version{0, 99, 99}) {
    result.paramBranches.emplace_back(root_utils::getBranch(categoryChain, root_utils::paramBranchName));
  } else {
    result.paramBranches.emplace_back(root_utils::getBranch(categoryChain, root_utils::intKeyName));
    result.paramBranches.emplace_back(root_utils::getBranch(categoryChain, root_utils::intValueName));

    result.paramBranches.emplace_back(root_utils::getBranch(categoryChain, root_utils::floatKeyName));
    result.paramBranches.emplace_back(root_utils::getBranch(categoryChain, root_utils::floatValueName));

    result.paramBranches.emplace_back(root_utils::getBranch(categoryChain, root_utils::doubleKeyName));
    result.paramBranches.emplace_back(root_utils::getBranch(categoryChain, root_utils::doubleValueName));

    result.paramBranches.emplace_back(root_utils::getBranch(categoryChain, root_utils::stringKeyName));
    result.paramBranches.emplace_back(root_utils::getBranch(categoryChain, root_utils::stringValueName));
  }

  return result;
}

podio::GenericParameters
TTreeReaderCommon::readEntryParameters(std::vector<podio::root_utils::CollectionBranches>& paramBranches,
                                       TChain* chain, const podio::version::Version& fileVersion,
                                       bool reloadBranches, unsigned int localEntry) {
  GenericParameters params;

  if (fileVersion < podio::version::Version{0, 99, 99}) {
    // Parameter branch is always the last one
    auto& paramBranch = paramBranches[0];

    // Make sure to have a valid branch pointer after switching trees in the chain
    // as well as on the first event
    if (reloadBranches) {
      paramBranch.data = root_utils::getBranch(chain, root_utils::paramBranchName);
    }
    auto* branch = paramBranch.data;

    auto* emd = &params;
    branch->SetAddress(&emd);
    branch->GetEntry(localEntry);
  } else {
    root_utils::readParams<int>(paramBranches, chain, params, reloadBranches, localEntry);
    root_utils::readParams<float>(paramBranches, chain, params, reloadBranches, localEntry);
    root_utils::readParams<double>(paramBranches, chain, params, reloadBranches, localEntry);
    root_utils::readParams<std::string>(paramBranches, chain, params, reloadBranches, localEntry);
  }

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

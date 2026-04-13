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

void TTreeReaderCommon::openMetaChain(const std::vector<std::string>& filenames, podio::version::Version& fileVersion,
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
      if (auto* edmVersionBranch = root_utils::getBranch(m_metaChain.get(), root_utils::edmVersionBranchName(name))) {
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

podio::GenericParameters
TTreeReaderCommon::readEntryParameters(std::vector<podio::root_utils::CollectionBranches>& paramBranches, TChain* chain,
                                       const podio::version::Version& fileVersion, bool reloadBranches,
                                       unsigned int localEntry) {
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

} // namespace podio::root_utils

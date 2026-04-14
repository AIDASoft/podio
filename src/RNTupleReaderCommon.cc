#include "podio/utilities/RNTupleHelpers.h"
#include "rntuple_utils.h"
#include "rootUtils.h"

#include <algorithm>
#include <cstdint>
#include <string>
#include <tuple>
#include <vector>

namespace podio {

std::vector<std::string_view> RNTupleReaderCommon::getAvailableCategories() const {
  std::vector<std::string_view> cats;
  cats.reserve(m_availableCategories.size());
  for (const auto& cat : m_availableCategories) {
    cats.emplace_back(cat);
  }
  return cats;
}

void RNTupleReaderCommon::openMetaData(const std::vector<std::string>& filenames,
                                       podio::version::Version& fileVersion,
                                       podio::DatamodelDefinitionHolder& datamodelHolder) {
  m_filenames.insert(m_filenames.end(), filenames.begin(), filenames.end());

  for (const auto& filename : filenames) {
    m_metadata_readers.try_emplace(filename, root_compat::RNTupleReader::Open(root_utils::metaTreeName, filename));
  }

  m_metadata = root_compat::RNTupleReader::Open(root_utils::metaTreeName, filenames[0]);

  auto versionView = m_metadata->GetView<std::vector<uint16_t>>(root_utils::versionBranchName);
  const auto version = versionView(0);
  fileVersion = podio::version::Version{version[0], version[1], version[2]};

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
  datamodelHolder = DatamodelDefinitionHolder(std::move(edm), std::move(edmVersions));

  auto availableCategoriesField = m_metadata->GetView<std::vector<std::string>>(root_utils::availableCategories);
  m_availableCategories = availableCategoriesField(0);
}

GenericParameters RNTupleReaderCommon::readEventMetaData(root_compat::RNTupleReader* reader,
                                                         const unsigned localEntry) {
  GenericParameters params;
  rntuple_utils::readParams<int>(reader, localEntry, params);
  rntuple_utils::readParams<float>(reader, localEntry, params);
  rntuple_utils::readParams<double>(reader, localEntry, params);
  rntuple_utils::readParams<std::string>(reader, localEntry, params);
  return params;
}

bool RNTupleReaderCommon::initCategoryCommon(std::string_view category,
                                             std::vector<podio::root_utils::CollectionWriteInfo>& collInfo,
                                             std::shared_ptr<const podio::CollectionIDTable>& idTable) {
  if (std::ranges::find(m_availableCategories, category) == m_availableCategories.end()) {
    return false;
  }

  const auto& filename = m_filenames[0];
  auto collInfoView = m_metadata_readers[filename]->GetView<std::vector<root_utils::CollectionWriteInfo>>(
      {root_utils::collInfoName(category)});

  collInfo = collInfoView(0);
  idTable = root_utils::makeCollIdTable(collInfo);
  return true;
}

} // namespace podio

#include "podio/ROOTNTupleReader.h"
#include "podio/CollectionBase.h"
#include "podio/CollectionBufferFactory.h"
#include "podio/CollectionBuffers.h"
#include "podio/CollectionIDTable.h"
#include "podio/GenericParameters.h"
#include "rootUtils.h"

#include "TClass.h"
#include <ROOT/RError.hxx>
#include <memory>

namespace podio {

template <typename T>
void ROOTNTupleReader::readParams(const std::string& name, unsigned entNum, GenericParameters& params) {
  auto keyView = m_readers[name][0]->GetView<std::vector<std::string>>(root_utils::getGPKeyName<T>());
  auto valueView = m_readers[name][0]->GetView<std::vector<std::vector<T>>>(root_utils::getGPValueName<T>());

  for (size_t i = 0; i < keyView(entNum).size(); ++i) {
    params.getMap<T>().emplace(std::move(keyView(entNum)[i]), std::move(valueView(entNum)[i]));
  }
}

GenericParameters ROOTNTupleReader::readEventMetaData(const std::string& name, unsigned entNum) {
  GenericParameters params;

  readParams<int>(name, entNum, params);
  readParams<float>(name, entNum, params);
  readParams<double>(name, entNum, params);
  readParams<std::string>(name, entNum, params);

  return params;
}

bool ROOTNTupleReader::initCategory(const std::string& category) {
  if (std::find(m_availableCategories.begin(), m_availableCategories.end(), category) == m_availableCategories.end()) {
    return false;
  }
  // Assume that the metadata is the same in all files
  auto filename = m_filenames[0];

  auto id = m_metadata_readers[filename]->GetView<std::vector<unsigned int>>(root_utils::idTableName(category));
  m_collectionInfo[category].id = id(0);

  auto collectionName =
      m_metadata_readers[filename]->GetView<std::vector<std::string>>(root_utils::collectionName(category));
  m_collectionInfo[category].name = collectionName(0);

  auto collectionType =
      m_metadata_readers[filename]->GetView<std::vector<std::string>>(root_utils::collInfoName(category));
  m_collectionInfo[category].type = collectionType(0);

  auto subsetCollection =
      m_metadata_readers[filename]->GetView<std::vector<short>>(root_utils::subsetCollection(category));
  m_collectionInfo[category].isSubsetCollection = subsetCollection(0);

  auto schemaVersion = m_metadata_readers[filename]->GetView<std::vector<SchemaVersionT>>("schemaVersion_" + category);
  m_collectionInfo[category].schemaVersion = schemaVersion(0);

  return true;
}

void ROOTNTupleReader::openFile(const std::string& filename) {
  openFiles({filename});
}

void ROOTNTupleReader::openFiles(const std::vector<std::string>& filenames) {

  m_filenames.insert(m_filenames.end(), filenames.begin(), filenames.end());
  for (auto& filename : filenames) {
    if (m_metadata_readers.find(filename) == m_metadata_readers.end()) {
      m_metadata_readers[filename] = ROOT::Experimental::RNTupleReader::Open(root_utils::metaTreeName, filename);
    }
  }

  m_metadata = ROOT::Experimental::RNTupleReader::Open(root_utils::metaTreeName, filenames[0]);

  auto versionView = m_metadata->GetView<std::vector<uint16_t>>(root_utils::versionBranchName);
  auto version = versionView(0);

  m_fileVersion = podio::version::Version{version[0], version[1], version[2]};

  auto edmView = m_metadata->GetView<std::vector<std::tuple<std::string, std::string>>>(root_utils::edmDefBranchName);
  auto edm = edmView(0);

  auto availableCategoriesField = m_metadata->GetView<std::vector<std::string>>(root_utils::availableCategories);
  m_availableCategories = availableCategoriesField(0);
}

unsigned ROOTNTupleReader::getEntries(const std::string& name) {
  if (m_readers.find(name) == m_readers.end()) {
    for (auto& filename : m_filenames) {
      try {
        m_readers[name].emplace_back(ROOT::Experimental::RNTupleReader::Open(name, filename));
      } catch (const ROOT::Experimental::RException& e) {
        std::cout << "Category " << name << " not found in file " << filename << std::endl;
      }
    }
    m_totalEntries[name] = std::accumulate(m_readers[name].begin(), m_readers[name].end(), 0,
                                           [](int total, auto& reader) { return total + reader->GetNEntries(); });
  }
  return m_totalEntries[name];
}

std::unique_ptr<ROOTFrameData> ROOTNTupleReader::readNextEntry(const std::string& name) {
  return readEntry(name, m_entries[name]);
}

std::unique_ptr<ROOTFrameData> ROOTNTupleReader::readEntry(const std::string& category, const unsigned entNum) {
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

  m_entries[category] = entNum + 1;

  ROOTFrameData::BufferMap buffers;
  auto dentry = m_readers[category][0]->GetModel()->GetDefaultEntry();

  // This map is needed to keep the pointers to the vectors that
  // will be written later alive
  std::map<std::pair<std::string, int>, std::vector<podio::ObjectID>*> tmp;

  for (size_t i = 0; i < m_collectionInfo[category].id.size(); ++i) {
    const auto collectionClass = TClass::GetClass(m_collectionInfo[category].type[i].c_str());

    auto collection =
        std::unique_ptr<podio::CollectionBase>(static_cast<podio::CollectionBase*>(collectionClass->New()));

    // const std::string bufferClassName = "std::vector<" + collection->getDataTypeName() + ">";
    const auto bufferClassName = collection->getTypeName();
    const auto bufferClass =
      m_collectionInfo[category].isSubsetCollection[i] ? nullptr : TClass::GetClass(std::string(bufferClassName).c_str());

    const bool isSubsetColl = bufferClass == nullptr;

    const auto& bufferFactory = podio::CollectionBufferFactory::instance();
    auto maybeBuffers = bufferFactory.createBuffers(m_collectionInfo[category].type[i],
                                                    m_collectionInfo[category].schemaVersion[i], isSubsetColl);
    auto collBuffers = maybeBuffers.value_or(podio::CollectionReadBuffers{});

    if (!maybeBuffers) {
      std::cout << "WARNING: Buffers couldn't be created for collection " << m_collectionInfo[category].name[i]
                << " of type " << m_collectionInfo[category].type[i] << " and schema version "
                << m_collectionInfo[category].schemaVersion[i] << std::endl;
      return nullptr;
    }

    if (!isSubsetColl) {
      dentry->CaptureValueUnsafe(m_collectionInfo[category].name[i], collBuffers.data);
    }
    if (auto* refCollections = collBuffers.references) {
      for (size_t j = 0; j < refCollections->size(); ++j) {
        auto vec = new std::vector<podio::ObjectID>;
        const auto brName = root_utils::refBranch(m_collectionInfo[category].name[i], j);
        dentry->CaptureValueUnsafe(brName, vec);
        tmp[{brName, j}] = vec;
      }
    }

    if (auto* vecMembers = collBuffers.vectorMembers) {
      for (size_t j = 0; j < vecMembers->size(); ++j) {
        const auto typeName = "vector<" + vecMembers->at(j).first + ">";
        const auto brName = root_utils::vecBranch(m_collectionInfo[category].name[i], j);
        dentry->CaptureValueUnsafe(brName, vecMembers->at(j).second);
      }
    }

    buffers.emplace(m_collectionInfo[category].name[i], std::move(collBuffers));
  }

  m_readers[category][0]->LoadEntry(entNum);

  for (size_t i = 0; i < m_collectionInfo[category].id.size(); ++i) {
    auto collBuffers = buffers[m_collectionInfo[category].name[i]];
    if (auto* refCollections = collBuffers.references) {
      for (size_t j = 0; j < refCollections->size(); ++j) {
        const auto brName = root_utils::refBranch(m_collectionInfo[category].name[i], j);
        refCollections->at(j) = std::unique_ptr<std::vector<podio::ObjectID>>(tmp[{brName, j}]);
      }
    }
  }

  auto parameters = readEventMetaData(category, entNum);
  if (!m_table) {
    m_table = std::make_shared<CollectionIDTable>(m_collectionInfo[category].id, m_collectionInfo[category].name);
  }

  return std::make_unique<ROOTFrameData>(std::move(buffers), m_table, std::move(parameters));
}

} // namespace podio

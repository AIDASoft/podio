#include "podio/ROOTNTupleReader.h"
#include "podio/CollectionBase.h"
#include "podio/CollectionBuffers.h"
#include "podio/CollectionIDTable.h"
#include "podio/GenericParameters.h"
#include "rootUtils.h"

#include "TClass.h"
#include <ROOT/RError.hxx>
#include <memory>

namespace podio {

GenericParameters ROOTNTupleReader::readEventMetaData(const std::string& name, unsigned entNum) {
  GenericParameters params;

  auto intKeyView    = m_readers[name][0]->GetView<std::vector<std::string>>(root_utils::intKey);
  auto floatKeyView  = m_readers[name][0]->GetView<std::vector<std::string>>(root_utils::floatKey);
  auto doubleKeyView = m_readers[name][0]->GetView<std::vector<std::string>>(root_utils::doubleKey);
  auto stringKeyView = m_readers[name][0]->GetView<std::vector<std::string>>(root_utils::stringKey);

  auto intValueView    = m_readers[name][0]->GetView<std::vector<std::vector<int>>>(root_utils::intValue);
  auto floatValueView  = m_readers[name][0]->GetView<std::vector<std::vector<float>>>(root_utils::floatValue);
  auto doubleValueView = m_readers[name][0]->GetView<std::vector<std::vector<double>>>(root_utils::doubleValue);
  auto stringValueView = m_readers[name][0]->GetView<std::vector<std::vector<std::string>>>(root_utils::stringValue);

  auto keys = intKeyView(entNum);
  auto valuesInt = intValueView(entNum);
  for (size_t i = 0; i < keys.size(); ++i) {
    params.getIntMap()[keys[i]] = valuesInt[i];
  }
  keys = floatKeyView(entNum);
  auto valuesFloat = floatValueView(entNum);
  for (size_t i = 0; i < keys.size(); ++i) {
    params.getFloatMap()[keys[i]] = valuesFloat[i];
  }
  keys = doubleKeyView(entNum);
  auto valuesDouble = doubleValueView(entNum);
  for (size_t i = 0; i < keys.size(); ++i) {
    params.getDoubleMap()[keys[i]] = valuesDouble[i];
  }
  keys = stringKeyView(entNum);
  auto valuesString = stringValueView(entNum);
  for (size_t i = 0; i < keys.size(); ++i) {
    params.getStringMap()[keys[i]] = valuesString[i];
  }

  return params;
}

bool ROOTNTupleReader::initCategory(const std::string& category) {
  if (std::find(m_availableCategories.begin(), m_availableCategories.end(), category) == m_availableCategories.end()) {
    return false;
  }
  // Assume that the metadata is the same in all files
  auto filename = m_filenames[0];

  auto id = m_metadata_readers[filename]->GetView<std::vector<int>>(root_utils::idTableName(category));
  m_collectionId[category] = id(0);

  std::cout << "Getting collectionName" << std::endl;
  auto collectionName = m_metadata_readers[filename]->GetView<std::vector<std::string>>(root_utils::collectionName(category));
  m_collectionName[category] = collectionName(0);

  std::cout << "Getting collectionType" << std::endl;
  auto collectionType = m_metadata_readers[filename]->GetView<std::vector<std::string>>(root_utils::collInfoName(category));
  m_collectionType[category] = collectionType(0);
   
  std::cout << "Getting subsetCollection" << std::endl;
  auto subsetCollection = m_metadata_readers[filename]->GetView<std::vector<short>>(root_utils::subsetCollection(category));
  m_isSubsetCollection[category] = subsetCollection(0);

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
      }
      catch (const ROOT::Experimental::RException& e) {
        std::cout << "Category " << name << " not found in file " << filename << std::endl;
      }
    }
  }
  m_totalEntries[name] = std::accumulate(m_readers[name].begin(), m_readers[name].end(), 0, [](int total, auto& reader) {return total + reader->GetNEntries();});
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

  if (m_collectionId.find(category) == m_collectionId.end()) {
    if (!initCategory(category)) {
      return nullptr;
    }
  }

  std::cout << "Reading entry with category " << category << " and number " << entNum << std::endl;
  m_entries[category] = entNum+1;

  ROOTFrameData::BufferMap buffers;
  auto dentry = m_readers[category][0]->GetModel()->GetDefaultEntry();

  std::map<std::pair<std::string, int>, std::vector<podio::ObjectID>*> tmp;

  for (size_t i = 0; i < m_collectionId[category].size(); ++i) {
    std::cout << "i = " << i << " " << m_collectionId[category][i] << " " << m_collectionType[category][i] << " " << m_collectionName[category][i] << std::endl;

    const auto collectionClass = TClass::GetClass(m_collectionType[category][i].c_str());

    auto collection =
        std::unique_ptr<podio::CollectionBase>(static_cast<podio::CollectionBase*>(collectionClass->New()));

    const std::string bufferClassName = "std::vector<" + collection->getDataTypeName() + ">";
    const auto bufferClass = m_isSubsetCollection[category][i] ? nullptr : TClass::GetClass(bufferClassName.c_str());

    auto collBuffers = podio::CollectionReadBuffers();
    const bool isSubsetColl = bufferClass == nullptr;
    if (!isSubsetColl) {
      collBuffers.data = bufferClass->New();
    }
    collection->setSubsetCollection(isSubsetColl);

    auto tmpBuffers = collection->createBuffers();
    collBuffers.createCollection = std::move(tmpBuffers.createCollection);
    collBuffers.recast = std::move(tmpBuffers.recast);

    if (auto* refs = tmpBuffers.references) {
      collBuffers.references = new podio::CollRefCollection(refs->size());
    }
    if (auto* vminfo = tmpBuffers.vectorMembers) {
      collBuffers.vectorMembers = new podio::VectorMembersInfo();
      collBuffers.vectorMembers->reserve(vminfo->size());

      for (const auto& [type, _] : (*vminfo)) {
        const auto* vecClass = TClass::GetClass(("vector<" + type + ">").c_str());
        collBuffers.vectorMembers->emplace_back(type, vecClass->New());
      }
    }

    if (!isSubsetColl) {
      dentry->CaptureValueUnsafe(m_collectionName[category][i], collBuffers.data);
    }
    if (auto* refCollections = collBuffers.references) {
      for (size_t j = 0; j < refCollections->size(); ++j) {
        // // The unique_ptrs are nullptrs at the beginning, we first initialize
        // // them and then fill the values with the read data since
        // refCollections->at(j) = std::make_unique<std::vector<podio::ObjectID>>();
        // const auto brName = root_utils::refBranch(m_collectionName[category][i], j);
        // std::cout << "brName = " << brName << " " << (refCollections->at(j) == nullptr) << std::endl;
        // dentry->CaptureValueUnsafe(brName, (*refCollections)[j].get());

        auto vec = new std::vector<podio::ObjectID>;
        const auto brName = root_utils::refBranch(m_collectionName[category][i], j);
        dentry->CaptureValueUnsafe(brName, vec);
        tmp[{brName, j}] = vec;
      }
    }

    if (auto* vecMembers = collBuffers.vectorMembers) {
      for (size_t j = 0; j < vecMembers->size(); ++j) {
        const auto typeName = "vector<" + vecMembers->at(j).first + ">";
        const auto brName = root_utils::vecBranch(m_collectionName[category][i], j);
        dentry->CaptureValueUnsafe(brName, vecMembers->at(j).second);
      }
    }

    std::cout << "CaptureValueUnsafe done" << std::endl;
    buffers.emplace(m_collectionName[category][i], std::move(collBuffers));
  }

  m_readers[category][0]->LoadEntry(entNum);

  for (size_t i = 0; i < m_collectionId[category].size(); ++i) {
    auto collBuffers = buffers[m_collectionName[category][i]];
    if (auto* refCollections = collBuffers.references) {
      for (size_t j = 0; j < refCollections->size(); ++j) {
        const auto brName = root_utils::refBranch(m_collectionName[category][i], j);
        refCollections->at(j) = std::unique_ptr<std::vector<podio::ObjectID>>(tmp[{brName, j}]);
      }
    }

  }

  auto parameters = readEventMetaData(category, entNum);
  auto table = std::make_shared<CollectionIDTable>();

  auto names = m_collectionName[category];
  auto ids = m_collectionId[category];

  // Sort the ids and collection names to recreate the collection ID table with
  // the names following the order of the IDs
  std::vector<std::pair<int, std::string>> v;
  v.reserve(names.size());
  for (size_t i = 0; i < names.size(); ++i) {
    v.emplace_back(std::make_pair<int, std::string>(int(ids[i]), std::string(names[i])));
  }
  std::sort(v.begin(), v.end());
  for (auto& [id, name] : v) {
    table->add(name);
  }

  return std::make_unique<ROOTFrameData>(std::move(buffers), table, std::move(parameters));
}

} // namespace podio

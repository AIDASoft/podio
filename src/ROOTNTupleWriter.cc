#include "podio/ROOTNTupleWriter.h"
#include "podio/CollectionBase.h"
#include "podio/GenericParameters.h"
#include "podio/SchemaEvolution.h"
#include "podio/podioVersion.h"
#include "rootUtils.h"

#include "TFile.h"
#include <ROOT/RNTuple.hxx>
#include <ROOT/RNTupleModel.hxx>

#include <algorithm>

namespace podio {

ROOTNTupleWriter::ROOTNTupleWriter(const std::string& filename) :
    m_metadata(ROOT::Experimental::RNTupleModel::Create()),
    m_file(new TFile(filename.c_str(), "RECREATE", "data file")) {
}

ROOTNTupleWriter::~ROOTNTupleWriter() {
  if (!m_finished) {
    finish();
  }
}

template <typename T>
void ROOTNTupleWriter::fillParams(const std::string& category, GenericParameters& params) {
  auto gpKeys = m_writers[category]->GetModel()->Get<std::vector<std::string>>(root_utils::getGPKeyName<T>());
  auto gpValues = m_writers[category]->GetModel()->Get<std::vector<std::vector<T>>>(root_utils::getGPValueName<T>());
  gpKeys->clear();
  gpKeys->reserve(params.getMap<T>().size());
  gpValues->clear();
  gpValues->reserve(params.getMap<T>().size());
  for (auto& [k, v] : params.getMap<T>()) {
    gpKeys->emplace_back(k);
    gpValues->emplace_back(v);
  }
}

void ROOTNTupleWriter::writeFrame(const podio::Frame& frame, const std::string& category) {
  writeFrame(frame, category, frame.getAvailableCollections());
}

void ROOTNTupleWriter::writeFrame(const podio::Frame& frame, const std::string& category,
                                  const std::vector<std::string>& collsToWrite) {

  std::vector<StoreCollection> collections;
  collections.reserve(collsToWrite.size());
  for (const auto& name : collsToWrite) {
    auto* coll = frame.getCollectionForWrite(name);
    collections.emplace_back(name, const_cast<podio::CollectionBase*>(coll));
  }

  bool new_category = false;
  if (m_writers.find(category) == m_writers.end()) {
    new_category = true;
    auto model = createModels(collections);
    m_writers[category] = ROOT::Experimental::RNTupleWriter::Append(std::move(model), category, *m_file.get(), {});
  }

  auto entry = m_writers[category]->GetModel()->GetDefaultEntry();

  ROOT::Experimental::RNTupleWriteOptions options;
  options.SetCompression(ROOT::RCompressionSetting::EDefaults::kUseGeneralPurpose);

  for (const auto& [name, coll] : collections) {
    auto collBuffers = coll->getBuffers();
    if (collBuffers.vecPtr) {
      entry->CaptureValueUnsafe(name, (void*)collBuffers.vecPtr);
    }

    if (auto refColls = collBuffers.references) {
      int i = 0;
      for (auto& c : (*refColls)) {
        const auto brName = root_utils::refBranch(name, i++);
        entry->CaptureValueUnsafe(brName, c.get());
      }
    }

    if (auto vmInfo = collBuffers.vectorMembers) {
      int i = 0;
      for (auto& [type, vec] : (*vmInfo)) {
        const auto typeName = "vector<" + type + ">";
        const auto brName = root_utils::vecBranch(name, i++);
        auto ptr = *(std::vector<int>**)vec;
        entry->CaptureValueUnsafe(brName, ptr);
      }
    }

    // Not supported
    // entry->CaptureValueUnsafe(root_utils::paramBranchName,
    // &const_cast<podio::GenericParameters&>(frame.getParameters()));

    if (new_category) {
      m_collectionInfo[category].id.emplace_back(coll->getID());
      m_collectionInfo[category].name.emplace_back(name);
      m_collectionInfo[category].type.emplace_back(coll->getTypeName());
      m_collectionInfo[category].isSubsetCollection.emplace_back(coll->isSubsetCollection());
      m_collectionInfo[category].schemaVersion.emplace_back(coll->getSchemaVersion());
    }
  }

  auto params = frame.getParameters();
  fillParams<int>(category, params);
  fillParams<float>(category, params);
  fillParams<double>(category, params);
  fillParams<std::string>(category, params);

  m_writers[category]->Fill();
  m_categories.insert(category);
}

std::unique_ptr<ROOT::Experimental::RNTupleModel>
ROOTNTupleWriter::createModels(const std::vector<StoreCollection>& collections) {
  auto model = ROOT::Experimental::RNTupleModel::Create();
  for (auto& [name, coll] : collections) {
    const auto collBuffers = coll->getBuffers();

    if (collBuffers.vecPtr) {
      auto collClassName = "std::vector<" + coll->getDataTypeName() + ">";
      auto field = ROOT::Experimental::Detail::RFieldBase::Create(name, collClassName).Unwrap();
      model->AddField(std::move(field));
    }

    if (auto refColls = collBuffers.references) {
      int i = 0;
      for (auto& c [[maybe_unused]] : (*refColls)) {
        const auto brName = root_utils::refBranch(name, i);
        auto collClassName = "vector<podio::ObjectID>";
        auto field = ROOT::Experimental::Detail::RFieldBase::Create(brName, collClassName).Unwrap();
        model->AddField(std::move(field));
        ++i;
      }
    }

    if (auto vminfo = collBuffers.vectorMembers) {
      int i = 0;
      for (auto& [type, vec] : (*vminfo)) {
        const auto typeName = "vector<" + type + ">";
        const auto brName = root_utils::vecBranch(name, i);
        auto field = ROOT::Experimental::Detail::RFieldBase::Create(brName, typeName).Unwrap();
        model->AddField(std::move(field));
        ++i;
      }
    }
  }

  // Not supported by ROOT because podio::GenericParameters has map types
  // so we have to split them manually
  // model->MakeField<podio::GenericParameters>(root_utils::paramBranchName);

  // gp = Generic Parameters
  auto gpintKeys = model->MakeField<std::vector<std::string>>(root_utils::intKeyName);
  auto gpfloatKeys = model->MakeField<std::vector<std::string>>(root_utils::floatKeyName);
  auto gpdoubleKeys = model->MakeField<std::vector<std::string>>(root_utils::doubleKeyName);
  auto gpstringKeys = model->MakeField<std::vector<std::string>>(root_utils::stringKeyName);

  auto gpintValues = model->MakeField<std::vector<std::vector<int>>>(root_utils::intValueName);
  auto gpfloatValues = model->MakeField<std::vector<std::vector<float>>>(root_utils::floatValueName);
  auto gpdoubleValues = model->MakeField<std::vector<std::vector<double>>>(root_utils::doubleValueName);
  auto gpstringValues = model->MakeField<std::vector<std::vector<std::string>>>(root_utils::stringValueName);

  model->Freeze();
  return model;
}

void ROOTNTupleWriter::finish() {

  auto podioVersion = podio::version::build_version;
  auto versionField = m_metadata->MakeField<std::vector<uint16_t>>(root_utils::versionBranchName);
  *versionField = {podioVersion.major, podioVersion.minor, podioVersion.patch};

  auto edmDefinitions = m_datamodelCollector.getDatamodelDefinitionsToWrite();
  auto edmField =
      m_metadata->MakeField<std::vector<std::tuple<std::string, std::string>>>(root_utils::edmDefBranchName);
  *edmField = edmDefinitions;

  auto availableCategoriesField = m_metadata->MakeField<std::vector<std::string>>(root_utils::availableCategories);
  for (auto& [c, _] : m_collectionInfo) {
    availableCategoriesField->push_back(c);
  }

  for (auto& category : m_categories) {
    auto idField = m_metadata->MakeField<std::vector<int>>({root_utils::idTableName(category)});
    *idField = m_collectionInfo[category].id;
    auto collectionNameField = m_metadata->MakeField<std::vector<std::string>>({root_utils::collectionName(category)});
    *collectionNameField = m_collectionInfo[category].name;
    auto collectionTypeField = m_metadata->MakeField<std::vector<std::string>>({root_utils::collInfoName(category)});
    *collectionTypeField = m_collectionInfo[category].type;
    auto subsetCollectionField = m_metadata->MakeField<std::vector<short>>({root_utils::subsetCollection(category)});
    *subsetCollectionField = m_collectionInfo[category].isSubsetCollection;
    auto schemaVersionField = m_metadata->MakeField<std::vector<SchemaVersionT>>({"schemaVersion_" + category});
    *schemaVersionField = m_collectionInfo[category].schemaVersion;
  }

  m_metadata->Freeze();
  m_metadataWriter =
      ROOT::Experimental::RNTupleWriter::Append(std::move(m_metadata), root_utils::metaTreeName, *m_file, {});

  m_metadataWriter->Fill();

  m_file->Write();

  // All the tuple writers must be deleted before the file so that they flush
  // unwritten output
  m_writers.clear();
  m_metadataWriter.reset();

  m_finished = true;
}

} // namespace podio

#include "podio/ROOTNTupleWriter.h"
#include "podio/GenericParameters.h"
#include "rootUtils.h"
#include "podio/CollectionBase.h"
#include "podio/podioVersion.h"

#include "TFile.h"
#include <ROOT/RNTuple.hxx>
#include <ROOT/RNTupleModel.hxx>

#include <algorithm>

namespace podio {

ROOTNTupleWriter::ROOTNTupleWriter(const std::string& filename) :
    m_metadata(nullptr),
    m_writers(),
    m_file(new TFile(filename.c_str(),"RECREATE","data file")),
    m_categories(),
    m_collectionId(),
    m_collectionName(),
    m_collectionType(),
    m_isSubsetCollection()
  {
    m_metadata = ROOT::Experimental::RNTupleModel::Create();
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
    // coll->prepareForWrite();
    auto collBuffers = coll->getBuffers();
    if (collBuffers.vecPtr) {
      std::cout << "Capturing unsafe " << name << std::endl;

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
        std::cout << typeName << " " << brName << std::endl;
        auto ptr = *(std::vector<int>**)vec;
        std::cout << "Vector: Size is " << ptr->size();
        entry->CaptureValueUnsafe(brName, ptr);
      }
    }

    // Not supported
    // entry->CaptureValueUnsafe(root_utils::paramBranchName, &const_cast<podio::GenericParameters&>(frame.getParameters()));

    if (new_category) {
      m_collectionId[category].emplace_back(coll->getID());
      m_collectionName[category].emplace_back(name);
      m_collectionType[category].emplace_back(coll->getTypeName());
      m_isSubsetCollection[category].emplace_back(coll->isSubsetCollection());
    }
  }

  auto params = frame.getParameters();
  auto intMap = params.getIntMap();
  auto floatMap = params.getFloatMap();
  auto doubleMap = params.getDoubleMap();
  auto stringMap = params.getStringMap();

  auto gpintKeys = m_writers[category]->GetModel()->Get<std::vector<std::string>>("GP_int_keys");
  auto gpfloatKeys = m_writers[category]->GetModel()->Get<std::vector<std::string>>("GP_float_keys");
  auto gpdoubleKeys = m_writers[category]->GetModel()->Get<std::vector<std::string>>("GP_double_keys");
  auto gpstringKeys = m_writers[category]->GetModel()->Get<std::vector<std::string>>("GP_string_keys");

  auto gpintValues = m_writers[category]->GetModel()->Get<std::vector<std::vector<int>>>("GP_int_values");
  auto gpfloatValues = m_writers[category]->GetModel()->Get<std::vector<std::vector<float>>>("GP_float_values");
  auto gpdoubleValues = m_writers[category]->GetModel()->Get<std::vector<std::vector<double>>>("GP_double_values");
  auto gpstringValues = m_writers[category]->GetModel()->Get<std::vector<std::vector<std::string>>>("GP_string_values");

  gpintKeys->clear();
  gpintValues->clear();
  for (auto& [k, v] : intMap) {
    gpintKeys->emplace_back(k);
    gpintValues->emplace_back(v);
  }

  gpfloatKeys->clear();
  gpfloatValues->clear();
  for (auto& [k, v] : floatMap) {
    gpfloatKeys->emplace_back(k);
    gpfloatValues->emplace_back(v);
    for (auto& x : v) {
      std::cout << "floatMap: " << x << std::endl;
    }
  }
  gpdoubleKeys->clear();
  gpdoubleValues->clear();
  for (auto& [k, v] : doubleMap) {
    gpdoubleKeys->emplace_back(k);
    gpdoubleValues->emplace_back(v);
  }
  gpstringKeys->clear();
  gpstringValues->clear();
  for (auto& [k, v] : stringMap) {
    gpstringKeys->emplace_back(k);
    gpstringValues->emplace_back(v);
  }

  m_writers[category]->Fill();
  m_categories.insert(category);
}

std::unique_ptr<ROOT::Experimental::RNTupleModel> ROOTNTupleWriter::createModels(const std::vector<StoreCollection>& collections) {
  auto model = ROOT::Experimental::RNTupleModel::Create();
  for (auto& [name, coll] : collections) {
    const auto collBuffers = coll->getBuffers();

    if (collBuffers.vecPtr) {
      auto collClassName = "std::vector<" + coll->getDataTypeName() +">";
      std::cout << name << " " << collClassName << std::endl;
      std::cout << "Making field with name = " << name << " and collClassName = " << collClassName << std::endl;
      auto field = ROOT::Experimental::Detail::RFieldBase::Create(name, collClassName).Unwrap();
      model->AddField(std::move(field));
    }

    if (auto refColls = collBuffers.references) {
      int i = 0;
      for (auto& c [[maybe_unused]] : (*refColls)) {
        const auto brName = root_utils::refBranch(name, i);
        auto collClassName = "vector<podio::ObjectID>";
        std::cout << "Making reference field with name = " << brName << " and collClassName = " << collClassName << std::endl;
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
  auto gpintKeys = model->MakeField<std::vector<std::string>>("GP_int_keys");
  auto gpfloatKeys = model->MakeField<std::vector<std::string>>("GP_float_keys");
  auto gpdoubleKeys = model->MakeField<std::vector<std::string>>("GP_double_keys");
  auto gpstringKeys = model->MakeField<std::vector<std::string>>("GP_string_keys");

  auto gpintValues = model->MakeField<std::vector<std::vector<int>>>("GP_int_values");
  auto gpfloatValues = model->MakeField<std::vector<std::vector<float>>>("GP_float_values");
  auto gpdoubleValues = model->MakeField<std::vector<std::vector<double>>>("GP_double_values");
  auto gpstringValues = model->MakeField<std::vector<std::vector<std::string>>>("GP_string_values");

  model->Freeze();
  return model;
}

void ROOTNTupleWriter::finish() {

  auto podioVersion = podio::version::build_version;
  auto version_field = m_metadata->MakeField<std::vector<int>>(root_utils::versionBranchName);
  *version_field = {podioVersion.major, podioVersion.minor, podioVersion.patch};

  auto edmDefinitions = m_datamodelCollector.getDatamodelDefinitionsToWrite();
  auto edm_field = m_metadata->MakeField<std::vector<std::tuple<std::string, std::string>>>(root_utils::edmDefBranchName);
  *edm_field = edmDefinitions;

  auto availableCategoriesField = m_metadata->MakeField<std::vector<std::string>>("available_categories");
  for (auto& [c, _] : m_collectionId ) {
    availableCategoriesField->push_back(c);
  }

  for (auto& category : m_categories) {
    auto idField = m_metadata->MakeField<std::vector<int>>(root_utils::idTableName(category));
    *idField = m_collectionId[category];
    auto collectionNameField = m_metadata->MakeField<std::vector<std::string>>(category + "_name");
    *collectionNameField = m_collectionName[category];
    auto collectionTypeField = m_metadata->MakeField<std::vector<std::string>>(root_utils::collInfoName(category));
    *collectionTypeField = m_collectionType[category];
    auto subsetCollectionField = m_metadata->MakeField<std::vector<bool>>(category + "_test");
    *subsetCollectionField = m_isSubsetCollection[category];
  }

  m_metadata->Freeze();
  m_metadataWriter = ROOT::Experimental::RNTupleWriter::Append(std::move(m_metadata), root_utils::metaTreeName, *m_file.get(), {});

  m_metadataWriter->Fill();

  m_file->Write();

  // All the tuple writers have to be deleted before the file so that they flush
  // unwritten output
  m_writers.clear();
  m_metadataWriter.reset();
}

} //namespace podio

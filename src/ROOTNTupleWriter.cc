#include <ROOT/RNTuple.hxx>
#include <ROOT/RNTupleModel.hxx>
#include <algorithm>

#include "podio/GenericParameters.h"
#include "rootUtils.h"

#include "podio/CollectionBase.h"
#include "podio/EventStore.h"
#include "podio/ROOTWriter.h"
#include "podio/podioVersion.h"

// ROOT specifc includes
#include "TFile.h"

#include "podio/ROOTNTupleWriter.h"

#include "datamodel/ExampleMCData.h"

namespace podio {

ROOTNTupleWriter::ROOTNTupleWriter(const std::string& filename) :
    m_metadata(nullptr),
    m_writers(),
    m_file(new TFile(filename.c_str(),"RECREATE","data file")),
    m_categories(),
    m_collectionId(),
    m_collectionType(),
    m_isSubsetCollection()
  {
    m_metadata = rnt::RNTupleModel::Create();
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
    auto model = createModels(collections, frame.getParameters());
    m_writers[category] = rnt::RNTupleWriter::Append(std::move(model), category, *m_file.get(), {});
  }

  m_entry = m_writers[category]->GetModel()->GetDefaultEntry();

  rnt::RNTupleWriteOptions options;
  options.SetCompression(ROOT::RCompressionSetting::EDefaults::kUseGeneralPurpose);

  for (const auto& [name, coll] : collections) {
    // coll->prepareForWrite();
    auto collBuffers = coll->getBuffers();
    if (collBuffers.vecPtr) {
      std::cout << "Capturing unsafe " << name << std::endl;

      // auto v = (std::vector<ExampleMCData>*)collBuffers.data;
      // std::cout << "Size is " << v->size() << std::endl;
      // std::cout << (*v)[0].energy << std::endl;
      // for (auto& x : *v) {
      //   std::cout << "Inside loop " << std::endl;
      //   std::cout << x.energy << std::endl;
      // }

      m_entry->CaptureValueUnsafe(name, (void*)collBuffers.vecPtr);

      // std::cout << "After capturing " << std::endl;
      // v = (std::vector<ExampleMCData>*)collBuffers.data;
      // std::cout << "Size is " << v->size() << std::endl;
      // std::cout << (*v)[0].energy << std::endl;
      // for (auto& x : *v) {
      //   std::cout << "Inside loop " << std::endl;
      //   std::cout << x.energy << std::endl;
      // }

      // auto ptr = (std::vector<ExampleMCData>*)collBuffers.data;
      // auto start = (char*)ptr->data();
      // for (int i = 0; i < 13; ++i) {
      //     printf("%x ", start[i]);
      // }
      // std::cout << std::endl;

    }

    if (auto refColls = collBuffers.references) {
      int i = 0;
      for (auto& c : (*refColls)) {
        const auto brName = root_utils::refBranch(name, i++);
        m_entry->CaptureValueUnsafe(brName, c.get());
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
        m_entry->CaptureValueUnsafe(brName, ptr);
        // if (m_first) {
        //   // m_entry->CaptureValueUnsafe(brName, (void*) vec);
        //   m_first = false;
        // }
      }
    }

    // Not supported
    // m_entry->CaptureValueUnsafe(root_utils::paramBranchName, &const_cast<podio::GenericParameters&>(frame.getParameters()));



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

std::unique_ptr<rnt::RNTupleModel> ROOTNTupleWriter::createModels(const std::vector<StoreCollection>& collections,
                                                                  const podio::GenericParameters& params) {
  auto model = rnt::RNTupleModel::Create();
  for (auto& [name, coll] : collections) {
    const auto collBuffers = coll->getBuffers();

    if (collBuffers.vecPtr) {
      auto collClassName = "std::vector<" + coll->getDataTypeName() +">";
      std::cout << name << " " << collClassName << std::endl;
      std::cout << "Making field with name = " << name << " and collClassName = " << collClassName << std::endl;
      auto field = rnt::Detail::RFieldBase::Create(name, collClassName).Unwrap();
      model->AddField(std::move(field));
    }

    if (auto refColls = collBuffers.references) {
      int i = 0;
      for (auto& c : (*refColls)) {
        const auto brName = root_utils::refBranch(name, i);
        auto collClassName = "vector<podio::ObjectID>";
        std::cout << "Making reference field with name = " << brName << " and collClassName = " << collClassName << std::endl;
        auto field = rnt::Detail::RFieldBase::Create(brName, collClassName).Unwrap();
        model->AddField(std::move(field));
        ++i;
      }
    }

    if (auto vminfo = collBuffers.vectorMembers) {
      int i = 0;
      for (auto& [type, vec] : (*vminfo)) {
        const auto typeName = "vector<" + type + ">";
        const auto brName = root_utils::vecBranch(name, i);
        auto field = rnt::Detail::RFieldBase::Create(brName, typeName).Unwrap();
        model->AddField(std::move(field));
        ++i;
      }
    }
  }
  
  // gp = Generic Parameters
  auto gpintKeys = model->MakeField<std::vector<std::string>>("GP_int_keys");
  auto gpfloatKeys = model->MakeField<std::vector<std::string>>("GP_float_keys");
  auto gpdoubleKeys = model->MakeField<std::vector<std::string>>("GP_double_keys");
  auto gpstringKeys = model->MakeField<std::vector<std::string>>("GP_string_keys");

  auto gpintValues = model->MakeField<std::vector<std::vector<int>>>("GP_int_values");
  auto gpfloatValues = model->MakeField<std::vector<std::vector<float>>>("GP_float_values");
  auto gpdoubleValues = model->MakeField<std::vector<std::vector<double>>>("GP_double_values");
  auto gpstringValues = model->MakeField<std::vector<std::vector<std::string>>>("GP_string_values");

  // Not supported by ROOT
  // model->MakeField<podio::GenericParameters>(root_utils::paramBranchName);

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
  m_metadataWriter = rnt::RNTupleWriter::Append(std::move(m_metadata), root_utils::metaTreeName, *m_file.get(), {});

  m_metadataWriter->Fill();

  m_file->Write();

  // All the tuple writers have to be deleted before the file so that they flush
  // unwritten output
  m_writers.clear();
  m_metadataWriter.reset();
}

} //namespace podio

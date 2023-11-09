#include "podio/ROOTNTupleWriter.h"
#include "podio/CollectionBase.h"
#include "podio/DatamodelRegistry.h"
#include "podio/GenericParameters.h"
#include "podio/SchemaEvolution.h"
#include "podio/podioVersion.h"
#include "rootUtils.h"

#include "TFile.h"
#include <ROOT/RField.hxx>
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
std::pair<std::vector<std::string>&, std::vector<std::vector<T>>&> ROOTNTupleWriter::getKeyValueVectors() {
  if constexpr (std::is_same_v<T, int>) {
    return {m_intkeys, m_intvalues};
  } else if constexpr (std::is_same_v<T, float>) {
    return {m_floatkeys, m_floatvalues};
  } else if constexpr (std::is_same_v<T, double>) {
    return {m_doublekeys, m_doublevalues};
  } else if constexpr (std::is_same_v<T, std::string>) {
    return {m_stringkeys, m_stringvalues};
  } else {
    throw std::runtime_error("Unknown type");
  }
}

template <typename T>
void ROOTNTupleWriter::fillParams(GenericParameters& params, ROOT::Experimental::REntry* entry) {
  auto [key, value] = getKeyValueVectors<T>();
  entry->CaptureValueUnsafe(root_utils::getGPKeyName<T>(), &key);
  entry->CaptureValueUnsafe(root_utils::getGPValueName<T>(), &value);

  key.clear();
  key.reserve(params.getMap<T>().size());
  value.clear();
  value.reserve(params.getMap<T>().size());

  for (auto& [kk, vv] : params.getMap<T>()) {
    key.emplace_back(kk);
    value.emplace_back(vv);
  }
}

void ROOTNTupleWriter::writeFrame(const podio::Frame& frame, const std::string& category) {
  writeFrame(frame, category, frame.getAvailableCollections());
}

void ROOTNTupleWriter::writeFrame(const podio::Frame& frame, const std::string& category,
                                  const std::vector<std::string>& collsToWrite) {
  auto& catInfo = getCategoryInfo(category);

  // Use the writer as proxy to check whether this category has been initialized
  // already and do so if not
  const bool new_category = (catInfo.writer == nullptr);
  if (new_category) {
    // This is the minimal information that we need for now
    catInfo.name = collsToWrite;
  }

  std::vector<StoreCollection> collections;
  collections.reserve(catInfo.name.size());
  // Only loop over the collections that were requested in the first Frame of
  // this category
  for (const auto& name : catInfo.name) {
    auto* coll = frame.getCollectionForWrite(name);
    if (!coll) {
      // Make sure all collections that we want to write are actually available
      // NOLINTNEXTLINE(performance-inefficient-string-concatenation)
      throw std::runtime_error("Collection '" + name + "' in category '" + category + "' is not available in Frame");
    }

    collections.emplace_back(name, const_cast<podio::CollectionBase*>(coll));
  }

  if (new_category) {
    // Now we have enough info to populate the rest
    auto model = createModels(collections);
    catInfo.writer = ROOT::Experimental::RNTupleWriter::Append(std::move(model), category, *m_file.get(), {});

    for (const auto& [name, coll] : collections) {
      catInfo.id.emplace_back(coll->getID());
      catInfo.type.emplace_back(coll->getTypeName());
      catInfo.isSubsetCollection.emplace_back(coll->isSubsetCollection());
      catInfo.schemaVersion.emplace_back(coll->getSchemaVersion());
    }
  } else {
    if (!root_utils::checkConsistentColls(catInfo.name, collsToWrite)) {
      throw std::runtime_error("Trying to write category '" + category + "' with inconsistent collection content." +
                               root_utils::getInconsistentCollsMsg(catInfo.name, collsToWrite));
    }
  }

  auto entry = m_categories[category].writer->GetModel()->CreateBareEntry();

  ROOT::Experimental::RNTupleWriteOptions options;
  options.SetCompression(ROOT::RCompressionSetting::EDefaults::kUseGeneralPurpose);

  for (const auto& [name, coll] : collections) {
    auto collBuffers = coll->getBuffers();
    if (collBuffers.vecPtr) {
      entry->CaptureValueUnsafe(name, (void*)collBuffers.vecPtr);
    }

    if (coll->isSubsetCollection()) {
      auto& refColl = (*collBuffers.references)[0];
      const auto brName = root_utils::subsetBranch(name);
      entry->CaptureValueUnsafe(brName, refColl.get());
    } else {

      const auto relVecNames = podio::DatamodelRegistry::instance().getRelationNames(coll->getValueTypeName());
      if (auto refColls = collBuffers.references) {
        int i = 0;
        for (auto& c : (*refColls)) {
          const auto brName = root_utils::refBranch(name, relVecNames.relations[i]);
          entry->CaptureValueUnsafe(brName, c.get());
          ++i;
        }
      }

      if (auto vmInfo = collBuffers.vectorMembers) {
        int i = 0;
        for (auto& [type, vec] : (*vmInfo)) {
          const auto typeName = "vector<" + type + ">";
          const auto brName = root_utils::vecBranch(name, relVecNames.vectorMembers[i]);
          auto ptr = *(std::vector<int>**)vec;
          entry->CaptureValueUnsafe(brName, ptr);
          ++i;
        }
      }
    }

    // Not supported
    // entry->CaptureValueUnsafe(root_utils::paramBranchName,
    // &const_cast<podio::GenericParameters&>(frame.getParameters()));
  }

  auto params = frame.getParameters();
  fillParams<int>(params, entry.get());
  fillParams<float>(params, entry.get());
  fillParams<double>(params, entry.get());
  fillParams<std::string>(params, entry.get());

  m_categories[category].writer->Fill(*entry);
}

std::unique_ptr<ROOT::Experimental::RNTupleModel>
ROOTNTupleWriter::createModels(const std::vector<StoreCollection>& collections) {
  auto model = ROOT::Experimental::RNTupleModel::CreateBare();
  for (auto& [name, coll] : collections) {
    const auto collBuffers = coll->getBuffers();

    if (collBuffers.vecPtr) {
      auto collClassName = "std::vector<" + std::string(coll->getDataTypeName()) + ">";
      auto field = ROOT::Experimental::Detail::RFieldBase::Create(name, collClassName).Unwrap();
      model->AddField(std::move(field));
    }

    if (coll->isSubsetCollection()) {
      const auto brName = root_utils::subsetBranch(name);
      auto collClassName = "vector<podio::ObjectID>";
      auto field = ROOT::Experimental::Detail::RFieldBase::Create(brName, collClassName).Unwrap();
      model->AddField(std::move(field));
    } else {

      const auto relVecNames = podio::DatamodelRegistry::instance().getRelationNames(coll->getValueTypeName());
      if (auto refColls = collBuffers.references) {
        int i = 0;
        for (auto& c [[maybe_unused]] : (*refColls)) {
          const auto brName = root_utils::refBranch(name, relVecNames.relations[i]);
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
          const auto brName = root_utils::vecBranch(name, relVecNames.vectorMembers[i]);
          auto field = ROOT::Experimental::Detail::RFieldBase::Create(brName, typeName).Unwrap();
          model->AddField(std::move(field));
          ++i;
        }
      }
    }
  }

  // Not supported by ROOT because podio::GenericParameters has map types
  // so we have to split them manually
  // model->MakeField<podio::GenericParameters>(root_utils::paramBranchName);

  model->AddField(
      ROOT::Experimental::Detail::RFieldBase::Create(root_utils::intKeyName, "std::vector<std::string>>").Unwrap());
  model->AddField(
      ROOT::Experimental::Detail::RFieldBase::Create(root_utils::floatKeyName, "std::vector<std::string>>").Unwrap());
  model->AddField(
      ROOT::Experimental::Detail::RFieldBase::Create(root_utils::doubleKeyName, "std::vector<std::string>>").Unwrap());
  model->AddField(
      ROOT::Experimental::Detail::RFieldBase::Create(root_utils::stringKeyName, "std::vector<std::string>>").Unwrap());

  model->AddField(
      ROOT::Experimental::Detail::RFieldBase::Create(root_utils::intValueName, "std::vector<std::vector<int>>")
          .Unwrap());
  model->AddField(
      ROOT::Experimental::Detail::RFieldBase::Create(root_utils::floatValueName, "std::vector<std::vector<float>>")
          .Unwrap());
  model->AddField(
      ROOT::Experimental::Detail::RFieldBase::Create(root_utils::doubleValueName, "std::vector<std::vector<double>>")
          .Unwrap());
  model->AddField(ROOT::Experimental::Detail::RFieldBase::Create(root_utils::stringValueName,
                                                                 "std::vector<std::vector<std::string>>")
                      .Unwrap());

  model->Freeze();
  return model;
}

ROOTNTupleWriter::CollectionInfo& ROOTNTupleWriter::getCategoryInfo(const std::string& category) {
  if (auto it = m_categories.find(category); it != m_categories.end()) {
    return it->second;
  }

  auto [it, _] = m_categories.try_emplace(category, CollectionInfo{});
  return it->second;
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
  for (auto& [c, _] : m_categories) {
    availableCategoriesField->push_back(c);
  }

  for (auto& [category, collInfo] : m_categories) {
    auto idField = m_metadata->MakeField<std::vector<unsigned int>>({root_utils::idTableName(category)});
    *idField = collInfo.id;
    auto collectionNameField = m_metadata->MakeField<std::vector<std::string>>({root_utils::collectionName(category)});
    *collectionNameField = collInfo.name;
    auto collectionTypeField = m_metadata->MakeField<std::vector<std::string>>({root_utils::collInfoName(category)});
    *collectionTypeField = collInfo.type;
    auto subsetCollectionField = m_metadata->MakeField<std::vector<short>>({root_utils::subsetCollection(category)});
    *subsetCollectionField = collInfo.isSubsetCollection;
    auto schemaVersionField = m_metadata->MakeField<std::vector<SchemaVersionT>>({"schemaVersion_" + category});
    *schemaVersionField = collInfo.schemaVersion;
  }

  m_metadata->Freeze();
  m_metadataWriter =
      ROOT::Experimental::RNTupleWriter::Append(std::move(m_metadata), root_utils::metaTreeName, *m_file, {});

  m_metadataWriter->Fill();

  m_file->Write();

  // All the tuple writers must be deleted before the file so that they flush
  // unwritten output
  for (auto& [_, catInfo] : m_categories) {
    catInfo.writer.reset();
  }
  m_metadataWriter.reset();

  m_finished = true;
}

std::tuple<std::vector<std::string>, std::vector<std::string>>
ROOTNTupleWriter::checkConsistency(const std::vector<std::string>& collsToWrite, const std::string& category) const {
  if (const auto it = m_categories.find(category); it != m_categories.end()) {
    return root_utils::getInconsistentColls(it->second.name, collsToWrite);
  }

  return {std::vector<std::string>{}, collsToWrite};
}

} // namespace podio

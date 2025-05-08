#include "podio/RNTupleWriter.h"
#include "podio/DatamodelRegistry.h"
#include "podio/podioVersion.h"
#include "podio/utilities/RootHelpers.h"
#include "rootUtils.h"

#include "TFile.h"

#include <ROOT/RField.hxx>
#include <ROOT/RNTupleModel.hxx>

#include <ROOT/RVersion.hxx>

// Adjust for the API stabilization of RNTuple
// https://github.com/root-project/root/pull/17804
#if ROOT_VERSION_CODE >= ROOT_VERSION(6, 35, 0)
using ROOT::RFieldBase;
using ROOT::RNTupleWriteOptions;
#else
using ROOT::Experimental::RFieldBase;
using ROOT::Experimental::RNTupleWriteOptions;
#endif

namespace podio {

RNTupleWriter::RNTupleWriter(const std::string& filename) :
    m_file(new TFile(filename.c_str(), "RECREATE", "data file")) {
}

RNTupleWriter::~RNTupleWriter() {
  if (!m_finished) {
    finish();
  }
}

template <typename T>
root_utils::ParamStorage<T>& RNTupleWriter::getParamStorage(CategoryInfo& catInfo) {
  if constexpr (std::is_same_v<T, int>) {
    return catInfo.intParams;
  } else if constexpr (std::is_same_v<T, float>) {
    return catInfo.floatParams;
  } else if constexpr (std::is_same_v<T, double>) {
    return catInfo.doubleParams;
  } else if constexpr (std::is_same_v<T, std::string>) {
    return catInfo.stringParams;
  } else {
    throw std::runtime_error("Unknown type");
  }
}

template <typename T>
void RNTupleWriter::fillParams(const GenericParameters& params, CategoryInfo& catInfo, root_compat::REntry* entry) {
  auto& paramStorage = getParamStorage<T>(catInfo);
  paramStorage = params.getKeysAndValues<T>();
  entry->BindRawPtr(root_utils::getGPKeyName<T>(), &paramStorage.keys);
  entry->BindRawPtr(root_utils::getGPValueName<T>(), &paramStorage.values);
}

void RNTupleWriter::writeFrame(const podio::Frame& frame, const std::string& category) {
  writeFrame(frame, category, frame.getAvailableCollections());
}

void RNTupleWriter::writeFrame(const podio::Frame& frame, const std::string& category,
                               const std::vector<std::string>& collsToWrite) {
  auto& catInfo = getCategoryInfo(category);

  // Use the writer as proxy to check whether this category has been initialized
  // already and do so if not
  const bool new_category = catInfo.writer == nullptr;
  if (new_category) {
    // This is the minimal information that we need for now
    catInfo.names = podio::utils::sortAlphabeticaly(collsToWrite);
  }

  std::vector<root_utils::StoreCollection> collections;
  collections.reserve(catInfo.names.size());
  // Only loop over the collections that were requested in the first Frame of
  // this category
  for (const auto& name : catInfo.names) {
    const auto* coll = frame.getCollectionForWrite(name);
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
    catInfo.writer = root_compat::RNTupleWriter::Append(std::move(model), category, *m_file.get(), {});

    catInfo.collInfo.reserve(collections.size());
    for (const auto& [name, coll] : collections) {
      catInfo.collInfo.emplace_back(coll->getID(), std::string(coll->getTypeName()), coll->isSubsetCollection(),
                                    coll->getSchemaVersion(), name, root_utils::getStorageTypeName(coll));
    }
  } else {
    if (!root_utils::checkConsistentColls(catInfo.collInfo, collsToWrite)) {
      throw std::runtime_error("Trying to write category '" + category + "' with inconsistent collection content. " +
                               root_utils::getInconsistentCollsMsg(catInfo.names, collsToWrite));
    }
  }

  const auto entry = m_categories[category].writer->GetModel().CreateBareEntry();

  RNTupleWriteOptions options;
  options.SetCompression(ROOT::RCompressionSetting::EDefaults::kUseGeneralPurpose);

  for (const auto& [name, coll] : collections) {
    const auto collBuffers = coll->getBuffers();
    if (collBuffers.vecPtr) {
      entry->BindRawPtr(name, static_cast<void*>(collBuffers.vecPtr));
    }

    if (coll->isSubsetCollection()) {
      const auto& refColl = (*collBuffers.references)[0];
      const auto brName = root_utils::subsetBranch(name);
      entry->BindRawPtr(brName, refColl.get());

    } else {

      const auto relVecNames = podio::DatamodelRegistry::instance().getRelationNames(coll->getValueTypeName());
      if (const auto refColls = collBuffers.references) {
        size_t i = 0;
        for (const auto& c : *refColls) {
          const auto brName = root_utils::refBranch(name, relVecNames.relations[i]);
          entry->BindRawPtr(brName, c.get());
          ++i;
        }
      }

      if (const auto vmInfo = collBuffers.vectorMembers) {
        size_t i = 0;
        for (const auto& [type, vec] : *vmInfo) {
          const auto typeName = "vector<" + type + ">";
          const auto brName = root_utils::vecBranch(name, relVecNames.vectorMembers[i]);
          auto ptr = *static_cast<std::vector<int>**>(vec);
          entry->BindRawPtr(brName, ptr);
          ++i;
        }
      }
    }

    // Not supported
    // entry->CaptureValueUnsafe(root_utils::paramBranchName,
    // &const_cast<podio::GenericParameters&>(frame.getParameters()));
  }

  const auto& params = frame.getParameters();
  fillParams<int>(params, catInfo, entry.get());
  fillParams<float>(params, catInfo, entry.get());
  fillParams<double>(params, catInfo, entry.get());
  fillParams<std::string>(params, catInfo, entry.get());

  m_categories[category].writer->Fill(*entry);
}

std::unique_ptr<root_compat::RNTupleModel>
RNTupleWriter::createModels(const std::vector<root_utils::StoreCollection>& collections) {
  auto model = root_compat::RNTupleModel::CreateBare();

  for (const auto& [name, coll] : collections) {
    // For the first entry in each category we also record the datamodel
    // definition
    m_datamodelCollector.registerDatamodelDefinition(coll, name);

    const auto collBuffers = coll->getBuffers();

    if (collBuffers.vecPtr) {
      const auto collClassName = "std::vector<" + std::string(coll->getDataTypeName()) + ">";
      auto field = RFieldBase::Create(name, collClassName).Unwrap();
      model->AddField(std::move(field));
    }

    if (coll->isSubsetCollection()) {
      const auto brName = root_utils::subsetBranch(name);
      const auto collClassName = "vector<podio::ObjectID>";
      auto field = RFieldBase::Create(brName, collClassName).Unwrap();
      model->AddField(std::move(field));
    } else {

      const auto relVecNames = podio::DatamodelRegistry::instance().getRelationNames(coll->getValueTypeName());
      if (const auto refColls = collBuffers.references) {
        size_t i = 0;
        for (const auto& c [[maybe_unused]] : *refColls) {
          const auto brName = root_utils::refBranch(name, relVecNames.relations[i]);
          const auto collClassName = "vector<podio::ObjectID>";
          auto field = RFieldBase::Create(brName, collClassName).Unwrap();
          model->AddField(std::move(field));
          ++i;
        }
      }

      if (const auto vminfo = collBuffers.vectorMembers) {
        size_t i = 0;
        for (const auto& [type, vec] : *vminfo) {
          const auto typeName = "vector<" + type + ">";
          const auto brName = root_utils::vecBranch(name, relVecNames.vectorMembers[i]);
          auto field = RFieldBase::Create(brName, typeName).Unwrap();
          model->AddField(std::move(field));
          ++i;
        }
      }
    }
  }

  // Not supported by ROOT because podio::GenericParameters has map types
  // so we have to split them manually
  // model->MakeField<podio::GenericParameters>(root_utils::paramBranchName);

  model->AddField(RFieldBase::Create(root_utils::intKeyName, "std::vector<std::string>>").Unwrap());
  model->AddField(RFieldBase::Create(root_utils::floatKeyName, "std::vector<std::string>>").Unwrap());
  model->AddField(RFieldBase::Create(root_utils::doubleKeyName, "std::vector<std::string>>").Unwrap());
  model->AddField(RFieldBase::Create(root_utils::stringKeyName, "std::vector<std::string>>").Unwrap());

  model->AddField(RFieldBase::Create(root_utils::intValueName, "std::vector<std::vector<int>>").Unwrap());
  model->AddField(RFieldBase::Create(root_utils::floatValueName, "std::vector<std::vector<float>>").Unwrap());
  model->AddField(RFieldBase::Create(root_utils::doubleValueName, "std::vector<std::vector<double>>").Unwrap());
  model->AddField(RFieldBase::Create(root_utils::stringValueName, "std::vector<std::vector<std::string>>").Unwrap());

  model->Freeze();
  return model;
}

RNTupleWriter::CategoryInfo& RNTupleWriter::getCategoryInfo(const std::string& category) {
  if (const auto it = m_categories.find(category); it != m_categories.end()) {
    return it->second;
  }

  const auto [it, _] = m_categories.try_emplace(category, CategoryInfo{});
  return it->second;
}

void RNTupleWriter::finish() {
  auto metadata = root_compat::RNTupleModel::Create();

  const auto podioVersion = podio::version::build_version;
  auto versionField = metadata->MakeField<std::vector<uint16_t>>(root_utils::versionBranchName);
  *versionField = {podioVersion.major, podioVersion.minor, podioVersion.patch};

  const auto edmDefinitions = m_datamodelCollector.getDatamodelDefinitionsToWrite();
  for (const auto& [name, _] : edmDefinitions) {
    const auto edmVersion = DatamodelRegistry::instance().getDatamodelVersion(name);
    if (edmVersion) {
      const auto edmVersionField = metadata->MakeField<std::vector<uint16_t>>(root_utils::edmVersionBranchName(name));
      *edmVersionField = {edmVersion->major, edmVersion->minor, edmVersion->patch};
    }
  }

  const auto edmField =
      metadata->MakeField<std::vector<std::tuple<std::string, std::string>>>(root_utils::edmDefBranchName);
  *edmField = std::move(edmDefinitions);

  const auto availableCategoriesField = metadata->MakeField<std::vector<std::string>>(root_utils::availableCategories);
  for (const auto& [c, _] : m_categories) {
    availableCategoriesField->push_back(c);
  }

  for (const auto& [category, collInfo] : m_categories) {
    const auto collInfoField =
        metadata->MakeField<std::vector<root_utils::CollectionWriteInfo>>({root_utils::collInfoName(category)});
    *collInfoField = collInfo.collInfo;
  }

  metadata->Freeze();
  const auto metadataWriter =
      root_compat::RNTupleWriter::Append(std::move(metadata), root_utils::metaTreeName, *m_file, {});

  metadataWriter->Fill();

  m_file->Write();

  // All the tuple writers must be deleted before the file so that they flush
  // unwritten output
  for (auto& [_, catInfo] : m_categories) {
    catInfo.writer.reset();
  }

  m_finished = true;
}

std::tuple<std::vector<std::string>, std::vector<std::string>>
RNTupleWriter::checkConsistency(const std::vector<std::string>& collsToWrite, const std::string& category) const {
  if (const auto it = m_categories.find(category); it != m_categories.end()) {
    return root_utils::getInconsistentColls(it->second.names, collsToWrite);
  }

  return {std::vector<std::string>{}, collsToWrite};
}

} // namespace podio

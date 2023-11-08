#ifndef PODIO_ROOTNTUPLEWRITER_H
#define PODIO_ROOTNTUPLEWRITER_H

#include "podio/CollectionBase.h"
#include "podio/Frame.h"
#include "podio/GenericParameters.h"
#include "podio/SchemaEvolution.h"
#include "podio/utilities/DatamodelRegistryIOHelpers.h"

#include "TFile.h"
#include <ROOT/RNTuple.hxx>
#include <ROOT/RNTupleModel.hxx>

#include <string>
#include <unordered_map>
#include <vector>

namespace podio {

class ROOTNTupleWriter {
public:
  ROOTNTupleWriter(const std::string& filename);
  ~ROOTNTupleWriter();

  ROOTNTupleWriter(const ROOTNTupleWriter&) = delete;
  ROOTNTupleWriter& operator=(const ROOTNTupleWriter&) = delete;

  template <typename T>
  void fillParams(GenericParameters& params, ROOT::Experimental::REntry* entry);

  void writeFrame(const podio::Frame& frame, const std::string& category);
  void writeFrame(const podio::Frame& frame, const std::string& category, const std::vector<std::string>& collsToWrite);
  void finish();

private:
  using StoreCollection = std::pair<const std::string&, podio::CollectionBase*>;
  std::unique_ptr<ROOT::Experimental::RNTupleModel> createModels(const std::vector<StoreCollection>& collections);

  std::unique_ptr<ROOT::Experimental::RNTupleModel> m_metadata{};
  std::unique_ptr<ROOT::Experimental::RNTupleWriter> m_metadataWriter{};

  std::unique_ptr<TFile> m_file{};

  DatamodelDefinitionCollector m_datamodelCollector{};

  struct CollectionInfo {
    std::vector<uint32_t> id{};
    std::vector<std::string> name{};
    std::vector<std::string> type{};
    std::vector<short> isSubsetCollection{};
    std::vector<SchemaVersionT> schemaVersion{};
    std::unique_ptr<ROOT::Experimental::RNTupleWriter> writer{nullptr};
  };
  CollectionInfo& getCategoryInfo(const std::string& category);

  std::unordered_map<std::string, CollectionInfo> m_categories{};

  bool m_finished{false};

  std::vector<std::string> m_intkeys{}, m_floatkeys{}, m_doublekeys{}, m_stringkeys{};

  std::vector<std::vector<int>> m_intvalues{};
  std::vector<std::vector<float>> m_floatvalues{};
  std::vector<std::vector<double>> m_doublevalues{};
  std::vector<std::vector<std::string>> m_stringvalues{};

  template <typename T>
  std::pair<std::vector<std::string>&, std::vector<std::vector<T>>&> getKeyValueVectors();
};

} // namespace podio

#endif // PODIO_ROOTNTUPLEWRITER_H

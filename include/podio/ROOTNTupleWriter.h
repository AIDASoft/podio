#ifndef PODIO_ROOTNTUPLEWRITER_H
#define PODIO_ROOTNTUPLEWRITER_H

#include "podio/CollectionBase.h"
#include "podio/GenericParameters.h"
#include "podio/utilities/DatamodelRegistryIOHelpers.h"
#include "podio/Frame.h"

#include "TFile.h"
#include <ROOT/RNTuple.hxx>
#include <ROOT/RNTupleModel.hxx>

#include <string>
#include <vector>
#include <unordered_map>

namespace podio {

class ROOTNTupleWriter {
public:
  ROOTNTupleWriter(const std::string& filename);
  ~ROOTNTupleWriter();

  ROOTNTupleWriter(const ROOTNTupleWriter&) = delete;
  ROOTNTupleWriter& operator=(const ROOTNTupleWriter&) = delete;

  template<typename T>
  void fillParams(const std::string& category, GenericParameters& params);

  void writeFrame(const podio::Frame& frame, const std::string& category);
  void writeFrame(const podio::Frame& frame, const std::string& category, const std::vector<std::string>& collsToWrite);
  void finish();

private:

  using StoreCollection = std::pair<const std::string&, podio::CollectionBase*>;
  std::unique_ptr<ROOT::Experimental::RNTupleModel> createModels(const std::vector<StoreCollection>& collections);

  std::unique_ptr<ROOT::Experimental::RNTupleModel> m_metadata{};
  std::unordered_map<std::string, std::unique_ptr<ROOT::Experimental::RNTupleWriter>> m_writers{};
  std::unique_ptr<ROOT::Experimental::RNTupleWriter> m_metadataWriter{};

  std::unique_ptr<TFile> m_file{};

  DatamodelDefinitionCollector m_datamodelCollector{};

  struct CollectionInfo {
    std::vector<int> id{};
    std::vector<std::string> name{};
    std::vector<std::string> type{};
    std::vector<short> isSubsetCollection{};
  };

  std::unordered_map<std::string, CollectionInfo> m_collectionInfo{};

  std::set<std::string> m_categories{};

  bool m_finished{false};

};

} //namespace podio

#endif //PODIO_ROOTNTUPLEWRITER_H

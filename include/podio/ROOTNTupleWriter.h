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
#include <iostream>
#include <string_view>

namespace podio {

class ROOTNTupleWriter {
public:
  ROOTNTupleWriter(const std::string& filename);
  // ~ROOTNTupleWriter();

  ROOTNTupleWriter(const ROOTNTupleWriter&) = delete;
  ROOTNTupleWriter& operator=(const ROOTNTupleWriter&) = delete;

  void writeFrame(const podio::Frame& frame, const std::string& category);
  void writeFrame(const podio::Frame& frame, const std::string& category, const std::vector<std::string>& collsToWrite);
  void finish();

private:

  using StoreCollection = std::pair<const std::string&, podio::CollectionBase*>;
  std::unique_ptr<ROOT::Experimental::RNTupleModel> createModels(const std::vector<StoreCollection>& collections);

  std::unique_ptr<ROOT::Experimental::RNTupleModel> m_metadata {nullptr};
  std::map<std::string, std::unique_ptr<ROOT::Experimental::RNTupleWriter>> m_writers;
  std::unique_ptr<ROOT::Experimental::RNTupleWriter> m_metadataWriter {nullptr};

  std::unique_ptr<TFile> m_file {nullptr};

  DatamodelDefinitionCollector m_datamodelCollector{};

  std::set<std::string> m_categories;
  std::map<std::string, std::vector<int>> m_collectionId;
  std::map<std::string, std::vector<std::string>> m_collectionName;
  std::map<std::string, std::vector<std::string>> m_collectionType;
  std::map<std::string, std::vector<bool>> m_isSubsetCollection;

};

} //namespace podio

#endif //PODIO_ROOTNTUPLEWRITER_H

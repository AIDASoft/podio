
// podio specific includes
#include "podio/ROOTReader.h"

namespace podio {


  void ROOTReader::openFile(const std::string& filename){
    openFiles({filename});
  }

  void ROOTReader::openFiles(const std::vector<std::string>& filenames){
    m_chain = new TChain("events");
    for (const auto& filename:  filenames) {
      m_chain->Add(filename.c_str());
    }
    CollectionIDTable* l_table = new CollectionIDTable();
    auto metadatatree = static_cast<TTree*>(m_chain->GetFile()->Get("metadata"));
    metadatatree->SetBranchAddress("CollectionIDs",&l_table);
    metadatatree->GetEntry(0);
    auto l_names = l_table->names();
    std::vector<int> l_collectionIDs;
    for (auto name: l_names) {
      l_collectionIDs.push_back(l_table->collectionID(name));

    }
    m_table = new CollectionIDTable(l_collectionIDs, l_names);
  }

  void ROOTReader::closeFile(){
    closeFiles();
  }

  void ROOTReader::closeFiles() {
    delete m_chain;
  }

  void ROOTReader::readEvent(){
    m_chain->GetEntry(m_eventNumber);
    // first prepare all collections in memory...
    for(auto inputs : m_inputs){
      inputs.first->prepareAfterRead();
    }
    // ...then clean-up the references between them
//    for(auto inputs : m_inputs){
  //    inputs.first->setReferences(m_registry);

  //  }
  }


  void ROOTReader::endOfEvent() {
    ++m_eventNumber;
    m_inputs.clear();
  }

  unsigned ROOTReader::getEntries() const {
    return m_chain->GetEntries();
  }

  void ROOTReader::goToEvent(unsigned eventNumber) {
    m_eventNumber = eventNumber;
    m_inputs.clear();
  }


} //namespace

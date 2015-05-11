
// ROOT specifc includes
#include "TFile.h"
#include "TTree.h"

// podio specific includes
#include "podio/CollectionBase.h"
#include "podio/EventStore.h"
#include "podio/ROOTWriter.h"

namespace podio {

  ROOTWriter::ROOTWriter(const std::string& filename, EventStore* store) :
    m_filename(filename),
    m_store(store),
    m_file(new TFile(filename.c_str(),"RECREATE","data file")),
    m_datatree(new TTree("events","Events tree")),
    m_metadatatree(new TTree("metadata", "Metadata tree"))
  {}

  ROOTWriter::~ROOTWriter(){
    delete m_file;
  }

  void ROOTWriter::writeEvent(){
    for (auto& coll : m_storedCollections){
      coll->prepareForWrite();
    }
    m_datatree->Fill();
  }

  void ROOTWriter::finish(){
    // now we want to safe the metadata
    m_metadatatree->Branch("CollectionIDs",m_store->getCollectionIDTable());
    m_metadatatree->Fill();
    m_file->Write();
    m_file->Close();
  }

} // namespace

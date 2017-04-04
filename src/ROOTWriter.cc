
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

 void ROOTWriter::registerForWrite(const std::string& name){
    const podio::CollectionBase* tmp_coll(nullptr);
    m_store->get(name, tmp_coll);
    podio::CollectionBase* coll = const_cast<CollectionBase*>(tmp_coll);
    std::string className( typeid(*(tmp_coll)).name() );
    size_t  pos = className.find_first_not_of("0123456789");
    className.erase(0,pos);
    // demangling the namespace: due to namespace additional characters were introduced:
    // e.g. N3fcc18TrackHit
    // remove any number+char before the namespace:
    pos = className.find_first_of("0123456789");
    if (pos != std::string::npos) {
      size_t pos1 = className.find_first_not_of("0123456789", pos);
      className.erase(0, pos1);
    }
    // replace any numbers between namespace and class with "::"
    pos = className.find_first_of("0123456789");
    if (pos != std::string::npos) {
      size_t pos1 = className.find_first_not_of("0123456789", pos);
      className.replace(pos, pos1-pos, "::");
    }
    // transform XCollection into vector<XData>
    pos = className.find("Collection");
    className.erase(pos,pos+10);
    std::string collClassName = "vector<"+className+"Data>";

    if(coll==nullptr) {
      std::cerr<<"no such collection to write, throw exception."<<std::endl;
    }
    else {
      m_datatree->Branch(name.c_str(),  collClassName.c_str(), coll->getBufferAddress());
      auto colls = coll->referenceCollections();
      if (colls != nullptr){
      int i = 0;
      for(auto& c : (*colls)){
        m_datatree->Branch((name+"#"+std::to_string(i)).c_str(),c);
        ++i;
      }
    }
      m_storedCollections.emplace_back(coll);
    }
  }

} // namespace

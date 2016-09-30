// podio specific includes
#include "podio/CollectionBase.h"
#include "podio/EventStore.h"
#include "podio/ASCIIWriter.h"

namespace podio {

  ASCIIWriter::ASCIIWriter(const std::string& filename, EventStore* store) :
    m_filename(filename),
    m_store(store),m_file( new std::ofstream ){

    m_file->open( filename , std::ofstream::binary  ) ;
  }

  ASCIIWriter::~ASCIIWriter(){
    delete m_file ;
  }

  void ASCIIWriter::writeEvent(){
    
    unsigned i = 0 ;
    for (auto& coll : m_storedCollections){
      coll->prepareForWrite();
      
 
      const std::string& name = m_collectionNames[i++] ;
      std::cout << " writing collection " << name << std::endl ;
      
      *m_file << name << "  " ;

      ColWriterBase* wrt = m_map[ name ] ;

      wrt->writeCollection( coll, *m_file ) ;

    }
  }
  
  void ASCIIWriter::finish(){
    // // now we want to safe the metadata
    // m_metadatatree->Branch("CollectionIDs",m_store->getCollectionIDTable());
    // m_metadatatree->Fill();
    // m_file->Write();
    
    m_file->close();
  }
  
} // namespace

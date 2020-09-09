#ifndef SIOReader_H
#define SIOReader_H


#include <algorithm>
#include <map>
#include <string>
#include <vector>
#include <iostream>


#include "podio/ICollectionProvider.h"
#include "podio/IReader.h"
#include "podio/EventStore.h"
#include "podio/SIOBlock.h"

// -- sio headers
#include <sio/definitions.h>
#include <sio/exception.h>
#include <sio/api.h>
#include <sio/buffer.h>

namespace podio {

  class CollectionBase;
  class CollectionIDTable;

/**
   This class has the function to read available data from disk
   and to prepare collections and buffers.
**/
  class SIOReader : public IReader {
    friend EventStore;
  public:
    SIOReader();
    ~SIOReader();
    void openFile(const std::string& filename);
    void closeFile();

    /// Read all collections requested
    void readEvent();

    /// Read CollectionIDTable from SIO file
    CollectionIDTable* getCollectionIDTable() override final {return m_table;}

    /// Check if file is valid
    virtual bool isValid() const override final;

    // /// request the named collection of type T to be read and register with store
    // template<typename T>
    // void registerCollection(const std::string& name, EventStore* store){

    //   // create and register SIOBlock ...

    //   T* col = new T ;
    //   auto blk = podio::SIOBlockFactory::instance().createBlock( col, name ) ;

    //   if(!blk){
    // 	std::string typeName = demangleClassName( col ) ;
    // 	throw std::runtime_error(" could not create block for type: "+typeName );
    //   }

    //   blk->setCollectionProvider( store ) ;

    //   m_blocks.push_back( blk ) ;

    //   store->registerCollection( name, col ) ;

    //   m_table.add( name ) ;
    // }

    void endOfEvent();

  private:
    /// Implementation for collection reading
    CollectionBase* readCollection(const std::string& name) override final;

    /// read event meta data for current event
    GenericParameters* readEventMetaData() override final { return m_eventMetaData->metadata; }

    /// read the collection meta data
    std::map<int,GenericParameters>* readCollectionMetaData() override final;

    /// read the run meta data
    std::map<int,GenericParameters>* readRunMetaData() override final;

  private:
    void readCollectionIDTable();
    void readMetaDataRecord(std::shared_ptr<SIONumberedMetaDataBlock> mdBlock);

    typedef std::pair<CollectionBase*, std::string> Input;
    std::vector<Input> m_inputs;
    CollectionIDTable* m_table{nullptr}; // will be owned by the EventStore
    int m_eventNumber{0};
    int m_lastEvtRead{-1};

    std::shared_ptr<SIOEventMetaDataBlock> m_eventMetaData{};
    std::shared_ptr<SIONumberedMetaDataBlock> m_runMetaData{};
    std::shared_ptr<SIONumberedMetaDataBlock> m_collectionMetaData{};

    sio::ifstream    m_stream{} ;
    sio::record_info m_rec_info{} ;
    sio::buffer      m_info_buffer{ sio::max_record_info_len } ;
    sio::buffer      m_rec_buffer{ sio::mbyte } ;
    sio::buffer      m_unc_buffer{ sio::mbyte } ;
    sio::block_list  m_blocks {} ;
  };


} // namespace

#endif

#ifndef SIOWRITER_H
#define SIOWRITER_H

#include "podio/CollectionBase.h"
#include "podio/EventStore.h"

// SIO specific includes
#include <sio/definitions.h>
#include <sio/exception.h>
#include <sio/api.h>

#include <string>
#include <vector>
#include <iostream>

// forward declarations


namespace podio {
  class Registry;
  class SIOMetaDataBlock;

  class SIOWriter {

  public:
    SIOWriter(const std::string& filename, EventStore* store);
    ~SIOWriter();

    void registerForWrite(const std::string& name);
    void writeMetaData() ;
    void writeEvent();
    void finish();

  private:
    // members
    std::string m_filename{};
    EventStore* m_store=nullptr;

    sio::ofstream m_stream{} ;
    sio::block_list  m_blocks {} ;
    sio::buffer m_buffer{ sio::mbyte } ;
    sio::buffer m_com_buffer{ sio::mbyte } ;
    bool m_metaData = false ;
    SIOMetaDataBlock* m_metaDataBlock=nullptr;
  };

} //namespace
#endif

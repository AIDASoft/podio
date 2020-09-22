#ifndef SIOBlock_H
#define SIOBlock_H

#include <podio/CollectionBase.h>
#include <podio/CollectionIDTable.h>
#include <podio/GenericParameters.h>
#include <podio/EventStore.h>

#include <sio/block.h>
#include <sio/version.h>
#include <sio/io_device.h>

#include <map>
#include <string>
#include <memory>

namespace podio {

  template <typename devT, typename PODData>
  void handlePODDataSIO( devT &device , PODData* data, size_t size) {
    unsigned count =  size * sizeof(PODData) ;
    char* dataPtr = reinterpret_cast<char*> (data) ;
    device.data( dataPtr , count ) ;
  }


/// Base class for sio::block handlers used with PODIO
  class SIOBlock: public sio::block {

  public:

    SIOBlock( const std::string &nam, sio::version_type vers) :
      sio::block( nam, vers ){
    }

    podio::CollectionBase* getCollection() { return _col; }

    std::string name(){ return sio::block::name() ; }

    void setCollection(podio::CollectionBase* col) {
      _col = col ;
    }

    virtual SIOBlock* const create(const std::string& name)=0 ;

    // create a new collection for this block
    virtual void createCollection() = 0;

  protected:

    podio::CollectionBase*  _col{} ;
  };

  /**
   * A dedicated block for handling the I/O of the CollectionIDTable
   */
  class SIOCollectionIDTableBlock : public sio::block {
  public:
    SIOCollectionIDTableBlock() :
      sio::block("CollectionIDs", sio::version::encode_version(0, 1)) {}

    SIOCollectionIDTableBlock(podio::EventStore* store) :
      sio::block("CollectionIDs", sio::version::encode_version(0, 1)),
      _store(store), _table(store->getCollectionIDTable()) {}

    virtual void read(sio::read_device& device, sio::version_type version) override;
    virtual void write(sio::write_device& device) override;

    podio::CollectionIDTable* getTable() { return _table; }
    const std::vector<std::string>& getTypeNames() const { return _types; }

  private:
    podio::EventStore* _store{nullptr};
    podio::CollectionIDTable* _table {nullptr};
    std::vector<std::string> _types;
  };


  /**
   * A block for handling the EventMeta data
   */
  class SIOEventMetaDataBlock : public sio::block {
  public:
    SIOEventMetaDataBlock() :
      sio::block("EventMetaData", sio::version::encode_version(0, 1)) {}

    virtual void read(sio::read_device& device, sio::version_type version) override;
    virtual void write(sio::write_device& device) override;

    podio::GenericParameters* metadata{nullptr};
  };

  /**
   * A block for handling the run and collection meta data
   */
  class SIONumberedMetaDataBlock : public sio::block {
  public:
    SIONumberedMetaDataBlock(const std::string& name) :
      sio::block(name, sio::version::encode_version(0, 1)) {}

    virtual void read(sio::read_device& device, sio::version_type version) override;
    virtual void write(sio::write_device& device) override;

    std::map<int, GenericParameters>* data{nullptr};
  };


/// factory for creating sio::blocks for a given type of EDM-collection
  class SIOBlockFactory {
  private:
    SIOBlockFactory(){};

    typedef std::map<std::string, SIOBlock*> BlockMap ;
    BlockMap  _map ;
  public:
    void registerBlockForCollection(std::string type, SIOBlock* b){ _map[type] = b ; }

    std::shared_ptr<SIOBlock> createBlock( const podio::CollectionBase* col, const std::string& name) const;

    // return a block with a new collection (used for reading )
    std::shared_ptr<SIOBlock> createBlock( const std::string& typeStr, const std::string& name) const;

    static SIOBlockFactory& instance() {
      static SIOBlockFactory me ;
      return me ;
    }
  };

/// helper method for class name demangling
  inline std::string demangleClassName(std::string rawName){
    std::string className( rawName );
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
    return className ;
  }

  template <class T>
  std::string demangleClassName(const T* t){
    return ( typeid(*(t)).name() );
  }


  class SIOBlockLibraryLoader {
  private:
    SIOBlockLibraryLoader();
    /**
     * Load a library with the given name via dlopen
     */
    void loadLib(const std::string& libname);
    /**
     * Get all files that are found on LD_LIBRARY_PATH and that have "SioBlocks"
     * in their name
     */
    static std::vector<std::string> getLibNames();

    std::map<std::string, void*> _loadedLibs{};

  public:
    static SIOBlockLibraryLoader& instance() {
      static SIOBlockLibraryLoader instance;
      return instance;
    }
  };

  namespace sio_helpers {
    /// marker for showing that a TOC has been stored in the file
    static constexpr uint32_t SIOTocMarker = 0xc001fea7;
    /// the number of bits necessary to store the SIOTocMarker and the actual
    /// position of the start of the SIOFileTOCRecord
    static constexpr int SIOTocInfoSize = sizeof(uint64_t); // i.e. usually 8
    /// The name of the TOCRecord
    static constexpr const char* SIOTocRecordName = "podio_SIO_TOC_Record";

    // should hopefully be enough for all practical purposes
    using position_type = uint32_t;
  }


  class SIOFileTOCRecord {
  public:
    using PositionType = sio_helpers::position_type;
    void addRecord(const std::string& name, PositionType startPos);

    size_t getNRecords(const std::string& name) const;

  private:
    friend class SIOFileTOCRecordBlock;

    using RecordListType = std::pair<std::string, std::vector<PositionType>>;
    using MapType = std::vector<RecordListType>;

    MapType m_recordMap{};
  };

  struct SIOFileTOCRecordBlock : public sio::block {
      SIOFileTOCRecordBlock() : sio::block(sio_helpers::SIOTocRecordName, sio::version::encode_version(0, 1)) {}

    virtual void read(sio::read_device& device, sio::version_type version) override;
    virtual void write(sio::write_device& device) override;

    SIOFileTOCRecord* record;
  };

} // end namespace
#endif

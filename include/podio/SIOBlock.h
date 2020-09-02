#ifndef SIOBlock_H
#define SIOBlock_H

#include <podio/CollectionBase.h>

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


  /// helper struct for handling SIO blocks
  struct SIOMetaData {
    int colID;
    std::string name;
    std::string typeName;
  };

  // helper class for reading/writing meta data
  class SIOMetaDataBlock: public sio::block {

  public:
    std::vector<SIOMetaData> m_vec;

    SIOMetaDataBlock( const std::string &nam, sio::version_type vers) :
      sio::block( nam, vers ) {
    }

    virtual void read( sio::read_device &device, sio::version_type vers ) override;

    virtual void write( sio::write_device &device ) override;

    void add(int id, std::string nam, std::string typ) {
      m_vec.push_back( {id, nam, typ} );
    }

  };

/// Base class for sio::block handlers used with PODIO
  class SIOBlock: public sio::block {

  public:


    SIOBlock( const std::string &nam, sio::version_type vers) :
      sio::block( nam, vers ){
    }

    podio::CollectionBase* getCollection() { return _col; }
    podio::ICollectionProvider* getCollectionProvider(){ return _store; }

    std::string name(){ return sio::block::name() ; }


    void setCollection(podio::CollectionBase* col) {
      _col = col ;
    }
    void setCollectionProvider(podio::ICollectionProvider* iCP ){ _store = iCP ; }

    void prepareAfterRead(){ _col->prepareAfterRead(); }

    void setReferences(){
      if(_store != nullptr)
        _col->setReferences(_store);
    }

    virtual SIOBlock* const create(const std::string& name)=0 ;

    // create a new collection for this block
    virtual void createCollection() = 0;

  protected:

    podio::CollectionBase*  _col{} ;
    podio::ICollectionProvider* _store{} ;

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

} // end namespace
#endif

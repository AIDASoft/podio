#ifndef USERDATACOLLECTION_H
#define USERDATACOLLECTION_H

#include "podio/CollectionBase.h"

#include <map>
#include <string>
#include <utility>
#include <vector>
#include <typeindex>


namespace podio {


  ///helper class providing names for basic types used in UserDataCollection<T>
  class  UserDataTypes{
  private:
    UserDataTypes() = default ;
    UserDataTypes(const UserDataTypes& ) = delete;
    UserDataTypes& operator=(const UserDataTypes& ) = delete;
    ~UserDataTypes() = default;

    const std::map< const std::type_index, const std::string >_typeMap =
    {
      { std::type_index( typeid(int) ),   "podio::int"   },
      { std::type_index( typeid(long) ),  "podio::long"  },
      { std::type_index( typeid(float) ), "podio::float" },
      { std::type_index( typeid(double) ),"podio::double"}
    } ;

  public:

    std::string name( std::type_index idx ){
      auto it = _typeMap.find( idx ) ;
      return it != _typeMap.end() ? it->second  :  "UNKNOWN"  ;
    }

    // typename w/ 'podio::' replaced by 'podio_'
    std::string sio_name( std::type_index idx ){
      return std::string("podio_") + name( idx ).substr( 7 , 1024)  ;
    }

    static UserDataTypes& instance(){
      static UserDataTypes me ;
      return me ;
    }
  } ;


  /// Templated base class for storing std::vectors of basic types as user data in PODIO
  template <typename BasicType>
  class UserDataCollection : public CollectionBase {

  private:
    std::vector<BasicType> _vec{} ;
    std::vector<BasicType>* _vecPtr = &_vec ;
    int m_collectionID{0};
    CollRefCollection m_refCollections{};
    VectorMembersInfo m_vecmem_info{};

  public:

    UserDataCollection() = default ;
    UserDataCollection(const UserDataCollection& ) = delete;
    UserDataCollection& operator=(const UserDataCollection& ) = delete;
    ~UserDataCollection() = default;


    /// prepare buffers for serialization
    void prepareForWrite() override final {}

    /// re-create collection from buffers after read
    void  prepareAfterRead() override final {}

    /// initialize references after read
    bool setReferences(const ICollectionProvider* ) override final {
      return true ;
    }

    /// set collection ID
    void setID(unsigned id) override final {
      m_collectionID = id;
    }

    /// get collection ID
    unsigned getID() const override final {
      return m_collectionID ;
    }

    /// Get the collection buffers for this collection
    podio::CollectionBuffers getBuffers() override final {
      return { &_vecPtr,
	&m_refCollections, // only need to store the ObjectIDs of the referenced objects
	&m_vecmem_info } ;
    }

    /// check for validity of the container after read
    bool isValid() const override final {
      return true ;
    }

    /// number of elements in the collection
    size_t size() const override final {
      return _vec.size() ;
    }

    /// fully qualified type name of elements - with namespace
    std::string getValueTypeName() const override final {

      return UserDataTypes::instance().name( typeid(BasicType) ) ;
    }

    /// clear the collection and all internal states
    void clear() override final {
      _vec.clear() ;
    } ;


    /// check if this collection is a subset collection - no subset possible
    bool isSubsetCollection() const override final {
      return false ;
    }

    /// declare this collection to be a subset collectionv - no effect
    void setSubsetCollection(bool) override final {}

    /// access to the actual data vector
    typename std::vector<BasicType>& vec() {
      return _vec;
    }

    /// const access to the actual data vector
    const typename std::vector<BasicType>& vec() const {
      return _vec;
    }


  };

  /// some typedefs to make ROOT happy

  typedef UserDataCollection<int>    intCollection ;
  typedef UserDataCollection<long>   longCollection ;
  typedef UserDataCollection<float>  floatCollection ;
  typedef UserDataCollection<double> doubleCollection ;

  typedef int    intData ;
  typedef long   longData ;
  typedef float  floatData ;
  typedef double doubleData ;

} // namespace

#endif

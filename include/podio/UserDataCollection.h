#ifndef USERDATACOLLECTION_H
#define USERDATACOLLECTION_H

#include "podio/CollectionBase.h"

#include <map>
#include <string>
#include <utility>
#include <vector>
#include <typeindex>


namespace podio {

  template <typename BasicType>
  std::string userDataTypeName() { throw std::runtime_error( std::string(" unsupported type for UserDataCollection: ")
							     + typeid(BasicType).name() ) ; }

  template <>  std::string userDataTypeName<int>()   {return "int" ; }
  template <>  std::string userDataTypeName<float>() {return "float" ; }
  template <>  std::string userDataTypeName<long>()  {return "long" ; }
  template <>  std::string userDataTypeName<double>(){return "double" ; }


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

    /// fully qualified type name
    std::string getTypeName() const override { return std::string("podio::UserDataCollection<")
	+ userDataTypeName< BasicType >() + ">" ; }

    /// fully qualified type name of elements - with namespace
    std::string getValueTypeName() const override { return userDataTypeName< BasicType >() ; }

    /// fully qualified type name of stored POD elements - with namespace
    std::string getDataTypeName() const override { return  userDataTypeName< BasicType >() ; }

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


    // ----- some wrapers for std::vector and access to the complete std::vector (if really needed)

    typename std::vector<BasicType>::iterator begin() { return _vec.begin() ; }
    typename std::vector<BasicType>::iterator end()   { return _vec.end() ; }
    typename std::vector<BasicType>::const_iterator begin() const { return _vec.begin() ; }
    typename std::vector<BasicType>::const_iterator end()   const { return _vec.end() ; }

    typename std::vector<BasicType>::reference operator[](size_t idx) { return _vec[idx] ; }
    typename std::vector<BasicType>::const_reference operator[](size_t idx) const { return _vec[idx] ; }

    void resize( size_t count ) { _vec.resize( count ) ; }
    void push_back( const BasicType& value ) { _vec.push_back( value ) ; }

    /// access to the actual data vector
    typename std::vector<BasicType>& vec() {
      return _vec;
    }

    /// const access to the actual data vector
    const typename std::vector<BasicType>& vec() const {
      return _vec;
    }


  };

  /// some typedefs to make ROOT and the ROOTWriter happy

  typedef UserDataCollection<int>    User_intCollection ;
  typedef UserDataCollection<long>   User_longCollection ;
  typedef UserDataCollection<float>  User_floatCollection ;
  typedef UserDataCollection<double> User_doubleCollection ;

} // namespace

#endif

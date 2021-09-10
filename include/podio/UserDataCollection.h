#ifndef USERDATACOLLECTION_H
#define USERDATACOLLECTION_H

#include "podio/CollectionBase.h"

#include <map>
#include <string>
#include <utility>
#include <vector>
#include <typeindex>

#define PODIO_ADD_USER_TYPE(type) template<> constexpr const char* userDataTypeName<type>( return #type; }


namespace podio {




  namespace detail {

    // some templates to ensure valid and supported user types
    // as suggested by T.Madlener, DESY

    /** tuple of basic types supported in user vector
     */
    using SupportedUserDataTypes = std::tuple<
      int,
      float,
      long,
      double
      >;

    /**
     * Helper function to check whether a type T is in an std::tuple<Ts...>
     */
    template<typename T, typename ...Ts>
    constexpr bool inTuple(std::tuple<Ts...>) {
      return (std::is_same_v<T, Ts> || ...);
    }

    /**
     * Compile time helper function to check whether the given type is in the list
     * of supported types
     */
    template<typename T>
    constexpr bool isSupported() {
      return inTuple<T>(SupportedUserDataTypes{});
    }
  }

  /**
   * Alias template to be used to enable template specializations only for the types listed in the
   * SupportedUserDataTypes list
   */
  template<typename T>
  using EnableIfSupportedUserType = std::enable_if_t<detail::isSupported<T>()>;


  /** helper template to provide readable type names for basic types
   */
  template <typename BasicType, typename = EnableIfSupportedUserType<BasicType> >
  constexpr const char*  userDataTypeName() ;

  template <> constexpr const char* userDataTypeName<int>()   {return "int" ; }
  template <> constexpr const char* userDataTypeName<float>() {return "float" ; }
  template <> constexpr const char* userDataTypeName<long>()  {return "long" ; }
  template <> constexpr const char* userDataTypeName<double>(){return "double" ; }


  /** Collection of basic types for additional user data not defined in the EDM.
   *  The data is stored in an std::vector<basic_type>. Supported are all basic types supported in
   *  PODIO - @see SupportedUserDataTypes.
   *  @author F.Gaede, DESY
   *  @date Sep 2021
   */
  template<typename BasicType, typename = EnableIfSupportedUserType< BasicType> >
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
	&m_refCollections,
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

} // namespace

#endif

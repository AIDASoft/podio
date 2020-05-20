#include "podio/GenericParameters.h"

#include <algorithm>

namespace podio{


  int GenericParameters::getIntVal(const std::string & key) const {
    
    IntMap::iterator it = _intMap.find( key ) ;

    if( it == _intMap.end() )  return 0 ;

    IntVec &  iv =  it->second ;

    return iv[0] ;
  }

  float GenericParameters::getFloatVal(const std::string & key) const {

    FloatMap::iterator it = _floatMap.find( key ) ;

    if( it == _floatMap.end() )  return 0 ;

    FloatVec &  fv =  it->second ;

    return fv[0] ;
  }

  const std::string & GenericParameters::getStringVal(const std::string & key) const {

    static std::string empty("") ;
    
    StringMap::iterator it = _stringMap.find( key ) ;
    
    if( it == _stringMap.end() )  return empty ;
    
    StringVec &  sv =  it->second ;
    
    return sv[0] ;
  }

  IntVec & GenericParameters::getIntVals(const std::string & key, IntVec & values) const {

    IntMap::iterator it = _intMap.find( key ) ;

    if( it != _intMap.end() ) {
      values.insert( values.end() , it->second.begin() , it->second.end() ) ;
    }

    return values ;
  }

  FloatVec & GenericParameters::getFloatVals(const std::string & key, FloatVec & values) const {

    FloatMap::iterator it = _floatMap.find( key ) ;

    if( it != _floatMap.end() ) {
      values.insert( values.end() , it->second.begin() , it->second.end() ) ;
    }
    return values ;
  }

  StringVec & GenericParameters::getStringVals(const std::string & key, StringVec & values) const {

    StringMap::iterator it = _stringMap.find( key ) ;

    if( it != _stringMap.end() ) {
      values.insert( values.end() , it->second.begin() , it->second.end() ) ;
    }
    return values ;
  }


  const StringVec & GenericParameters::getIntKeys(StringVec & keys) const  {

    std::transform( _intMap.begin() , _intMap.end() , back_inserter( keys )  ,
		[](auto const& pair){ return pair.first; }) ;

    return keys ;
  }

  const StringVec & GenericParameters::getFloatKeys(StringVec & keys) const  {
    
    std::transform( _floatMap.begin() , _floatMap.end() , back_inserter( keys )  ,
		[](auto const& pair){ return pair.first; }) ;

    return keys ;
  }

  const StringVec & GenericParameters::getStringKeys(StringVec & keys) const  {

    std::transform( _stringMap.begin() , _stringMap.end() , back_inserter( keys ),
		    [](auto const& pair){ return pair.first; }) ;
		    
    return keys ;
  }
  
  int GenericParameters::getNInt(const std::string & key) const {

    IntMap::iterator it = _intMap.find( key ) ;

    if( it == _intMap.end() )
      return 0 ;
    else
      return it->second.size() ;
  }

  int GenericParameters::getNFloat(const std::string & key) const {

    FloatMap::iterator it = _floatMap.find( key ) ;

    if( it == _floatMap.end() )  
      return 0 ;
    else
      return it->second.size() ;
  }

  int GenericParameters::getNString(const std::string & key) const {

    StringMap::iterator it = _stringMap.find( key ) ;

    if( it == _stringMap.end() )  
      return 0 ;
    else
      return it->second.size() ;
  }

  void GenericParameters::setValue(const std::string & key, int value){

    _intMap[ key ].clear() ;
    _intMap[ key ].push_back( value ) ;
  }

  void GenericParameters::setValue(const std::string & key, float value){

    _floatMap[ key ].clear() ;
    _floatMap[ key ].push_back( value ) ;
  }

  void GenericParameters::setValue(const std::string & key, const std::string & value) {

    _stringMap[ key ].clear() ;
    _stringMap[ key ].push_back( value ) ;

  }



  void GenericParameters::setValues(const std::string & key,const IntVec & values){
    
    _intMap[ key ].assign(  values.begin() , values.end() ) ;
  }
  
  void GenericParameters::setValues(const std::string & key,const  FloatVec & values){

    _floatMap[ key ].assign(  values.begin() , values.end() ) ;
  }
  
  void GenericParameters::setValues(const std::string & key, const StringVec & values){

    _stringMap[ key ].assign(  values.begin() , values.end() ) ;
  }

} // namespace 

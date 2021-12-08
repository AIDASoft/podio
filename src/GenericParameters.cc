#include "podio/GenericParameters.h"

#include <algorithm>

namespace podio{

  template<typename T>
  T getValFromMap(const GenericParameters::MapType<T>& map, const std::string& key) {
    const auto it = map.find(key);
    if (it == map.end()) return T{};
    const auto& iv = it->second;
    return iv[0];
  }

  // overload for string such that return by const ref is preferred
  const std::string& getValFromMap(const GenericParameters::MapType<std::string>& map, const std::string& key) {
    static const std::string empty("");
    auto it = map.find(key);
    if (it == map.end()) return empty;
    const auto& iv = it->second;
    return iv[0];
  }

  int GenericParameters::getIntVal(const std::string & key) const {
    return getValFromMap(_intMap, key);
  }

  float GenericParameters::getFloatVal(const std::string & key) const {
    return getValFromMap(_floatMap, key);
  }

  const std::string & GenericParameters::getStringVal(const std::string & key) const {
    return getValFromMap(_stringMap, key);
  }

  template<typename T>
  std::vector<T>& fillValsFromMap(const GenericParameters::MapType<T>& map, const std::string& key, std::vector<T>& values) {
    auto it = map.find(key);
    if (it != map.end()) {
      values.insert(values.end(), it->second.begin(), it->second.end());
    }
    return values;
  }

  IntVec & GenericParameters::getIntVals(const std::string & key, IntVec & values) const {
    return fillValsFromMap(_intMap, key, values);
  }

  FloatVec & GenericParameters::getFloatVals(const std::string & key, FloatVec & values) const {
    return fillValsFromMap(_floatMap, key, values);
  }

  StringVec & GenericParameters::getStringVals(const std::string & key, StringVec & values) const {
    return fillValsFromMap(_stringMap, key, values);
  }

  template<typename T>
  const StringVec& getKeys(const GenericParameters::MapType<T>& map, StringVec& keys) {
    std::transform(map.begin(), map.end(), std::back_inserter(keys),
                   [] (auto const& pair) { return pair.first; });
    return keys;
  }

  const StringVec & GenericParameters::getIntKeys(StringVec & keys) const  {
    return getKeys(_intMap, keys);
  }

  const StringVec & GenericParameters::getFloatKeys(StringVec & keys) const  {
    return getKeys(_floatMap, keys);
  }

  const StringVec & GenericParameters::getStringKeys(StringVec & keys) const  {
    return getKeys(_stringMap, keys);
  }

  template<typename T>
  int getStoredElements(const GenericParameters::MapType<T>& map, const std::string& key) {
    auto it = map.find( key ) ;
    if( it == map.end() )
      return 0 ;
    else
      return it->second.size() ;
  }

  int GenericParameters::getNInt(const std::string & key) const {
    return getStoredElements(_intMap, key);
  }

  int GenericParameters::getNFloat(const std::string & key) const {
    return getStoredElements(_floatMap, key);
  }

  int GenericParameters::getNString(const std::string & key) const {
    return getStoredElements(_stringMap, key);
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

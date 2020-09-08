// -*- C++ -*-
#ifndef GenericParameters_H
#define GenericParameters_H 1

#include <map>
#include <vector>
#include <string>

namespace podio {

  typedef std::vector<int> IntVec ;
  typedef std::vector<float> FloatVec ;
  typedef std::vector<std::string> StringVec ;

  /** GenericParameters objects allow to store generic named parameters of type
   *  int, float and string or vectors of these types. 
   *  They can be used  to store (user) meta data that is 
   *  run, event or collection dependent. 
   *  (based on lcio::LCParameters)
   * 
   * @author F. Gaede, DESY 
   * @date Apr 2020
   */
  
  class GenericParameters {
  public:
    template<typename T>
    using MapType = std::map<std::string, std::vector<T>>;

  private:
    using IntMap = MapType<int>;
    using FloatMap = MapType<float>;
    using StringMap = MapType<std::string>;

  public:
   
    /** Returns the first integer value for the given key.
     */
    int getIntVal(const std::string & key) const  ;
    
    /** Returns the first float value for the given key.
     */
    float getFloatVal(const std::string & key) const ;
    
    /** Returns the first string value for the given key.
     */
    const std::string & getStringVal(const std::string & key) const ;
    
    /** Adds all integer values for the given key to values.
     *  Returns a reference to values for convenience.
     */
    IntVec & getIntVals(const std::string & key, IntVec & values) const ;
    
    /** Adds all float values for the given key to values.
     *  Returns a reference to values for convenience.
     */
    FloatVec & getFloatVals(const std::string & key, FloatVec & values) const ;
    
    /** Adds all float values for the given key to values.
     *  Returns a reference to values for convenience.
     */
     StringVec & getStringVals(const std::string & key, StringVec & values) const ;
    
    /** Returns a list of all keys of integer parameters.
     */
    const StringVec & getIntKeys( StringVec & keys) const  ;

    /** Returns a list of all keys of float parameters.
     */
    const StringVec & getFloatKeys(StringVec & keys)  const ;

    /** Returns a list of all keys of string parameters.
     */
    const StringVec & getStringKeys(StringVec & keys)  const ;
    
    /** The number of integer values stored for this key.
     */ 
    int getNInt(const std::string & key) const ;
    
    /** The number of float values stored for this key.
     */ 
    int getNFloat(const std::string & key) const ;
    
    /** The number of string values stored for this key.
     */ 
    int getNString(const std::string & key) const ;
    
    /** Set integer value for the given key.
     */
    void setValue(const std::string & key, int value) ;

    /** Set float value for the given key.
     */
    void setValue(const std::string & key, float value) ;

    /** Set string value for the given key.
     */
    void setValue(const std::string & key, const std::string & value) ;

    /** Set integer values for the given key.
     */
    void setValues(const std::string & key, const IntVec & values);

    /** Set float values for the given key.
     */
    void setValues(const std::string & key, const FloatVec & values);

    /** Set string values for the given key.
     */
    void setValues(const std::string & key, const StringVec & values);

    /// erase all elements
    void clear() {
      _intMap.clear();
      _floatMap.clear();
      _stringMap.clear();
    }

    /**
     * Get the internal int map (necessary for serialization with SIO)
     */
    const IntMap& getIntMap() const { return _intMap; }
    IntMap& getIntMap() { return _intMap; }

    /**
     * Get the floaternal float map (necessary for serialization with SIO)
     */
    const FloatMap& getFloatMap() const { return _floatMap; }
    FloatMap& getFloatMap() { return _floatMap; }

    /**
     * Get the stringernal string map (necessary for serialization with SIO)
     */
    const StringMap& getStringMap() const { return _stringMap; }
    StringMap& getStringMap() { return _stringMap; }

  protected:

    IntMap _intMap{} ;
    FloatMap _floatMap{} ;
    StringMap _stringMap{} ;

  }; // class
} // namespace podio
#endif

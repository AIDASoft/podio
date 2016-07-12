#ifndef COLLECTIONIDTABLE_H
#define COLLECTIONIDTABLE_H

#include <string>
#include <vector>
#include <mutex>

namespace podio {

  class CollectionIDTable{

  public:

    /// default constructor
    CollectionIDTable() :
      m_collectionIDs(), m_names()
    {};

    /// constructor from existing ID:name mapping
    CollectionIDTable(const std::vector<int> ids, std::vector<std::string> names) :
      m_collectionIDs(ids), m_names(names)
    {};

    /// return collection ID for given name
    int collectionID(const std::string& name) const;

    /// return name for given collection ID
    const std::string name(int collectionID) const;

    /// Check if collection name is known
    bool present(const std::string& name) const; 

    /// register new name to the table
    /// returns assigned collection ID
    int add(const std::string& name);

    /// Prints collection information
    void print() const;

  private:
    std::vector<int>              m_collectionIDs;
    std::vector<std::string>      m_names;
    mutable std::recursive_mutex  m_mutex;
  };


} //namespace
#endif

#ifndef COLLECTIONIDTABLE_H
#define COLLECTIONIDTABLE_H

#include <string>
#include <vector>
#include <mutex>

namespace albers {

  class CollectionIDTable{

  public:

    CollectionIDTable() :
      m_collectionIDs(), m_names()
    {};

    CollectionIDTable(const std::vector<int> ids, std::vector<std::string> names) :
      m_collectionIDs(ids), m_names(names)
    {};

    int collectionID(const std::string& name) const;

    const std::string name(int collectionID) const;

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

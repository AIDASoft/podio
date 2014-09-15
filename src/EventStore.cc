
// albers specific includes
#include "albers/Reader.h"
#include "albers/CollectionBase.h"
#include "albers/EventStore.h"

namespace albers {

  EventStore::EventStore(Registry* registry) :
    m_reader(nullptr),
    m_writer(nullptr),
    m_registry(registry)
  {}

  EventStore::~EventStore(){
    for (auto& coll : m_collections){
      delete coll.second;
    }
  }

  bool EventStore::doGet(const std::string& name, CollectionBase*& collection) const {
    auto result = std::find_if(begin(m_collections), end(m_collections),
                               [name](const CollPair& item)->bool { return name==item.first; }
			      );

    if (result != end(m_collections)){
      auto tmp = result->second;
      if (tmp != nullptr){
        collection = tmp;
        return true;
      }
    } else if (m_reader != nullptr) {
      auto tmp = m_reader->readCollection(name);
      if (tmp != nullptr){
        m_collections.emplace_back(std::make_pair(name,tmp));
	collection = tmp;
        return true;
      }
    } else {
      return false;
    }
    return false;
  }


  void EventStore::next(){
    //    if (m_writer != nullptr) m_writer->write();
    for (auto& coll : m_collections){
      coll.second->clear();
    }
  }

} // namespace

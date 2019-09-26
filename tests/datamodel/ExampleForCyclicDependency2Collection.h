//AUTOMATICALLY GENERATED - DO NOT EDIT

#ifndef ExampleForCyclicDependency2Collection_H
#define  ExampleForCyclicDependency2Collection_H

#include <string>
#include <vector>
#include <deque>
#include <array>
#include <algorithm>
#include <iostream>
#include <iomanip>

// podio specific includes
#include "podio/ICollectionProvider.h"
#include "podio/CollectionBase.h"
#include "podio/CollectionIDTable.h"

// datamodel specific includes
#include "ExampleForCyclicDependency2Data.h"
#include "ExampleForCyclicDependency2.h"
#include "ExampleForCyclicDependency2Obj.h"


typedef std::vector<ExampleForCyclicDependency2Data> ExampleForCyclicDependency2DataContainer;
typedef std::deque<ExampleForCyclicDependency2Obj*> ExampleForCyclicDependency2ObjPointerContainer;

class ExampleForCyclicDependency2CollectionIterator {

  public:
    ExampleForCyclicDependency2CollectionIterator(int index, const ExampleForCyclicDependency2ObjPointerContainer* collection) : m_index(index), m_object(nullptr), m_collection(collection) {}

    bool operator!=(const ExampleForCyclicDependency2CollectionIterator& x) const {
      return m_index != x.m_index; //TODO: may not be complete
    }

    const ExampleForCyclicDependency2 operator*() const;
    const ExampleForCyclicDependency2* operator->() const;
    const ExampleForCyclicDependency2CollectionIterator& operator++() const;

  private:
    mutable int m_index;
    mutable ExampleForCyclicDependency2 m_object;
    const ExampleForCyclicDependency2ObjPointerContainer* m_collection;
};

/**
A Collection is identified by an ID.
*/

class ExampleForCyclicDependency2Collection : public podio::CollectionBase {

public:
  typedef const ExampleForCyclicDependency2CollectionIterator const_iterator;

  ExampleForCyclicDependency2Collection();
//  ExampleForCyclicDependency2Collection(const ExampleForCyclicDependency2Collection& ) = delete; // deletion doesn't work w/ ROOT IO ! :-(
//  ExampleForCyclicDependency2Collection(ExampleForCyclicDependency2Vector* data, int collectionID);
  ~ExampleForCyclicDependency2Collection();

  void clear() override final;

  /// operator to allow pointer like calling of members a la LCIO  \n     
  ExampleForCyclicDependency2Collection* operator->() { return (ExampleForCyclicDependency2Collection*) this ; }

  /// Append a new object to the collection, and return this object.
  ExampleForCyclicDependency2 create();

  /// Append a new object to the collection, and return this object.
  /// Initialized with the parameters given
  template<typename... Args>
  ExampleForCyclicDependency2 create(Args&&... args);

  /// number of elements in the collection
  int size() const override final ;

  /// fully qualified type name of elements - with namespace
  std::string getValueTypeName() const override { return std::string("ExampleForCyclicDependency2") ; } ;

  /// Returns the const object of given index
  const ExampleForCyclicDependency2 operator[](unsigned int index) const;
  /// Returns the object of a given index
  ExampleForCyclicDependency2 operator[](unsigned int index);
  /// Returns the const object of given index
  const ExampleForCyclicDependency2 at(unsigned int index) const;
  /// Returns the object of given index
  ExampleForCyclicDependency2 at(unsigned int index);


  /// Append object to the collection
  void push_back(ConstExampleForCyclicDependency2 object);

  void prepareForWrite() override final;
  void prepareAfterRead() override final;
  void setBuffer(void* address) override final;
  bool setReferences(const podio::ICollectionProvider* collectionProvider) override final;

  podio::CollRefCollection* referenceCollections() override final { return &m_refCollections;};

  podio::VectorMembersInfo* vectorMembers() override {return &m_vecmem_info ; }

  void setID(unsigned ID) override final {
    m_collectionID = ID;
    std::for_each(m_entries.begin(),m_entries.end(),
                 [ID](ExampleForCyclicDependency2Obj* obj){obj->id = {obj->id.index,static_cast<int>(ID)}; }
    );
  };

  bool isValid() const override final {
    return m_isValid;
  }

  // support for the iterator protocol
  const const_iterator begin() const {
    return const_iterator(0, &m_entries);
  }
  const const_iterator end() const {
    return const_iterator(m_entries.size(), &m_entries);
  }

  /// returns the address of the pointer to the data buffer
  void* getBufferAddress() override final { return (void*)&m_data;};

  /// returns the pointer to the data buffer
  std::vector<ExampleForCyclicDependency2Data>* _getBuffer() { return m_data;};

   

private:
  bool m_isValid;
  int m_collectionID;
  ExampleForCyclicDependency2ObjPointerContainer m_entries;
  // members to handle 1-to-N-relations
  std::vector<::ConstExampleForCyclicDependency1>* m_rel_ref; ///< Relation buffer for read / write

  //members to handle vector members

  // members to handle streaming
  podio::CollRefCollection m_refCollections;
  podio::VectorMembersInfo m_vecmem_info ;
  ExampleForCyclicDependency2DataContainer* m_data;
};

std::ostream& operator<<( std::ostream& o,const ExampleForCyclicDependency2Collection& v);


template<typename... Args>
ExampleForCyclicDependency2  ExampleForCyclicDependency2Collection::create(Args&&... args){
  int size = m_entries.size();
  auto obj = new ExampleForCyclicDependency2Obj({size,m_collectionID},{args...});
  m_entries.push_back(obj);
  return ExampleForCyclicDependency2(obj);
}



#endif

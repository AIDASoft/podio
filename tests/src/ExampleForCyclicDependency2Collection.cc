// standard includes
#include "ExampleForCyclicDependency1Collection.h"
#include <stdexcept>

#include "ExampleForCyclicDependency2Collection.h"

ExampleForCyclicDependency2Collection::ExampleForCyclicDependency2Collection()
    : m_isValid(false), m_collectionID(0), m_entries(),
      m_rel_ref(new std::vector<::ConstExampleForCyclicDependency1>()),
      m_data(new ExampleForCyclicDependency2DataContainer()) {
  m_refCollections.push_back(new std::vector<podio::ObjectID>());
}

ExampleForCyclicDependency2Collection::
    ~ExampleForCyclicDependency2Collection() {
  clear();
  if (m_data != nullptr)
    delete m_data;
  for (auto &pointer : m_refCollections) {
    if (pointer != nullptr)
      delete pointer;
  }
  if (m_rel_ref != nullptr) {
    delete m_rel_ref;
  }
}

const ExampleForCyclicDependency2 ExampleForCyclicDependency2Collection::
operator[](unsigned int index) const {
  return ExampleForCyclicDependency2(m_entries[index]);
}

const ExampleForCyclicDependency2
ExampleForCyclicDependency2Collection::at(unsigned int index) const {
  return ExampleForCyclicDependency2(m_entries.at(index));
}

ExampleForCyclicDependency2 ExampleForCyclicDependency2Collection::
operator[](unsigned int index) {
  return ExampleForCyclicDependency2(m_entries[index]);
}

ExampleForCyclicDependency2
ExampleForCyclicDependency2Collection::at(unsigned int index) {
  return ExampleForCyclicDependency2(m_entries.at(index));
}

int ExampleForCyclicDependency2Collection::size() const {
  return m_entries.size();
}

ExampleForCyclicDependency2 ExampleForCyclicDependency2Collection::create() {
  auto obj = new ExampleForCyclicDependency2Obj();
  m_entries.emplace_back(obj);

  obj->id = {int(m_entries.size() - 1), m_collectionID};
  return ExampleForCyclicDependency2(obj);
}

void ExampleForCyclicDependency2Collection::clear() {
  m_data->clear();
  for (auto &pointer : m_refCollections) {
    pointer->clear();
  }
  for (auto &item : (*m_rel_ref)) {
    item.unlink();
  }
  m_rel_ref->clear();

  for (auto &obj : m_entries) {
    delete obj;
  }
  m_entries.clear();
}

void ExampleForCyclicDependency2Collection::prepareForWrite() {
  auto size = m_entries.size();
  m_data->reserve(size);
  for (auto &obj : m_entries) {
    m_data->push_back(obj->data);
  }
  for (auto &pointer : m_refCollections) {
    pointer->clear();
  }

  for (int i = 0, size = m_data->size(); i != size; ++i) {
  }
  for (auto &obj : m_entries) {
    if (obj->m_ref != nullptr) {
      m_refCollections[0]->emplace_back(obj->m_ref->getObjectID());
    } else {
      m_refCollections[0]->push_back({-2, -2});
    }
  }
}

void ExampleForCyclicDependency2Collection::prepareAfterRead() {
  int index = 0;
  for (auto &data : *m_data) {
    auto obj =
        new ExampleForCyclicDependency2Obj({index, m_collectionID}, data);

    m_entries.emplace_back(obj);
    ++index;
  }
  m_isValid = true;
}

bool ExampleForCyclicDependency2Collection::setReferences(
    const podio::ICollectionProvider *collectionProvider) {

  for (unsigned int i = 0, size = m_entries.size(); i != size; ++i) {
    auto id = (*m_refCollections[0])[i];
    if (id.index != podio::ObjectID::invalid) {
      CollectionBase *coll = nullptr;
      collectionProvider->get(id.collectionID, coll);
      ExampleForCyclicDependency1Collection *tmp_coll =
          static_cast<ExampleForCyclicDependency1Collection *>(coll);
      m_entries[i]->m_ref =
          new ConstExampleForCyclicDependency1((*tmp_coll)[id.index]);
    } else {
      m_entries[i]->m_ref = nullptr;
    }
  }

  return true; // TODO: check success
}

void ExampleForCyclicDependency2Collection::push_back(
    ConstExampleForCyclicDependency2 object) {
  int size = m_entries.size();
  auto obj = object.m_obj;
  if (obj->id.index == podio::ObjectID::untracked) {
    obj->id = {size, m_collectionID};
    m_entries.push_back(obj);

  } else {
    throw std::invalid_argument("Object already in a collection. Cannot add it "
                                "to a second collection ");
  }
}

void ExampleForCyclicDependency2Collection::setBuffer(void *address) {
  if (m_data != nullptr)
    delete m_data;
  m_data = static_cast<ExampleForCyclicDependency2DataContainer *>(address);
}

const ExampleForCyclicDependency2
    ExampleForCyclicDependency2CollectionIterator::operator*() const {
  m_object.m_obj = (*m_collection)[m_index];
  return m_object;
}

const ExampleForCyclicDependency2 *
    ExampleForCyclicDependency2CollectionIterator::operator->() const {
  m_object.m_obj = (*m_collection)[m_index];
  return &m_object;
}

const ExampleForCyclicDependency2CollectionIterator &
ExampleForCyclicDependency2CollectionIterator::operator++() const {
  ++m_index;
  return *this;
}

std::ostream &operator<<(std::ostream &o,
                         const ExampleForCyclicDependency2Collection &v) {
  std::ios::fmtflags old_flags = o.flags();
  o << "id:          " << std::endl;
  for (int i = 0; i < v.size(); i++) {
    o << std::scientific << std::showpos << std::setw(12) << v[i].id() << " "
      << std::endl;
    o << "     ref : ";
    o << v[i].ref().id() << std::endl;
  }
  o.flags(old_flags);
  return o;
}

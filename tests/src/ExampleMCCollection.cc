// standard includes
#include "ExampleMCCollection.h"
#include <stdexcept>

#include "ExampleMCCollection.h"

ExampleMCCollection::ExampleMCCollection()
    : m_isValid(false), m_collectionID(0), m_entries(),
      m_rel_parents(new std::vector<::ConstExampleMC>()),
      m_rel_daughters(new std::vector<::ConstExampleMC>()),
      m_data(new ExampleMCDataContainer()) {
  m_refCollections.push_back(new std::vector<podio::ObjectID>());
  m_refCollections.push_back(new std::vector<podio::ObjectID>());
}

ExampleMCCollection::~ExampleMCCollection() {
  clear();
  if (m_data != nullptr)
    delete m_data;
  for (auto &pointer : m_refCollections) {
    if (pointer != nullptr)
      delete pointer;
  }
  if (m_rel_parents != nullptr) {
    delete m_rel_parents;
  }
  if (m_rel_daughters != nullptr) {
    delete m_rel_daughters;
  }
}

const ExampleMC ExampleMCCollection::operator[](unsigned int index) const {
  return ExampleMC(m_entries[index]);
}

const ExampleMC ExampleMCCollection::at(unsigned int index) const {
  return ExampleMC(m_entries.at(index));
}

ExampleMC ExampleMCCollection::operator[](unsigned int index) {
  return ExampleMC(m_entries[index]);
}

ExampleMC ExampleMCCollection::at(unsigned int index) {
  return ExampleMC(m_entries.at(index));
}

int ExampleMCCollection::size() const { return m_entries.size(); }

ExampleMC ExampleMCCollection::create() {
  auto obj = new ExampleMCObj();
  m_entries.emplace_back(obj);
  m_rel_parents_tmp.push_back(obj->m_parents);
  m_rel_daughters_tmp.push_back(obj->m_daughters);

  obj->id = {int(m_entries.size() - 1), m_collectionID};
  return ExampleMC(obj);
}

void ExampleMCCollection::clear() {
  m_data->clear();
  for (auto &pointer : m_refCollections) {
    pointer->clear();
  }
  // clear relations to parents. Make sure to unlink() the reference data s they
  // may be gone already.
  for (auto &pointer : m_rel_parents_tmp) {
    for (auto &item : (*pointer)) {
      item.unlink();
    };
    delete pointer;
  }
  m_rel_parents_tmp.clear();
  for (auto &item : (*m_rel_parents)) {
    item.unlink();
  }
  m_rel_parents->clear();
  // clear relations to daughters. Make sure to unlink() the reference data s
  // they may be gone already.
  for (auto &pointer : m_rel_daughters_tmp) {
    for (auto &item : (*pointer)) {
      item.unlink();
    };
    delete pointer;
  }
  m_rel_daughters_tmp.clear();
  for (auto &item : (*m_rel_daughters)) {
    item.unlink();
  }
  m_rel_daughters->clear();

  for (auto &obj : m_entries) {
    delete obj;
  }
  m_entries.clear();
}

void ExampleMCCollection::prepareForWrite() {
  auto size = m_entries.size();
  m_data->reserve(size);
  for (auto &obj : m_entries) {
    m_data->push_back(obj->data);
  }
  for (auto &pointer : m_refCollections) {
    pointer->clear();
  }
  int parents_index = 0;
  int daughters_index = 0;

  for (int i = 0, size = m_data->size(); i != size; ++i) {
    (*m_data)[i].parents_begin = parents_index;
    (*m_data)[i].parents_end += parents_index;
    parents_index = (*m_data)[i].parents_end;
    for (auto it : (*m_rel_parents_tmp[i])) {
      if (it.getObjectID().index == podio::ObjectID::untracked)
        throw std::runtime_error("Trying to persistify untracked object");
      m_refCollections[0]->emplace_back(it.getObjectID());
      m_rel_parents->push_back(it);
    }
    (*m_data)[i].daughters_begin = daughters_index;
    (*m_data)[i].daughters_end += daughters_index;
    daughters_index = (*m_data)[i].daughters_end;
    for (auto it : (*m_rel_daughters_tmp[i])) {
      if (it.getObjectID().index == podio::ObjectID::untracked)
        throw std::runtime_error("Trying to persistify untracked object");
      m_refCollections[1]->emplace_back(it.getObjectID());
      m_rel_daughters->push_back(it);
    }
  }
}

void ExampleMCCollection::prepareAfterRead() {
  int index = 0;
  for (auto &data : *m_data) {
    auto obj = new ExampleMCObj({index, m_collectionID}, data);
    obj->m_parents = m_rel_parents;
    obj->m_daughters = m_rel_daughters;
    m_entries.emplace_back(obj);
    ++index;
  }
  m_isValid = true;
}

bool ExampleMCCollection::setReferences(
    const podio::ICollectionProvider *collectionProvider) {
  for (unsigned int i = 0, size = m_refCollections[0]->size(); i != size; ++i) {
    auto id = (*m_refCollections[0])[i];
    if (id.index != podio::ObjectID::invalid) {
      CollectionBase *coll = nullptr;
      collectionProvider->get(id.collectionID, coll);
      ExampleMCCollection *tmp_coll = static_cast<ExampleMCCollection *>(coll);
      auto tmp = (*tmp_coll)[id.index];
      m_rel_parents->emplace_back(tmp);
    } else {
      m_rel_parents->emplace_back(nullptr);
    }
  }
  for (unsigned int i = 0, size = m_refCollections[1]->size(); i != size; ++i) {
    auto id = (*m_refCollections[1])[i];
    if (id.index != podio::ObjectID::invalid) {
      CollectionBase *coll = nullptr;
      collectionProvider->get(id.collectionID, coll);
      ExampleMCCollection *tmp_coll = static_cast<ExampleMCCollection *>(coll);
      auto tmp = (*tmp_coll)[id.index];
      m_rel_daughters->emplace_back(tmp);
    } else {
      m_rel_daughters->emplace_back(nullptr);
    }
  }

  return true; // TODO: check success
}

void ExampleMCCollection::push_back(ConstExampleMC object) {
  int size = m_entries.size();
  auto obj = object.m_obj;
  if (obj->id.index == podio::ObjectID::untracked) {
    obj->id = {size, m_collectionID};
    m_entries.push_back(obj);
    m_rel_parents_tmp.push_back(obj->m_parents);
    m_rel_daughters_tmp.push_back(obj->m_daughters);

  } else {
    throw std::invalid_argument("Object already in a collection. Cannot add it "
                                "to a second collection ");
  }
}

void ExampleMCCollection::setBuffer(void *address) {
  if (m_data != nullptr)
    delete m_data;
  m_data = static_cast<ExampleMCDataContainer *>(address);
}

const ExampleMC ExampleMCCollectionIterator::operator*() const {
  m_object.m_obj = (*m_collection)[m_index];
  return m_object;
}

const ExampleMC *ExampleMCCollectionIterator::operator->() const {
  m_object.m_obj = (*m_collection)[m_index];
  return &m_object;
}

const ExampleMCCollectionIterator &ExampleMCCollectionIterator::
operator++() const {
  ++m_index;
  return *this;
}

std::ostream &operator<<(std::ostream &o, const ExampleMCCollection &v) {
  std::ios::fmtflags old_flags = o.flags();
  o << "id:          energy:       PDG:          " << std::endl;
  for (int i = 0; i < v.size(); i++) {
    o << std::scientific << std::showpos << std::setw(12) << v[i].id() << " "
      << std::setw(12) << v[i].energy() << " " << std::setw(12) << v[i].PDG()
      << " " << std::endl;
    o << "     parents : ";
    for (unsigned j = 0, N = v[i].parents_size(); j < N; ++j)
      o << v[i].parents(j).id() << " ";
    o << std::endl;
    o << "     daughters : ";
    for (unsigned j = 0, N = v[i].daughters_size(); j < N; ++j)
      o << v[i].daughters(j).id() << " ";
    o << std::endl;
  }
  o.flags(old_flags);
  return o;
}

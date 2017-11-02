// standard includes
#include <stdexcept>

#include "ExampleWithVectorMemberCollection.h"

ExampleWithVectorMemberCollection::ExampleWithVectorMemberCollection()
    : m_isValid(false), m_collectionID(0), m_entries(),
      m_data(new ExampleWithVectorMemberDataContainer()) {}

ExampleWithVectorMemberCollection::~ExampleWithVectorMemberCollection() {
  clear();
  if (m_data != nullptr)
    delete m_data;
}

const ExampleWithVectorMember ExampleWithVectorMemberCollection::
operator[](unsigned int index) const {
  return ExampleWithVectorMember(m_entries[index]);
}

const ExampleWithVectorMember
ExampleWithVectorMemberCollection::at(unsigned int index) const {
  return ExampleWithVectorMember(m_entries.at(index));
}

ExampleWithVectorMember ExampleWithVectorMemberCollection::
operator[](unsigned int index) {
  return ExampleWithVectorMember(m_entries[index]);
}

ExampleWithVectorMember
ExampleWithVectorMemberCollection::at(unsigned int index) {
  return ExampleWithVectorMember(m_entries.at(index));
}

int ExampleWithVectorMemberCollection::size() const { return m_entries.size(); }

ExampleWithVectorMember ExampleWithVectorMemberCollection::create() {
  auto obj = new ExampleWithVectorMemberObj();
  m_entries.emplace_back(obj);

  obj->id = {int(m_entries.size() - 1), m_collectionID};
  return ExampleWithVectorMember(obj);
}

void ExampleWithVectorMemberCollection::clear() {
  m_data->clear();

  for (auto &obj : m_entries) {
    delete obj;
  }
  m_entries.clear();
}

void ExampleWithVectorMemberCollection::prepareForWrite() {
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
}

void ExampleWithVectorMemberCollection::prepareAfterRead() {
  int index = 0;
  for (auto &data : *m_data) {
    auto obj = new ExampleWithVectorMemberObj({index, m_collectionID}, data);

    m_entries.emplace_back(obj);
    ++index;
  }
  m_isValid = true;
}

bool ExampleWithVectorMemberCollection::setReferences(
    const podio::ICollectionProvider *collectionProvider) {

  return true; // TODO: check success
}

void ExampleWithVectorMemberCollection::push_back(
    ConstExampleWithVectorMember object) {
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

void ExampleWithVectorMemberCollection::setBuffer(void *address) {
  if (m_data != nullptr)
    delete m_data;
  m_data = static_cast<ExampleWithVectorMemberDataContainer *>(address);
}

const ExampleWithVectorMember ExampleWithVectorMemberCollectionIterator::
operator*() const {
  m_object.m_obj = (*m_collection)[m_index];
  return m_object;
}

const ExampleWithVectorMember *ExampleWithVectorMemberCollectionIterator::
operator->() const {
  m_object.m_obj = (*m_collection)[m_index];
  return &m_object;
}

const ExampleWithVectorMemberCollectionIterator &
    ExampleWithVectorMemberCollectionIterator::operator++() const {
  ++m_index;
  return *this;
}

std::ostream &operator<<(std::ostream &o,
                         const ExampleWithVectorMemberCollection &v) {
  std::ios::fmtflags old_flags = o.flags();
  o << "id:          " << std::endl;
  for (int i = 0; i < v.size(); i++) {
    o << std::scientific << std::showpos << std::setw(12) << v[i].id() << " "
      << std::endl;
    o.flags(old_flags);
  }
  return o;
}

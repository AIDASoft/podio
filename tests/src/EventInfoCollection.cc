// standard includes
#include <stdexcept>

#include "EventInfoCollection.h"

EventInfoCollection::EventInfoCollection()
    : m_isValid(false), m_collectionID(0), m_entries(),
      m_data(new EventInfoDataContainer()) {}

EventInfoCollection::~EventInfoCollection() {
  clear();
  if (m_data != nullptr)
    delete m_data;
}

const EventInfo EventInfoCollection::operator[](unsigned int index) const {
  return EventInfo(m_entries[index]);
}

const EventInfo EventInfoCollection::at(unsigned int index) const {
  return EventInfo(m_entries.at(index));
}

EventInfo EventInfoCollection::operator[](unsigned int index) {
  return EventInfo(m_entries[index]);
}

EventInfo EventInfoCollection::at(unsigned int index) {
  return EventInfo(m_entries.at(index));
}

int EventInfoCollection::size() const { return m_entries.size(); }

EventInfo EventInfoCollection::create() {
  auto obj = new EventInfoObj();
  m_entries.emplace_back(obj);

  obj->id = {int(m_entries.size() - 1), m_collectionID};
  return EventInfo(obj);
}

void EventInfoCollection::clear() {
  m_data->clear();

  for (auto &obj : m_entries) {
    delete obj;
  }
  m_entries.clear();
}

void EventInfoCollection::prepareForWrite() {
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

void EventInfoCollection::prepareAfterRead() {
  int index = 0;
  for (auto &data : *m_data) {
    auto obj = new EventInfoObj({index, m_collectionID}, data);

    m_entries.emplace_back(obj);
    ++index;
  }
  m_isValid = true;
}

bool EventInfoCollection::setReferences(
    const podio::ICollectionProvider *collectionProvider) {

  return true; // TODO: check success
}

void EventInfoCollection::push_back(ConstEventInfo object) {
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

void EventInfoCollection::setBuffer(void *address) {
  if (m_data != nullptr)
    delete m_data;
  m_data = static_cast<EventInfoDataContainer *>(address);
}

const EventInfo EventInfoCollectionIterator::operator*() const {
  m_object.m_obj = (*m_collection)[m_index];
  return m_object;
}

const EventInfo *EventInfoCollectionIterator::operator->() const {
  m_object.m_obj = (*m_collection)[m_index];
  return &m_object;
}

const EventInfoCollectionIterator &EventInfoCollectionIterator::
operator++() const {
  ++m_index;
  return *this;
}

std::ostream &operator<<(std::ostream &o, const EventInfoCollection &v) {
  std::ios::fmtflags old_flags = o.flags();
  o << "id:          Number:       " << std::endl;
  for (int i = 0; i < v.size(); i++) {
    o << std::scientific << std::showpos << std::setw(12) << v[i].id() << " "
      << std::setw(12) << v[i].Number() << " " << std::endl;
  }
  o.flags(old_flags);
  return o;
}

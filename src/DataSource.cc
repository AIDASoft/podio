#include "podio/DataSource.h"
#include "podio/Reader.h"
#include "podio/utilities/Glob.h"

// podio
#include <podio/FrameCategories.h>

// ROOT
#include <ROOT/RDF/RColumnReaderBase.hxx>

// STL
#include <cstddef>
#include <memory>

namespace podio {

// Column reader that wraps a pointer to the per-slot CollectionBase* pointer
class PodioColumnReader : public ROOT::Detail::RDF::RColumnReaderBase {
  const podio::CollectionBase** fPtr;

public:
  explicit PodioColumnReader(const podio::CollectionBase** ptr) : fPtr(ptr) {
  }
  PodioColumnReader(const PodioColumnReader&) = delete;
  PodioColumnReader& operator=(const PodioColumnReader&) = delete;
  void* GetImpl(Long64_t) override {
    // Return the actual collection pointer (T*), not the address of the storage (T**)
    // RColumnReaderBase::Get<T> does *static_cast<T*>(GetImpl()), so we return the T* itself
    return const_cast<void*>(static_cast<const void*>(*fPtr));
  }
};

DataSource::DataSource(const std::string& filePath, int nEvents, const std::vector<std::string>& collNames) :
    DataSource(utils::expand_glob(filePath), nEvents, collNames) {
}

DataSource::DataSource(const std::vector<std::string>& filePathList, int nEvents,
                       const std::vector<std::string>& collNames) :
    m_filePathList{filePathList} {
  SetupInput(nEvents, collNames);
}

void DataSource::SetupInput(int nEvents, const std::vector<std::string>& collsToRead) {
  if (m_filePathList.empty()) {
    throw std::runtime_error("podio::DataSource: No input files provided!");
  }

  // Check if the provided file(s) exists and contain required metadata is done
  // by podio::Reader

  // Create probing frame
  podio::Frame frame;
  unsigned int nEventsInFiles = 0;
  auto podioReader = podio::makeReader(m_filePathList);
  nEventsInFiles = podioReader.getEntries(podio::Category::Event);
  frame = podioReader.readFrame(podio::Category::Event, 0, collsToRead);

  // Determine over how many events to run
  if (nEventsInFiles == 0) {
    throw std::runtime_error("podio::DataSource: No events found!");
  }

  if (nEvents < 0) {
    m_nEvents = nEventsInFiles;
  } else if (nEvents == 0) {
    throw std::runtime_error("podio::DataSource: Requested to run over zero events!");
  } else {
    m_nEvents = nEvents;
  }
  if (nEventsInFiles < m_nEvents) {
    m_nEvents = nEventsInFiles;
  }

  // Get collections stored in the files and build fast lookup map
  std::vector<std::string> collNames = frame.getAvailableCollections();
  for (auto&& collName : collNames) {
    const podio::CollectionBase* coll = frame.get(collName);
    if (coll) {
      m_columnIndex[collName] = m_columnNames.size();
      m_columnNames.emplace_back(std::move(collName));
      m_columnTypes.emplace_back(coll->getTypeName());
    }
  }
}

void DataSource::SetNSlots(unsigned int nSlots) {
  RDataSource::SetNSlots(nSlots);

  // Build one range per slot; if there are fewer events than slots, cap at m_nEvents ranges
  const unsigned int effectiveSlots = std::min(fNSlots, static_cast<unsigned int>(m_nEvents));
  const ULong64_t eventsPerSlot = m_nEvents / effectiveSlots;
  for (size_t i = 0; i < (effectiveSlots - 1); ++i) {
    m_rangesAll.emplace_back(eventsPerSlot * i, eventsPerSlot * (i + 1));
  }
  m_rangesAll.emplace_back(eventsPerSlot * (effectiveSlots - 1), m_nEvents);
  m_rangesCursor = 0;

  // Collections indexed [column][slot]
  m_Collections.resize(m_columnNames.size(), std::vector<const podio::CollectionBase*>(fNSlots, nullptr));

  // Initialize podio readers
  for (size_t i = 0; i < fNSlots; ++i) {
    m_podioReaders.emplace_back(std::make_unique<podio::Reader>(podio::makeReader(m_filePathList)));
  }

  for (size_t i = 0; i < fNSlots; ++i) {
    m_frames.emplace_back(std::make_unique<podio::Frame>());
  }
}

void DataSource::Initialize() {
  m_rangesCursor = 0;
}

std::vector<std::pair<ULong64_t, ULong64_t>> DataSource::GetEntryRanges() {
  if (m_rangesCursor >= m_rangesAll.size()) {
    return {};
  }
  const size_t end = std::min(m_rangesCursor + fNSlots, m_rangesAll.size());
  std::vector<std::pair<ULong64_t, ULong64_t>> result(m_rangesAll.cbegin() + m_rangesCursor,
                                                      m_rangesAll.cbegin() + end);
  m_rangesCursor = end;
  return result;
}

void DataSource::InitSlot(unsigned int, ULong64_t) {
}

bool DataSource::SetEntry(unsigned int slot, ULong64_t entry) {
  m_frames[slot] = std::make_unique<podio::Frame>(
      m_podioReaders[slot]->readFrameLazy(podio::Category::Event, entry, m_activeCollectionNames));

  for (auto collectionIndex : m_activeCollections) {
    m_Collections[collectionIndex][slot] = m_frames[slot]->get(m_columnNames[collectionIndex]);
  }

  return true;
}

void DataSource::FinalizeSlot(unsigned int) {
}

void DataSource::Finalize() {
}

const std::vector<std::string>& DataSource::GetColumnNames() const {
  return m_columnNames;
}

bool DataSource::HasColumn(std::string_view columnName) const {
  return m_columnIndex.count(std::string(columnName)) > 0;
}

std::string DataSource::GetTypeName(std::string_view columnName) const {
  auto itr = m_columnIndex.find(std::string(columnName));
  if (itr == m_columnIndex.end()) {
    std::string errMsg = "podio::DataSource: Type name for \"";
    errMsg += columnName;
    errMsg += "\" not found!";
    throw std::runtime_error(errMsg);
  }

  return m_columnTypes.at(itr->second);
}

std::unique_ptr<ROOT::Detail::RDF::RColumnReaderBase>
DataSource::GetColumnReaders(unsigned int slot, std::string_view columnName, const std::type_info&) {
  auto itr = m_columnIndex.find(std::string(columnName));
  if (itr == m_columnIndex.end()) {
    std::string errMsg = "podio::DataSource: Can't find requested column \"";
    errMsg += columnName;
    errMsg += "\"!";
    throw std::runtime_error(errMsg);
  }
  const auto columnIndex = itr->second;

  if (std::find(m_activeCollections.begin(), m_activeCollections.end(), columnIndex) == m_activeCollections.end()) {
    m_activeCollections.emplace_back(columnIndex);
    m_activeCollectionNames.emplace_back(m_columnNames[columnIndex]);
  }

  return std::make_unique<PodioColumnReader>(&m_Collections[columnIndex][slot]);
}

ROOT::RDataFrame CreateDataFrame(const std::vector<std::string>& filePathList,
                                 const std::vector<std::string>& collsToRead) {
  ROOT::RDataFrame rdf(std::make_unique<DataSource>(filePathList, -1, collsToRead));

  return rdf;
}

ROOT::RDataFrame CreateDataFrame(const std::string& filePath, const std::vector<std::string>& collsToRead) {
  ROOT::RDataFrame rdf(std::make_unique<DataSource>(filePath, -1, collsToRead));

  return rdf;
}
} // namespace podio

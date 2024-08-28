#include "podio/ROOTDataSource.h"

// STL
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <filesystem>
#include <exception>
#include <memory>

// ROOT
#include <TFile.h>

// podio
#include <podio/Frame.h>
#include <podio/ROOTReader.h>

namespace podio {
  ROOTDataSource::ROOTDataSource(const std::string& filePath,
                         int nEvents) : m_nSlots{1} {
    m_filePathList.emplace_back(filePath);
    SetupInput(nEvents);
  }


  ROOTDataSource::ROOTDataSource(
      const std::vector<std::string>& filePathList,
      int nEvents) : m_nSlots{1},
                     m_filePathList{filePathList} {
    SetupInput(nEvents);
  }


  /// @TODO Check for the existence of the file, which might be coming from web
  ///       or EOS.
  void ROOTDataSource::SetupInput(int nEvents) {
    // std::cout << "podio::ROOTDataSource: Constructing the source ..." << std::endl;

    if (m_filePathList.empty()) {
      throw std::runtime_error("podio::ROOTDataSource: No input files provided!");
    }

    for (const auto& filePath : m_filePathList) {
      // Check if file exists
      // if (!std::filesystem::exists(filePath)) {
      //   throw std::runtime_error("podio::ROOTDataSource: Provided file \""
      //                            + filePath + "\" does not exist!");
      // }

      // Check if the provided file contains required metadata
      std::unique_ptr<TFile> inFile(TFile::Open(filePath.data(), "READ"));
      auto metadata = inFile->Get("podio_metadata");
      if (!metadata) {
        throw std::runtime_error(
            "podio::ROOTDataSource: Provided file is missing podio metadata!");
      }
    }

    // Create probing frame
    podio::Frame frame;
    unsigned int nEventsInFiles = 0;
    podio::ROOTReader podioReader;
    podioReader.openFiles(m_filePathList);
    nEventsInFiles = podioReader.getEntries("events");
    frame = podio::Frame(podioReader.readEntry("events", 0));

    // Determine over how many events to run
    if (nEventsInFiles > 0) {
      /*
      std::cout << "podio::ROOTDataSource: Found " << nEventsInFiles
                << " events in files: \n";
      for (const auto& filePath : m_filePathList) {
        std::cout << "               - " << filePath << "\n";
      }
      */
    } else {
      throw std::runtime_error("podio::ROOTDataSource: No events found!");
    }

    if (nEvents < 0) {
      m_nEvents = nEventsInFiles;
    } else if (nEvents == 0) {
      throw std::runtime_error(
          "podio::ROOTDataSource: Requested to run over zero events!");
    } else {
      m_nEvents = nEvents;
    }
    if (nEventsInFiles < m_nEvents) {
      m_nEvents = nEventsInFiles;
    }

    // std::cout << "podio::ROOTDataSource: Running over " << m_nEvents << " events."
    //           << std::endl;

    // Get collections stored in the files
    std::vector<std::string> collNames = frame.getAvailableCollections();
    // std::cout << "podio::ROOTDataSource: Found following collections:\n";
    for (auto& collName: collNames) {
      const podio::CollectionBase* coll = frame.get(collName);
      if (coll->isValid()) {
        m_columnNames.emplace_back(collName);
        m_columnTypes.emplace_back(coll->getValueTypeName());
        // std::cout << "                - " << collName << "\n";
      }
    }
  }


  void
  ROOTDataSource::SetNSlots(unsigned int nSlots) {
    // std::cout << "podio::ROOTDataSource: Setting num. of slots to: " << nSlots
    //           << std::endl;
    m_nSlots = nSlots;

    if (m_nSlots > m_nEvents) {
      throw std::runtime_error("podio::ROOTDataSource: Number of events too small!");
    }

    int eventsPerSlot = m_nEvents / m_nSlots;
    for (size_t i = 0; i < (m_nSlots - 1); ++i) {
      m_rangesAll.emplace_back(eventsPerSlot * i, eventsPerSlot * (i + 1));
    }
    m_rangesAll.emplace_back(eventsPerSlot * (m_nSlots - 1), m_nEvents);
    m_rangesAvailable = m_rangesAll;

    // Initialize set of addresses needed
    m_Collections.resize(
        m_columnNames.size(),
        std::vector<const podio::CollectionBase*>(m_nSlots, nullptr));

    // Initialize podio readers
    for (size_t i = 0; i < m_nSlots; ++i) {
      m_podioReaders.emplace_back(std::make_unique<podio::ROOTReader>());
    }

    for (size_t i = 0; i < m_nSlots; ++i) {
      m_podioReaders[i]->openFiles(m_filePathList);
    }

    for (size_t i = 0; i < m_nSlots; ++i) {
      m_frames.emplace_back(
          std::make_unique<podio::Frame>(
              podio::Frame(m_podioReaders[i]->readEntry("events", 0))));
    }
  }


  void
  ROOTDataSource::Initialize() {
    // std::cout << "podio::ROOTDataSource: Initializing the source ..." << std::endl;
  }


  std::vector<std::pair<ULong64_t, ULong64_t>>
  ROOTDataSource::GetEntryRanges() {
    // std::cout << "podio::ROOTDataSource: Getting entry ranges ..." << std::endl;

    std::vector<std::pair<ULong64_t, ULong64_t>> rangesToBeProcessed;
    for (auto& range: m_rangesAvailable) {
      rangesToBeProcessed.emplace_back(
          std::pair<ULong64_t, ULong64_t>{range.first, range.second}
      );
      if (rangesToBeProcessed.size() >= m_nSlots) {
        break;
      }
    }

    if (m_rangesAvailable.size() > m_nSlots) {
      m_rangesAvailable.erase(m_rangesAvailable.begin(),
                              m_rangesAvailable.begin() + m_nSlots);
    } else {
      m_rangesAvailable.erase(m_rangesAvailable.begin(),
                              m_rangesAvailable.end());
    }


    /*
    std::cout << "podio::ROOTDataSource: Ranges to be processed:\n";
    for (auto& range: rangesToBeProcessed) {
      std::cout << "               {" << range.first << ", " << range.second
                << "}\n";
    }

    if (m_rangesAvailable.size() > 0) {

      std::cout << "podio::ROOTDataSource: Ranges remaining:\n";
      for (auto& range: m_rangesAvailable) {
        std::cout << "               {" << range.first << ", " << range.second
                  << "}\n";
      }
    } else {
      std::cout << "podio::ROOTDataSource: No more remaining ranges.\n";
    }
    */

    return rangesToBeProcessed;
  }


  void
  ROOTDataSource::InitSlot([[maybe_unused]] unsigned int slot,
                       [[maybe_unused]] ULong64_t firstEntry) {
    // std::cout << "podio::ROOTDataSource: Initializing slot: " << slot
    //           << " with first entry " << firstEntry << std::endl;
  }


  bool
  ROOTDataSource::SetEntry(unsigned int slot, ULong64_t entry) {
    // std::cout << "podio::ROOTDataSource: In slot: " << slot << ", setting entry: "
    //           << entry << std::endl;

    m_frames[slot] = std::make_unique<podio::Frame>(
        podio::Frame(m_podioReaders[slot]->readEntry("events", entry)));

    for (auto& collectionIndex: m_activeCollections) {
      m_Collections[collectionIndex][slot] =
          m_frames[slot]->get(m_columnNames.at(collectionIndex));
      /*
      std::cout << "CollName: " << m_columnNames.at(collectionIndex) << "\n";
      std::cout << "Address: " << m_Collections[collectionIndex][slot] << "\n";
      std::cout << "Coll size: " << m_Collections[collectionIndex][slot]->size() << "\n";
      if (m_Collections[collectionIndex][slot]->isValid()) {
        std::cout << "Collection valid\n";
      }
      */
    }

    return true;
  }


  void
  ROOTDataSource::FinalizeSlot([[maybe_unused]] unsigned int slot) {
    /*
    std::cout << "podio::ROOTDataSource: Finalizing slot: " << slot << std::endl;
    std::cout << "Reader: " << &m_podioReaderRefs[slot].get() << std::endl;

    for (auto& collectionIndex: m_activeCollections) {
      std::cout << "CollName: " << m_columnNames.at(collectionIndex) << "\n";
      std::cout << "Address: " << m_Collections[collectionIndex][slot] << "\n";
      if (m_Collections[collectionIndex][slot]->isValid()) {
        std::cout << "Collection valid\n";
      }
      std::cout << "Coll size: " << m_Collections[collectionIndex][slot]->size() << "\n";
    }
    */
  }


  void
  ROOTDataSource::Finalize() {
    // std::cout << "podio::ROOTDataSource: Finalizing ..." << std::endl;
  }


  Record_t
  ROOTDataSource::GetColumnReadersImpl(
      std::string_view columnName,
      [[maybe_unused]] const std::type_info& typeInfo) {
    /*
    std::cout << "podio::ROOTDataSource: Getting column reader implementation for column:\n"
              << "                   " << columnName
              << "\n               with type: " << typeInfo.name() << std::endl;
    */

    auto itr = std::find(m_columnNames.begin(), m_columnNames.end(),
                         columnName);
    if (itr == m_columnNames.end()) {
      std::string errMsg = "podio::ROOTDataSource: Can't find requested column \"";
      errMsg += columnName;
      errMsg += "\"!";
      throw std::runtime_error(errMsg);
    }
    auto columnIndex = std::distance(m_columnNames.begin(), itr);
    m_activeCollections.emplace_back(columnIndex);
    /*
    std::cout << "podio::ROOTDataSource: Active collections so far:\n"
              << "               ";
    for (auto& i: m_activeCollections) {
      std::cout << i << ", ";
    }
    std::cout << std::endl;
    */

    Record_t columnReaders(m_nSlots);
    for (size_t slotIndex = 0; slotIndex < m_nSlots; ++slotIndex) {
      /*
      std::cout << "               Column index: " << columnIndex << "\n";
      std::cout << "               Slot index: " << slotIndex << "\n";
      std::cout << "               Address: "
                << &m_Collections[columnIndex][slotIndex]
                << std::endl;
      */
      columnReaders[slotIndex] = (void*) &m_Collections[columnIndex][slotIndex];
    }

    return columnReaders;
  }


  const std::vector<std::string>&
  ROOTDataSource::GetColumnNames() const {
    // std::cout << "podio::ROOTDataSource: Looking for column names" << std::endl;

    return m_columnNames;
  }


  bool
  ROOTDataSource::HasColumn(std::string_view columnName) const {
    // std::cout << "podio::ROOTDataSource: Looking for column: " << columnName
    //           << std::endl;

    if (std::find(m_columnNames.begin(),
                  m_columnNames.end(),
                  columnName) != m_columnNames.end()) {
      return true;
    }

    return false;
  }


  std::string
  ROOTDataSource::GetTypeName(std::string_view columnName) const {
    // std::cout << "podio::ROOTDataSource: Looking for type name of column: "
    //           << columnName << std::endl;

    auto itr = std::find(m_columnNames.begin(), m_columnNames.end(),
                         columnName);
    if (itr != m_columnNames.end()) {
      auto i = std::distance(m_columnNames.begin(), itr);
      // std::cout << "podio::ROOTDataSource: Found type name: "
      //           << m_columnTypes.at(i) << std::endl;

      return m_columnTypes.at(i) + "Collection";
    }

    return "float";
  }


  ROOT::RDataFrame
  CreateDataFrame(const std::vector<std::string>& filePathList) {
    ROOT::RDataFrame rdf(std::make_unique<ROOTDataSource>(filePathList));

    return rdf;
  }


  ROOT::RDataFrame
  CreateDataFrame(const std::string& filePath) {
    ROOT::RDataFrame rdf(std::make_unique<ROOTDataSource>(filePath));

    return rdf;
  }
}

#ifndef PODIO_DATASOURCE_H__
#define PODIO_DATASOURCE_H__

// STL
#include <string>
#include <vector>

// ROOT
#include <ROOT/RDataFrame.hxx>
#include <ROOT/RDataSource.hxx>

// Podio
#include <podio/CollectionBase.h>
#include <podio/Frame.h>
#include <podio/ROOTReader.h>

namespace podio {
using Record_t = std::vector<void*>;

class ROOTDataSource : public ROOT::RDF::RDataSource {
public:
  ///
  /// @brief Construct the podio::ROOTDataSource from the provided file.
  ///
  explicit ROOTDataSource(const std::string& filePath, int nEvents = -1);

  ///
  /// @brief Construct the podio::ROOTDataSource from the provided file
  ///        list.
  ///
  explicit ROOTDataSource(const std::vector<std::string>& filePathList, int nEvents = -1);

  ///
  /// @brief Inform the podio::ROOTDataSource of the desired level of
  ///        parallelism.
  ///
  void SetNSlots(unsigned int nSlots) override;

  ///
  /// @brief Retrieve from podio::ROOTDataSource per-thread readers for the
  ///        desired columns.
  ///
  template <typename T>
  std::vector<T**> GetColumnReaders(std::string_view columnName);

  ///
  /// @brief Inform podio::ROOTDataSource that an event-loop is about to
  ///        start.
  ///
  void Initialize() override;

  ///
  /// @brief Retrieve from podio::ROOTDataSource a set of ranges of entries
  ///        that can be processed concurrently.
  ///
  std::vector<std::pair<ULong64_t, ULong64_t>> GetEntryRanges() override;

  ///
  /// @brief Inform podio::ROOTDataSource that a certain thread is about to
  ///        start working on a certain range of entries.
  ///
  void InitSlot(unsigned int slot, ULong64_t firstEntry) override;

  ///
  /// @brief Inform podio::ROOTDataSource that a certain thread is about to
  ///        start working on a certain entry.
  ///
  bool SetEntry(unsigned int slot, ULong64_t entry) override;

  ///
  /// @brief Inform podio::ROOTDataSource that a certain thread finished
  ///        working on a certain range of entries.
  ///
  void FinalizeSlot(unsigned int slot) override;

  ///
  /// @brief Inform podio::ROOTDataSource that an event-loop finished.
  ///
  void Finalize() override;

  ///
  /// @brief Returns a reference to the collection of the dataset's column
  ///        names
  ///
  const std::vector<std::string>& GetColumnNames() const override;

  ///
  /// @brief Checks if the dataset has a certain column.
  ///
  bool HasColumn(std::string_view columnName) const override;

  ///
  /// @brief Type of a column as a string. Required for JITting.
  ///
  std::string GetTypeName(std::string_view columnName) const override;

protected:
  ///
  /// @brief Type-erased vector of pointers to pointers to column
  ///        values --- one per slot.
  ///
  Record_t GetColumnReadersImpl(std::string_view name, const std::type_info& typeInfo) override;

  std::string AsString() override {
    return "Podio data source";
  };

private:
  /// Number of slots/threads
  unsigned int m_nSlots = 1;

  /// Input filename
  std::vector<std::string> m_filePathList = {};

  /// Total number of events
  unsigned int m_nEvents = 0;

  /// Ranges of events available to be processed
  std::vector<std::pair<ULong64_t, ULong64_t>> m_rangesAvailable = {};

  /// Ranges of events available ever created
  std::vector<std::pair<ULong64_t, ULong64_t>> m_rangesAll = {};

  /// Column names
  std::vector<std::string> m_columnNames{};

  /// Column types
  std::vector<std::string> m_columnTypes = {};

  /// Collections, m_Collections[columnIndex][slotIndex]
  std::vector<std::vector<const podio::CollectionBase*>> m_Collections = {};

  /// Active collections
  std::vector<unsigned int> m_activeCollections = {};

  /// Root podio readers
  std::vector<std::unique_ptr<podio::ROOTReader>> m_podioReaders = {};

  /// Podio frames
  std::vector<std::unique_ptr<podio::Frame>> m_frames = {};

  ///
  /// @brief Setup input for the podio::ROOTDataSource.
  ///
  /// @param[in]  Number of events.
  /// @return     void.
  ///
  void SetupInput(int nEvents);
};

template <typename T>
std::vector<T**> ROOTDataSource::GetColumnReaders(std::string_view columnName) {
  std::cout << "podio::ROOTDataSource: Getting column readers for column: " << columnName << std::endl;

  std::vector<T**> readers;

  return readers;
}

///
/// @brief Create RDataFrame from multiple Podio files.
///
/// @param[in] filePathList  List of file paths from which the RDataFrame
///                          will be created.
/// @return                  RDataFrame created from input file list.
///
ROOT::RDataFrame CreateDataFrame(const std::vector<std::string>& filePathList);

///
/// @brief Create RDataFrame from a Podio file.
///
/// @param[in] filePath  File path from which the RDataFrame will be created.
/// @return              RDataFrame created from input file list.
///
ROOT::RDataFrame CreateDataFrame(const std::string& filePath);
} // namespace podio

#endif /* PODIO_DATASOURCE_H__ */

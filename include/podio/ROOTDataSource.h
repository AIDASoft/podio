#ifndef PODIO_DATASOURCE_H__
#define PODIO_DATASOURCE_H__

// STL
#include <vector>
#include <string>

// ROOT
#include <ROOT/RDataFrame.hxx>
#include <ROOT/RDataSource.hxx>

// Podio
#include <podio/Frame.h>
#include <podio/ROOTReader.h>
#include <podio/CollectionBase.h>

namespace podio {
  using Record_t = std::vector<void*>;

  class ROOTDataSource : public ROOT::RDF::RDataSource {
    public:
      explicit ROOTDataSource(const std::string& filePath, int nEvents = -1);
      explicit ROOTDataSource(const std::vector<std::string>& filePathList,
                              int nEvents = -1);

      void SetNSlots(unsigned int nSlots) override;

      template<typename T>
      std::vector<T**> GetColumnReaders(std::string_view columnName);

      void Initialize() override;

      std::vector<std::pair<ULong64_t, ULong64_t>> GetEntryRanges() override;

      void InitSlot(unsigned int slot, ULong64_t firstEntry) override;

      bool SetEntry(unsigned int slot, ULong64_t entry) override;

      void FinalizeSlot(unsigned int slot) override;

      void Finalize() override;

      const std::vector<std::string>& GetColumnNames() const override;

      bool HasColumn(std::string_view columnName) const override;

      std::string GetTypeName(std::string_view columnName) const override;

    protected:
      Record_t GetColumnReadersImpl (
          std::string_view name,
          const std::type_info& typeInfo) override;

      std::string AsString() override { return "Podio data source"; };

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
      std::vector<std::string> m_columnNames {};
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
      /// Setup input
      void SetupInput(int nEvents);
  };


  /**
   * \brief Retrieve from podio::ROOTDataSource per-thread readers for the
   *        desired columns.
   */
  template<typename T>
  std::vector<T**>
  ROOTDataSource::GetColumnReaders(std::string_view columnName) {
    std::cout << "podio::ROOTDataSource: Getting column readers for column: "
              << columnName << std::endl;

    std::vector<T**> readers;

    return readers;
  }

  /**
   * @brief Create RDataFrame from multiple Podio files.
   *
   * @param[in] filePathList  List of file paths from which the RDataFrame
   *                          will be created.
   * @return                  RDataFrame created from input file list.
   */
  ROOT::RDataFrame CreateDataFrame(const std::vector<std::string>& filePathList);

  /**
   * @brief Create RDataFrame from a Podio file.
   *
   * @param[in] filePath  File path from which the RDataFrame will be created.
   * @return              RDataFrame created from input file list.
   */
  ROOT::RDataFrame CreateDataFrame(const std::string& filePath);
}

#endif /* PODIO_DATASOURCE_H__ */

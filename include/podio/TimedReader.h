#ifndef PODIO_TIMEDREADER_H
#define PODIO_TIMEDREADER_H

#include "podio/BenchmarkRecorder.h"
#include "podio/BenchmarkUtil.h"

#include "podio/GenericParameters.h"
#include "podio/IReader.h"

#include <map>

namespace podio {

template <class WrappedReader>
class TimedReader : public IReader {
  using ClockT = benchmark::ClockT;

public:
  template <typename... Args>
  TimedReader(benchmark::BenchmarkRecorder& recorder, Args&&... args) :
      m_start(ClockT::now()),
      m_reader(WrappedReader(std::forward<Args>(args)...)),
      m_end(ClockT::now()),
      m_recorder(recorder),
      m_perEventTree(m_recorder.addTree(
          "event_times", {"read_collections", "read_ev_md", "read_run_md", "read_coll_md", "end_of_event"})) {
    m_recorder.addTree("setup_times", {"constructor", "open_file", "close_file", "read_collection_ids", "get_entries"});
    m_recorder.recordTime("setup_times", "constructor", m_end - m_start);
  }

  ~TimedReader() {
    // Timing deconstructors is not straight forward when wrapping a value.
    // Since nothing is usually happening in them in any case, we simply don't
    // do it. We still have to fill the setup_times tree here though.
    m_recorder.Fill("setup_times");
  }

  /// Read Collection of given name
  /// Does not set references yet.
  CollectionBase* readCollection(const std::string& name) override {
    const auto [result, duration] = benchmark::run_member_timed(m_reader, &IReader::readCollection, name);
    // since we cannot in general know how many collections there will be read
    // we simply sum up all the requests in an event and record that
    m_totalCollectionReadTime += duration;
    return result;
  }

  /// Get CollectionIDTable of read-in data
  CollectionIDTable* getCollectionIDTable() override {
    return runTimed(false, "read_collection_ids", &IReader::getCollectionIDTable);
  }

  /// read event meta data from file
  GenericParameters* readEventMetaData() override {
    return runTimed(true, "read_ev_md", &IReader::readEventMetaData);
  }

  std::map<int, GenericParameters>* readCollectionMetaData() override {
    return runTimed(true, "read_coll_md", &IReader::readCollectionMetaData);
  }

  std::map<int, GenericParameters>* readRunMetaData() override {
    return runTimed(true, "read_run_md", &IReader::readRunMetaData);
  }

  /// get the number of events available from this reader
  unsigned getEntries() const override {
    return runTimed(false, "get_entries", &IReader::getEntries);
  }

  /// Prepare the reader to read the next event
  void endOfEvent() override {
    runVoidTimed(true, "end_of_event", &IReader::endOfEvent);

    m_perEventTree.recordTime("read_collections", m_totalCollectionReadTime);
    m_perEventTree.Fill();
    m_totalCollectionReadTime = std::chrono::nanoseconds{0};
  }

  // not benchmarking this one
  bool isValid() const override {
    return m_reader.isValid();
  }

  void openFile(const std::string& filename) override {
    runVoidTimed(false, "open_file", &IReader::openFile, filename);
  }

  void closeFile() override {
    runVoidTimed(false, "close_file", &IReader::closeFile);
  }

  podio::version::Version currentFileVersion() const override {
    // no need to time this as it is really just a very simple get
    return m_reader.currentFileVersion();
  }

private:
  void recordTime(bool perEvent, const std::string& step, ClockT::duration duration) const {
    if (perEvent) {
      m_perEventTree.recordTime(step, duration);
    } else {
      m_recorder.recordTime("setup_times", step, duration);
    }
  }

  template <typename FuncT, typename... Args>
  inline std::invoke_result_t<FuncT, WrappedReader, Args...> runTimed(bool perEvent, const std::string& step,
                                                                      FuncT func, Args&&... args) {
    const auto [result, duration] = benchmark::run_member_timed(m_reader, func, std::forward<Args>(args)...);

    recordTime(perEvent, step, duration);

    return result;
  }

  template <typename FuncT, typename... Args>
  inline std::invoke_result_t<FuncT, WrappedReader, Args...> runTimed(bool perEvent, const std::string& step,
                                                                      FuncT func, Args&&... args) const {
    const auto [result, duration] = benchmark::run_member_timed(m_reader, func, std::forward<Args>(args)...);

    recordTime(perEvent, step, duration);

    return result;
  }

  template <typename FuncT, typename... Args>
  inline void runVoidTimed(bool perEvent, const std::string& step, FuncT func, Args&&... args) {
    const auto duration = benchmark::run_void_member_timed(m_reader, func, std::forward<Args>(args)...);

    recordTime(perEvent, step, duration);
  }

  // NOTE: c++ initializes its class members in the order they are defined not
  // in the order in which they appear in the initializer list!
  ClockT::time_point m_start; // to time the construction
  WrappedReader m_reader;     // the decorated reader that does the actual work
  ClockT::time_point m_end;   // to time the constructor call

  benchmark::BenchmarkRecorder& m_recorder;
  // Keep a reference to this one around, to save the look-up in each event
  benchmark::BenchmarkRecorderTree& m_perEventTree;
  ClockT::duration m_totalCollectionReadTime{std::chrono::nanoseconds{0}};
};

} // namespace podio

#endif

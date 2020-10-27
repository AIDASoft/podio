#ifndef PODIO_TIMEDWRITER_H__
#define PODIO_TIMEDWRITER_H__

#include "podio/BenchmarkUtil.h"
#include "podio/BenchmarkRecorder.h"

#include <string>
#include <chrono>

namespace podio {

template<class WrappedWriter>
class TimedWriter {
  using ClockT = benchmark::ClockT;

public:
  template<typename ...Args>
  TimedWriter(benchmark::BenchmarkRecorder& recorder, Args&&... args) :
    m_start(ClockT::now()),
    m_writer(WrappedWriter(std::forward<Args>(args)...)),
    m_end(ClockT::now()),
    m_recorder(recorder),
    m_perEventTree(m_recorder.addTree("event_times", {"write_event"}))
  {
    m_recorder.addTree("setup_times", {"constructor", "finish", "register_for_write"});
    m_recorder.recordTime("setup_times", "constructor", m_end - m_start);
  }

  ~TimedWriter()
  {
    m_recorder.recordTime("setup_times", "register_for_write", m_registerTime);
    m_recorder.Fill("setup_times");
  }

  void registerForWrite(const std::string& name) {
    // summing up the times it takes for all the collections to be registered
    // here, since we do not know in advance how many collections there will be
    // in the end
    const auto duration = benchmark::run_void_member_timed(m_writer, &WrappedWriter::registerForWrite, name);
    m_registerTime += duration;
  }

  void writeEvent() {
    m_perEventTree.recordTime("write_event",
                              benchmark::run_void_member_timed(m_writer, &WrappedWriter::writeEvent));
    m_perEventTree.Fill();
  }

  void finish() {
    m_recorder.recordTime("setup_times", "finish",
                          benchmark::run_void_member_timed(m_writer, &WrappedWriter::finish));
  }

private:
  ClockT::time_point m_start;
  WrappedWriter m_writer;
  ClockT::time_point m_end;
  benchmark::BenchmarkRecorder& m_recorder;
  benchmark::BenchmarkRecorderTree& m_perEventTree;
  ClockT::duration m_registerTime{std::chrono::nanoseconds{0}};
};

}

#endif

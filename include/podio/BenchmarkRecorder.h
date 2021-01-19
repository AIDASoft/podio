#ifndef PODIO_BENCHMARK_RECORDER_H__
#define PODIO_BENCHMARK_RECORDER_H__

#include "podio/BenchmarkUtil.h"

#include "TTree.h"
#include "TFile.h"

#include <vector>
#include <chrono>
#include <string>
#include <algorithm>
#include <utility>
#include <deque>

namespace podio::benchmark {


class BenchmarkRecorderTree {
public:
  BenchmarkRecorderTree() = delete;
  // Avoid some possible issues that could arise from copying by simply
  // disallowing it
  BenchmarkRecorderTree(const BenchmarkRecorderTree&) = delete;
  BenchmarkRecorderTree& operator=(const BenchmarkRecorderTree&) = delete;

  BenchmarkRecorderTree(TFile* recFile, const std::string& name, const std::vector<std::string>& steps) :
    m_stepNames(steps), m_stepTimes(steps.size()) {
    recFile->cd();
    m_recordTree = new TTree(name.c_str(), "time recording tree");
    m_recordTree->SetDirectory(recFile);

    for (size_t i = 0; i < m_stepNames.size(); ++i) {
      m_recordTree->Branch(m_stepNames[i].c_str(), &m_stepTimes[i]);
    }
  }

  template<typename TimingResolution=std::chrono::nanoseconds>
  void recordTime(const std::string& stepName, const ClockT::duration time) {
    const auto it = std::find(m_stepNames.cbegin(), m_stepNames.cend(), stepName);
    const auto index = std::distance(m_stepNames.cbegin(), it);
    m_stepTimes[index] = std::chrono::duration_cast<TimingResolution>(time).count();
  }

  void Fill() {
    m_recordTree->Fill();
  }

  void Write() {
    m_recordTree->Write();
  }

private:
  TTree* m_recordTree{nullptr};
  std::vector<std::string> m_stepNames;
  std::vector<double> m_stepTimes;
};


class BenchmarkRecorder {
public:
  BenchmarkRecorder() = delete;
  BenchmarkRecorder(const std::string& recFileName="podio_benchmark_file.root") {
    m_recordFile = new TFile(recFileName.c_str(), "recreate");
  }

  BenchmarkRecorder(const BenchmarkRecorder&) = delete;
  BenchmarkRecorder operator=(const BenchmarkRecorder&) = delete;

  ~BenchmarkRecorder() {
    for (auto& [name, tree] : m_recordTrees) {
      tree.Write();
    }
    m_recordFile->Write("", TObject::kWriteDelete);
    m_recordFile->Close();
  }

  template<typename TimingResolution=std::chrono::nanoseconds>
  void recordTime(const std::string& treeName, const std::string& stepName, const ClockT::duration time) {
    auto it = std::find_if(m_recordTrees.begin(), m_recordTrees.end(),
                           [&treeName] (const auto& recTree) { return recTree.first == treeName; });

    it->second.template recordTime<TimingResolution>(stepName, time);
  }

  void Fill(const std::string& treeName) {
    auto it = std::find_if(m_recordTrees.begin(), m_recordTrees.end(),
                           [&treeName] (const auto& recTree) { return recTree.first == treeName; });
    it->second.Fill();
  }

  BenchmarkRecorderTree& addTree(const std::string& name, const std::vector<std::string>& steps) {
    return m_recordTrees.emplace_back(std::piecewise_construct,
                                      std::forward_as_tuple(name),
                                      std::forward_as_tuple(m_recordFile, name, steps)).second;
  }

private:
  TFile* m_recordFile{nullptr};
  // Stable references outside!!
  std::deque<std::pair<std::string, BenchmarkRecorderTree>> m_recordTrees;
};

}

#endif

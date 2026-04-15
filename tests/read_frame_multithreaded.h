#ifndef PODIO_TESTS_READ_FRAME_MULTITHREADED_H // NOLINT(llvm-header-guard): folder structure not suitable
#define PODIO_TESTS_READ_FRAME_MULTITHREADED_H // NOLINT(llvm-header-guard): folder structure not suitable

#include "datamodel/EventInfoCollection.h"
#include "datamodel/ExampleClusterCollection.h"
#include "datamodel/ExampleHitCollection.h"
#include "datamodel/ExampleMCCollection.h"

#include "podio/Frame.h"
#include "podio/UserDataCollection.h"

#include <atomic>
#include <iostream>
#include <mutex>
#include <random>
#include <string>
#include <thread>
#include <vector>

/// Verify that a Frame's content matches what createRandomFrame would have
/// produced for the given frameId. Returns 0 on success, 1 on failure.
int checkRandomFrame(const podio::Frame& frame, int frameId) {
  std::mt19937 rng(frameId);
  // Same distributions as in makeMTFrame
  std::uniform_int_distribution<int> hitCountDist(1, 10);
  std::uniform_int_distribution<int> clusterCountDist(1, 5);
  std::uniform_int_distribution<int> mcCountDist(1, 8);
  std::uniform_int_distribution<int> userDataCountDist(1, 15);
  std::uniform_real_distribution<double> realDist(-100.0, 100.0);
  std::uniform_int_distribution<int> intDist(0, 1000);

  const int expectedHits = hitCountDist(rng);
  const auto& hits = frame.get<ExampleHitCollection>("hits");
  if (hits.size() != static_cast<size_t>(expectedHits)) {
    std::cerr << "Frame " << frameId << ": expected " << expectedHits << " hits, got " << hits.size() << std::endl;
    return 1;
  }
  for (int i = 0; i < expectedHits; ++i) {
    const auto expectedCellID = static_cast<unsigned long long>(intDist(rng));
    const double expectedX = realDist(rng);
    const double expectedY = realDist(rng);
    const double expectedZ = realDist(rng);
    const double expectedEnergy = realDist(rng);
    if (hits[i].cellID() != expectedCellID || hits[i].x() != expectedX || hits[i].y() != expectedY ||
        hits[i].z() != expectedZ || hits[i].energy() != expectedEnergy) {
      std::cerr << "Frame " << frameId << ": hit " << i << " data mismatch\n"
                << "  cellID:  expected=" << expectedCellID << " actual=" << hits[i].cellID() << "\n"
                << "  x:       expected=" << expectedX << " actual=" << hits[i].x() << "\n"
                << "  y:       expected=" << expectedY << " actual=" << hits[i].y() << "\n"
                << "  z:       expected=" << expectedZ << " actual=" << hits[i].z() << "\n"
                << "  energy:  expected=" << expectedEnergy << " actual=" << hits[i].energy() << std::endl;
      return 1;
    }
  }

  const int expectedClusters = clusterCountDist(rng);
  const auto& clusters = frame.get<ExampleClusterCollection>("clusters");
  if (clusters.size() != static_cast<size_t>(expectedClusters)) {
    std::cerr << "Frame " << frameId << ": expected " << expectedClusters << " clusters, got " << clusters.size()
              << std::endl;
    return 1;
  }
  for (int i = 0; i < expectedClusters; ++i) {
    const double expectedEnergy = realDist(rng);
    if (clusters[i].energy() != expectedEnergy) {
      std::cerr << "Frame " << frameId << ": cluster " << i << " energy mismatch"
                << " (expected=" << expectedEnergy << " actual=" << clusters[i].energy() << ")" << std::endl;
      return 1;
    }
  }

  const int expectedMCs = mcCountDist(rng);
  const auto& mcparticles = frame.get<ExampleMCCollection>("mcparticles");
  if (mcparticles.size() != static_cast<size_t>(expectedMCs)) {
    std::cerr << "Frame " << frameId << ": expected " << expectedMCs << " mcparticles, got " << mcparticles.size()
              << std::endl;
    return 1;
  }
  for (int i = 0; i < expectedMCs; ++i) {
    const double expectedEnergy = realDist(rng);
    const int expectedPDG = intDist(rng);
    if (mcparticles[i].energy() != expectedEnergy || mcparticles[i].PDG() != expectedPDG) {
      std::cerr << "Frame " << frameId << ": mcparticle " << i << " data mismatch\n"
                << "  energy:  expected=" << expectedEnergy << " actual=" << mcparticles[i].energy() << "\n"
                << "  PDG:     expected=" << expectedPDG << " actual=" << mcparticles[i].PDG() << std::endl;
      return 1;
    }
  }

  const int expectedUserDoubles = userDataCountDist(rng);
  const auto& userDoubles = frame.get<podio::UserDataCollection<double>>("userDoubles");
  if (userDoubles.size() != static_cast<size_t>(expectedUserDoubles)) {
    std::cerr << "Frame " << frameId << ": expected " << expectedUserDoubles << " userDoubles, got "
              << userDoubles.size() << std::endl;
    return 1;
  }
  for (int i = 0; i < expectedUserDoubles; ++i) {
    const double expectedVal = realDist(rng);
    if (userDoubles[i] != expectedVal) {
      std::cerr << "Frame " << frameId << ": userDouble " << i << " value mismatch"
                << " (expected=" << expectedVal << " actual=" << userDoubles[i] << ")" << std::endl;
      return 1;
    }
  }

  return 0;
}

template <typename ReaderT>
int read_frames_multithreaded(const std::string& filename, int nThreads, unsigned expectedEntries) {
  ReaderT reader;
  reader.openFile(filename);

  if (reader.getEntries("events") != expectedEntries) {
    std::cerr << "Expected " << expectedEntries << " entries, got " << reader.getEntries("events") << std::endl;
    return 1;
  }

  std::mutex readerMutex;
  std::atomic<unsigned> nextEntry{0};
  std::atomic<int> failures{0};

  {
    std::vector<std::jthread> threads;
    for (int t = 0; t < nThreads; ++t) {
      threads.emplace_back([&]() {
        while (true) {
          const unsigned entry = nextEntry.fetch_add(1);
          if (entry >= expectedEntries) {
            break;
          }

          podio::Frame frame;
          {
            std::lock_guard<std::mutex> lock(readerMutex);
            frame = podio::Frame(reader.readEntry("events", entry));
          }
          // Verify frame content in parallel (no lock needed)
          const auto& info = frame.get<EventInfoCollection>("info");
          const int frameId = info[0].Number();
          failures += checkRandomFrame(frame, frameId);
        }
      });
    }
  }

  return failures.load();
}

#endif // PODIO_TESTS_READ_FRAME_MULTITHREADED_H

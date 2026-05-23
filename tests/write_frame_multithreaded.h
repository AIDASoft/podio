#ifndef PODIO_TESTS_WRITE_FRAME_MULTITHREADED_H // NOLINT(llvm-header-guard): folder structure not suitable
#define PODIO_TESTS_WRITE_FRAME_MULTITHREADED_H // NOLINT(llvm-header-guard): folder structure not suitable

#include "datamodel/EventInfoCollection.h"
#include "datamodel/ExampleClusterCollection.h"
#include "datamodel/ExampleHitCollection.h"
#include "datamodel/ExampleMCCollection.h"

#include "podio/Frame.h"
#include "podio/UserDataCollection.h"

#include <atomic>
#include <mutex>
#include <random>
#include <string>
#include <thread>
#include <vector>

/// Create a Frame with deterministic but varied content based on frameId. The
/// frameId is stored in the EventInfo collection so that we can generate the
/// same random number sequence when reading the frame back for checking
podio::Frame createRandomFrame(int frameId) {
  std::mt19937 rng(frameId);
  // Distributions for collection sizes
  std::uniform_int_distribution<int> hitCountDist(1, 10);
  std::uniform_int_distribution<int> clusterCountDist(1, 5);
  std::uniform_int_distribution<int> mcCountDist(1, 8);
  std::uniform_int_distribution<int> userDataCountDist(1, 15);
  // Distributions for data values
  std::uniform_real_distribution<double> realDist(-100.0, 100.0);
  std::uniform_int_distribution<int> intDist(0, 1000);

  podio::Frame frame{};

  EventInfoCollection info;
  auto infoItem = info.create();
  infoItem.Number(frameId);
  frame.put(std::move(info), "info");

  const int nHits = hitCountDist(rng);
  ExampleHitCollection hits;
  for (int i = 0; i < nHits; ++i) {
    const auto cellID = static_cast<unsigned long long>(intDist(rng));
    const double x = realDist(rng);
    const double y = realDist(rng);
    const double z = realDist(rng);
    const double energy = realDist(rng);
    hits.create(cellID, x, y, z, energy);
  }
  frame.put(std::move(hits), "hits");

  const int nClusters = clusterCountDist(rng);
  ExampleClusterCollection clusters;
  for (int i = 0; i < nClusters; ++i) {
    clusters.create(realDist(rng));
  }
  frame.put(std::move(clusters), "clusters");

  const int nMCs = mcCountDist(rng);
  ExampleMCCollection mcparticles;
  for (int i = 0; i < nMCs; ++i) {
    const double energy = realDist(rng);
    const int pdg = intDist(rng);
    mcparticles.create(energy, pdg);
  }
  frame.put(std::move(mcparticles), "mcparticles");

  const int nUserDoubles = userDataCountDist(rng);
  podio::UserDataCollection<double> userDoubles;
  userDoubles.resize(nUserDoubles);
  for (int i = 0; i < nUserDoubles; ++i) {
    userDoubles[i] = realDist(rng);
  }
  frame.put(std::move(userDoubles), "userDoubles");

  return frame;
}

template <typename WriterT>
int write_frames_multithreaded(const std::string& filename, int nThreads, int framesPerThread) {
  WriterT writer(filename);
  std::mutex writerMutex;
  std::atomic<int> frameCounter{0};
  {
    std::vector<std::jthread> threads;
    for (int t = 0; t < nThreads; ++t) {
      threads.emplace_back([&]() {
        for (int f = 0; f < framesPerThread; ++f) {
          const int frameId = frameCounter.fetch_add(1);
          auto frame = createRandomFrame(frameId);

          {
            std::lock_guard<std::mutex> lock(writerMutex);
            writer.writeFrame(frame, "events");
          }
        }
      });
    }
  }

  writer.finish();
  return 0;
}

#endif // PODIO_TESTS_WRITE_FRAME_MULTITHREADED_H

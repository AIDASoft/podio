#ifndef PODIO_TESTS_READ_PARTIAL_H // NOLINT(llvm-header-guard): folder structure not suitable
#define PODIO_TESTS_READ_PARTIAL_H // NOLINT(llvm-header-guard): folder structure not suitable

#include "podio/Frame.h"

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

void read_partial_collections(const podio::Frame& event, const std::vector<std::string>& collsToRead) {
  for (const auto& name : collsToRead) {
    event.get(name);
  }
}

/**
 * This is a test case that will always work in normal builds and will only fail
 * in builds with sanitizers enabled, where they will start to fail in case of
 * e.g. memory leaks.
 */
template <typename ReaderT>
int read_partial_frames(const std::string& filename) {
  auto reader = ReaderT();
  try {
    reader.openFile(filename);
  } catch (const std::runtime_error& e) {
    std::cerr << "File " << filename << " could not be opened. aborting." << std::endl;
    return 1;
  }

  for (auto i = 0u; i < reader.getEntries(podio::Category::Event); ++i) {
    const auto event = podio::Frame(reader.readEntry(podio::Category::Event, i));
    read_partial_collections(event, {"mcparticles", "info", "hits", "clusters"});
  }

  return 0;
}
#endif // PODIO_TESTS_READ_PARTIAL_H

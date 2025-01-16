#include <podio/Frame.h>

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

/// Roundtrip test for ensuring that files that have been written with Frames
/// that have been read from file selecting only a few collections to be read
/// are still properly usable when read again.
///
/// The flow is
///
/// - Open the original file (using one that has been produced by other I/O
///   tests)
/// - Read a Frame selecting only a few of the available collections
/// - Write this frame into the outputFile without doing any further checks
///   (that is done by other tests)
/// - Read back that file and check that it can still be used
///   - Reading full frames
///   - and frames with a selection of collections
///
/// do_roundrip ties everything together and is the only function that needs to
/// be called by backend specific tests

/// The collection names that will be written into the new file
const std::vector<std::string> collectionSelection = {"mcparticles", "links", "userInts", "hits"};

/// The collections to select in the second round
const std::vector<std::string> roundTripSelection = {"hits", "userInts"};

/// Write a new file containing only a few selected collections and only one
/// event
template <typename ReaderT, typename WriterT>
void writeSelectedFile(const std::string& originalFile, const std::string& outputFile) {
  auto reader = ReaderT();
  reader.openFile(originalFile);
  const auto event = podio::Frame(reader.readEntry("events", 0, collectionSelection));

  auto writer = WriterT(outputFile);
  writer.writeFrame(event, "events");
}

bool compareUnordered(std::vector<std::string> lhs, std::vector<std::string> rhs) {
  std::ranges::sort(lhs);
  std::ranges::sort(rhs);
  return std::ranges::equal(lhs, rhs);
}

std::ostream& operator<<(std::ostream& os, const std::vector<std::string>& vec) {
  os << "[";
  std::string delim = "";
  for (const auto& v : vec) {
    os << std::exchange(delim, ", ") << v;
  }
  return os << "]";
}

/// Read the file that has been created and check all available collections
template <typename ReaderT>
int readSelectedFileFull(const std::string& filename) {
  auto reader = ReaderT();
  reader.openFile(filename);

  const auto event = podio::Frame(reader.readEntry("events", 0));

  if (!compareUnordered(event.getAvailableCollections(), collectionSelection)) {
    std::cerr
        << "Collection names that are available from the selected collections file are not as expected (expected: "
        << collectionSelection << ", actual " << event.getAvailableCollections() << ")" << std::endl;
    return 1;
  }

  return 0;
}

/// Read the file that has been created and check whether selecting on that file
/// again also works
template <typename ReaderT>
int readSelectedFilePartial(const std::string& filename) {
  auto reader = ReaderT();
  reader.openFile(filename);

  const auto event = podio::Frame(reader.readEntry("events", 0, roundTripSelection));

  if (!compareUnordered(event.getAvailableCollections(), roundTripSelection)) {
    std::cerr
        << "Collection names that are available from the selected collections file are not as expected (expected: "
        << roundTripSelection << ", actual " << event.getAvailableCollections() << ")" << std::endl;
    return 1;
  }

  return 0;
}

template <typename ReaderT, typename WriterT>
int do_roundtrip(const std::string& originalFile, const std::string& outputFile) {
  writeSelectedFile<ReaderT, WriterT>(originalFile, outputFile);

  return readSelectedFileFull<ReaderT>(outputFile) + readSelectedFilePartial<ReaderT>(outputFile);
}

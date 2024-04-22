#include "podio/Reader.h"

#include "podio/ROOTReader.h"
#if PODIO_ENABLE_RNTUPLE
  #include "podio/RNTupleReader.h"
#endif
#if PODIO_ENABLE_SIO
  #include "podio/SIOFrameReader.h"
#endif

#include "TFile.h"
#include "TKey.h"
#include <memory>

namespace podio {

template <typename T>
Reader::Reader(std::unique_ptr<T> reader) : m_self(std::make_unique<ReaderModel<T>>(reader.release())) {
}

Reader makeReader(const std::string& filename) {
  return makeReader(std::vector<std::string>{filename});
}

Reader makeReader(const std::vector<std::string>& filenames) {

  auto suffix = filenames[0].substr(filenames[0].find_last_of(".") + 1);
  for (size_t i = 1; i < filenames.size(); ++i) {
    if (filenames[i].substr(filenames[i].find_last_of(".") + 1) != suffix) {
      throw std::runtime_error("All files must have the same extension");
    }
  }

  if (suffix == "root") {
    // Check only the first file for RNTuples
    TFile* file = TFile::Open(filenames[0].c_str());
    bool hasRNTuple = false;

    for (auto key : *file->GetListOfKeys()) {
      auto tkey = dynamic_cast<TKey*>(key);

      // if (tkey && tkey->GetClassName() == "ROOT::Experimental::RNTuple") {
      if (tkey && std::string(tkey->GetClassName()) == "ROOT::Experimental::RNTuple") {
        hasRNTuple = true;
        break;
      }
    }
    if (hasRNTuple) {
#if PODIO_ENABLE_RNTUPLE
      auto actualReader = std::make_unique<RNTupleReader>();
      actualReader->openFiles(filenames);
      Reader reader{std::move(actualReader)};
      return reader;
#else
      throw std::runtime_error("ROOT RNTuple reader not available. Please recompile with ROOT RNTuple support.");
#endif
    } else {
      auto actualReader = std::make_unique<ROOTReader>();
      actualReader->openFiles(filenames);
      Reader reader{std::move(actualReader)};
      return reader;
    }
  } else if (suffix == "sio") {
#if PODIO_ENABLE_SIO
    auto actualReader = std::make_unique<SIOFrameReader>();
    actualReader->openFiles(filenames);
    Reader reader{std::move(actualReader)};
    return reader;
#else
    throw std::runtime_error("SIO reader not available. Please recompile with SIO support.");
#endif
  }

  throw std::runtime_error("Unknown file extension: " + suffix);
}

} // namespace podio

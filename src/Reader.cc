#include "podio/Reader.h"

#include "podio/ROOTReader.h"
#if PODIO_ENABLE_RNTUPLE
  #include "podio/RNTupleReader.h"
#endif
#if PODIO_ENABLE_SIO
  #include "podio/SIOReader.h"
#endif

#include "podio/utilities/Glob.h"
#include "podio/utilities/ReaderUtils.h"

#include "TFile.h"
#include "TKey.h"
#include <memory>

namespace podio {

template <typename T>
Reader::Reader(std::unique_ptr<T> reader) : m_self(std::make_unique<ReaderModel<T>>(std::move(reader))) {
}

Reader makeReader(const std::string& filename) {
  return makeReader(utils::expand_glob(filename));
}

Reader makeReader(const std::vector<std::string>& filenames) {

  if (filenames.empty()) {
    throw std::runtime_error("No files given to create a Podio Reader");
  }

  auto suffix = filenames[0].substr(filenames[0].find_last_of(".") + 1);
  for (size_t i = 1; i < filenames.size(); ++i) {
    if (filenames[i].substr(filenames[i].find_last_of(".") + 1) != suffix) {
      throw std::runtime_error("All files must have the same extension");
    }
  }

  if (suffix == "root") {
    TFile* file = TFile::Open(filenames[0].c_str());
    bool hasRNTuple = false;

    if (!file) {
      throw std::runtime_error("Could not open file: " + filenames[0]);
    }

    for (auto key : *file->GetListOfKeys()) {
      auto tkey = dynamic_cast<TKey*>(key);

      if (tkey) {
        const auto className = std::string(tkey->GetClassName());
        if (className == "ROOT::Experimental::RNTuple" || className == "ROOT::RNTuple") {
          hasRNTuple = true;
          break;
        }
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
    if (filenames.size() > 1) {
      throw std::runtime_error("The SIO reader does currently not support reading multiple files");
    }
    auto actualReader = std::make_unique<SIOReader>();
    actualReader->openFile(filenames[0]);
    Reader reader{std::move(actualReader)};
    return reader;
#else
    throw std::runtime_error("SIO reader not available. Please recompile with SIO support.");
#endif
  }

  throw std::runtime_error("Unknown file extension: " + suffix);
}

std::optional<std::map<std::string, SizeStats>> Reader::getSizeStats(std::string_view category) const {
  if (const auto* rootReader = dynamic_cast<ReaderModel<ROOTReader>*>(m_self.get())) {
    return rootReader->m_reader->getSizeStats(category);
  }
  return std::nullopt;
}

} // namespace podio

#ifndef PODIO_ROOTWRITEOPTIONS_H
#define PODIO_ROOTWRITEOPTIONS_H

#include <optional>

namespace podio {

/// Common configuration options for the ROOT based writers, i.e. the ROOTWriter
/// (TTree based) and the RNTupleWriter.
struct ROOTWriteOptions {
  /// ROOT compression setting: algorithm*100+level (0 disables compression).
  /// Use ROOT::RCompressionSetting::EDefaults values, e.g.:
  ///   kUseGeneralPurpose=505 (ZSTD-5), kUseAnalysis=404 (LZ4-4), kUseSmallest=207 (LZMA-7).
  /// If unset, the default of the respective writer is used.
  std::optional<int> compressionSettings{};
};

} // namespace podio

#endif // PODIO_ROOTWRITEOPTIONS_H

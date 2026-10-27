#ifndef PODIO_WRITEROPTIONS_H
#define PODIO_WRITEROPTIONS_H

#include <cstddef>
#include <optional>
#include <string>

namespace podio {

/// Configuration options for the ROOTWriter (TTree backend).
///
/// Also available as ROOTWriter::Options.
struct ROOTWriterOptions {
  /// ROOT compression setting: algorithm*100+level (0 disables compression).
  /// Use ROOT::RCompressionSetting::EDefaults values, e.g.:
  ///   kUseGeneralPurpose=505 (ZSTD-5), kUseAnalysis=404 (LZ4-4), kUseSmallest=207 (LZMA-7).
  /// If unset, the TFile default (kUseCompiledDefault) is used.
  std::optional<int> compressionSettings{};
};

/// Configuration options for the RNTupleWriter.
///
/// Also available as RNTupleWriter::Options.
struct RNTupleWriterOptions {
  /// ROOT compression setting: algorithm*100+level (0 disables compression).
  /// Use ROOT::RCompressionSetting::EDefaults values, e.g.:
  ///   kUseGeneralPurpose=505 (ZSTD-5), kUseAnalysis=404 (LZ4-4), kUseSmallest=207 (LZMA-7).
  /// If unset, the RNTuple default from ROOT is used.
  std::optional<int> compressionSettings{};
};

/// Configuration options for the ArrowWriter (Parquet backend).
///
/// Also available as ArrowWriter::Options.
struct ArrowWriterOptions {
  size_t maxBufferedRows = 1000;
  std::string compression = "";
};

} // namespace podio

#endif // PODIO_WRITEROPTIONS_H

#ifndef PODIO_WRITER_H
#define PODIO_WRITER_H

#include "podio/Frame.h"
#include "podio/FrameCategories.h"
#include "podio/ROOTWriteOptions.h"

#include <any>
#include <concepts>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace podio {

/// Generic (type erased) writer class that can handle different I/O backends
/// (almost) transparently
///
/// Offers some more high level functionality compared to the lower level
/// backend specific writers that this class wraps. In addition, it provides
/// convenience methods to deal specifically with the "events" frame category.
///
/// @note Since this simply wraps lower level writers, some of the limitations
/// of the wrapped writers will still apply, e.g. if used for writing ROOT files
/// frames of a given category will have to have the same contents.
///
/// @note The recommended way to construct is to use WriterFor or the
/// makeWriter() function since they handle the instantiation of the correct low
/// level writers
class Writer {
private:
  struct WriterConcept {
    virtual ~WriterConcept() = default;

    virtual void writeFrame(const podio::Frame& frame, std::string_view category,
                            const std::vector<std::string>& collections) = 0;
    virtual void finish() = 0;
  };

private:
  template <typename T>
  struct WriterModel final : WriterConcept {
    WriterModel(std::unique_ptr<T> writer) : m_writer(std::move(writer)) {
    }
    WriterModel(const WriterModel&) = delete;
    WriterModel& operator=(const WriterModel&) = delete;
    WriterModel(WriterModel&&) = default;
    WriterModel& operator=(WriterModel&&) = default;

    ~WriterModel() override = default;

    void writeFrame(const podio::Frame& frame, std::string_view category,
                    const std::vector<std::string>& collections) override {
      return m_writer->writeFrame(frame, category, collections);
    }
    void finish() override {
      return m_writer->finish();
    }
    std::unique_ptr<T> m_writer{nullptr};
  };

  std::unique_ptr<WriterConcept> m_self{nullptr};

public:
  /// Create a Writer from a lower level writer
  ///
  /// @tparam T the type of the low level writer (will be deduced)
  /// @param writer A low level writer that does the actual work
  template <typename T>
  Writer(std::unique_ptr<T> writer) : m_self(std::make_unique<WriterModel<T>>(std::move(writer))) {
  }

  Writer(const Writer&) = delete;
  Writer& operator=(const Writer&) = delete;
  Writer(Writer&&) = default;
  Writer& operator=(Writer&&) = default;

  /// Destructor
  ///
  /// This also takes care of writing all the necessary metadata to read files
  /// back again.
  ~Writer() = default;

  /// Store the given frame with the given category
  ///
  /// This stores all available categories from the passed frame
  ///
  /// @param frame    The frame to write
  /// @param category The category name under which this frame should be stored
  void writeFrame(const podio::Frame& frame, std::string_view category) {
    return m_self->writeFrame(frame, category, frame.getAvailableCollections());
  }

  /// Store the given Frame with the given category.
  ///
  /// This stores only the desired collections and not the complete frame.
  ///
  /// @param frame        The Frame to store
  /// @param category     The category name under which this Frame should be
  ///                     stored
  /// @param collections  The collection names that should be written
  void writeFrame(const podio::Frame& frame, std::string_view category, const std::vector<std::string>& collections) {
    return m_self->writeFrame(frame, category, collections);
  }

  /// Store the given frame under the "events" category
  ///
  /// This stores all available categories from the passed frame
  ///
  /// @param frame    The frame to write
  void writeEvent(const podio::Frame& frame) {
    writeFrame(frame, podio::Category::Event, frame.getAvailableCollections());
  }

  /// Store the given Frame under the "events" category
  ///
  /// This stores only the desired collections and not the complete frame.
  ///
  /// @param frame        The Frame to store
  /// @param collections  The collection names that should be written
  void writeEvent(const podio::Frame& frame, const std::vector<std::string>& collections) {
    writeFrame(frame, podio::Category::Event, collections);
  }

  /// Write the current file, including all the necessary metadata to read it
  /// again.
  ///
  /// @note The destructor will also call this, so letting a Writer go out of
  /// scope is also a viable way to write a readable file
  void finish() {
    return m_self->finish();
  }
};

/// Builder for a Writer that allows to configure the backend and its options.
///
/// The backend is determined when create() is called, in the following order
/// - An explicitly selected backend (via either withBackend() or withType()) is
///   used.
/// - The extension of the filename selects the backend. For ".root" files the
///   PODIO_DEFAULT_WRITE_RNTUPLE environment variable switches from TTree to
///   RNTuple, if it is set to a non-empty value.
///
/// Options passed via withOptions() never select a backend, but they have to be
/// usable with the backend that has been determined as described above.
///
/// All configuration errors are detected before any file is created.
///
/// @code{.cpp}
/// #include <podio/Writer.h>
///
/// auto writer = podio::WriterFor{"output.root"}.withBackend(podio::WriterFor::Backend::RNTuple).create();
/// auto writer = podio::WriterFor{"output.root"}.withType("rntuple").create();
/// // TTree or RNTuple depending on PODIO_DEFAULT_WRITE_RNTUPLE
/// auto writer = podio::WriterFor{"output.root"}.withOptions({.compressionSettings = 505}).create();
/// // Always RNTuple
/// auto writer = podio::WriterFor{"output.root"}
///                   .withBackend(podio::WriterFor::Backend::RNTuple)
///                   .withOptions({.compressionSettings = 505})
///                   .create();
/// @endcode
class WriterFor {
public:
  /// The I/O backends that can be explicitly selected
  enum class Backend {
    TTree,   ///< TTree based ROOT files (ROOTWriter)
    RNTuple, ///< RNTuple based ROOT files (RNTupleWriter)
    SIO,     ///< SIO files (SIOWriter)
    Arrow,   ///< Arrow / Parquet directories (ArrowWriter)
  };

  /// Start configuring a Writer for the given file
  ///
  /// @param filename The filename of the output file that will be created
  explicit WriterFor(std::string filename) : m_filename(std::move(filename)) {
  }

  /// Explicitly select the backend
  ///
  /// @param backend The backend to use
  ///
  /// @throws std::invalid_argument If a backend has already been selected
  /// @throws std::runtime_error If support for the backend has not been built
  WriterFor& withBackend(Backend backend);

  /// Explicitly select the backend via its name
  ///
  /// @param type One of "default", "root", "rntuple", "sio" or "parquet"
  ///             (case insensitive). "default" does not select a backend, it
  ///             is determined from the options or the filename instead
  ///
  /// @throws std::runtime_error If the type is not known or if support for
  ///         the backend has not been built
  /// @throws std::invalid_argument If a backend has already been selected
  WriterFor& withType(const std::string& type);

  /// Set the options for the ROOT based writers (TTree and RNTuple)
  ///
  /// These options do not select between TTree and RNTuple. The format is
  /// determined by the explicitly selected backend, or by the
  /// PODIO_DEFAULT_WRITE_RNTUPLE environment variable and the filename. It is
  /// an error if this results in a backend that is not ROOT based.
  ///
  /// Replaces any previously set options.
  ///
  /// @note This overload exists to allow passing the options as a braced
  /// initializer list, e.g. `withOptions({.compressionSettings = 505})`
  ///
  /// @param options The options for the ROOT based writers
  WriterFor& withOptions(const ROOTWriteOptions& options) {
    m_options = options;
    return *this;
  }

  /// Set the options of a specific low level writer
  ///
  /// The options do not select a backend. They are only checked against the
  /// backend that has been determined when create() is called.
  ///
  /// Replaces any previously set options.
  ///
  /// @tparam OptionsT The options of a low level writer, e.g.
  ///                  ArrowWriter::Options
  /// @param options   The backend specific options
  template <std::copy_constructible OptionsT>
  WriterFor& withOptions(const OptionsT& options) {
    m_options = options;
    return *this;
  }

  /// Create the Writer
  ///
  /// @returns A fully initialized Writer for the I/O backend that has been
  ///          determined
  ///
  /// @throws std::invalid_argument If the options can not be used with the
  ///         selected backend
  /// @throws std::runtime_error If the filename can not be associated to an I/O
  ///         backend or if support for the desired I/O backend has not been
  ///         built
  Writer create() const;

private:
  /// Throw if a backend has already been explicitly selected
  void ensureNoBackendSelected() const;

  std::string m_filename;
  /// The explicitly selected backend (if any)
  std::optional<Backend> m_backend{};
  /// The options for the low level writer (if any). These are only resolved
  /// in create() once the backend is known
  std::any m_options{};
};

/// Create a Writer that is able to write files for the desired backend
///
/// Will look at the desired filename as well as the type argument to decide on
/// the backend. Equivalent to `WriterFor{filename}.withType(type).create()`, see
/// WriterFor for more details.
///
/// @param filename The filename of the output file that will be created.
/// @param type     The (optional) type argument to explicitly select a
///                 backend; see WriterFor::withType()
///
/// @returns A fully initialized Writer for the I/O backend that has been
///         determined
///
/// @throws std::runtime_error In case the suffix or the type can not be
///         associated to an I/O backend or if support for the desired I/O
///         backend has not been built
Writer makeWriter(const std::string& filename, const std::string& type = "default");

} // namespace podio

#endif // PODIO_WRITER_H

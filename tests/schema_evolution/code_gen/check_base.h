#ifndef PODIO_TESTS_SCHEMAEVOLUTION_CODEGEN_CHECKBASE_H // NOLINT(llvm-header-guard): folder structure not suitable
#define PODIO_TESTS_SCHEMAEVOLUTION_CODEGEN_CHECKBASE_H // NOLINT(llvm-header-guard): folder structure not suitable

#include <podio/Frame.h>
#include <podio/ROOTReader.h>
#include <podio/ROOTWriter.h>
#if PODIO_ENABLE_RNTUPLE
  #include <podio/RNTupleReader.h>
  #include <podio/RNTupleWriter.h>
#endif

#include <iostream>

#if defined(PODIO_SCHEMA_EVOLUTION_RNTUPLE) && !PODIO_ENABLE_RNTUPLE
  #error "Cannot build schema evolution tests for RNTuple without RNTuple support"
#endif

#ifdef PODIO_SCHEMA_EVOLUTION_RNTUPLE
using WriterT = podio::RNTupleWriter;
using ReaderT = podio::RNTupleReader;
  #define FILE_SUFFIX "_rntuple.root"
#else
using WriterT = podio::ROOTWriter;
using ReaderT = podio::ROOTReader;
  #define FILE_SUFFIX ".root"
#endif

#define ASSERT_EQUAL(actual, expected, msg)                                                                            \
  if ((expected) != (actual)) {                                                                                        \
    std::cerr << "Assertion failed " << #expected << " == " << #actual << " | " << msg << " (expected: " << expected   \
              << ", actual: " << actual << ") at " << __FILE__ << ":" << __LINE__;                                     \
    return 1;                                                                                                          \
  }

#ifdef PODIO_SCHEMA_EVOLUTION_TEST_WRITE
  /// Macro to write a frame using the specified writer type and filename
  /// setting up the corresponding writer and a Frame named event. Wraps the
  /// passed code into a dedicated scope.
  ///
  /// @param WRITER_TYPE The type of writer to use (e.g., WriterT)
  /// @param FILENAME The name of the file to write to
  #define WRITE_WITH(WRITER_TYPE, FILENAME, ...)                                                                       \
    {{std::cout << "Writing file " << FILENAME << std::endl;                                                           \
    auto writer = WRITER_TYPE(FILENAME);                                                                               \
    auto event = podio::Frame();                                                                                       \
    {__VA_ARGS__};                                                                                                     \
    writer.writeFrame(event, podio::Category::Event);                                                                  \
    }                                                                                                                  \
    }

  /// Macro to write a collection of the specified type to a test file using a
  /// pre-defined WriterT and constructing the filename to write to from the
  /// TEST_CASE and FILE_SUFFIX pre-processor constants. In addition to setting
  /// up a writer and an event Frame it also creates the collection of type
  /// COLL_TYPE and a first element using coll and elem as variable names
  /// respectively. Wraps the passed code into a dedicated scope
  ///
  /// @param COLL_TYPE The type of collection to create and write
  #define WRITE_AS(COLL_TYPE, ...)                                                                                     \
    {WRITE_WITH(WriterT, (TEST_CASE FILE_SUFFIX), {                                                                    \
      auto coll = COLL_TYPE();                                                                                         \
      auto elem = coll.create();                                                                                       \
      {__VA_ARGS__};                                                                                                   \
      event.put(std::move(coll), "evolution_collection");                                                              \
    })}
#else
  #define WRITE_WITH(...)
  #define WRITE_AS(...)
#endif

#ifdef PODIO_SCHEMA_EVOLUTION_TEST_READ
  /// Macro to read a frame using the specified reader type and filename. Wraps
  /// the specified code into a dedicated scope after setting up the reader and
  /// reading the event Frame.
  ///
  /// @param READER_TYPE The type of reader to use
  /// @param FILENAME The name of the file to read from
  #define READ_WITH(READER_TYPE, FILENAME, ...)                                                                        \
    {{auto reader = READER_TYPE();                                                                                     \
    std::cout << "Reading file " << FILENAME << std::endl;                                                             \
    reader.openFile(FILENAME);                                                                                         \
    auto event = podio::Frame(reader.readEntry(podio::Category::Event, 0));                                            \
    {__VA_ARGS__};                                                                                                     \
    return 0;                                                                                                          \
    }                                                                                                                  \
    }

  /// Macro to read a collection of the specified type from a test file using
  /// the READ_WITH macro using the a pre-defined ReaderT and constructing the
  /// filename to read from from the TEST_CASE and FILE_SUFFIX pre-processor
  /// constants. Reads the collection of type COLL_TYPE and retrieves the first
  /// element from it before passing on to the specified code.
  ///
  /// @param COLL_TYPE The type of collection to read
  #define READ_AS(COLL_TYPE, ...)                                                                                      \
    {READ_WITH(ReaderT, (TEST_CASE FILE_SUFFIX), {                                                                     \
      const auto& coll = event.get<COLL_TYPE>("evolution_collection");                                                 \
      const auto elem = coll[0];                                                                                       \
      {__VA_ARGS__};                                                                                                   \
    })}
#else
  #define READ_WITH(...)
  #define READ_AS(...)
#endif

#endif // PODIO_TESTS_SCHEMAEVOLUTION_CODEGEN_CHECKBASE_H

#ifndef PODIO_TESTS_SCHEMAEVOLUTION_CODEGEN_CHECKBASE_H // NOLINT(llvm-header-guard): folder structure not suitable
#define PODIO_TESTS_SCHEMAEVOLUTION_CODEGEN_CHECKBASE_H // NOLINT(llvm-header-guard): folder structure not suitable

#include <podio/Frame.h>
#include <podio/ROOTReader.h>
#include <podio/ROOTWriter.h>
#ifdef PODIO_ENABLE_RNTUPLE
  #include <podio/RNTupleReader.h>
  #include <podio/RNTupleWriter.h>
#endif

#include <iostream>

#if defined(PODIO_SCHEMA_EVOLUTION_RNTUPLE) && !defined(PODIO_ENABLE_RNTUPLE)
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
  #define WRITE_WITH(WRITER_TYPE, FILENAME, COLL_TYPE, ...)                                                            \
    {{std::cout << "Writing file " << FILENAME << std::endl;                                                           \
    auto writer = WRITER_TYPE(FILENAME);                                                                               \
    auto event = podio::Frame();                                                                                       \
    {                                                                                                                  \
      auto coll = COLL_TYPE();                                                                                         \
      auto elem = coll.create();                                                                                       \
      __VA_ARGS__                                                                                                      \
      event.put(std::move(coll), "evolution_collection");                                                              \
    }                                                                                                                  \
    writer.writeFrame(event, podio::Category::Event);                                                                  \
    }                                                                                                                  \
    }
  #define WRITE_AS(COLL_TYPE, ...) {WRITE_WITH(WriterT, (TEST_CASE FILE_SUFFIX), COLL_TYPE, __VA_ARGS__)}
#else
  #define WRITE_WITH(...)
  #define WRITE_AS(...)
#endif

#ifdef PODIO_SCHEMA_EVOLUTION_TEST_READ
  #define READ_WITH(READER_TYPE, FILENAME, COLL_TYPE, ...)                                                             \
    {{auto reader = READER_TYPE();                                                                                     \
    std::cout << "Reading file " << FILENAME << std::endl;                                                             \
    reader.openFile(FILENAME);                                                                                         \
    auto event = podio::Frame(reader.readEntry(podio::Category::Event, 0));                                            \
    const auto& coll = event.get<COLL_TYPE>("evolution_collection");                                                   \
    {                                                                                                                  \
      const auto elem = coll[0];                                                                                       \
      __VA_ARGS__                                                                                                      \
    }                                                                                                                  \
    return 0;                                                                                                          \
    }                                                                                                                  \
    }
  #define READ_AS(COLL_TYPE, ...) {READ_WITH(ReaderT, (TEST_CASE FILE_SUFFIX), COLL_TYPE, __VA_ARGS__)}
#else
  #define READ_WITH(...)
  #define READ_AS(...)
#endif

#endif // PODIO_TESTS_SCHEMAEVOLUTION_CODEGEN_CHECKBASE_H

#include "datamodel/TestTypeCollection.h"

#include <podio/Frame.h>

#include <iostream>

#define ASSERT_EQUAL(actual, expected, msg)                                                                            \
  if ((expected) != (actual)) {                                                                                        \
    std::cerr << "Assertion failed " << #expected << " == " << #actual << " | " << msg << " (expected: " << expected   \
              << ", actual: " << actual << ")";                                                                        \
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
#else
  #define WRITE_WITH(...)
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
#else
  #define READ_WITH(...)
#endif

#include "podio/ROOTReader.h"
#include "podio/ROOTWriter.h"

int main() {
  WRITE_WITH(podio::ROOTWriter, "components_new_member.root", TestTypeCollection, {
    elem.s().x = 42;
    elem.s().y = 43;
    elem.s().z = 44;
    elem.s().p = {1, 2, 3, 4};
  });
  READ_WITH(podio::ROOTReader, "components_new_member.root", TestTypeCollection,
            {
              ASSERT_EQUAL(elem.s().t, 0, "New component member variable is not zero initialized");
              ASSERT_EQUAL(elem.s().x, 42, "Member variable unrelated to schema evolution has changed");
              ASSERT_EQUAL(elem.s().y, 43, "Member variable unrelated to schema evolution has changed");
              ASSERT_EQUAL(elem.s().z, 44, "Member variable unrelated to schema evolution has changed");
              ASSERT_EQUAL(elem.s().p[0], 1, "Member variable unrelated to schema evolution has changed");
              ASSERT_EQUAL(elem.s().p[1], 2, "Member variable unrelated to schema evolution has changed");
              ASSERT_EQUAL(elem.s().p[2], 3, "Member variable unrelated to schema evolution has changed");
              ASSERT_EQUAL(elem.s().p[3], 4, "Member variable unrelated to schema evolution has changed");
            }

  );
}

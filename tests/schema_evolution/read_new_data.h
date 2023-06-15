#ifndef PODIO_TESTS_SCHEMAEVOLUTION_READNEWDATA_H // NOLINT(llvm-header-guard): folder structure not suitable
#define PODIO_TESTS_SCHEMAEVOLUTION_READNEWDATA_H // NOLINT(llvm-header-guard): folder structure not suitable

#include "datamodel/ExampleHitCollection.h"
#include "datamodel/ExampleWithArrayComponentCollection.h"

#include "podio/Frame.h"

#include <iostream>
#include <string>

int readSimpleStruct(const podio::Frame& event) {
  const auto& coll = event.get<ExampleWithArrayComponentCollection>("simpleStructTest");
  auto elem = coll[0];
  const auto sstruct = elem.s();

  if (sstruct.y != 0 || sstruct.x != 42 || sstruct.z != 123) {
    return 1;
  }
  return 0;
}

int readExampleHit(const podio::Frame& event) {
  const auto& coll = event.get<ExampleHitCollection>("datatypeMemberAdditionTest");
  auto elem = coll[0];

  if (elem.energy() != 0) {
    return 1;
  }
  if (elem.x() != 1.23 || elem.y() != 1.23 || elem.z() != 1.23 || elem.cellID() != 0xcaffee) {
    return 1;
  }

  return 0;
}

template <typename ReaderT>
int read_new_data(const std::string& filename) {
  ReaderT reader{};
  reader.openFile(filename);

  const auto event = podio::Frame(reader.readEntry("events", 0));

  int result = 0;
  result += readSimpleStruct(event);
  result += readExampleHit(event);

  return result;
}

#endif // PODIO_TESTS_SCHEMAEVOLUTION_WRITEOLDDATA_H

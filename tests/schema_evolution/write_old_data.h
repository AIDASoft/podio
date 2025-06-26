#ifndef PODIO_TESTS_SCHEMAEVOLUTION_WRITEOLDDATA_H // NOLINT(llvm-header-guard): folder structure not suitable
#define PODIO_TESTS_SCHEMAEVOLUTION_WRITEOLDDATA_H // NOLINT(llvm-header-guard): folder structure not suitable

#include "datamodel/ExampleClusterCollection.h"
#include "datamodel/ExampleHitCollection.h"
#include "datamodel/ExampleWithARelationCollection.h"
#include "datamodel/ExampleWithNamespaceCollection.h"

#include "podio/Frame.h"

#include <string>

auto writeExampleHit() {
  ExampleHitCollection coll;
  auto elem = coll.create();
  elem.x(1.23);
  elem.y(1.23);
  elem.z(1.23);
  elem.cellID(0xcaffee);

  return coll;
}

auto writeExampleCluster() {
  ExampleClusterCollection coll;
  auto elem = coll.create(3.14);
  return coll;
}

auto writeExampleWithNamespace() {
  ex42::ExampleWithNamespaceCollection coll;
  auto elem = coll.create();
  elem.y(42);
  elem.x(123);

  return coll;
}

auto writeExampleWithARelation() {
  ex42::ExampleWithARelationCollection coll;
  auto elem = coll.create();
  elem.number(3.14f);

  return coll;
}

podio::Frame createFrame() {
  podio::Frame event;

  event.put(writeExampleCluster(), "datatypeMemberRenameTest");
  event.put(writeExampleHit(), "datatypeMemberAdditionTest");
  event.put(writeExampleWithNamespace(), "componentMemberRenameTest");
  event.put(writeExampleWithARelation(), "floatToDoubleMemberTest");

  return event;
}

template <typename WriterT>
int writeData(const std::string& filename) {
  WriterT writer(filename);

  auto event = createFrame();
  writer.writeFrame(event, "events");

  writer.finish();

  return 0;
}

#endif // PODIO_TESTS_SCHEMAEVOLUTION_WRITEOLDDATA_H

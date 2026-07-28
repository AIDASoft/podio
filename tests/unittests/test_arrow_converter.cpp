#include "catch2/catch_test_macros.hpp"

#include "podio/Frame.h"

// Test datatypes
#include "datamodel/EventInfoCollection.h"
#include "datamodel/ExampleClusterCollection.h"
#include "datamodel/ExampleHitCollection.h"
#include "datamodel/ExampleWithArrayCollection.h"
#include "datamodel/ExampleWithFixedWidthIntegersCollection.h"
#include "datamodel/ExampleWithOneRelationCollection.h"
#include "datamodel/ExampleWithVectorMemberCollection.h"

// Arrow Frame Converter
#include "podio/utilities/ArrowFrameConverter.h"
#include "podio/utilities/ArrowTypeRegistry.h"

// Test frame validation helpers
#include "read_frame.h"
#include "write_frame.h"

// Arrow headers
#include <arrow/api.h>
#include <arrow/table.h>
#include <arrow/type.h>
#include <iostream>

TEST_CASE("ArrowFrameConverter - convertFrameToTable Verification", "[arrow][converter]") {
  auto originalFrame = makeFrame(0);

  std::vector<std::string> colls = {"hits", "clusters", "OneRelation", "WithVectorMember", "hitRefs"};

  // Convert to Arrow Table
  auto table = podio::convertFrameToTable(originalFrame, colls);
  REQUIRE(table != nullptr);
  REQUIRE(table->num_rows() == 1);

  // Validate the built Table fully
  auto validateStatus = table->ValidateFull();
  REQUIRE(validateStatus.ok());

  // Check columns present
  auto schema = table->schema();
  REQUIRE(schema->GetFieldByName("hits") != nullptr);
  REQUIRE(schema->GetFieldByName("clusters") != nullptr);
  REQUIRE(schema->GetFieldByName("OneRelation") != nullptr);
  REQUIRE(schema->GetFieldByName("WithVectorMember") != nullptr);
  REQUIRE(schema->GetFieldByName("hitRefs") != nullptr);
  REQUIRE(schema->GetFieldByName("frame_parameters") != nullptr);

  // Assertions on hits table structure and metadata
  auto hitsField = schema->GetFieldByName("hits");
  auto hitsMeta = hitsField->metadata();
  REQUIRE(hitsMeta != nullptr);
  REQUIRE(hitsMeta->Get("value_type").ValueOrDie() == "ExampleHit");
  REQUIRE(hitsMeta->Get("is_subset").ValueOrDie() == "0");

  auto hitsArray = std::static_pointer_cast<arrow::ListArray>(table->GetColumnByName("hits")->chunk(0));
  REQUIRE(hitsArray != nullptr);
  REQUIRE(hitsArray->length() == 1);
  REQUIRE(hitsArray->value_length(0) == 2);

  auto hitsStruct = std::static_pointer_cast<arrow::StructArray>(hitsArray->values());
  auto hitsX = std::static_pointer_cast<arrow::DoubleArray>(hitsStruct->GetFieldByName("x"));
  REQUIRE(hitsX->Value(0) == 0.0);
  REQUIRE(hitsX->Value(1) == 1.0);

  auto hitsEnergy = std::static_pointer_cast<arrow::DoubleArray>(hitsStruct->GetFieldByName("energy"));
  REQUIRE(hitsEnergy->Value(0) == 23.0);
  REQUIRE(hitsEnergy->Value(1) == 12.0);

  // Assertions on clusters table structure and metadata
  auto clustersField = schema->GetFieldByName("clusters");
  auto clustersMeta = clustersField->metadata();
  REQUIRE(clustersMeta->Get("value_type").ValueOrDie() == "ExampleCluster");

  auto clustersArray = std::static_pointer_cast<arrow::ListArray>(table->GetColumnByName("clusters")->chunk(0));
  REQUIRE(clustersArray->value_length(0) == 3);

  auto clustersStruct = std::static_pointer_cast<arrow::StructArray>(clustersArray->values());
  auto clustersEnergy = std::static_pointer_cast<arrow::DoubleArray>(clustersStruct->GetFieldByName("energy"));
  REQUIRE(clustersEnergy->Value(0) == 23.0);
  REQUIRE(clustersEnergy->Value(1) == 12.0);
  REQUIRE(clustersEnergy->Value(2) == 35.0);

  auto clustersHits = std::static_pointer_cast<arrow::ListArray>(clustersStruct->GetFieldByName("Hits"));
  REQUIRE(clustersHits->value_length(0) == 1);
  REQUIRE(clustersHits->value_length(2) == 2);

  auto clustersHitsStruct = std::static_pointer_cast<arrow::StructArray>(clustersHits->values());
  auto clustersHitsIndex = std::static_pointer_cast<arrow::Int32Array>(clustersHitsStruct->GetFieldByName("index"));
  REQUIRE(clustersHitsIndex->Value(0) == 0); // hit0 index

  // Assertions on OneRelation
  auto relArray = std::static_pointer_cast<arrow::ListArray>(table->GetColumnByName("OneRelation")->chunk(0));
  REQUIRE(relArray->value_length(0) == 2);

  auto relStruct = std::static_pointer_cast<arrow::StructArray>(relArray->values());
  auto relCluster = std::static_pointer_cast<arrow::StructArray>(relStruct->GetFieldByName("cluster"));
  auto relClusterIndex = std::static_pointer_cast<arrow::Int32Array>(relCluster->GetFieldByName("index"));
  REQUIRE(relClusterIndex->Value(0) == 2); // cluster index 2

  // Assertions on WithVectorMember
  auto vecArray = std::static_pointer_cast<arrow::ListArray>(table->GetColumnByName("WithVectorMember")->chunk(0));
  REQUIRE(vecArray->value_length(0) == 2);

  auto vecStruct = std::static_pointer_cast<arrow::StructArray>(vecArray->values());
  auto vecCount = std::static_pointer_cast<arrow::ListArray>(vecStruct->GetFieldByName("count"));
  auto vecCountVal = std::static_pointer_cast<arrow::Int32Array>(vecCount->values());
  REQUIRE(vecCountVal->Value(0) == 0);
  REQUIRE(vecCountVal->Value(1) == 10);

  // Assertions on hitRefs (subset collection)
  auto subField = schema->GetFieldByName("hitRefs");
  REQUIRE(subField->metadata()->Get("is_subset").ValueOrDie() == "1");

  auto subArray = std::static_pointer_cast<arrow::ListArray>(table->GetColumnByName("hitRefs")->chunk(0));
  REQUIRE(subArray->value_length(0) == 2);

  auto subObjectID = std::static_pointer_cast<arrow::StructArray>(subArray->values());
  auto subIndex = std::static_pointer_cast<arrow::Int32Array>(subObjectID->GetFieldByName("index"));
  REQUIRE(subIndex->Value(0) == 1); // pointing to hits[1]
  REQUIRE(subIndex->Value(1) == 0); // pointing to hits[0]

  // Assertions on frame parameters
  auto paramArray = std::static_pointer_cast<arrow::StructArray>(table->GetColumnByName("frame_parameters")->chunk(0));
  REQUIRE(paramArray != nullptr);
  REQUIRE(paramArray->length() == 1);

  // Verify int_params
  auto intParams = std::static_pointer_cast<arrow::MapArray>(paramArray->GetFieldByName("int_params"));
  REQUIRE(intParams != nullptr);
  REQUIRE(intParams->length() == 1);
  REQUIRE(intParams->value_length(0) == 3);
  auto intKeys = std::static_pointer_cast<arrow::StringArray>(intParams->keys());
  REQUIRE(intKeys->GetString(0) == "SomeValue");
  REQUIRE(intKeys->GetString(1) == "SomeVectorData");
  REQUIRE(intKeys->GetString(2) == "anInt");
}

TEST_CASE("ArrowFrameConverter - convertTableToFrame Reader-Only Verification", "[arrow][converter][reader]") {
  auto arrowType = podio::ArrowTypeRegistry::instance().getType("ExampleHit");
  REQUIRE(arrowType != nullptr);

  std::unique_ptr<arrow::ArrayBuilder> builder;
  auto status = arrow::MakeBuilder(arrow::default_memory_pool(), arrowType, &builder);
  REQUIRE(status.ok());
  auto* collectionBuilder = static_cast<arrow::ListBuilder*>(builder.get());
  auto* objectBuilder = static_cast<arrow::StructBuilder*>(collectionBuilder->value_builder());
  auto* builder_cellID = static_cast<arrow::UInt64Builder*>(objectBuilder->child(0));
  auto* builder_x = static_cast<arrow::DoubleBuilder*>(objectBuilder->child(1));
  auto* builder_y = static_cast<arrow::DoubleBuilder*>(objectBuilder->child(2));
  auto* builder_z = static_cast<arrow::DoubleBuilder*>(objectBuilder->child(3));
  auto* builder_energy = static_cast<arrow::DoubleBuilder*>(objectBuilder->child(4));

  REQUIRE(collectionBuilder->Append().ok());

  // Hit 1
  REQUIRE(builder_cellID->Append(0x100ULL).ok());
  REQUIRE(builder_x->Append(10.0).ok());
  REQUIRE(builder_y->Append(20.0).ok());
  REQUIRE(builder_z->Append(30.0).ok());
  REQUIRE(builder_energy->Append(5.5).ok());
  REQUIRE(objectBuilder->Append().ok());

  // Hit 2
  REQUIRE(builder_cellID->Append(0x200ULL).ok());
  REQUIRE(builder_x->Append(-1.0).ok());
  REQUIRE(builder_y->Append(-2.0).ok());
  REQUIRE(builder_z->Append(-3.0).ok());
  REQUIRE(builder_energy->Append(0.5).ok());
  REQUIRE(objectBuilder->Append().ok());

  std::shared_ptr<arrow::Array> array;
  REQUIRE(collectionBuilder->Finish(&array).ok());

  auto metadata = arrow::KeyValueMetadata::Make({"value_type", "is_subset", "coll_id"}, {"ExampleHit", "0", "101"});
  auto field = arrow::field("Hits", arrowType, true, std::move(metadata));

  auto schema = arrow::schema({field});
  auto batch = arrow::RecordBatch::Make(schema, 1, {array});
  REQUIRE(batch != nullptr);
  auto tableResult = arrow::Table::FromRecordBatches({batch});
  REQUIRE(tableResult.ok());
  const auto& table = tableResult.ValueOrDie();

  auto frame = podio::convertTableToFrame(table, 0);
  REQUIRE(frame != nullptr);

  const auto& recHits = frame->get<ExampleHitCollection>("Hits");
  REQUIRE(recHits.size() == 2);
  REQUIRE(recHits[0].cellID() == 0x100ULL);
  REQUIRE(recHits[0].x() == 10.0f);
  REQUIRE(recHits[0].energy() == 5.5);
  REQUIRE(recHits[1].cellID() == 0x200ULL);
  REQUIRE(recHits[1].x() == -1.0f);
  REQUIRE(recHits[1].energy() == 0.5);
}

void verifyEventNoUserData(const podio::Frame& event, int eventNum) {
  const float evtWeight = event.getParameter<float>("UserEventWeight").value();
  REQUIRE(evtWeight == 100.f * eventNum);

  std::stringstream ss;
  ss << " event_number_" << eventNum;
  const auto evtName = event.getParameter<std::string>("UserEventName").value();
  REQUIRE(evtName == ss.str());

  const auto someVectorData = event.getParameter<std::vector<int>>("SomeVectorData").value();
  REQUIRE(someVectorData.size() == 4);
  for (int i = 0; i < 4; ++i) {
    REQUIRE(someVectorData[i] == i + 1);
  }

  const auto doubleParams = event.getParameter<std::vector<double>>("SomeVectorData").value();
  REQUIRE(doubleParams.size() == 2);
  REQUIRE(doubleParams[0] == eventNum * 1.1);
  REQUIRE(doubleParams[1] == eventNum * 2.2);

  checkHitCollection(event, eventNum);

  auto& hits = event.get<ExampleHitCollection>("hits");

  auto& hitRefs = event.get<ExampleHitCollection>("hitRefs");
  REQUIRE(hitRefs.size() == hits.size());
  REQUIRE((hits[1] == hitRefs[0] && hits[0] == hitRefs[1]));

  checkClusterCollection(event, hits);

  auto& mcpRefs = event.get<ExampleMCCollection>("mcParticleRefs");
  for (auto ref : mcpRefs) {
    const auto daughters = ref.daughters();
    if (!daughters.empty()) {
      REQUIRE(daughters[0].isAvailable());
    }
    const auto parents = ref.parents();
    if (!parents.empty()) {
      REQUIRE(parents[0].isAvailable());
    }
  }

  checkMCParticleCollection(event, podio::version::build_version);

  auto& mcps = event.get<ExampleMCCollection>("mcparticles");

  for (auto pr : mcpRefs) {
    REQUIRE(static_cast<unsigned>(pr.getObjectID().collectionID) != mcpRefs.getID());
  }

  auto& moreMCs = event.get<ExampleMCCollection>("moreMCs");
  REQUIRE(mcps.size() == moreMCs.size());

  for (size_t index = 0; index < mcps.size(); ++index) {
    REQUIRE(mcps[index].energy() == moreMCs[index].energy());
    REQUIRE(mcps[index].daughters().size() == moreMCs[index].daughters().size());
  }

  REQUIRE(mcpRefs.size() == mcps.size());
  for (size_t i = 0; i < mcpRefs.size(); ++i) {
    if (i < 5) {
      REQUIRE(mcpRefs[i] == mcps[2 * i + 1]);
    } else {
      const int index = (i - 5) * 2;
      REQUIRE(mcpRefs[i] == moreMCs[index]);
    }
  }

  const auto& rels = event.get<ExampleWithOneRelationCollection>("OneRelation");
  REQUIRE(rels.size() == 2);

  auto& vecs = event.get<ExampleWithVectorMemberCollection>("WithVectorMember");
  REQUIRE(vecs.size() == 2);

  for (auto vec : vecs) {
    REQUIRE(vec.count().size() == 2);
  }
  REQUIRE(vecs[0].count(0) == eventNum);
  REQUIRE(vecs[0].count(1) == eventNum + 10);
  REQUIRE(vecs[1].count(0) == eventNum + 1);
  REQUIRE(vecs[1].count(1) == eventNum + 11);

  auto& arrays = event.get<ExampleWithArrayCollection>("arrays");
  REQUIRE(!arrays.empty());
  auto array = arrays[0];
  REQUIRE(array.myArray(1) == eventNum);
  REQUIRE(array.arrayStruct().data.p.at(2) == 2 * eventNum);
  REQUIRE(array.structArray(0).x == eventNum);

  auto& nmspaces = event.get<ex42::ExampleWithARelationCollection>("WithNamespaceRelation");
  auto& copies = event.get<ex42::ExampleWithARelationCollection>("WithNamespaceRelationCopy");

  auto cpytest = ex42::ExampleWithARelationCollection{};
  for (size_t j = 0; j < nmspaces.size(); j++) {
    auto nmsp = nmspaces[j];
    auto cpy = copies[j];
    cpytest.push_back(nmsp.clone());
    if (nmsp.ref().isAvailable()) {
      REQUIRE(nmsp.ref().component().x == cpy.ref().component().x);
      REQUIRE(nmsp.ref().component().y == cpy.ref().component().y);
      REQUIRE(nmsp.ref().x() == cpy.ref().x());
      REQUIRE(nmsp.number() == cpy.number());
      REQUIRE(nmsp.ref().getObjectID() == cpy.ref().getObjectID());
    }
    auto cpy_it = cpy.refs_begin();
    for (auto it = nmsp.refs_begin(); it != nmsp.refs_end(); ++it, ++cpy_it) {
      REQUIRE(it->component().x == cpy_it->component().x);
      REQUIRE(it->component().y == cpy_it->component().y);
      REQUIRE(it->getObjectID() == cpy_it->getObjectID());
    }
  }

  const auto& fixedWidthInts = event.get<ExampleWithFixedWidthIntegersCollection>("fixedWidthInts");
  REQUIRE(fixedWidthInts.size() == 3);

  auto maxValues = fixedWidthInts[0];
  const auto& maxComps = maxValues.fixedWidthStruct();
  REQUIRE(maxValues.fixedI16() == std::numeric_limits<std::int16_t>::max());
  REQUIRE(maxValues.fixedU32() == std::numeric_limits<std::uint32_t>::max());
  REQUIRE(maxValues.fixedU64() == std::numeric_limits<std::uint64_t>::max());
  REQUIRE(maxComps.fixedInteger64 == std::numeric_limits<std::int64_t>::max());
  REQUIRE(maxComps.fixedInteger32 == std::numeric_limits<std::int32_t>::max());
  REQUIRE(maxComps.fixedUnsigned16 == std::numeric_limits<std::uint16_t>::max());

  auto minValues = fixedWidthInts[1];
  const auto& minComps = minValues.fixedWidthStruct();
  REQUIRE(minValues.fixedI16() == std::numeric_limits<std::int16_t>::min());
  REQUIRE(minValues.fixedU32() == std::numeric_limits<std::uint32_t>::min());
  REQUIRE(minValues.fixedU64() == std::numeric_limits<std::uint64_t>::min());
  REQUIRE(minComps.fixedInteger64 == std::numeric_limits<std::int64_t>::min());
  REQUIRE(minComps.fixedInteger32 == std::numeric_limits<std::int32_t>::min());
  REQUIRE(minComps.fixedUnsigned16 == std::numeric_limits<std::uint16_t>::min());

  auto arbValues = fixedWidthInts[2];
  const auto& arbComps = arbValues.fixedWidthStruct();
  REQUIRE(arbValues.fixedI16() == std::int16_t{-12345});
  REQUIRE(arbValues.fixedU32() == std::uint32_t{1234567890});
  REQUIRE(arbValues.fixedU64() == std::uint64_t{1234567890123456789});
  REQUIRE(arbComps.fixedInteger64 == std::int64_t{-1234567890123456789});
  REQUIRE(arbComps.fixedInteger32 == std::int64_t{-1234567890});
  REQUIRE(arbComps.fixedUnsigned16 == std::uint16_t{12345});
}

TEST_CASE("ArrowFrameConverter - Comprehensive Round-Trip (No-UserData)", "[arrow][converter][common]") {
  auto originalFrame = makeFrame(0);

  const std::vector<std::string> colls = {"mcparticles",
                                          "moreMCs",
                                          "arrays",
                                          "mcParticleRefs",
                                          "hits",
                                          "hitRefs",
                                          "refs",
                                          "refs2",
                                          "clusters",
                                          "OneRelation",
                                          "info",
                                          "WithVectorMember",
                                          "VectorMemberSubsetColl",
                                          "fixedWidthInts",
                                          "WithNamespaceMember",
                                          "WithNamespaceRelation",
                                          "WithNamespaceRelationCopy",
                                          "emptyCollection",
                                          "emptySubsetColl",
                                          "extension_Contained",
                                          "extension_ExternalComponent",
                                          "extension_ExternalRelation",
                                          "interface_examples",
                                          "anotherHits",
                                          "extension_interface_relation",
                                          "links",
                                          "links_with_interfaces",
                                          "extension_interface_links"};

  auto table = podio::convertFrameToTable(originalFrame, colls);
  REQUIRE(table != nullptr);

  auto reconstructedFrame = podio::convertTableToFrame(table, 0);
  REQUIRE(reconstructedFrame != nullptr);

  verifyEventNoUserData(*reconstructedFrame, 0);

  processExtensions(*reconstructedFrame, 0, podio::version::build_version);
  checkVecMemSubsetColl(*reconstructedFrame);
  checkInterfaceCollection(*reconstructedFrame);
  checkInterfaceExtension(*reconstructedFrame);

  const auto& hits = reconstructedFrame->get<ExampleHitCollection>("hits");
  const auto& clusters = reconstructedFrame->get<ExampleClusterCollection>("clusters");
  checkLinkCollection(*reconstructedFrame, hits, clusters);

  // Verify Link collection with interfaces
  const auto& interfaceLinks = reconstructedFrame->get<TestInterfaceLinkCollection>("links_with_interfaces");
  REQUIRE(interfaceLinks.size() == 3);
  const auto& mcps = reconstructedFrame->get<ExampleMCCollection>("mcparticles");
  REQUIRE(interfaceLinks[0].get<ExampleCluster>() == clusters[0]);
  REQUIRE(interfaceLinks[0].get<TypeWithEnergy>() == hits[0]);
  REQUIRE(interfaceLinks[1].get<ExampleCluster>() == clusters[1]);
  REQUIRE(interfaceLinks[1].get<TypeWithEnergy>() == mcps[0]);
  REQUIRE(interfaceLinks[2].get<ExampleCluster>() == clusters[0]);
  REQUIRE(interfaceLinks[2].get<TypeWithEnergy>() == clusters[1]);
}

TEST_CASE("ArrowFrameConverter - convertTableToFrame Verification (Multi-Row / rowIndex > 0)",
          "[arrow][converter][reader]") {
  auto frame1 = makeFrame(0);
  auto frame2 = makeFrame(1);

  const std::vector<std::string> colls = {"mcparticles",
                                          "moreMCs",
                                          "arrays",
                                          "mcParticleRefs",
                                          "hits",
                                          "hitRefs",
                                          "refs",
                                          "refs2",
                                          "clusters",
                                          "OneRelation",
                                          "info",
                                          "WithVectorMember",
                                          "VectorMemberSubsetColl",
                                          "fixedWidthInts",
                                          "WithNamespaceMember",
                                          "WithNamespaceRelation",
                                          "WithNamespaceRelationCopy",
                                          "emptyCollection",
                                          "emptySubsetColl",
                                          "extension_Contained",
                                          "extension_ExternalComponent",
                                          "extension_ExternalRelation",
                                          "interface_examples",
                                          "anotherHits",
                                          "extension_interface_relation",
                                          "links",
                                          "links_with_interfaces",
                                          "extension_interface_links"};

  // Convert both frames to Arrow Tables
  auto table1 = podio::convertFrameToTable(frame1, colls);
  REQUIRE(table1 != nullptr);
  auto table2 = podio::convertFrameToTable(frame2, colls);
  REQUIRE(table2 != nullptr);

  // Concatenate tables to simulate a multi-row table (e.g. from multiple events)
  auto concatResult = arrow::ConcatenateTables({table1, table2});
  REQUIRE(concatResult.ok());
  const auto& concatTable = concatResult.ValueOrDie();
  REQUIRE(concatTable->num_rows() == 2);

  auto verifyFrame = [](const podio::Frame& reconstructedFrame, int eventNum) {
    verifyEventNoUserData(reconstructedFrame, eventNum);

    processExtensions(reconstructedFrame, eventNum, podio::version::build_version);
    checkVecMemSubsetColl(reconstructedFrame);
    checkInterfaceCollection(reconstructedFrame);
    checkInterfaceExtension(reconstructedFrame);

    const auto& hits = reconstructedFrame.get<ExampleHitCollection>("hits");
    const auto& clusters = reconstructedFrame.get<ExampleClusterCollection>("clusters");
    checkLinkCollection(reconstructedFrame, hits, clusters);

    // Verify Link collection with interfaces
    const auto& interfaceLinks = reconstructedFrame.get<TestInterfaceLinkCollection>("links_with_interfaces");
    REQUIRE(interfaceLinks.size() == 3);
    const auto& mcps = reconstructedFrame.get<ExampleMCCollection>("mcparticles");
    REQUIRE(interfaceLinks[0].get<ExampleCluster>() == clusters[0]);
    REQUIRE(interfaceLinks[0].get<TypeWithEnergy>() == hits[0]);
    REQUIRE(interfaceLinks[1].get<ExampleCluster>() == clusters[1]);
    REQUIRE(interfaceLinks[1].get<TypeWithEnergy>() == mcps[0]);
    REQUIRE(interfaceLinks[2].get<ExampleCluster>() == clusters[0]);
    REQUIRE(interfaceLinks[2].get<TypeWithEnergy>() == clusters[1]);
  };

  // --- Reconstruct and verify Frame 1 at rowIndex = 0 ---
  auto reconstructedFrame1 = podio::convertTableToFrame(concatTable, 0);
  REQUIRE(reconstructedFrame1 != nullptr);
  verifyFrame(*reconstructedFrame1, 0);

  // --- Reconstruct and verify Frame 2 at rowIndex = 1 ---
  auto reconstructedFrame2 = podio::convertTableToFrame(concatTable, 1);
  REQUIRE(reconstructedFrame2 != nullptr);
  verifyFrame(*reconstructedFrame2, 1);
}

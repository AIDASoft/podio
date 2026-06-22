#include "catch2/catch_test_macros.hpp"

#include "podio/Frame.h"

// Test datatypes
#include "datamodel/EventInfoCollection.h"
#include "datamodel/ExampleClusterCollection.h"
#include "datamodel/ExampleHitCollection.h"
#include "datamodel/ExampleWithOneRelationCollection.h"
#include "datamodel/ExampleWithVectorMemberCollection.h"

// Arrow Frame Converter
#include "podio/utilities/ArrowFrameConverter.h"

// Arrow headers
#include <arrow/api.h>

TEST_CASE("ArrowFrameConverter - convertFrameToTable Verification", "[arrow][converter]") {
  podio::Frame originalFrame;

  // 1. Prepare some hits
  ExampleHitCollection hits;
  auto hit1 = MutableExampleHit(0x42ULL, 1.0f, 2.0f, 3.0f, 10.5);
  auto hit2 = MutableExampleHit(0x43ULL, 4.0f, 5.0f, 6.0f, 20.5);
  hits.push_back(hit1);
  hits.push_back(hit2);

  // 2. Prepare some clusters with relations to hits
  ExampleClusterCollection clusters;
  auto cluster1 = MutableExampleCluster();
  cluster1.energy(100.0);
  cluster1.addHits(hit1);
  cluster1.addHits(hit2);
  clusters.push_back(cluster1);

  // 3. Prepare a relation collection
  ExampleWithOneRelationCollection relColls;
  auto relObj = MutableExampleWithOneRelation();
  relObj.cluster(cluster1);
  relColls.push_back(relObj);

  // 4. Prepare a vector member collection
  ExampleWithVectorMemberCollection vecColls;
  auto vecObj = MutableExampleWithVectorMember();
  vecObj.addcount(42);
  vecObj.addcount(137);
  vecColls.push_back(vecObj);

  // 5. Prepare a subset collection of hits
  ExampleHitCollection subsetHits;
  subsetHits.setSubsetCollection(true);
  subsetHits.push_back(hit1);

  // Put them all into the frame
  originalFrame.put(std::move(hits), "Hits");
  originalFrame.put(std::move(clusters), "Clusters");
  originalFrame.put(std::move(relColls), "OneRelation");
  originalFrame.put(std::move(vecColls), "VectorMember");
  originalFrame.put(std::move(subsetHits), "SubsetHits");

  // Put some frame parameters
  originalFrame.putParameter("anInt", 42);
  originalFrame.putParameter("someFloats", std::vector<float>{1.23f, 2.34f, 3.45f});
  originalFrame.putParameter("someStrings", std::vector<std::string>{"hello", "world"});

  std::vector<std::string> colls = {"Hits", "Clusters", "OneRelation", "VectorMember", "SubsetHits"};

  // Convert to Arrow Table
  auto table = podio::convertFrameToTable(originalFrame, colls);
  REQUIRE(table != nullptr);
  REQUIRE(table->num_rows() == 1);

  // Validate the built Table fully
  auto validateStatus = table->ValidateFull();
  REQUIRE(validateStatus.ok());

  // Check columns present
  auto schema = table->schema();
  REQUIRE(schema->GetFieldByName("Hits") != nullptr);
  REQUIRE(schema->GetFieldByName("Clusters") != nullptr);
  REQUIRE(schema->GetFieldByName("OneRelation") != nullptr);
  REQUIRE(schema->GetFieldByName("VectorMember") != nullptr);
  REQUIRE(schema->GetFieldByName("SubsetHits") != nullptr);
  REQUIRE(schema->GetFieldByName("frame_parameters") != nullptr);

  // Assertions on Hits table structure and metadata
  auto hitsField = schema->GetFieldByName("Hits");
  auto hitsMeta = hitsField->metadata();
  REQUIRE(hitsMeta != nullptr);
  REQUIRE(hitsMeta->Get("value_type").ValueOrDie() == "ExampleHit");
  REQUIRE(hitsMeta->Get("is_subset").ValueOrDie() == "0");

  auto hitsArray = std::static_pointer_cast<arrow::ListArray>(table->GetColumnByName("Hits")->chunk(0));
  REQUIRE(hitsArray != nullptr);
  REQUIRE(hitsArray->length() == 1);
  REQUIRE(hitsArray->value_length(0) == 2);

  auto hitsStruct = std::static_pointer_cast<arrow::StructArray>(hitsArray->values());
  auto hitsX = std::static_pointer_cast<arrow::DoubleArray>(hitsStruct->GetFieldByName("x"));
  REQUIRE(hitsX->Value(0) == 1.0);
  REQUIRE(hitsX->Value(1) == 4.0);

  auto hitsEnergy = std::static_pointer_cast<arrow::DoubleArray>(hitsStruct->GetFieldByName("energy"));
  REQUIRE(hitsEnergy->Value(0) == 10.5);
  REQUIRE(hitsEnergy->Value(1) == 20.5);

  // Assertions on Clusters table structure and metadata
  auto clustersField = schema->GetFieldByName("Clusters");
  auto clustersMeta = clustersField->metadata();
  REQUIRE(clustersMeta->Get("value_type").ValueOrDie() == "ExampleCluster");

  auto clustersArray = std::static_pointer_cast<arrow::ListArray>(table->GetColumnByName("Clusters")->chunk(0));
  REQUIRE(clustersArray->value_length(0) == 1);

  auto clustersStruct = std::static_pointer_cast<arrow::StructArray>(clustersArray->values());
  auto clustersEnergy = std::static_pointer_cast<arrow::DoubleArray>(clustersStruct->GetFieldByName("energy"));
  REQUIRE(clustersEnergy->Value(0) == 100.0);

  auto clustersHits = std::static_pointer_cast<arrow::ListArray>(clustersStruct->GetFieldByName("Hits"));
  REQUIRE(clustersHits->value_length(0) == 2);

  auto clustersHitsStruct = std::static_pointer_cast<arrow::StructArray>(clustersHits->values());
  auto clustersHitsIndex = std::static_pointer_cast<arrow::Int32Array>(clustersHitsStruct->GetFieldByName("index"));
  REQUIRE(clustersHitsIndex->Value(0) == 0); // hit1 index
  REQUIRE(clustersHitsIndex->Value(1) == 1); // hit2 index

  // Assertions on OneRelation
  auto relArray = std::static_pointer_cast<arrow::ListArray>(table->GetColumnByName("OneRelation")->chunk(0));
  REQUIRE(relArray->value_length(0) == 1);

  auto relStruct = std::static_pointer_cast<arrow::StructArray>(relArray->values());
  auto relCluster = std::static_pointer_cast<arrow::StructArray>(relStruct->GetFieldByName("cluster"));
  auto relClusterIndex = std::static_pointer_cast<arrow::Int32Array>(relCluster->GetFieldByName("index"));
  REQUIRE(relClusterIndex->Value(0) == 0); // cluster1 index

  // Assertions on VectorMember
  auto vecArray = std::static_pointer_cast<arrow::ListArray>(table->GetColumnByName("VectorMember")->chunk(0));
  REQUIRE(vecArray->value_length(0) == 1);

  auto vecStruct = std::static_pointer_cast<arrow::StructArray>(vecArray->values());
  auto vecCount = std::static_pointer_cast<arrow::ListArray>(vecStruct->GetFieldByName("count"));
  auto vecCountVal = std::static_pointer_cast<arrow::Int32Array>(vecCount->values());
  REQUIRE(vecCountVal->Value(0) == 42);
  REQUIRE(vecCountVal->Value(1) == 137);

  // Assertions on SubsetHits
  auto subField = schema->GetFieldByName("SubsetHits");
  REQUIRE(subField->metadata()->Get("is_subset").ValueOrDie() == "1");

  auto subArray = std::static_pointer_cast<arrow::ListArray>(table->GetColumnByName("SubsetHits")->chunk(0));
  REQUIRE(subArray->value_length(0) == 1);

  auto subObjectID = std::static_pointer_cast<arrow::StructArray>(subArray->values());
  auto subIndex = std::static_pointer_cast<arrow::Int32Array>(subObjectID->GetFieldByName("index"));
  REQUIRE(subIndex->Value(0) == 0); // pointing to hit1 index

  // Assertions on frame parameters
  auto paramArray = std::static_pointer_cast<arrow::StructArray>(table->GetColumnByName("frame_parameters")->chunk(0));
  REQUIRE(paramArray != nullptr);
  REQUIRE(paramArray->length() == 1);

  // Verify int_params
  auto intParams = std::static_pointer_cast<arrow::MapArray>(paramArray->GetFieldByName("int_params"));
  REQUIRE(intParams != nullptr);
  REQUIRE(intParams->length() == 1);
  auto intKeys = std::static_pointer_cast<arrow::StringArray>(intParams->keys());
  auto intItems = std::static_pointer_cast<arrow::ListArray>(intParams->items());
  REQUIRE(intKeys->GetString(0) == "anInt");
  auto intValues = std::static_pointer_cast<arrow::Int32Array>(intItems->values());
  REQUIRE(intValues->Value(0) == 42);

  // Verify float_params
  auto floatParams = std::static_pointer_cast<arrow::MapArray>(paramArray->GetFieldByName("float_params"));
  REQUIRE(floatParams != nullptr);
  REQUIRE(floatParams->length() == 1);
  auto floatKeys = std::static_pointer_cast<arrow::StringArray>(floatParams->keys());
  auto floatItems = std::static_pointer_cast<arrow::ListArray>(floatParams->items());
  REQUIRE(floatKeys->GetString(0) == "someFloats");
  auto floatValues = std::static_pointer_cast<arrow::FloatArray>(floatItems->values());
  REQUIRE(floatValues->Value(0) == 1.23f);
  REQUIRE(floatValues->Value(1) == 2.34f);
  REQUIRE(floatValues->Value(2) == 3.45f);

  // Verify string_params
  auto stringParams = std::static_pointer_cast<arrow::MapArray>(paramArray->GetFieldByName("string_params"));
  REQUIRE(stringParams != nullptr);
  REQUIRE(stringParams->length() == 1);
  auto stringKeys = std::static_pointer_cast<arrow::StringArray>(stringParams->keys());
  auto stringItems = std::static_pointer_cast<arrow::ListArray>(stringParams->items());
  REQUIRE(stringKeys->GetString(0) == "someStrings");
  auto stringValues = std::static_pointer_cast<arrow::StringArray>(stringItems->values());
  REQUIRE(stringValues->GetString(0) == "hello");
  REQUIRE(stringValues->GetString(1) == "world");
}

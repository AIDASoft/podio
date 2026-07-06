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
#include <arrow/type.h>
#include <iostream>
#include <nlohmann/json.hpp>

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

// ============================================================================
// Arrow Table & Frame Parameters JSON Serialization Helpers
// ============================================================================

namespace {

// wrap_relation:
// In PODIO's native JSON output:
//   - A one-to-one relation is wrapped in a 1-element array: "cluster": [{"collectionID": X, "index": Y}]
//   - A one-to-many relation or subset collection is a simple flat list of elements: "Hits": [{"collectionID": X,
//   "index": Y}, ...]
// Since the Arrow Table structures them both as relation reference structs, we use wrap_relation=false inside list
// loops (like listToJson) to prevent wrapping each individual element of a one-to-many list in its own array.
nlohmann::json arrayElementToJson(const arrow::Array& array, int64_t idx, bool wrap_relation = true);

nlohmann::json structToJson(const arrow::StructArray& struct_array, int64_t idx, bool wrap_relation = true) {
  auto struct_type = struct_array.struct_type();

  bool is_relation = false;
  if (struct_array.num_fields() == 2) {
    std::string f0 = struct_type->field(0)->name();
    std::string f1 = struct_type->field(1)->name();
    if ((f0 == "collectionID" && f1 == "index") || (f0 == "index" && f1 == "collectionID")) {
      is_relation = true;
    }
  }

  if (is_relation) {
    auto ref = nlohmann::json::object();
    for (int f = 0; f < struct_array.num_fields(); ++f) {
      std::string name = struct_type->field(f)->name();
      const auto& field_array = struct_array.field(f);
      ref[name] = arrayElementToJson(*field_array, idx, true);
    }
    if (wrap_relation) {
      return nlohmann::json::array({ref});
    } else {
      return ref;
    }
  }

  auto j = nlohmann::json::object();
  for (int f = 0; f < struct_array.num_fields(); ++f) {
    std::string name = struct_type->field(f)->name();
    const auto& field_array = struct_array.field(f);
    j[name] = arrayElementToJson(*field_array, idx, true);
  }
  return j;
}

nlohmann::json listToJson(const arrow::ListArray& list_array, int64_t idx) {
  auto j = nlohmann::json::array();
  int64_t offset = list_array.value_offset(idx);
  int64_t length = list_array.value_length(idx);
  const auto& values = list_array.values();
  for (int64_t val_i = 0; val_i < length; ++val_i) {
    j.push_back(arrayElementToJson(*values, offset + val_i, false));
  }
  return j;
}

nlohmann::json fixedSizeListToJson(const arrow::FixedSizeListArray& list_array, int64_t idx) {
  auto j = nlohmann::json::array();
  int64_t offset = list_array.value_offset(idx);
  int64_t length = list_array.value_length();
  const auto& values = list_array.values();
  for (int64_t val_i = 0; val_i < length; ++val_i) {
    j.push_back(arrayElementToJson(*values, offset + val_i, false));
  }
  return j;
}

nlohmann::json mapToJson(const arrow::MapArray& map_array, int64_t idx) {
  auto j = nlohmann::json::object();
  int64_t offset = map_array.value_offset(idx);
  int64_t length = map_array.value_length(idx);
  const auto& keys = map_array.keys();
  const auto& items = map_array.items();

  auto string_keys = std::static_pointer_cast<arrow::StringArray>(keys);
  for (int64_t k = 0; k < length; ++k) {
    std::string key = string_keys->GetString(offset + k);
    j[key] = arrayElementToJson(*items, offset + k, true);
  }
  return j;
}

nlohmann::json arrayElementToJson(const arrow::Array& array, int64_t idx, bool wrap_relation) {
  if (array.IsNull(idx)) {
    return nullptr;
  }
  switch (array.type()->id()) {
  case arrow::Type::BOOL:
    return static_cast<const arrow::BooleanArray&>(array).Value(idx);
  case arrow::Type::INT8:
    return static_cast<const arrow::Int8Array&>(array).Value(idx);
  case arrow::Type::UINT8:
    return static_cast<const arrow::UInt8Array&>(array).Value(idx);
  case arrow::Type::INT16:
    return static_cast<const arrow::Int16Array&>(array).Value(idx);
  case arrow::Type::UINT16:
    return static_cast<const arrow::UInt16Array&>(array).Value(idx);
  case arrow::Type::INT32:
    return static_cast<const arrow::Int32Array&>(array).Value(idx);
  case arrow::Type::UINT32:
    return static_cast<const arrow::UInt32Array&>(array).Value(idx);
  case arrow::Type::INT64:
    return static_cast<const arrow::Int64Array&>(array).Value(idx);
  case arrow::Type::UINT64:
    return static_cast<const arrow::UInt64Array&>(array).Value(idx);
  case arrow::Type::FLOAT:
    return static_cast<const arrow::FloatArray&>(array).Value(idx);
  case arrow::Type::DOUBLE:
    return static_cast<const arrow::DoubleArray&>(array).Value(idx);
  case arrow::Type::STRING:
    return static_cast<const arrow::StringArray&>(array).GetString(idx);
  case arrow::Type::STRUCT:
    return structToJson(static_cast<const arrow::StructArray&>(array), idx, wrap_relation);
  case arrow::Type::LIST:
    return listToJson(static_cast<const arrow::ListArray&>(array), idx);
  case arrow::Type::FIXED_SIZE_LIST:
    return fixedSizeListToJson(static_cast<const arrow::FixedSizeListArray&>(array), idx);
  case arrow::Type::MAP:
    return mapToJson(static_cast<const arrow::MapArray&>(array), idx);
  default:
    throw std::runtime_error("Unsupported Arrow type in JSON converter: " + array.type()->ToString());
  }
}

nlohmann::json arrowTableToJson(const std::shared_ptr<arrow::Table>& table) {
  auto j = nlohmann::json::object();
  for (int c = 0; c < table->num_columns(); ++c) {
    std::string name = table->schema()->field(c)->name();
    auto chunked_arr = table->column(c);
    auto chunk = chunked_arr->chunk(0);
    j[name] = arrayElementToJson(*chunk, 0);
  }
  return j;
}

nlohmann::json genericParametersToJson(const podio::GenericParameters& params) {
  auto j = nlohmann::json::object();

  auto int_params = nlohmann::json::object();
  for (const auto& key : params.getKeys<int>()) {
    int_params[key] = params.get<std::vector<int>>(key).value_or(std::vector<int>{});
  }
  j["int_params"] = int_params;

  auto float_params = nlohmann::json::object();
  for (const auto& key : params.getKeys<float>()) {
    float_params[key] = params.get<std::vector<float>>(key).value_or(std::vector<float>{});
  }
  j["float_params"] = float_params;

  auto double_params = nlohmann::json::object();
  for (const auto& key : params.getKeys<double>()) {
    double_params[key] = params.get<std::vector<double>>(key).value_or(std::vector<double>{});
  }
  j["double_params"] = double_params;

  auto string_params = nlohmann::json::object();
  for (const auto& key : params.getKeys<std::string>()) {
    string_params[key] = params.get<std::vector<std::string>>(key).value_or(std::vector<std::string>{});
  }
  j["string_params"] = string_params;

  return j;
}

} // namespace

TEST_CASE("ArrowFrameConverter - JSON Equivalence Check", "[arrow][converter][json]") {
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
  // Add an unset relation reference object
  auto unsetRelObj = MutableExampleWithOneRelation();
  relColls.push_back(unsetRelObj);

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

  // 6. Prepare a collection with fixed-size arrays (FixedSizeList)
  ExampleWithArrayCollection arrays;
  std::array<int, 4> arrayTest = {0, 1, 2, 3};
  std::array<int, 4> arrayTest2 = {4, 5, 6, 7};
  NotSoSimpleStruct a;
  a.data.p = arrayTest2;
  ex2::NamespaceStruct nstruct;
  nstruct.x = 42;
  std::array<ex2::NamespaceStruct, 4> structArrayTest = {nstruct, nstruct, nstruct, nstruct};
  auto arrayObj = MutableExampleWithArray(a, arrayTest, arrayTest, arrayTest, arrayTest, structArrayTest);
  arrays.push_back(arrayObj);

  // 7. Prepare an empty collection
  ExampleClusterCollection emptyClusterColl;

  // 8. Prepare a fixed-width integers collection
  ExampleWithFixedWidthIntegersCollection fixedWidthInts;
  auto fixedWidthObj = fixedWidthInts.create();
  fixedWidthObj.fixedI16(-12345);
  fixedWidthObj.fixedU32(1234567890U);
  fixedWidthObj.fixedU64(1234567890123456789ULL);

  auto& maxComp = fixedWidthObj.fixedWidthStruct();
  maxComp.fixedUnsigned16 = 65535;
  maxComp.fixedInteger64 = 9223372036854775807LL;
  maxComp.fixedInteger32 = 2147483647;

  std::array<int16_t, 2> arrVal = {-10, 20};
  fixedWidthObj.fixedWidthArray(arrVal);

  // Put them all into the frame
  originalFrame.put(std::move(hits), "Hits");
  originalFrame.put(std::move(clusters), "Clusters");
  originalFrame.put(std::move(relColls), "OneRelation");
  originalFrame.put(std::move(vecColls), "VectorMember");
  originalFrame.put(std::move(subsetHits), "SubsetHits");
  originalFrame.put(std::move(arrays), "ExampleWithArray");
  originalFrame.put(std::move(emptyClusterColl), "EmptyCluster");
  originalFrame.put(std::move(fixedWidthInts), "FixedWidthInts");

  // Put some frame parameters
  originalFrame.putParameter("anInt", 42);
  originalFrame.putParameter("someFloats", std::vector<float>{1.23f, 2.34f, 3.45f});
  originalFrame.putParameter("someStrings", std::vector<std::string>{"hello", "world"});

  std::vector<std::string> colls = {"Hits",       "Clusters",         "OneRelation",  "VectorMember",
                                    "SubsetHits", "ExampleWithArray", "EmptyCluster", "FixedWidthInts"};

  // Convert to Arrow Table
  auto table = podio::convertFrameToTable(originalFrame, colls);
  REQUIRE(table != nullptr);

  // Convert Arrow Table to JSON
  nlohmann::json arrowJson = arrowTableToJson(table);

  // Convert Frame to JSON directly
  nlohmann::json frameJson = nlohmann::json::object();
  frameJson["Hits"] = originalFrame.get<ExampleHitCollection>("Hits");
  frameJson["Clusters"] = originalFrame.get<ExampleClusterCollection>("Clusters");
  frameJson["OneRelation"] = originalFrame.get<ExampleWithOneRelationCollection>("OneRelation");
  frameJson["VectorMember"] = originalFrame.get<ExampleWithVectorMemberCollection>("VectorMember");
  frameJson["SubsetHits"] = originalFrame.get<ExampleHitCollection>("SubsetHits");
  frameJson["ExampleWithArray"] = originalFrame.get<ExampleWithArrayCollection>("ExampleWithArray");
  frameJson["EmptyCluster"] = originalFrame.get<ExampleClusterCollection>("EmptyCluster");
  frameJson["FixedWidthInts"] = originalFrame.get<ExampleWithFixedWidthIntegersCollection>("FixedWidthInts");
  frameJson["frame_parameters"] = genericParametersToJson(originalFrame.getParameters());

  // Compare the JSON representations
  REQUIRE(arrowJson == frameJson);
}

TEST_CASE("ArrowFrameConverter - convertTableToFrame Verification (Round-Trip)", "[arrow][converter][reader]") {
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

  // Reconstruct the Frame from the Table
  auto reconstructedFrame = podio::convertTableToFrame(table, 0);
  REQUIRE(reconstructedFrame != nullptr);

  // Verify Hits collection
  const auto& recHits = reconstructedFrame->get<ExampleHitCollection>("Hits");
  REQUIRE(recHits.size() == 2);
  REQUIRE(recHits[0].x() == 1.0f);
  REQUIRE(recHits[1].x() == 4.0f);
  REQUIRE(recHits[0].energy() == 10.5);

  // Verify Clusters collection and its hit relations
  const auto& recClusters = reconstructedFrame->get<ExampleClusterCollection>("Clusters");
  REQUIRE(recClusters.size() == 1);
  REQUIRE(recClusters[0].energy() == 100.0);
  REQUIRE(recClusters[0].Hits_size() == 2);
  REQUIRE(recClusters[0].Hits(0).x() == 1.0f);
  REQUIRE(recClusters[0].Hits(1).x() == 4.0f);

  // Verify OneRelation
  const auto& recRelColls = reconstructedFrame->get<ExampleWithOneRelationCollection>("OneRelation");
  REQUIRE(recRelColls.size() == 1);
  REQUIRE(recRelColls[0].cluster().energy() == 100.0);

  // Verify VectorMember
  const auto& recVecColls = reconstructedFrame->get<ExampleWithVectorMemberCollection>("VectorMember");
  REQUIRE(recVecColls.size() == 1);
  REQUIRE(recVecColls[0].count_size() == 2);
  REQUIRE(recVecColls[0].count(0) == 42);
  REQUIRE(recVecColls[0].count(1) == 137);

  // Verify SubsetHits
  const auto& recSubsetHits = reconstructedFrame->get<ExampleHitCollection>("SubsetHits");
  REQUIRE(recSubsetHits.isSubsetCollection());
  REQUIRE(recSubsetHits.size() == 1);
  REQUIRE(recSubsetHits[0].x() == 1.0f);

  // Verify Frame parameters
  REQUIRE(reconstructedFrame->getParameter<int>("anInt") == 42);
  auto recFloats = reconstructedFrame->getParameter<std::vector<float>>("someFloats");
  REQUIRE(recFloats.has_value());
  REQUIRE(recFloats->size() == 3);
  REQUIRE((*recFloats)[0] == 1.23f);
  auto recStrings = reconstructedFrame->getParameter<std::vector<std::string>>("someStrings");
  REQUIRE(recStrings.has_value());
  REQUIRE(recStrings->size() == 2);
  REQUIRE((*recStrings)[0] == "hello");
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
      auto d [[maybe_unused]] = daughters[0];
    }
    const auto parents = ref.parents();
    if (!parents.empty()) {
      auto d [[maybe_unused]] = parents[0];
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

  auto& refs = event.get<ExampleReferencingTypeCollection>("refs");
  auto ref = refs[0];
  for (auto cluster : ref.Clusters()) {
    for (auto hit [[maybe_unused]] : cluster.Hits()) {
    }
  }
  auto& rels [[maybe_unused]] = event.get<ExampleWithOneRelationCollection>("OneRelation");

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

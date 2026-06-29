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

  std::cout << "--- ARROW TABLE JSON ---" << std::endl;
  std::cout << arrowJson.dump(4) << std::endl;
  std::cout << "--- FRAME DIRECT JSON ---" << std::endl;
  std::cout << frameJson.dump(4) << std::endl;

  // Compare the JSON representations
  REQUIRE(arrowJson == frameJson);
}

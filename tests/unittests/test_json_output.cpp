// Tests for JSON serialization of generated datamodel types.

#include <podio/ObjectID.h>

#include "datamodel/ExampleClusterCollection.h"
#include "datamodel/ExampleHitCollection.h"
#include "datamodel/ExampleMCCollection.h"
#include "datamodel/ExampleWithOneRelationCollection.h"
#include "datamodel/ExampleWithVectorMemberCollection.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <nlohmann/json.hpp>

#include <limits>
#include <set>
#include <vector>

template <typename T>
nlohmann::json toJson(const T& obj) {
  return nlohmann::json(obj);
}

// ============================================================
// 1. POD members
// ============================================================

TEST_CASE("JSON output - plain POD members", "[json]") {
  auto hit = MutableExampleHit{};
  hit.energy(3.14);
  hit.x(1.0);
  hit.y(2.0);
  hit.z(3.0);
  hit.cellID(42);

  const auto j = toJson(hit);

  REQUIRE(j.is_object());
  REQUIRE(j.contains("energy"));
  REQUIRE(j["energy"] == 3.14);

  REQUIRE(j.contains("x"));
  REQUIRE(j["x"] == 1.0);

  REQUIRE(j.contains("y"));
  REQUIRE(j["y"] == 2.0);

  REQUIRE(j.contains("z"));
  REQUIRE(j["z"] == 3.0);

  REQUIRE(j.contains("cellID"));
  REQUIRE(j["cellID"] == 42);

  auto mc = MutableExampleMC{};
  mc.PDG(211);
  mc.energy(1.0);

  const auto mcJson = toJson(mc);

  REQUIRE(mcJson.is_object());
  REQUIRE(mcJson["PDG"] == 211);
  REQUIRE(mcJson["energy"] == 1.0);
}

TEST_CASE("JSON output - default object keeps keys", "[json]") {
  auto hit = MutableExampleHit{};

  const auto j = toJson(hit);

  REQUIRE(j.is_object());

  for (const auto& key : {"cellID", "energy", "x", "y", "z"}) {
    REQUIRE(j.contains(key));
  }
}

// ============================================================
// 2. Vector members
// ============================================================

TEST_CASE("JSON output - VectorMembers serialize as arrays", "[json]") {
  using Catch::Matchers::Equals;
  auto obj = MutableExampleWithVectorMember{};
  obj.addcount(10);
  obj.addcount(20);
  obj.addcount(30);

  const auto j = toJson(obj);

  REQUIRE(j.contains("count"));
  REQUIRE(j["count"].is_array());
  REQUIRE_THAT(j["count"].get<std::vector<int>>(), Equals(std::vector{10, 20, 30}));
}

TEST_CASE("JSON output - empty VectorMember becomes empty array", "[json]") {
  auto obj = MutableExampleWithVectorMember{};

  const auto j = toJson(obj);

  REQUIRE(j.contains("count"));
  REQUIRE(j["count"].is_array());
  REQUIRE(j["count"].empty());
}

TEST_CASE("JSON output - integer VectorMembers keep numeric type", "[json]") {
  using Catch::Matchers::Equals;
  auto obj = MutableExampleWithVectorMember{};
  obj.addcount(11);
  obj.addcount(22);
  obj.addcount(33);
  obj.addcount(44);

  const auto j = toJson(obj);

  REQUIRE(j["count"].is_array());
  REQUIRE_THAT(j["count"].get<std::vector<int>>(), Equals(std::vector{11, 22, 33, 44}));
}

// ============================================================
// 3. OneToOne relations
// ============================================================

TEST_CASE("JSON output - OneToOneRelation serializes", "[json]") {
  ExampleClusterCollection clusters{};
  ExampleWithOneRelationCollection recos{};

  auto cluster = clusters.create();
  cluster.energy(5.0);

  auto reco = recos.create();
  reco.cluster(cluster);

  const auto j = toJson(reco);

  REQUIRE(j.contains("cluster"));
  auto clusterRef = j["cluster"];
  if (clusterRef.is_array() && clusterRef.size() == 1) {
    clusterRef = clusterRef[0];
  }
  REQUIRE(clusterRef.is_object());
  REQUIRE(clusterRef.contains("collectionID"));
  REQUIRE(clusterRef.contains("index"));
  REQUIRE(clusterRef["index"] == 0);
}

TEST_CASE("JSON output - unset OneToOneRelation", "[json]") {
  auto reco = MutableExampleWithOneRelation{};

  const auto j = toJson(reco);

  REQUIRE(j.contains("cluster"));
  auto clusterRef = j["cluster"];
  if (clusterRef.is_array() && clusterRef.size() == 1) {
    clusterRef = clusterRef[0];
  }

  const bool isNull = clusterRef.is_null();
  const bool isSentinel = clusterRef.contains("collectionID") &&
      clusterRef["collectionID"] == static_cast<uint32_t>(podio::ObjectID::untracked);

  REQUIRE((isNull || isSentinel));
}

// ============================================================
// 4. OneToMany relations
// ============================================================

TEST_CASE("JSON output - OneToManyRelation serializes as array", "[json]") {
  auto h0 = MutableExampleHit{};
  auto h1 = MutableExampleHit{};
  auto h2 = MutableExampleHit{};
  auto cluster = MutableExampleCluster{};
  cluster.addHits(h0);
  cluster.addHits(h1);
  cluster.addHits(h2);

  const auto j = toJson(cluster);

  REQUIRE(j.contains("Hits"));
  REQUIRE(j["Hits"].is_array());
  REQUIRE(j["Hits"].size() == 3);
}

TEST_CASE("JSON output - empty OneToManyRelation", "[json]") {
  auto cluster = MutableExampleCluster{};

  const auto j = toJson(cluster);

  REQUIRE(j.contains("Hits"));
  REQUIRE(j["Hits"].is_array());
  REQUIRE(j["Hits"].empty());
}

// ============================================================
// 5. Collection serialization
// ============================================================

TEST_CASE("JSON output - collection serializes to array", "[json]") {
  ExampleHitCollection hits{};

  for (int i = 0; i < 5; ++i) {
    auto h = hits.create();
    h.energy(static_cast<float>(i));
  }

  const auto jCol = toJson(hits);

  REQUIRE(jCol.is_array());
  REQUIRE(jCol.size() == 5);
  for (size_t i = 0; i < jCol.size(); ++i) {
    REQUIRE(jCol[i]["energy"] == static_cast<double>(i));
  }
}

TEST_CASE("JSON output - empty collection", "[json]") {
  ExampleHitCollection hits{};

  const auto jCol = toJson(hits);

  REQUIRE(jCol.is_array());
  REQUIRE(jCol.empty());
}

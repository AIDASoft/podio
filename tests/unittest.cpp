// STL
#include <map>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include "catch2/catch_test_macros.hpp"

// podio specific includes
#include "datamodel/ExampleWithVectorMemberCollection.h"
#include "podio/EventStore.h"
#include "podio/podioVersion.h"

// Test data types
#include "datamodel/EventInfoCollection.h"
#include "datamodel/ExampleClusterCollection.h"
#include "datamodel/ExampleForCyclicDependency1Collection.h"
#include "datamodel/ExampleForCyclicDependency2Collection.h"
#include "datamodel/ExampleHitCollection.h"
#include "datamodel/ExampleWithOneRelationCollection.h"
#include "datamodel/MutableExampleWithComponent.h"
#include "podio/UserDataCollection.h"

TEST_CASE("AutoDelete", "[basics][memory-management]") {
  auto store = podio::EventStore();
  auto hit1 = MutableEventInfo();
  auto hit2 = MutableEventInfo();
  auto hit3 = MutableEventInfo();
  auto& coll = store.create<EventInfoCollection>("info");
  coll.push_back(hit1);
  hit3 = hit2;
}

TEST_CASE("Basics", "[basics][memory-management]") {
  auto store = podio::EventStore();
  // Adding
  auto& collection = store.create<ExampleHitCollection>("name");
  auto hit1 = collection.create(0xcaffeeULL, 0., 0., 0., 0.); // initialize w/ value
  auto hit2 = collection.create();                            // default initialize
  hit2.energy(12.5);
  // Retrieving
  const ExampleHitCollection* coll2(nullptr);
  REQUIRE(store.get("name", coll2));
  const ExampleHitCollection* coll3(nullptr);
  REQUIRE_FALSE(store.get("wrongName", coll3));
}

TEST_CASE("Assignment-operator ref count", "[basics][memory-management]") {
  // Make sure that the assignment operator handles the reference count
  // correctly. (Will trigger in an ASan build if it is not the case)
  // See https://github.com/AIDASoft/podio/issues/200
  std::map<int, ExampleHit> hitMap;
  for (int i = 0; i < 10; ++i) {
    auto hit = ExampleHit(0x42ULL, i, i, i, i);
    hitMap[i] = hit;
  }

  // hits should still be valid here
  for (int i = 0; i < 10; ++i) {
    const auto hit = hitMap[i];
    REQUIRE(hit.energy() == i);
  }
}

TEST_CASE("Clearing", "[ASAN-FAIL][THREAD-FAIL][basics][memory-management]") {
  bool success = true;
  auto store = podio::EventStore();
  auto& hits = store.create<ExampleHitCollection>("hits");
  auto& clusters = store.create<ExampleClusterCollection>("clusters");
  auto& oneRels = store.create<ExampleWithOneRelationCollection>("OneRelation");
  auto nevents = unsigned(1000);
  for (unsigned i = 0; i < nevents; ++i) {
    hits.clear();
    clusters.clear();
    auto hit1 = hits.create();
    auto hit2 = ExampleHit();
    hit1.energy(double(i));
    auto cluster = clusters.create();
    cluster.addHits(hit1);
    cluster.addHits(hit2);
    hits.push_back(hit2);
    auto oneRel = MutableExampleWithOneRelation();
    oneRel.cluster(cluster);
    oneRel.cluster(cluster);
    oneRels.push_back(oneRel);
  }
  hits.clear();
  if (hits.size() != 0)
    success = false;
  REQUIRE(success);
}

TEST_CASE("Cloning", "[basics][memory-management]") {
  bool success = true;
  auto hit = MutableExampleHit();
  hit.energy(30);
  auto hit2 = hit.clone();
  hit2.energy(20);
  if (hit.energy() == hit2.energy())
    success = false;
  auto cluster = MutableExampleCluster();
  cluster.addHits(hit);
  auto cluster2 = cluster.clone();
  cluster.addHits(hit2);
  // check that the clone of a const object is mutable
  auto const_hit = ExampleHit();
  auto const_hit_clone = const_hit.clone();
  const_hit_clone.energy(30);
  REQUIRE(success);
}

TEST_CASE("Component", "[basics]") {
  auto info = MutableExampleWithComponent();
  info.component().data.x = 3;
  REQUIRE(3 == info.component().data.x);
}

TEST_CASE("Cyclic", "[LEAK-FAIL][basics][relations][memory-management]") {
  auto start = MutableExampleForCyclicDependency1();
  auto isAvailable = start.ref().isAvailable();
  REQUIRE_FALSE(isAvailable);
  auto end = MutableExampleForCyclicDependency2();
  start.ref(end);
  isAvailable = start.ref().isAvailable();
  REQUIRE(isAvailable);
  end.ref(start);
  REQUIRE(start == end.ref());
  auto end_eq = start.ref();
  auto start_eq = end_eq.ref();
  REQUIRE(start == start_eq);
  REQUIRE(start == start.ref().ref());
}

TEST_CASE("Invalid_refs", "[LEAK-FAIL][basics][relations]") {
  auto store = podio::EventStore();
  auto& hits = store.create<ExampleHitCollection>("hits");
  auto hit1 = hits.create(0xcaffeeULL, 0., 0., 0., 0.);
  auto hit2 = ExampleHit();
  auto& clusters = store.create<ExampleClusterCollection>("clusters");
  auto cluster = clusters.create();
  cluster.addHits(hit1);
  cluster.addHits(hit2);
  REQUIRE_THROWS_AS(clusters.prepareForWrite(), std::runtime_error);
}

TEST_CASE("Looping", "[basics]") {
  auto store = podio::EventStore();
  auto& coll = store.create<ExampleHitCollection>("name");
  auto hit1 = coll.create(0xbadULL, 0., 0., 0., 0.);
  auto hit2 = coll.create(0xcaffeeULL, 1., 1., 1., 1.);
  for (auto i = coll.begin(), end = coll.end(); i != end; ++i) {
    i->energy(42); // make sure that we can indeed change the energy here for
                   // non-const collections
  }
  REQUIRE(hit1.energy() == 42);
  REQUIRE(hit2.energy() == 42);

  for (int i = 0, end = coll.size(); i != end; ++i) {
    coll[i].energy(i); // reset it back to the original value
  }

  REQUIRE(coll[0].energy() == 0);
  REQUIRE(coll[1].energy() == 1);

  auto& constColl = store.get<ExampleHitCollection>("name");
  int index = 0;
  for (auto hit : constColl) {
    auto energy = hit.energy();
    REQUIRE(energy == index++);
  }
}

TEST_CASE("Notebook", "[basics]") {
  bool success = true;
  auto store = podio::EventStore();
  auto& hits = store.create<ExampleHitCollection>("hits");
  for (unsigned i = 0; i < 12; ++i) {
    auto hit = hits.create(0xcaffeeULL, 0., 0., 0., double(i));
  }
  auto energies = hits.energy<10>();
  int index = 0;
  for (auto energy : energies) {
    if (double(index) != energy)
      success = false;
    ++index;
  }
  REQUIRE(success);
}

TEST_CASE("OneToOneRelations", "[basics][relations]") {
  bool success = true;
  auto cluster = ExampleCluster();
  auto rel = MutableExampleWithOneRelation();
  rel.cluster(cluster);
  REQUIRE(success);
}

TEST_CASE("Podness", "[basics][code-gen]") {
  // fail this already at compile time
  STATIC_REQUIRE(std::is_standard_layout_v<ExampleClusterData>); // Generated data classes do not have standard layout
  STATIC_REQUIRE(std::is_trivially_copyable_v<ExampleClusterData>); // Generated data classes are not trivially copyable
  STATIC_REQUIRE(std::is_standard_layout_v<ExampleHitData>);    // Generated data classes do not have standard layout
  STATIC_REQUIRE(std::is_trivially_copyable_v<ExampleHitData>); // Generated data classes are not trivially copyable
  STATIC_REQUIRE(std::is_standard_layout_v<ExampleWithOneRelationData>); // Generated data classes do not have standard
                                                                         // layout
  STATIC_REQUIRE(std::is_trivially_copyable_v<ExampleWithOneRelationData>); // Generated data classes are not trivially
                                                                            // copyable
}

TEST_CASE("Referencing", "[basics][relations]") {
  auto store = podio::EventStore();
  auto& hits = store.create<ExampleHitCollection>("hits");
  auto hit1 = hits.create(0x42ULL, 0., 0., 0., 0.);
  auto hit2 = hits.create(0x42ULL, 1., 1., 1., 1.);
  auto& clusters = store.create<ExampleClusterCollection>("clusters");
  auto cluster = clusters.create();
  cluster.addHits(hit1);
  cluster.addHits(hit2);
  int index = 0;
  for (auto i = cluster.Hits_begin(), end = cluster.Hits_end(); i != end; ++i) {
    REQUIRE(i->energy() == index);
    ++index;
  }
}

TEST_CASE("VariadicCreate", "[basics]") {
  // Test that objects created via the variadic create template function handle relations correctly
  auto store = podio::EventStore();
  auto& clusters = store.create<ExampleClusterCollection>("clusters");

  auto variadic_cluster = clusters.create(3.14f);
  auto normal_cluster = clusters.create();
  normal_cluster.energy(42);

  variadic_cluster.addClusters(normal_cluster);
  REQUIRE(variadic_cluster.Clusters_size() == 1);
  REQUIRE(variadic_cluster.Clusters(0) == normal_cluster);
}

TEST_CASE("write_buffer", "[basics][io]") {
  auto store = podio::EventStore();
  auto& coll = store.create<ExampleHitCollection>("data");
  auto hit1 = coll.create(0x42ULL, 0., 0., 0., 0.);
  auto hit2 = coll.create(0x42ULL, 1., 1., 1., 1.);
  auto& clusters = store.create<ExampleClusterCollection>("clusters");
  auto cluster = clusters.create();
  // add a few related objects to also exercise relation writing
  cluster.addHits(hit1);
  cluster.addHits(hit2);

  REQUIRE_NOTHROW(clusters.prepareForWrite());
  auto buffers = clusters.getBuffers();
  REQUIRE(buffers.dataAsVector<ExampleClusterData>()->size() == clusters.size());

  // a second call should not crash the whole thing and leave everything untouched
  REQUIRE_NOTHROW(clusters.prepareForWrite());
  REQUIRE(clusters.getBuffers().data == buffers.data);

  auto& ref_coll = store.create<ExampleWithOneRelationCollection>("onerel");
  auto withRef = ref_coll.create();
  REQUIRE_NOTHROW(ref_coll.prepareForWrite());
}

/*
TEST_CASE("Arrays") {
  auto obj = ExampleWithArray();
  obj.array({1,2,3});
  REQUIRE( obj.array()[0] == 1);
}
*/

TEST_CASE("Extracode", "[basics][code-gen]") {
  auto ev = MutableEventInfo();
  ev.setNumber(42);
  REQUIRE(ev.getNumber() == 42);

  int ia[3] = {1, 2, 3};
  auto simple = SimpleStruct(ia);
  REQUIRE(simple.x == 1);
  REQUIRE(simple.y == 2);
  REQUIRE(simple.z == 3);
}

TEST_CASE("AssociativeContainer", "[basics]") {
  auto clu1 = ExampleCluster();
  auto clu2 = ExampleCluster();
  auto clu3 = ExampleCluster();
  auto clu4 = ExampleCluster();
  auto clu5 = ExampleCluster();

  std::set<ExampleCluster> cSet;
  cSet.insert(clu1);
  cSet.insert(clu2);
  cSet.insert(clu3);
  cSet.insert(clu4);
  cSet.insert(clu5);
  cSet.insert(clu1);
  cSet.insert(clu2);
  cSet.insert(clu3);
  cSet.insert(clu4);
  cSet.insert(clu5);

  REQUIRE(cSet.size() == 5);

  std::map<ExampleCluster, int> cMap;
  cMap[clu1] = 1;
  cMap[clu2] = 2;
  cMap[clu3] = 3;
  cMap[clu4] = 4;
  cMap[clu5] = 5;

  REQUIRE(cMap[clu3] == 3);

  cMap[clu3] = 42;

  REQUIRE(cMap[clu3] == 42);
}

TEST_CASE("Equality", "[basics]") {
  auto cluster = ExampleCluster();
  auto rel = MutableExampleWithOneRelation();
  rel.cluster(cluster);
  auto returned_cluster = rel.cluster();
  REQUIRE(cluster == returned_cluster);
  REQUIRE(returned_cluster == cluster);
}

TEST_CASE("NonPresentCollection", "[basics][event-store]") {
  auto store = podio::EventStore();
  REQUIRE_THROWS_AS(store.get<ExampleHitCollection>("NonPresentCollection"), std::runtime_error);
}

TEST_CASE("const correct indexed access to const collections", "[const-correctness]") {
  STATIC_REQUIRE(std::is_same_v<decltype(std::declval<const ExampleClusterCollection>()[0]),
                                ExampleCluster>); // const collections should only have indexed access to mutable
                                                  // objects
  STATIC_REQUIRE(std::is_same_v<decltype(std::declval<const ExampleClusterCollection>().at(0)),
                                ExampleCluster>); // const collections should only have indexed access to mutable
                                                  // objects
}

TEST_CASE("const correct indexed access to collections", "[const-correctness]") {
  auto store = podio::EventStore();
  auto& collection = store.create<ExampleHitCollection>("irrelevant name");

  STATIC_REQUIRE(std::is_same_v<decltype(collection), ExampleHitCollection&>); // collection created by store should not
                                                                               // be const

  STATIC_REQUIRE(std::is_same_v<decltype(collection[0]), MutableExampleHit>); // non-const collections should have
                                                                              // indexed access to mutable objects

  STATIC_REQUIRE(std::is_same_v<decltype(std::declval<ExampleClusterCollection>()[0]),
                                MutableExampleCluster>); // collections should have indexed access to mutable objects

  STATIC_REQUIRE(std::is_same_v<decltype(std::declval<ExampleClusterCollection>().at(0)),
                                MutableExampleCluster>); // collections should have indexed access to mutable objects
}

TEST_CASE("const correct iterators on const collections", "[const-correctness]") {
  const auto collection = ExampleHitCollection();
  // this essentially checks the whole "chain" from begin() / end() through
  // iterator operators
  for (auto hit : collection) {
    STATIC_REQUIRE(std::is_same_v<decltype(hit), ExampleHit>); // const collection iterators should only return
                                                               // immutable objects
  }

  // but we can exercise it in a detailed fashion as well to make it easier to
  // spot where things fail, should they fail
  STATIC_REQUIRE(std::is_same_v<decltype(std::declval<const ExampleHitCollection>().begin()),
                                ExampleHitCollectionIterator>); // const collection begin() should return a
                                                                // CollectionIterator

  STATIC_REQUIRE(std::is_same_v<decltype(std::declval<const ExampleHitCollection>().end()),
                                ExampleHitCollectionIterator>); // const collection end() should return a
                                                                // CollectionIterator

  STATIC_REQUIRE(std::is_same_v<decltype(*std::declval<const ExampleHitCollection>().begin()),
                                ExampleHit>); // CollectionIterator should only give access to immutable objects

  STATIC_REQUIRE(std::is_same_v<decltype(std::declval<ExampleHitCollectionIterator>().operator->()),
                                ExampleHit*>); // CollectionIterator should only give access to immutable objects
}

TEST_CASE("const correct iterators on collections", "[const-correctness]") {
  auto collection = ExampleClusterCollection();
  for (auto cluster : collection) {
    STATIC_REQUIRE(std::is_same_v<decltype(cluster), MutableExampleCluster>); // collection iterators should return
                                                                              // mutable objects
    cluster.energy(42);                                                       // this will necessarily also compile
  }

  // check the individual steps again from above, to see where things fail if they fail
  STATIC_REQUIRE(std::is_same_v<decltype(std::declval<ExampleClusterCollection>().end()),
                                ExampleClusterMutableCollectionIterator>); // non const collection end() should return a
                                                                           // MutableCollectionIterator

  STATIC_REQUIRE(std::is_same_v<decltype(std::declval<ExampleClusterCollection>().end()),
                                ExampleClusterMutableCollectionIterator>); // non const collection end() should return a
                                                                           // MutableCollectionIterator

  STATIC_REQUIRE(std::is_same_v<decltype(std::declval<ExampleClusterCollection>().end()),
                                ExampleClusterMutableCollectionIterator>); // collection end() should return a
                                                                           // MutableCollectionIterator

  STATIC_REQUIRE(std::is_same_v<decltype(*std::declval<ExampleClusterCollection>().begin()),
                                MutableExampleCluster>); // MutableCollectionIterator should give access to mutable
                                                         // objects

  STATIC_REQUIRE(std::is_same_v<decltype(std::declval<ExampleClusterMutableCollectionIterator>().operator->()),
                                MutableExampleCluster*>); // CollectionIterator should only give access to mutable
                                                          // objects
}

TEST_CASE("Subset collection basics", "[subset-colls]") {
  auto clusterRefs = ExampleClusterCollection();
  clusterRefs.setSubsetCollection();

  // The following will always be true
  REQUIRE(clusterRefs.isSubsetCollection());
  const auto refCollBuffers = clusterRefs.getBuffers();
  REQUIRE(refCollBuffers.data == nullptr);
  REQUIRE(refCollBuffers.vectorMembers->empty());
  REQUIRE(refCollBuffers.references->size() == 1u);
}

TEST_CASE("Subset collection can handle subsets", "[subset-colls]") {
  // Can only collect things that already live in a different colection
  auto clusters = ExampleClusterCollection();
  auto cluster = clusters.create();

  auto clusterRefs = ExampleClusterCollection();
  clusterRefs.setSubsetCollection();
  clusterRefs.push_back(cluster);

  auto clusterRef = clusterRefs[0];
  STATIC_REQUIRE(std::is_same_v<decltype(clusterRef), decltype(cluster)>); // Elements that can be obtained from a
                                                                           // collection and a subset collection should
                                                                           // have the same type

  REQUIRE(clusterRef == cluster);

  // These are "true" subsets, so changes should propagate
  cluster.energy(42);
  REQUIRE(clusterRef.energy() == 42);
  // Also in the other directon
  clusterRef.energy(-42);
  REQUIRE(cluster.energy() == -42);
}

TEST_CASE("Collection iterators work with subset collections", "[LEAK-FAIL][subset-colls]") {
  auto hits = ExampleHitCollection();
  auto hit1 = hits.create(0x42ULL, 0., 0., 0., 0.);
  auto hit2 = hits.create(0x42ULL, 1., 1., 1., 1.);

  auto hitRefs = ExampleHitCollection();
  hitRefs.setSubsetCollection();
  for (const auto h : hits)
    hitRefs.push_back(h);

  // index-based looping / access
  for (size_t i = 0; i < hitRefs.size(); ++i) {
    REQUIRE(hitRefs[i].energy() == i);
  }

  // range-based for loop
  int index = 0;
  for (const auto h : hitRefs) {
    REQUIRE(h.energy() == index++);
  }
}

TEST_CASE("Canont convert a normal collection into a subset collection", "[subset-colls]") {
  auto clusterRefs = ExampleClusterCollection();
  auto cluster = clusterRefs.create();

  REQUIRE_THROWS_AS(clusterRefs.setSubsetCollection(), std::logic_error);
}

TEST_CASE("Cannot convert a subset collection into a normal collection", "[subset-colls]") {
  auto clusterRefs = ExampleClusterCollection();
  clusterRefs.setSubsetCollection();

  auto clusters = ExampleClusterCollection();
  auto cluster = clusters.create();
  clusterRefs.push_back(cluster);

  REQUIRE_THROWS_AS(clusterRefs.setSubsetCollection(false), std::logic_error);
}

TEST_CASE("Subset collection only handles tracked objects", "[subset-colls]") {
  auto clusterRefs = ExampleClusterCollection();
  clusterRefs.setSubsetCollection();
  auto cluster = ExampleCluster();

  REQUIRE_THROWS_AS(clusterRefs.push_back(cluster), std::invalid_argument);
  REQUIRE_THROWS_AS(clusterRefs.create(), std::logic_error);
}

TEST_CASE("Move-only collections", "[collections][move-semantics]") {
  // Setup a few collections that will be used throughout below
  auto hitColl = ExampleHitCollection();
  auto clusterColl = ExampleClusterCollection();
  auto vecMemColl = ExampleWithVectorMemberCollection();
  auto userDataColl = podio::UserDataCollection<float>();

  constexpr auto nElements = 3u;
  for (auto i = 0u; i < nElements; ++i) {
    auto hit = hitColl.create();
    auto cluster = clusterColl.create();
    // create a few relations as well
    cluster.addHits(hit);

    auto vecMem = vecMemColl.create();
    vecMem.addcount(i);
    vecMem.addcount(2 * i);

    userDataColl.push_back(3.14f * i);
  }

  // Define a quick check function here for checking collections below
  const auto checkCollections = [nElements](const ExampleHitCollection& hits, const ExampleClusterCollection& clusters,
                                            const ExampleWithVectorMemberCollection& vectors,
                                            const podio::UserDataCollection<float>& userData) {
    // Basics
    REQUIRE(hits.size() == nElements);
    REQUIRE(clusters.size() == nElements);
    REQUIRE(vectors.size() == nElements);
    REQUIRE(userData.size() == nElements);

    int i = 0;
    for (auto cluster : clusters) {
      REQUIRE(cluster.Hits(0) == hits[i++]);
    }

    i = 0;
    for (const auto vec : vectors) {
      const auto counts = vec.count();
      REQUIRE(counts.size() == 2);
      REQUIRE(counts[0] == i);
      REQUIRE(counts[1] == i * 2);
      i++;
    }

    i = 0;
    for (const auto v : userData) {
      REQUIRE(v == 3.14f * i++);
    }
  };

  // Hopefully redundant check for setup
  checkCollections(hitColl, clusterColl, vecMemColl, userDataColl);

  SECTION("Move constructor") {
    // Move-construct collections and make sure the size is as expected
    auto newHits = std::move(hitColl);
    auto newClusters = std::move(clusterColl);
    auto newVecMems = std::move(vecMemColl);
    auto newUserData = std::move(userDataColl);

    checkCollections(newHits, newClusters, newVecMems, newUserData);
  }

  SECTION("Move assignment") {
    // Move assign collections and make sure everything is as expected
    auto newHits = ExampleHitCollection();
    newHits = std::move(hitColl);

    auto newClusters = ExampleClusterCollection();
    newClusters = std::move(clusterColl);

    auto newVecMems = ExampleWithVectorMemberCollection();
    newVecMems = std::move(vecMemColl);

    auto newUserData = podio::UserDataCollection<float>();
    newUserData = std::move(userDataColl);

    checkCollections(newHits, newClusters, newVecMems, newUserData);
  }

  SECTION("Prepared collections can be move constructed") {
    hitColl.prepareForWrite();
    auto newHits = std::move(hitColl);

    clusterColl.prepareForWrite();
    auto newClusters = std::move(clusterColl);

    vecMemColl.prepareForWrite();
    auto newVecMems = std::move(vecMemColl);

    userDataColl.prepareForWrite();
    auto newUserData = std::move(userDataColl);

    checkCollections(newHits, newClusters, newVecMems, newUserData);
  }

  SECTION("Prepared collections can be move assigned") {
    hitColl.prepareForWrite();
    clusterColl.prepareForWrite();
    vecMemColl.prepareForWrite();

    auto newHits = ExampleHitCollection();
    newHits = std::move(hitColl);

    auto newClusters = ExampleClusterCollection();
    newClusters = std::move(clusterColl);

    auto newVecMems = ExampleWithVectorMemberCollection();
    newVecMems = std::move(vecMemColl);

    auto newUserData = podio::UserDataCollection<float>();
    newUserData = std::move(userDataColl);

    checkCollections(newHits, newClusters, newVecMems, newUserData);
  }

  SECTION("Subset collections can be moved") {
    // NOTE: Does not apply to UserDataCollections!
    auto subsetHits = ExampleHitCollection();
    subsetHits.setSubsetCollection();
    for (auto hit : hitColl)
      subsetHits.push_back(hit);
    checkCollections(subsetHits, clusterColl, vecMemColl, userDataColl);

    auto newSubsetHits = std::move(subsetHits);
    REQUIRE(newSubsetHits.isSubsetCollection());
    checkCollections(newSubsetHits, clusterColl, vecMemColl, userDataColl);

    auto subsetClusters = ExampleClusterCollection();
    subsetClusters.setSubsetCollection();
    for (auto cluster : clusterColl)
      subsetClusters.push_back(cluster);
    checkCollections(newSubsetHits, subsetClusters, vecMemColl, userDataColl);

    // Test move-assignment here as well
    auto newSubsetClusters = ExampleClusterCollection();
    newSubsetClusters = std::move(subsetClusters);
    REQUIRE(newSubsetClusters.isSubsetCollection());
    checkCollections(newSubsetHits, newSubsetClusters, vecMemColl, userDataColl);

    auto subsetVecs = ExampleWithVectorMemberCollection();
    subsetVecs.setSubsetCollection();
    for (auto vec : vecMemColl)
      subsetVecs.push_back(vec);
    checkCollections(newSubsetHits, newSubsetClusters, subsetVecs, userDataColl);

    auto newSubsetVecs = std::move(subsetVecs);
    REQUIRE(newSubsetVecs.isSubsetCollection());
    checkCollections(hitColl, clusterColl, newSubsetVecs, userDataColl);
  }
}

TEST_CASE("Version tests", "[versioning]") {
  using namespace podio::version;
  // all of these comparisons should be possible at compile time -> STATIC_REQUIRE

  // major version checks
  constexpr Version ver_1{1};
  constexpr Version ver_2{2};
  constexpr Version ver_1_1{1, 1};
  constexpr Version ver_2_1{2, 1};
  constexpr Version ver_1_1_1{1, 1, 1};
  constexpr Version ver_1_0_2{1, 0, 2};
  constexpr Version ver_2_0_2{2, 0, 2};

  SECTION("Equality") {
    STATIC_REQUIRE(ver_1 == Version{1, 0, 0});
    STATIC_REQUIRE(ver_1 != ver_2);
    STATIC_REQUIRE(ver_1_1_1 == Version{1, 1, 1});
    STATIC_REQUIRE(ver_2_1 != ver_1_1);
    STATIC_REQUIRE(ver_1_0_2 != ver_2_0_2);
  }

  SECTION("Major version") {
    STATIC_REQUIRE(ver_1 < ver_2);
    STATIC_REQUIRE(Version{3} > ver_2);
  }

  SECTION("Minor version") {
    STATIC_REQUIRE(ver_1 < ver_1_1);
    STATIC_REQUIRE(ver_2_1 > ver_2);
    STATIC_REQUIRE(ver_1_1 < ver_2);
  }

  SECTION("Patch version") {
    STATIC_REQUIRE(ver_1 < ver_1_0_2);
    STATIC_REQUIRE(ver_1 < ver_1_1_1);
    STATIC_REQUIRE(ver_1_1_1 > ver_1_1);
    STATIC_REQUIRE(ver_2_0_2 < ver_2_1);
  }
}

TEST_CASE("Preprocessor version tests", "[versioning]") {
  SECTION("Basic functionality") {
    using namespace podio::version;
    // Check that preprocessor comparisons work
    STATIC_REQUIRE(PODIO_BUILD_VERSION == PODIO_VERSION(build_version.major, build_version.minor, build_version.patch));

    // Make sure that we can actually decode 64 bit versions
    STATIC_REQUIRE(decode_version(PODIO_BUILD_VERSION) == build_version);

    STATIC_REQUIRE(PODIO_MAJOR_VERSION(PODIO_BUILD_VERSION) == build_version.major);
    STATIC_REQUIRE(PODIO_MINOR_VERSION(PODIO_BUILD_VERSION) == build_version.minor);
    STATIC_REQUIRE(PODIO_PATCH_VERSION(PODIO_BUILD_VERSION) == build_version.patch);

    // Make a few checks where other versions are "maxed out"
    STATIC_REQUIRE(PODIO_MAJOR_VERSION(PODIO_VERSION(10000, 65535, 65535)) == 10000);
    STATIC_REQUIRE(PODIO_MINOR_VERSION(PODIO_VERSION(65535, 20000, 65535)) == 20000);
    STATIC_REQUIRE(PODIO_PATCH_VERSION(PODIO_VERSION(65535, 65535, 30000)) == 30000);
  }

  SECTION("Comparing") {
    // Using some large numbers here to check what happens if we start to
    // actually use the 16 available bits
    // patch version
    STATIC_REQUIRE(PODIO_VERSION(10000, 20000, 39999) < PODIO_VERSION(10000, 20000, 40000));

    // minor version
    STATIC_REQUIRE(PODIO_VERSION(10000, 30000, 33333) > PODIO_VERSION(10000, 29999, 33333));
    STATIC_REQUIRE(PODIO_VERSION(10000, 30000, 33333) < PODIO_VERSION(10000, 30001, 44444));

    // major version
    STATIC_REQUIRE(PODIO_VERSION(20000, 40000, 0) < PODIO_VERSION(20001, 40000, 0));
    STATIC_REQUIRE(PODIO_VERSION(20000, 40000, 10000) < PODIO_VERSION(20001, 30000, 0));
    STATIC_REQUIRE(PODIO_VERSION(20001, 40000, 10000) > PODIO_VERSION(20000, 40000, 20000));
    STATIC_REQUIRE(PODIO_VERSION(20000, 40000, 10000) > PODIO_VERSION(19999, 50000, 30000));
  }
}

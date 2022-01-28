#include "catch2/catch_test_macros.hpp"

#include "podio/AssociationCollection.h"

#include "datamodel/ExampleClusterCollection.h"
#include "datamodel/ExampleHitCollection.h"

#include <type_traits>

// Test datatypes (spelling them out here explicitly to make sure that
// assumptions about typedefs actually hold)
using TestA = podio::Association<ExampleHit, ExampleCluster>;
using TestMutA = podio::MutableAssociation<ExampleHit, ExampleCluster>;
using TestAColl = podio::AssociationCollection<ExampleHit, ExampleCluster>;
using TestAIter = podio::AssociationCollectionIterator<ExampleHit, ExampleCluster>;
using TestAMutIter = podio::AssociationMutableCollectionIterator<ExampleHit, ExampleCluster>;

TEST_CASE("Association constness", "[associations][static-checks]") {
  STATIC_REQUIRE(std::is_same_v<decltype(std::declval<TestMutA>().getFrom()), ExampleHit>);
  STATIC_REQUIRE(std::is_same_v<decltype(std::declval<TestMutA>().getTo()), ExampleCluster>);

  STATIC_REQUIRE(std::is_same_v<decltype(std::declval<TestA>().getFrom()), ExampleHit>);
  STATIC_REQUIRE(std::is_same_v<decltype(std::declval<TestA>().getTo()), ExampleCluster>);
}

TEST_CASE("Association basics", "[associations]") {
  auto cluster = MutableExampleCluster();
  auto hit = MutableExampleHit();

  auto mutAssoc = TestMutA();
  mutAssoc.setWeight(3.14f);
  mutAssoc.setFrom(hit);
  mutAssoc.setTo(cluster);

  REQUIRE(mutAssoc.getWeight() == 3.14f);
  REQUIRE(mutAssoc.getFrom() == hit);
  REQUIRE(mutAssoc.getTo() == cluster);

  SECTION("Copying") {
    auto otherAssoc = mutAssoc;
    REQUIRE(otherAssoc.getWeight() == 3.14f);
    REQUIRE(otherAssoc.getFrom() == hit);
    REQUIRE(otherAssoc.getTo() == cluster);

    auto otherCluster = ExampleCluster();
    auto otherHit = ExampleHit();
    otherAssoc.setFrom(otherHit);
    otherAssoc.setTo(otherCluster);
    otherAssoc.setWeight(42.0f);
    REQUIRE(otherAssoc.getWeight() == 42.0f);
    REQUIRE(otherAssoc.getFrom() == otherHit);
    REQUIRE(otherAssoc.getTo() == otherCluster);

    // Make sure original association changes as well
    REQUIRE(mutAssoc.getWeight() == 42.0f);
    REQUIRE(mutAssoc.getFrom() == otherHit);
    REQUIRE(mutAssoc.getTo() == otherCluster);
  }

  SECTION("Assignment") {
    auto otherAssoc = TestMutA();
    otherAssoc = mutAssoc;
    REQUIRE(otherAssoc.getWeight() == 3.14f);
    REQUIRE(otherAssoc.getFrom() == hit);
    REQUIRE(otherAssoc.getTo() == cluster);

    auto otherCluster = ExampleCluster();
    auto otherHit = ExampleHit();
    otherAssoc.setFrom(otherHit);
    otherAssoc.setTo(otherCluster);
    otherAssoc.setWeight(42.0f);
    REQUIRE(otherAssoc.getWeight() == 42.0f);
    REQUIRE(otherAssoc.getFrom() == otherHit);
    REQUIRE(otherAssoc.getTo() == otherCluster);

    // Make sure original association changes as well
    REQUIRE(mutAssoc.getWeight() == 42.0f);
    REQUIRE(mutAssoc.getFrom() == otherHit);
    REQUIRE(mutAssoc.getTo() == otherCluster);
  }

  SECTION("Implicit conversion") {
    // Use an immediately invoked lambda to check that the implicit conversion
    // is working as desired
    [hit, cluster](TestA assoc) {
      REQUIRE(assoc.getWeight() == 3.14f);
      REQUIRE(assoc.getFrom() == hit);
      REQUIRE(assoc.getTo() == cluster);
    }(mutAssoc);
  }

  SECTION("Cloning") {
    auto otherAssoc = mutAssoc.clone();
    REQUIRE(otherAssoc.getWeight() == 3.14f);
    REQUIRE(otherAssoc.getFrom() == hit);
    REQUIRE(otherAssoc.getTo() == cluster);

    auto otherCluster = ExampleCluster();
    auto otherHit = ExampleHit();
    otherAssoc.setFrom(otherHit);
    otherAssoc.setTo(otherCluster);
    otherAssoc.setWeight(42.0f);
    REQUIRE(otherAssoc.getWeight() == 42.0f);
    REQUIRE(otherAssoc.getFrom() == otherHit);
    REQUIRE(otherAssoc.getTo() == otherCluster);

    // Make sure original association is unchanged
    REQUIRE(mutAssoc.getWeight() == 3.14f);
    REQUIRE(mutAssoc.getFrom() == hit);
    REQUIRE(mutAssoc.getTo() == cluster);

    // Check cloning from an immutable one
    TestA assoc = mutAssoc;
    auto anotherAssoc = assoc.clone();
    anotherAssoc.setFrom(otherHit);
    anotherAssoc.setTo(otherCluster);
    anotherAssoc.setWeight(42.0f);
    REQUIRE(anotherAssoc.getWeight() == 42.0f);
    REQUIRE(anotherAssoc.getFrom() == otherHit);
    REQUIRE(anotherAssoc.getTo() == otherCluster);
  }

  SECTION("Equality operator") {
    auto otherAssoc = mutAssoc;
    REQUIRE(otherAssoc == mutAssoc);

    // Mutable and immutable associations should be comparable
    TestA assoc = mutAssoc;
    REQUIRE(assoc == mutAssoc);
  }
}

TEST_CASE("AssociationCollection basics", "[associations]") {
  auto coll = TestAColl();

  REQUIRE(coll.getValueTypeName() == "podio::Association<ExampleHit,ExampleCluster>");

  REQUIRE(true);
}

TEST_CASE("AssociationCollection constness", "[associations][static-checks][const-correctness]") {
  // Test type-aliases in AssociationCollection
  STATIC_REQUIRE(std::is_same_v<TestAColl::const_iterator, TestAIter>);
  STATIC_REQUIRE(std::is_same_v<TestAColl::iterator, TestAMutIter>);

  SECTION("const collections with const iterators") {
    const auto coll = TestAColl();
    // this essentially checks the whole "chain" from begin() / end() through
    // iterator operators
    for (auto assoc : coll) {
      STATIC_REQUIRE(std::is_same_v<decltype(assoc), TestA>); // const collection iterators should only return
                                                              // immutable objects
    }

    // check the individual steps again from above, to see where things fail if they fail
    STATIC_REQUIRE(std::is_same_v<decltype(std::declval<const TestAColl>().begin()),
                                  TestAColl::const_iterator>); // const collectionb begin() should return a
                                                               // AssociationCollectionIterator

    STATIC_REQUIRE(std::is_same_v<decltype(std::declval<const TestAColl>().end()),
                                  TestAColl::const_iterator>); // const collectionb end() should return a
                                                               // AssociationCollectionIterator

    STATIC_REQUIRE(std::is_same_v<decltype(*std::declval<const TestAColl>().begin()),
                                  TestA>); // AssociationCollectionIterator should only give access to immutable
                                           // objects

    STATIC_REQUIRE(std::is_same_v<decltype(std::declval<TestAIter>().operator->()),
                                  TestA*>); // AssociationCollectionIterator should only give access to immutable
                                            // objects
  }

  SECTION("non-const collections with mutable iterators") {
    auto coll = TestAColl();
    // this essentially checks the whole "chain" from begin() / end() through
    // iterator operators
    for (auto assoc : coll) {
      STATIC_REQUIRE(std::is_same_v<decltype(assoc), TestMutA>); // collection iterators should return return
                                                                 // mutable objects
    }

    // check the individual steps again from above, to see where things fail if they fail
    STATIC_REQUIRE(std::is_same_v<decltype(std::declval<TestAColl>().begin()),
                                  TestAColl::iterator>); // collection begin() should return a
                                                         // MutableCollectionIterator

    STATIC_REQUIRE(std::is_same_v<decltype(std::declval<TestAColl>().end()),
                                  TestAColl::iterator>); // collectionb end() should return a
                                                         // MutableCollectionIterator

    STATIC_REQUIRE(std::is_same_v<decltype(*std::declval<TestAColl>().begin()),
                                  TestMutA>); // MutableCollectionIterator should give access to immutable
                                              // mutable objects

    STATIC_REQUIRE(std::is_same_v<decltype(std::declval<TestAMutIter>().operator->()),
                                  TestMutA*>); // MutableCollectionIterator should give access to immutable
                                               // mutable objects
  }

  SECTION("const correct indexed access to const collections") {
    STATIC_REQUIRE(std::is_same_v<decltype(std::declval<const TestAColl>()[0]),
                                  TestA>); // const collections should only have indexed indexed access to immutable
                                           // objects

    STATIC_REQUIRE(std::is_same_v<decltype(std::declval<const TestAColl>().at(0)),
                                  TestA>); // const collections should only have indexed indexed access to immutable
                                           // objects
  }

  SECTION("const correct indexed access to collections") {
    STATIC_REQUIRE(std::is_same_v<decltype(std::declval<TestAColl>()[0]),
                                  TestMutA>); // collections should have indexed indexed access to mutable objects

    STATIC_REQUIRE(std::is_same_v<decltype(std::declval<TestAColl>().at(0)),
                                  TestMutA>); // collections should have indexed indexed access to mutable objects
  }
}

TEST_CASE("AssociationCollection subset collection", "[associations]") {
  auto assocs = TestAColl();
  auto assoc1 = assocs.create();
  assoc1.setWeight(1.0f);
  auto assoc2 = assocs.create();
  assoc2.setWeight(2.0f);

  auto assocRefs = TestAColl();
  assocRefs.setSubsetCollection();
  for (const auto a : assocs) {
    assocRefs.push_back(a);
  }

  SECTION("Collection iterators work with subset collections") {

    // index-based looping / access
    for (size_t i = 0; i < assocRefs.size(); ++i) {
      REQUIRE(assocRefs[i].getWeight() == i + 1);
    }

    // range-based for loop
    int index = 1;
    for (const auto a : assocRefs) {
      REQUIRE(a.getWeight() == index++);
    }
  }

  SECTION("Conversion failures") {
    // Cannot convert into a subset collection if elements already present
    REQUIRE_THROWS_AS(assocs.setSubsetCollection(), std::logic_error);

    // Connot convert a subset collection into a normal collection
    REQUIRE_THROWS_AS(assocRefs.setSubsetCollection(false), std::logic_error);
  }

  SECTION("Subset collection only handles tracked objects") {
    auto assoc = TestA();
    REQUIRE_THROWS_AS(assocRefs.push_back(assoc), std::invalid_argument);
    REQUIRE_THROWS_AS(assocRefs.create(), std::logic_error);
  }
}

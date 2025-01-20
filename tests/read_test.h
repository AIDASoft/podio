#ifndef PODIO_TESTS_READ_TEST_H // NOLINT(llvm-header-guard): folder structure not suitable
#define PODIO_TESTS_READ_TEST_H // NOLINT(llvm-header-guard): folder structure not suitable
// test data model
#include "datamodel/ExampleClusterCollection.h"
#include "datamodel/ExampleHitCollection.h"
#include "datamodel/ExampleMCCollection.h"
#include "datamodel/ExampleReferencingTypeCollection.h"
#include "datamodel/ExampleWithARelationCollection.h"
#include "datamodel/ExampleWithArrayCollection.h"
#include "datamodel/ExampleWithComponentCollection.h"
#include "datamodel/ExampleWithFixedWidthIntegersCollection.h"
#include "datamodel/ExampleWithNamespace.h"
#include "datamodel/ExampleWithOneRelationCollection.h"
#include "datamodel/ExampleWithVectorMemberCollection.h"
#include "datamodel/TestInterfaceLinkCollection.h"
#include "datamodel/TestLinkCollection.h"
#include "datamodel/TypeWithEnergy.h"

// podio specific includes
#include "podio/Frame.h"
#include "podio/LinkCollection.h"
#include "podio/UserDataCollection.h"
#include "podio/podioVersion.h"

// STL
#include <cassert>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <vector>

#define ASSERT(condition, msg)                                                                                         \
  if (!(condition)) {                                                                                                  \
    throw std::runtime_error(msg);                                                                                     \
  }

void checkIntUserDataCollection(const podio::Frame& event, int eventNum) {
  auto& usrInts = event.get<podio::UserDataCollection<uint64_t>>("userInts");
  ASSERT(usrInts.size() == static_cast<unsigned>(eventNum + 1), "userInts collection does not have the expected size")

  auto& uivec = usrInts.vec();
  int myInt = 0;
  for (int iu : uivec) {
    ASSERT(iu == myInt++, "userInts contents not as expected");
  }

  myInt = 0;
  for (int iu : usrInts) {
    ASSERT(iu == myInt++, "userInts contents not as expected");
  }
}

void checkHitCollection(const podio::Frame& event, int eventNum) {
  const auto& hits = event.get<ExampleHitCollection>("hits");

  ASSERT(hits.size() == 2, "size of hits collection not as expected");

  const auto expectedHit1 = ExampleHit(0xbadULL, 0., 0., 0., 23. + eventNum);
  const auto expectedHit2 = ExampleHit(0xcaffeeULL, 1., 0., 0., 12. + eventNum);

  const auto compareHits = [](const ExampleHit& hitA, const ExampleHit& hitB) {
    return hitA.cellID() == hitB.cellID() && hitA.energy() == hitB.energy() && hitA.x() == hitB.x() &&
        hitA.y() == hitB.y() && hitA.z() == hitB.z();
  };

  auto hit1 = hits[0];
  ASSERT(compareHits(hit1, expectedHit1), "first hit in hits not as expected");
  auto hit2 = hits[1];
  ASSERT(compareHits(hit2, expectedHit2), "second hit in hits not as expected");
}

void checkClusterCollection(const podio::Frame& event, const ExampleHitCollection& hits) {
  const auto& clusters = event.get<ExampleClusterCollection>("clusters");
  ASSERT(clusters.size() == 3, "size of clusters collection not as expected");

  auto clu0 = clusters[0];
  auto clu1 = clusters[1];
  auto cluster = clusters[2];
  ASSERT(clu0.Hits().size() == 1, "first cluster should only have one hit");
  ASSERT(clu1.Hits().size() == 1, "second cluster should only have one hit");
  ASSERT(cluster.Hits().size() == 2, "third cluster should have two hits");
  ASSERT(cluster.Clusters().size() == 2, "third cluster should have two clusters");
  ASSERT(cluster.Clusters(0) == clu0, "first cluster of third cluster not as expected");
  ASSERT(cluster.Clusters(1) == clu1, "second cluster of third cluster not as expected");

  auto hit1 = hits[0];
  auto hit2 = hits[1];
  ASSERT(clu0.Hits(0) == hit1, "hit related to first cluster not as expected");
  ASSERT(clu0.energy() == hit1.energy(), "energy of first cluster not as expected");
  ASSERT(clu1.Hits(0) == hit2, "hit related to second cluster not as expected");
  ASSERT(clu1.energy() == hit2.energy(), "energy of second cluster not as expected");
  ASSERT(cluster.Hits(0) == hit1, "first hit related to third cluster not as expected");
  ASSERT(cluster.Hits(1) == hit2, "second hit related to third cluster not as expected");
  ASSERT(cluster.energy() == hit1.energy() + hit2.energy(), "energy of third cluster not as expected");
}

void checkMCParticleCollection(const podio::Frame& event, const podio::version::Version fileVersion) {
  const auto& mcps = event.get<ExampleMCCollection>("mcparticles");
  ASSERT(mcps.size() == 10, "mcparticles collection does not have the correct size");

  auto mcp = mcps[0];
  ASSERT(mcp.daughters().size() == 4, "first mc particle does not have the expected number of daughters");
  ASSERT(mcp.daughters(0) == mcps[2], "daughter relation 0 for mcparticle 0 not as expected");
  ASSERT(mcp.daughters(1) == mcps[3], "daughter relation 1 for mcparticle 0 not as expected");
  ASSERT(mcp.daughters(2) == mcps[4], "daughter relation 2 for mcparticle 0 not as expected");
  ASSERT(mcp.daughters(3) == mcps[5], "daughter relation 3 for mcparticle 0 not as expected");

  mcp = mcps[1];
  ASSERT(mcp.daughters().size() == 4, "second mc particle does not have the expected number of daughters");
  ASSERT(mcp.daughters(0) == mcps[2], "daughter relation 0 for mcparticle 1 not as expected");
  ASSERT(mcp.daughters(1) == mcps[3], "daughter relation 1 for mcparticle 1 not as expected");
  ASSERT(mcp.daughters(2) == mcps[4], "daughter relation 2 for mcparticle 1 not as expected");
  ASSERT(mcp.daughters(3) == mcps[5], "daughter relation 3 for mcparticle 1 not as expected");

  mcp = mcps[2];
  ASSERT(mcp.daughters().size() == 4, "third mc particle does not have the expected number of daughters");
  ASSERT(mcp.daughters(0) == mcps[6], "daughter relation 0 for mcparticle 2 not as expected");
  ASSERT(mcp.daughters(1) == mcps[7], "daughter relation 1 for mcparticle 2 not as expected");
  ASSERT(mcp.daughters(2) == mcps[8], "daughter relation 2 for mcparticle 2 not as expected");
  ASSERT(mcp.daughters(3) == mcps[9], "daughter relation 3 for mcparticle 2 not as expected");

  mcp = mcps[3];
  ASSERT(mcp.daughters().size() == 4, "fourth mc particle does not have the expected number of daughters");
  ASSERT(mcp.daughters(0) == mcps[6], "daughter relation 0 for mcparticle 3 not as expected");
  ASSERT(mcp.daughters(1) == mcps[7], "daughter relation 1 for mcparticle 3 not as expected");
  ASSERT(mcp.daughters(2) == mcps[8], "daughter relation 2 for mcparticle 3 not as expected");
  ASSERT(mcp.daughters(3) == mcps[9], "daughter relation 3 for mcparticle 3 not as expected");

  // spot check some parent relations as well
  mcp = mcps[4];
  ASSERT(mcp.parents().size() == 2, "fivth mc particle does not have the expected number of parents");
  // Bugged writing before this version
  if (fileVersion >= podio::version::Version(1, 2, 0)) {
    ASSERT(mcp.parents(0) == mcps[0], "parent relation 0 for mcparticle 4 is not as expected");
    ASSERT(mcp.parents(1) == mcps[1], "parent relation 0 for mcparticle 4 is not as expected");
  }

  mcp = mcps[7];
  ASSERT(mcp.parents().size() == 2, "eigth mc particle does not have the expected number of parents");
  // Bugged writing before this version
  if (fileVersion >= podio::version::Version(1, 2, 1)) {
    ASSERT(mcp.parents(0) == mcps[2], "parent relation 0 for mcparticle 8 is not as expected");
    ASSERT(mcp.parents(1) == mcps[3], "parent relation 0 for mcparticle 8 is not as expected");
  }
}

void checkLinkCollection(const podio::Frame& event, const ExampleHitCollection& hits,
                         const ExampleClusterCollection& clusters) {
  const auto& links = event.get<TestLinkCollection>("links");
  const auto nLinks = std::min(clusters.size(), hits.size());
  ASSERT(links.size() == nLinks, "LinksColelction does not have the expected size");

  int linkIndex = 0;
  for (auto link : links) {
    ASSERT((link.getWeight() == 0.5 * linkIndex) && (link.getFrom() == hits[linkIndex]) &&
               (link.getTo() == clusters[nLinks - 1 - linkIndex]),
           "Link does not have expected content");
    linkIndex++;
  }
}

template <typename FixedWidthT>
bool check_fixed_width_value(FixedWidthT actual, FixedWidthT expected, const std::string& type) {
  if (actual != expected) {
    std::stringstream msg{};
    msg << "fixed width integer (" << type << ") value is not as expected: " << actual << " vs " << expected;
    throw std::runtime_error(msg.str());
  }
  return true;
}

void processEvent(const podio::Frame& event, int eventNum, podio::version::Version fileVersion) {
  const float evtWeight = event.getParameter<float>("UserEventWeight").value();
  if (evtWeight != 100.f * eventNum) {
    std::cout << " read UserEventWeight: " << evtWeight << " - expected : " << 100.f * eventNum << std::endl;
    throw std::runtime_error("Couldn't read event meta data parameters 'UserEventWeight'");
  }

  std::stringstream ss;
  ss << " event_number_" << eventNum;
  const auto evtName = event.getParameter<std::string>("UserEventName").value();

  if (evtName != ss.str()) {
    std::cout << " read UserEventName: " << evtName << " - expected : " << ss.str() << std::endl;
    throw std::runtime_error("Couldn't read event meta data parameters 'UserEventName'");
  }

  if (fileVersion > podio::version::Version{0, 14, 1}) {
    const auto someVectorData = event.getParameter<std::vector<int>>("SomeVectorData").value();
    if (someVectorData.size() != 4) {
      throw std::runtime_error("Couldn't read event meta data parameters: 'SomeVectorData'");
    }
    for (int i = 0; i < 4; ++i) {
      if (someVectorData[i] != i + 1) {
        throw std::runtime_error("Couldn't read event meta data parameters: 'SomeVectorData'");
      }
    }
  }

  if (fileVersion > podio::version::Version{0, 16, 2}) {
    const auto doubleParams = event.getParameter<std::vector<double>>("SomeVectorData").value();
    if (doubleParams.size() != 2 || doubleParams[0] != eventNum * 1.1 || doubleParams[1] != eventNum * 2.2) {
      throw std::runtime_error("Could not read event parameter: 'SomeDoubleValues' correctly");
    }
  }

  checkHitCollection(event, eventNum);

  auto& hits = event.get<ExampleHitCollection>("hits");

  if (fileVersion > podio::version::Version{0, 14, 0}) {
    auto& hitRefs = event.get<ExampleHitCollection>("hitRefs");
    if (hitRefs.size() != hits.size()) {
      throw std::runtime_error("hit and subset hit collection do not have the same size");
    }
    if (!(hits[1] == hitRefs[0] && hits[0] == hitRefs[1])) {
      throw std::runtime_error("hit subset collections do not have the expected contents");
    }
  }

  checkClusterCollection(event, hits);

  auto& clusters = event.get<ExampleClusterCollection>("clusters");

  if (fileVersion >= podio::version::Version{0, 13, 2}) {
    // Read the mcParticleRefs before reading any of the other collections that
    // are referenced to make sure that all the necessary relations are handled
    // correctly
    auto& mcpRefs = event.get<ExampleMCCollection>("mcParticleRefs");
    if (!mcpRefs.isValid()) {
      throw std::runtime_error("Collection 'mcParticleRefs' should be present");
    }

    // Only doing a very basic check here, that mainly just ensures that the
    // RelationRange is valid and does not segfault.
    for (auto ref : mcpRefs) {
      const auto daughters = ref.daughters();
      if (!daughters.empty()) {
        // This will segfault in case things are not working
        auto d [[maybe_unused]] = daughters[0];
      }

      const auto parents = ref.parents();
      if (!parents.empty()) {
        // This will segfault in case things are not working
        auto d [[maybe_unused]] = parents[0];
      }
    }
  }

  checkMCParticleCollection(event, fileVersion);

  auto& mcps = event.get<ExampleMCCollection>("mcparticles");
  if (!mcps.isValid()) {
    throw std::runtime_error("Collection 'mcparticles' should be present");
  }

  // Check the MCParticle subset collection only if it is technically possible
  // to be in the file
  if (fileVersion >= podio::version::Version{0, 13, 2}) {

    // Load the subset collection first to ensure that it pulls in objects that
    // have not been read yet
    auto& mcpRefs = event.get<ExampleMCCollection>("mcParticleRefs");
    if (!mcpRefs.isValid()) {
      throw std::runtime_error("Collection 'mcParticleRefs' should be present");
    }

    for (auto pr : mcpRefs) {
      if (static_cast<unsigned>(pr.getObjectID().collectionID) == mcpRefs.getID()) {
        throw std::runtime_error(
            "objects of a reference collection should have a different collectionID than the reference collection");
      }
    }

    auto& moreMCs = event.get<ExampleMCCollection>("moreMCs");

    // First check that the two mc collections that we store are the same
    if (mcps.size() != moreMCs.size()) {
      throw std::runtime_error("'mcparticles' and 'moreMCs' should have the same size");
    }

    for (size_t index = 0; index < mcps.size(); ++index) {
      // Not too detailed check here
      if (mcps[index].energy() != moreMCs[index].energy() ||
          mcps[index].daughters().size() != moreMCs[index].daughters().size()) {
        throw std::runtime_error("'mcparticles' and 'moreMCs' do not contain the same elements");
      }
    }

    if (mcpRefs.size() != mcps.size()) {
      throw std::runtime_error("'mcParticleRefs' collection has wrong size");
    }
    for (size_t i = 0; i < mcpRefs.size(); ++i) {
      if (i < 5) { // The first elements point into the mcps collection
        if (mcpRefs[i] != mcps[2 * i + 1]) {
          throw std::runtime_error("MCParticle reference does not point to the correct MCParticle");
        }
      } else { // The second half points into the moreMCs collection
        const int index = (i - 5) * 2;
        if (mcpRefs[i] != moreMCs[index]) {
          throw std::runtime_error("MCParticle reference does not point to the correct MCParticle");
        }
      }
    }
  }

  // std::cout << "Fetching collection 'refs'" << std::endl;
  auto& refs = event.get<ExampleReferencingTypeCollection>("refs");
  if (refs.isValid()) {
    auto ref = refs[0];
    for (auto cluster : ref.Clusters()) {
      for (auto hit [[maybe_unused]] : cluster.Hits()) {
        // std::cout << "  Referenced object has an energy of " << hit.energy() << std::endl;
      }
    }
  } else {
    throw std::runtime_error("Collection 'refs' should be present");
  }
  // std::cout << "Fetching collection 'OneRelation'" << std::endl;
  auto& rels = event.get<ExampleWithOneRelationCollection>("OneRelation");
  if (rels.isValid()) {
    // std::cout << "Referenced object has an energy of " << (*rels)[0].cluster().energy() << std::endl;
  } else {
    throw std::runtime_error("Collection 'OneRelation' should be present");
  }

  //  std::cout << "Fetching collection 'WithVectorMember'" << std::endl;
  auto& vecs = event.get<ExampleWithVectorMemberCollection>("WithVectorMember");
  if (vecs.isValid()) {
    if (vecs.size() != 2) {
      throw std::runtime_error("Collection 'WithVectorMember' should have two elements'");
    }

    for (auto vec : vecs) {
      if (vec.count().size() != 2) {
        throw std::runtime_error(
            "Element of 'WithVectorMember' collection should have two elements in its 'count' vector");
      }
    }
    if (vecs[0].count(0) != eventNum || vecs[0].count(1) != eventNum + 10 || vecs[1].count(0) != eventNum + 1 ||
        vecs[1].count(1) != eventNum + 11) {
      throw std::runtime_error("Element values of the 'count' vector in an element of the 'WithVectorMember' "
                               "collection do not have the expected values");
    }

    for (auto item : vecs) {
      for (auto c = item.count_begin(), end = item.count_end(); c != end; ++c) {
        std::cout << "  Counter value " << (*c) << std::endl;
      }
    }
  } else {
    throw std::runtime_error("Collection 'WithVectorMember' should be present");
  }

  auto& comps = event.get<ExampleWithComponentCollection>("Component");
  if (comps.isValid()) {
    auto comp = comps[0];
    int a [[maybe_unused]] = comp.component().data.x + comp.component().data.z;
  }

  auto& arrays = event.get<ExampleWithArrayCollection>("arrays");
  if (arrays.isValid() && !arrays.empty()) {
    auto array = arrays[0];
    if (array.myArray(1) != eventNum) {
      throw std::runtime_error("Array not properly set.");
    }
    if (array.arrayStruct().data.p.at(2) != 2 * eventNum) {
      throw std::runtime_error("Array not properly set.");
    }
    if (array.structArray(0).x != eventNum) {
      throw std::runtime_error("Array of struct not properly set.");
    }
  } else {
    throw std::runtime_error("Collection 'arrays' should be present");
  }

  auto& nmspaces = event.get<ex42::ExampleWithARelationCollection>("WithNamespaceRelation");
  auto& copies = event.get<ex42::ExampleWithARelationCollection>("WithNamespaceRelationCopy");

  auto cpytest = ex42::ExampleWithARelationCollection{};
  if (nmspaces.isValid() && copies.isValid()) {
    for (size_t j = 0; j < nmspaces.size(); j++) {
      auto nmsp = nmspaces[j];
      auto cpy = copies[j];
      cpytest.push_back(nmsp.clone());
      if (nmsp.ref().isAvailable()) {
        if (nmsp.ref().component().x != cpy.ref().component().x ||
            nmsp.ref().component().y != cpy.ref().component().y) {
          throw std::runtime_error("Copied item has differing component in OneToOne referenced item.");
        }
        // check direct accessors of POD sub members
        if (nmsp.ref().x() != cpy.ref().x()) {
          throw std::runtime_error("Getting wrong values when using direct accessors for sub members.");
        }
        if (nmsp.number() != cpy.number()) {
          throw std::runtime_error("Copied item has differing member.");
        }
        if (nmsp.ref().getObjectID() != cpy.ref().getObjectID()) {
          throw std::runtime_error("Copied item has wrong OneToOne references.");
        }
      }
      auto cpy_it = cpy.refs_begin();
      for (auto it = nmsp.refs_begin(); it != nmsp.refs_end(); ++it, ++cpy_it) {
        if (it->component().x != cpy_it->component().x || it->component().y != cpy_it->component().y) {
          throw std::runtime_error("Copied item has differing component in OneToMany referenced item.");
        }
        if (it->getObjectID() != cpy_it->getObjectID()) {
          throw std::runtime_error("Copied item has wrong OneToMany references.");
        }
      }
    }
  } else {
    throw std::runtime_error("Collection 'WithNamespaceRelation' and 'WithNamespaceRelationCopy' should be present");
  }

  if (fileVersion >= podio::version::Version{0, 13, 1}) {
    const auto& fixedWidthInts = event.get<ExampleWithFixedWidthIntegersCollection>("fixedWidthInts");
    if (not fixedWidthInts.isValid() or fixedWidthInts.size() != 3) {
      throw std::runtime_error("Collection \'fixedWidthInts\' should be present and have 3 elements");
    }

    auto maxValues = fixedWidthInts[0];
    const auto& maxComps = maxValues.fixedWidthStruct();
    check_fixed_width_value(maxValues.fixedI16(), std::numeric_limits<std::int16_t>::max(), "int16");
    check_fixed_width_value(maxValues.fixedU32(), std::numeric_limits<std::uint32_t>::max(), "uint32");
    check_fixed_width_value(maxValues.fixedU64(), std::numeric_limits<std::uint64_t>::max(), "uint64");
    check_fixed_width_value(maxComps.fixedInteger64, std::numeric_limits<std::int64_t>::max(), "int64");
    check_fixed_width_value(maxComps.fixedInteger32, std::numeric_limits<std::int32_t>::max(), "int32");
    check_fixed_width_value(maxComps.fixedUnsigned16, std::numeric_limits<std::uint16_t>::max(), "uint16");

    auto minValues = fixedWidthInts[1];
    const auto& minComps = minValues.fixedWidthStruct();
    check_fixed_width_value(minValues.fixedI16(), std::numeric_limits<std::int16_t>::min(), "int16");
    check_fixed_width_value(minValues.fixedU32(), std::numeric_limits<std::uint32_t>::min(), "uint32");
    check_fixed_width_value(minValues.fixedU64(), std::numeric_limits<std::uint64_t>::min(), "uint64");
    check_fixed_width_value(minComps.fixedInteger64, std::numeric_limits<std::int64_t>::min(), "int64");
    check_fixed_width_value(minComps.fixedInteger32, std::numeric_limits<std::int32_t>::min(), "int32");
    check_fixed_width_value(minComps.fixedUnsigned16, std::numeric_limits<std::uint16_t>::min(), "uint16");

    auto arbValues = fixedWidthInts[2];
    const auto& arbComps = arbValues.fixedWidthStruct();
    check_fixed_width_value(arbValues.fixedI16(), std::int16_t{-12345}, "int16");
    check_fixed_width_value(arbValues.fixedU32(), std::uint32_t{1234567890}, "uint32");
    check_fixed_width_value(arbValues.fixedU64(), std::uint64_t{1234567890123456789}, "uint64");
    check_fixed_width_value(arbComps.fixedInteger64, std::int64_t{-1234567890123456789}, "int64");
    check_fixed_width_value(arbComps.fixedInteger32, std::int32_t{-1234567890}, "int32");
    check_fixed_width_value(arbComps.fixedUnsigned16, std::uint16_t{12345}, "uint16");
  }

  if (fileVersion >= podio::version::Version{0, 13, 2}) {
    checkIntUserDataCollection(event, eventNum);

    auto& usrDbl = event.get<podio::UserDataCollection<double>>("userDoubles");
    if (usrDbl.size() != 100) {
      throw std::runtime_error(
          "Could not read all userDoubles properly (expected: 100, actual: " + std::to_string(usrDbl.size()) + ")");
    }

    for (double d : usrDbl) {
      if (d != 42.) {
        throw std::runtime_error("Couldn't read userDoubles properly");
      }
    }
  }

  // ======================= Links ==========================
  if (fileVersion >= podio::version::Version{1, 1, 99}) {
    checkLinkCollection(event, hits, clusters);
    auto& interfaceLinks = event.get<TestInterfaceLinkCollection>("links_with_interfaces");
    if (interfaceLinks.size() != 3) {
      throw std::runtime_error("Links with interfaces collection does not have the expected size (expected 3, actual " +
                               std::to_string(interfaceLinks.size()) + ")");
    }
    {
      auto link = interfaceLinks[0];
      if (link.get<ExampleCluster>() != clusters[0] || link.get<TypeWithEnergy>() != hits[0]) {
        throw std::runtime_error("Link with interface type could not be read back correctly");
      }
      link = interfaceLinks[1];
      if (link.get<ExampleCluster>() != clusters[1] || link.get<TypeWithEnergy>() != mcps[0]) {
        throw std::runtime_error("Link with interface type could not be read back correctly");
      }
      link = interfaceLinks[2];
      if (link.get<ExampleCluster>() != clusters[0] || link.get<TypeWithEnergy>() != clusters[1]) {
        throw std::runtime_error("Link with interface type could not be read back correctly");
      }
    }
  }
}

#endif // PODIO_TESTS_READ_TEST_H

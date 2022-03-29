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

// podio specific includes
#include "podio/EventStore.h"
#include "podio/IReader.h"
#include "podio/UserDataCollection.h"
#include "podio/podioVersion.h"

// STL
#include <cassert>
#include <exception>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <vector>

template <typename FixedWidthT>
bool check_fixed_width_value(FixedWidthT actual, FixedWidthT expected, const std::string& type) {
  if (actual != expected) {
    std::stringstream msg{};
    msg << "fixed width integer (" << type << ") value is not as expected: " << actual << " vs " << expected;
    throw std::runtime_error(msg.str());
  }
  return true;
}

void processEvent(podio::EventStore& store, int eventNum, podio::version::Version fileVersion) {

  const auto& evtMD = store.getEventMetaData();
  auto evtWeight = evtMD.getValue<float>("UserEventWeight");
  if (evtWeight != (float)100. * eventNum) {
    std::cout << " read UserEventWeight: " << evtWeight << " - expected : " << (float)100. * eventNum << std::endl;
    throw std::runtime_error("Couldn't read event meta data parameters 'UserEventWeight'");
  }
  std::stringstream ss;
  ss << " event_number_" << eventNum;
  const auto& evtMD2 = store.getEventMetaData();
  const auto& evtName = evtMD2.getValue<std::string>("UserEventName");
  if (evtName != ss.str()) {
    std::cout << " read UserEventName: " << evtName << " - expected : " << ss.str() << std::endl;
    throw std::runtime_error("Couldn't read event meta data parameters 'UserEventName'");
  }

  if (fileVersion > podio::version::Version{0, 14, 1}) {
    const auto& someVectorData = evtMD.getValue<std::vector<int>>("SomeVectorData");
    if (someVectorData.size() != 4) {
      throw std::runtime_error("Couldn't read event meta data parameters: 'SomeVectorData'");
    }
    for (int i = 0; i < 4; ++i) {
      if (someVectorData[i] != i + 1) {
        throw std::runtime_error("Couldn't read event meta data parameters: 'SomeVectorData'");
      }
    }
  }

  try {
    // not assigning to a variable, because it will remain unused, we just want
    // the exception here
    store.get<ExampleClusterCollection>("notthere");
  } catch (const std::runtime_error& err) {
    if (std::string(err.what()) != "No collection \'notthere\' is present in the EventStore") {
      throw std::runtime_error("Trying to get non present collection \'notthere' should throw an exception");
    }
  }

  // read collection meta data
  auto& hits = store.get<ExampleHitCollection>("hits");
  auto colMD = store.getCollectionMetaData(hits.getID());
  const auto& es = colMD.getValue<std::string>("CellIDEncodingString");
  if (es != std::string("system:8,barrel:3,layer:6,slice:5,x:-16,y:-16")) {
    std::cout << " meta data from collection 'hits' with id = " << hits.getID() << " read CellIDEncodingString: " << es
              << " - expected : system:8,barrel:3,layer:6,slice:5,x:-16,y:-16" << std::endl;
    throw std::runtime_error("Couldn't read event meta data parameters 'CellIDEncodingString'");
  }

  if (fileVersion > podio::version::Version{0, 14, 0}) {
    auto& hitRefs = store.get<ExampleHitCollection>("hitRefs");
    if (hitRefs.size() != hits.size()) {
      throw std::runtime_error("hit and subset hit collection do not have the same size");
    }
    if (!(hits[1] == hitRefs[0] && hits[0] == hitRefs[1])) {
      throw std::runtime_error("hit subset collections do not have the expected contents");
    }
  }

  auto& clusters = store.get<ExampleClusterCollection>("clusters");
  if (clusters.isValid()) {
    auto cluster = clusters[0];
    for (auto i = cluster.Hits_begin(), end = cluster.Hits_end(); i != end; ++i) {
      std::cout << "  Referenced hit has an energy of " << i->energy() << std::endl;
    }
  } else {
    throw std::runtime_error("Collection 'clusters' should be present");
  }

  auto& mcps = store.get<ExampleMCCollection>("mcparticles");
  if (!mcps.isValid()) {
    throw std::runtime_error("Collection 'mcparticles' should be present");
  }

  // check that we can retrieve the correct parent daughter relation
  // set in write_test.h :
  //-------- print relations for debugging:
  for (auto p : mcps) {
    std::cout << " particle " << p.getObjectID().index << " has daughters: ";
    for (auto it = p.daughters_begin(), end = p.daughters_end(); it != end; ++it) {
      std::cout << " " << it->getObjectID().index;
    }
    std::cout << "  and parents: ";
    for (auto it = p.parents_begin(), end = p.parents_end(); it != end; ++it) {
      std::cout << " " << it->getObjectID().index;
    }
    std::cout << std::endl;
  }

  // particle 0 has particles 2,3,4 and 5 as daughters:
  auto p = mcps[0];

  auto d0 = p.daughters(0);
  auto d1 = p.daughters(1);
  auto d2 = p.daughters(2);
  auto d3 = p.daughters(3);

  if (!(d0 == mcps[2])) {
    throw std::runtime_error(" error: 1. daughter of particle 0 is not particle 2 ");
  }
  if (!(d1 == mcps[3])) {
    throw std::runtime_error(" error: 2. daughter of particle 0 is not particle 3 ");
  }
  if (!(d2 == mcps[4])) {
    throw std::runtime_error(" error: 3. daughter of particle 0 is not particle 4 ");
  }
  if (!(d3 == mcps[5])) {
    throw std::runtime_error(" error: 4. daughter of particle 0 is not particle 5 ");
  }

  // particle 3 has particles 6,7,8 and 9 as daughters:
  p = mcps[3];

  d0 = p.daughters(0);
  d1 = p.daughters(1);
  d2 = p.daughters(2);
  d3 = p.daughters(3);

  if (!(d0 == mcps[6])) {
    throw std::runtime_error(" error: 1. daughter of particle 3 is not particle 6 ");
  }
  if (!(d1 == mcps[7])) {
    throw std::runtime_error(" error: 2. daughter of particle 3 is not particle 7 ");
  }
  if (!(d2 == mcps[8])) {
    throw std::runtime_error(" error: 3. daughter of particle 3 is not particle 8 ");
  }
  if (!(d3 == mcps[9])) {
    throw std::runtime_error(" error: 4. daughter of particle 3 is not particle 9 ");
  }

  // Check the MCParticle subset collection only if it is technically possible
  // to be in the file
  if (fileVersion >= podio::version::Version{0, 13, 2}) {

    // Load the subset collection first to ensure that it pulls in objects taht
    // have not been read yet
    auto& mcpRefs = store.get<ExampleMCCollection>("mcParticleRefs");
    if (!mcpRefs.isValid()) {
      throw std::runtime_error("Collection 'mcParticleRefs' should be present");
    }

    for (auto pr : mcpRefs) {
      if ((unsigned)pr.getObjectID().collectionID == mcpRefs.getID()) {
        throw std::runtime_error(
            "objects of a reference collection should have a different collectionID than the reference collection");
      }
    }

    auto& moreMCs = store.get<ExampleMCCollection>("moreMCs");

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
        if (!(mcpRefs[i] == mcps[2 * i + 1])) {
          throw std::runtime_error("MCParticle reference does not point to the correct MCParticle");
        }
      } else { // The second half points into the moreMCs collection
        const int index = (i - 5) * 2;
        if (!(mcpRefs[i] == moreMCs[index])) {
          throw std::runtime_error("MCParticle reference does not point to the correct MCParticle");
        }
      }
    }
  }

  // std::cout << "Fetching collection 'refs'" << std::endl;
  auto& refs = store.get<ExampleReferencingTypeCollection>("refs");
  if (refs.isValid()) {
    auto ref = refs[0];
    for (auto cluster : ref.Clusters()) {
      for (auto hit : cluster.Hits()) {
        // std::cout << "  Referenced object has an energy of " << hit.energy() << std::endl;
      }
    }
  } else {
    throw std::runtime_error("Collection 'refs' should be present");
  }
  // std::cout << "Fetching collection 'OneRelation'" << std::endl;
  auto& rels = store.get<ExampleWithOneRelationCollection>("OneRelation");
  if (rels.isValid()) {
    // std::cout << "Referenced object has an energy of " << (*rels)[0].cluster().energy() << std::endl;
  } else {
    throw std::runtime_error("Collection 'OneRelation' should be present");
  }

  //  std::cout << "Fetching collection 'WithVectorMember'" << std::endl;
  auto& vecs = store.get<ExampleWithVectorMemberCollection>("WithVectorMember");
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

  auto& comps = store.get<ExampleWithComponentCollection>("Component");
  if (comps.isValid()) {
    auto comp = comps[0];
    int a [[maybe_unused]] = comp.component().data.x + comp.component().data.z;
  }

  auto& arrays = store.get<ExampleWithArrayCollection>("arrays");
  if (arrays.isValid() && arrays.size() != 0) {
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

  auto& nmspaces = store.get<ex42::ExampleWithARelationCollection>("WithNamespaceRelation");
  auto& copies = store.get<ex42::ExampleWithARelationCollection>("WithNamespaceRelationCopy");
  auto& cpytest = store.create<ex42::ExampleWithARelationCollection>("TestConstCopy");
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
        if (!(nmsp.ref().getObjectID() == cpy.ref().getObjectID())) {
          throw std::runtime_error("Copied item has wrong OneToOne references.");
        }
      }
      auto cpy_it = cpy.refs_begin();
      for (auto it = nmsp.refs_begin(); it != nmsp.refs_end(); ++it, ++cpy_it) {
        if (it->component().x != cpy_it->component().x || it->component().y != cpy_it->component().y) {
          throw std::runtime_error("Copied item has differing component in OneToMany referenced item.");
        }
        if (!(it->getObjectID() == cpy_it->getObjectID())) {
          throw std::runtime_error("Copied item has wrong OneToMany references.");
        }
      }
    }
  } else {
    throw std::runtime_error("Collection 'WithNamespaceRelation' and 'WithNamespaceRelationCopy' should be present");
  }

  if (fileVersion >= podio::version::Version{0, 13, 1}) {
    const auto& fixedWidthInts = store.get<ExampleWithFixedWidthIntegersCollection>("fixedWidthInts");
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
    auto& usrInts = store.get<podio::UserDataCollection<uint64_t>>("userInts");

    auto& uivec = usrInts.vec();
    int myInt = 0;
    for (int iu : uivec) {
      if (iu != myInt++) {
        throw std::runtime_error("Couldn't read userInts properly");
      }
    }

    myInt = 0;
    for (int iu : usrInts) {
      if (iu != myInt++) {
        throw std::runtime_error("Couldn't read userInts properly");
      }
    }

    auto& usrDbl = store.get<podio::UserDataCollection<double>>("userDoubles");
    for (double d : usrDbl) {
      if (d != 42.) {
        throw std::runtime_error("Couldn't read userDoubles properly");
      }
    }
  }
}

void run_read_test(podio::IReader& reader) {
  auto store = podio::EventStore();
  store.setReader(&reader);

  const auto nEvents = reader.getEntries();

  // Some information in the test files dpends directly on the index. In
  // read-multiple, the same file is essentially read twice, so that at the file
  // change the index which is used for testing has to start at 0 again. This
  // function just wraps that. The magic number of 2000 is the number of events
  // that are present in each file that is written in write
  const auto correctIndex = [nEvents](unsigned index) {
    if (nEvents > 2000) {
      return index % (nEvents / 2);
    }
    return index;
  };

  for (unsigned i = 0; i < nEvents; ++i) {

    if (i % 1000 == 0) {
      std::cout << "reading event " << i << std::endl;
    }

    processEvent(store, correctIndex(i), reader.currentFileVersion());
    store.clear();
    reader.endOfEvent();
  }
}

#endif // PODIO_TESTS_READ_TEST_H

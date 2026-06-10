// Exercises podio::FrameFilter (driven by the registered per-type FilterHooks)
// over Frames built from the test datamodel. Mirrors the hand-written prototype
// scenarios in filtering.cpp, plus the keep-referenced / orphan-sweep capability
// that only the Frame-level filter provides.

#include "datamodel/ExampleClusterCollection.h"
#include "datamodel/ExampleHitCollection.h"
#include "datamodel/ExampleMCCollection.h"
#include "datamodel/ExampleWithOneRelationCollection.h"
#include "datamodel/TestLinkCollection.h"

#include "podio/Frame.h"
#include "podio/FrameFilter.h"

#include <catch2/catch_test_macros.hpp>

#include <string>

namespace {

/// MC tree: 0 (E=10) -> {1 (E=5) -> 3 (E=0.5), 2 (E=1)}
podio::Frame makeMCFrame() {
  ExampleMCCollection mcs;
  auto p0 = mcs.create();
  p0.energy(10.0);
  auto p1 = mcs.create();
  p1.energy(5.0);
  auto p2 = mcs.create();
  p2.energy(1.0);
  auto p3 = mcs.create();
  p3.energy(0.5);
  p0.adddaughters(p1);
  p0.adddaughters(p2);
  p1.adddaughters(p3);
  p1.addparents(p0);
  p2.addparents(p0);
  p3.addparents(p1);

  podio::Frame frame;
  frame.put(std::move(mcs), "mcparticles");
  return frame;
}

/// hits h0(1) h1(2) h2(0.5); clusters c0->{h0,h1} (+sub-cluster c1), c1->{h2};
/// refs r0->c0, r1->c1; links l0: h0->c0, l1: h2->c1
podio::Frame makeEventFrame() {
  ExampleHitCollection hits;
  auto h0 = hits.create();
  h0.energy(1.0);
  auto h1 = hits.create();
  h1.energy(2.0);
  auto h2 = hits.create();
  h2.energy(0.5);

  ExampleClusterCollection clusters;
  auto c0 = clusters.create();
  c0.addHits(h0);
  c0.addHits(h1);
  auto c1 = clusters.create();
  c1.addHits(h2);
  c0.addClusters(c1);

  ExampleWithOneRelationCollection refs;
  auto r0 = refs.create();
  r0.cluster(c0);
  auto r1 = refs.create();
  r1.cluster(c1);

  TestLinkCollection links;
  auto l0 = links.create();
  l0.setFrom(h0);
  l0.setTo(c0);
  auto l1 = links.create();
  l1.setFrom(h2);
  l1.setTo(c1);

  podio::Frame frame;
  frame.put(std::move(hits), "hits");
  frame.put(std::move(clusters), "clusters");
  frame.put(std::move(refs), "refs");
  frame.put(std::move(links), "links");
  return frame;
}

const std::string linkType = std::string(TestLinkCollection::valueTypeName);

} // namespace

TEST_CASE("FrameFilter: self-relations, leave unconnected", "[framefilter]") {
  const auto in = makeMCFrame();
  const auto out = podio::FrameFilter{in}
                       .keep("mcparticles", [](const ExampleMC& p) { return p.energy() >= 1.0; })
                       .run();

  const auto& mcs = out.get<ExampleMCCollection>("mcparticles");
  REQUIRE(mcs.size() == 3);
  REQUIRE(mcs[0].daughters().size() == 2); // p0 keeps both surviving daughters
  REQUIRE(mcs[1].daughters().empty());     // p1's removed leaf leaves it unconnected
  REQUIRE(mcs[1].parents()[0] == mcs[0]);
}

TEST_CASE("FrameFilter: whole event, leave unconnected", "[framefilter]") {
  const auto in = makeEventFrame();
  const auto out = podio::FrameFilter{in}
                       .keep("hits", [](const ExampleHit& h) { return h.energy() >= 1.0; })
                       .run();

  const auto& hits = out.get<ExampleHitCollection>("hits");
  const auto& clusters = out.get<ExampleClusterCollection>("clusters");
  const auto& links = out.get<TestLinkCollection>("links");
  REQUIRE(hits.size() == 2);
  REQUIRE(clusters.size() == 2);
  // c0 keeps both hits and its sub-cluster; c1 lost its only hit but survives.
  REQUIRE(clusters[0].Hits().size() == 2);
  REQUIRE(clusters[0].Clusters().size() == 1);
  REQUIRE(clusters[1].Hits().empty());
  // l1's "From" hit was removed -> unset; weight and "To" intact.
  REQUIRE(links.size() == 2);
  REQUIRE_FALSE(links[1].getFrom().isAvailable());
  REQUIRE(links[1].getTo() == clusters[1]);
}

TEST_CASE("FrameFilter: integrity cascade across the whole graph", "[framefilter]") {
  const auto in = makeEventFrame();
  const auto out = podio::FrameFilter{in}
                       .keep("hits", [](const ExampleHit& h) { return h.energy() >= 1.0; })
                       .cascade("ExampleCluster.Hits")
                       .cascade("ExampleCluster.Clusters")
                       .cascade("ExampleWithOneRelation.cluster")
                       .cascade(linkType + ".from")
                       .cascade(linkType + ".to")
                       .run();

  // h2 removed -> c1 (used h2) -> c0 (sub-cluster c1) -> both refs and both links.
  REQUIRE(out.get<ExampleHitCollection>("hits").size() == 2);
  REQUIRE(out.get<ExampleClusterCollection>("clusters").empty());
  REQUIRE(out.get<ExampleWithOneRelationCollection>("refs").empty());
  REQUIRE(out.get<TestLinkCollection>("links").empty());
}

TEST_CASE("FrameFilter: orphan sweep keeps shared, drops truly-orphaned", "[framefilter]") {
  // Two cluster collections sharing one hit collection; h1 is shared.
  ExampleHitCollection hits;
  auto h0 = hits.create();
  h0.energy(1.0);
  auto h1 = hits.create();
  h1.energy(2.0);
  auto h2 = hits.create();
  h2.energy(3.0);

  ExampleClusterCollection clustersA;
  auto ca = clustersA.create();
  ca.addHits(h0);
  ca.addHits(h1);
  ExampleClusterCollection clustersB;
  auto cb = clustersB.create();
  cb.addHits(h1);
  cb.addHits(h2);

  podio::Frame in;
  in.put(std::move(hits), "hits");
  in.put(std::move(clustersA), "clustersA");
  in.put(std::move(clustersB), "clustersB");

  // Keep clustersA (default), drop everything in clustersB, sweep orphan hits.
  const auto out = podio::FrameFilter{in}
                       .keep("clustersB", [](const ExampleCluster&) { return false; })
                       .keepReferenced("hits")
                       .run();

  const auto& hitsOut = out.get<ExampleHitCollection>("hits");
  // h0 and h1 are referenced by the surviving clustersA; h2 was referenced only
  // by the dropped clustersB, so it is the only truly orphaned hit.
  REQUIRE(hitsOut.size() == 2);
  REQUIRE(out.get<ExampleClusterCollection>("clustersB").empty());
  REQUIRE(out.get<ExampleClusterCollection>("clustersA")[0].Hits().size() == 2);
}

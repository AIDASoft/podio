// Prototype: filtering objects out of a (self-referential) collection while
// handling the relations that point at the removed objects. This lives in
// user space on purpose - it is a worked example of what a filtering utility
// could do, built only on the public generated API (clone + relation adders)
// and podio::ObjectID. See ExampleMC (parents/daughters) as a stand-in for a
// sim-particle collection.

#include "datamodel/ExampleClusterCollection.h"
#include "datamodel/ExampleHitCollection.h"
#include "datamodel/ExampleMCCollection.h"
#include "datamodel/ExampleWithOneRelationCollection.h"
#include "datamodel/TestLinkCollection.h"

#include "podio/ObjectID.h"

#include <catch2/catch_test_macros.hpp>

#include <functional>
#include <unordered_map>
#include <unordered_set>

namespace {

/// What to do with a relation whose target is being removed.
enum class OnDangling {
  LeaveUnconnected, ///< drop the edge, leave the surviving object unconnected
  Cascade           ///< keep the target too (removal of the target is forbidden)
};

/// Filter an ExampleMCCollection by an arbitrary predicate, rewriting the
/// self-referential parents/daughters relations of the survivors.
///
/// The policy is chosen per relation: a Cascade relation forces the referenced
/// object to survive as well (computed as a closure), whereas a LeaveUnconnected
/// relation simply drops edges that point at removed objects.
ExampleMCCollection filterMC(const ExampleMCCollection& input,
                             const std::function<bool(const ExampleMC&)>& keep,
                             OnDangling parentsPolicy, OnDangling daughtersPolicy) {
  // Phase 1: survivor set, seeded by the predicate and then closed under the
  // relations that are configured to cascade. The fixpoint loop is safe in the
  // presence of cycles (e.g. a particle that is its own ancestor).
  std::unordered_set<podio::ObjectID> survive;
  for (auto particle : input) {
    if (keep(particle)) {
      survive.insert(particle.getObjectID());
    }
  }

  for (bool changed = true; changed;) {
    changed = false;
    for (auto particle : input) {
      if (!survive.count(particle.getObjectID())) {
        continue;
      }
      if (parentsPolicy == OnDangling::Cascade) {
        for (auto rel : particle.parents()) {
          changed |= survive.insert(rel.getObjectID()).second;
        }
      }
      if (daughtersPolicy == OnDangling::Cascade) {
        for (auto rel : particle.daughters()) {
          changed |= survive.insert(rel.getObjectID()).second;
        }
      }
    }
  }

  // Phase 2: clone the survivors without relations (data members only) and
  // record a mapping from the old ObjectID to the new, mutable handle.
  ExampleMCCollection output;
  std::unordered_map<podio::ObjectID, MutableExampleMC> remap;
  for (auto particle : input) {
    if (!survive.count(particle.getObjectID())) {
      continue;
    }
    auto cloned = particle.clone(/*cloneRelations=*/false);
    remap.emplace(particle.getObjectID(), cloned);
    output.push_back(cloned);
  }

  // Phase 3: rewire the relations, keeping only the edges whose target also
  // survived. For Cascade relations every target is guaranteed to be present
  // (it was pulled into the survivor set above), so no edge is ever dropped.
  for (auto particle : input) {
    const auto self = remap.find(particle.getObjectID());
    if (self == remap.end()) {
      continue;
    }
    auto newParticle = self->second;
    for (auto rel : particle.parents()) {
      if (const auto target = remap.find(rel.getObjectID()); target != remap.end()) {
        newParticle.addparents(target->second);
      }
    }
    for (auto rel : particle.daughters()) {
      if (const auto target = remap.find(rel.getObjectID()); target != remap.end()) {
        newParticle.adddaughters(target->second);
      }
    }
  }

  return output;
}

/// Build a small particle tree to filter:
///
///   0 (E=10) ─┬─ 1 (E=5) ── 3 (E=0.5)
///             └─ 2 (E=1)
ExampleMCCollection makeTree() {
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

  return mcs;
}

} // namespace

TEST_CASE("filter leaves dangling relations unconnected", "[filtering]") {
  const auto input = makeTree();

  // Keep everything with energy >= 1, i.e. drop the leaf particle (E=0.5).
  const auto output = filterMC(
      input, [](const ExampleMC& p) { return p.energy() >= 1.0; }, OnDangling::LeaveUnconnected,
      OnDangling::LeaveUnconnected);

  // Survivors keep their input order: 0, 1, 2.
  REQUIRE(output.size() == 3);
  const auto p0 = output[0];
  const auto p1 = output[1];
  const auto p2 = output[2];

  // p0 still points at both of its daughters, which now live in the new collection.
  REQUIRE(p0.daughters().size() == 2);
  REQUIRE(p0.daughters()[0] == p1);
  REQUIRE(p0.daughters()[1] == p2);

  // p1's daughter (the removed leaf) is gone, leaving p1 unconnected downward,
  // but its parent edge to the surviving p0 is intact.
  REQUIRE(p1.daughters().empty());
  REQUIRE(p1.parents().size() == 1);
  REQUIRE(p1.parents()[0] == p0);
  REQUIRE(p2.parents()[0] == p0);
}

TEST_CASE("filter cascades along daughters", "[filtering]") {
  const auto input = makeTree();

  // Seed with only the root (E=10); cascading daughters must pull in the whole
  // subtree below it.
  const auto output = filterMC(
      input, [](const ExampleMC& p) { return p.energy() >= 6.0; }, OnDangling::LeaveUnconnected,
      OnDangling::Cascade);

  REQUIRE(output.size() == 4);
  // The transitive daughter (E=0.5) survived and is still attached to its parent.
  REQUIRE(output[1].daughters().size() == 1);
  REQUIRE(output[1].daughters()[0] == output[3]);
}

TEST_CASE("filter cascades along parents while dropping other daughters", "[filtering]") {
  const auto input = makeTree();

  // Seed with only the leaf (E=0.5); cascading parents pulls in its ancestor
  // chain (1 then 0) but not the sibling (2).
  const auto output = filterMC(
      input, [](const ExampleMC& p) { return p.energy() <= 0.6; }, OnDangling::Cascade,
      OnDangling::LeaveUnconnected);

  // Survivors in input order: 0, 1, 3 (2 is removed).
  REQUIRE(output.size() == 3);
  const auto p0 = output[0];
  const auto p1 = output[1];
  const auto p3 = output[2];

  // p0 used to have daughters {1, 2}; 2 was removed, so only the edge to 1 remains.
  REQUIRE(p0.daughters().size() == 1);
  REQUIRE(p0.daughters()[0] == p1);
  // The cascaded parent chain stays connected all the way down to the leaf.
  REQUIRE(p1.daughters().size() == 1);
  REQUIRE(p1.daughters()[0] == p3);
  REQUIRE(p3.parents()[0] == p1);
}

// ---------------------------------------------------------------------------
// Cross-collection filtering: filter one collection and rewire the relations of
// *another* collection that references it. The key is that every collection gets
// its own ObjectID-keyed remap, and rewiring a relation simply looks the target
// up in the remap of the collection that owns the target type - so the same
// pattern works whether the relation points within a collection (parents/
// daughters above) or across collections (cluster -> hit here).

namespace {

template <typename MutableT>
using Remap = std::unordered_map<podio::ObjectID, MutableT>;

struct FilteredHits {
  ExampleHitCollection hits;
  ExampleClusterCollection clusters;
};

/// Filter ExampleHits by a predicate and rebuild the ExampleClusters that
/// reference them through their `Hits` relation.
///
/// The per-relation policy applies to the cluster -> hit edge: LeaveUnconnected
/// drops edges to removed hits and keeps the cluster; Cascade instead removes any
/// cluster that referenced a removed hit.
FilteredHits filterHits(const ExampleHitCollection& inHits, const ExampleClusterCollection& inClusters,
                        const std::function<bool(const ExampleHit&)>& keepHit, OnDangling clusterHitsPolicy) {
  // Phase 1: survivor sets. Hits are decided by the predicate alone (they own no
  // relations here). A cluster only fails to survive when the policy cascades and
  // one of its hits was removed.
  std::unordered_set<podio::ObjectID> hitSurvive;
  for (auto hit : inHits) {
    if (keepHit(hit)) {
      hitSurvive.insert(hit.getObjectID());
    }
  }

  std::unordered_set<podio::ObjectID> clusterSurvive;
  for (auto cluster : inClusters) {
    bool survives = true;
    if (clusterHitsPolicy == OnDangling::Cascade) {
      for (auto hit : cluster.Hits()) {
        if (!hitSurvive.count(hit.getObjectID())) {
          survives = false;
          break;
        }
      }
    }
    if (survives) {
      clusterSurvive.insert(cluster.getObjectID());
    }
  }

  // Phase 2: clone survivors of each collection, building one remap per collection.
  FilteredHits out;
  Remap<MutableExampleHit> hitRemap;
  for (auto hit : inHits) {
    if (!hitSurvive.count(hit.getObjectID())) {
      continue;
    }
    auto cloned = hit.clone(/*cloneRelations=*/false);
    hitRemap.emplace(hit.getObjectID(), cloned);
    out.hits.push_back(cloned);
  }
  Remap<MutableExampleCluster> clusterRemap;
  for (auto cluster : inClusters) {
    if (!clusterSurvive.count(cluster.getObjectID())) {
      continue;
    }
    auto cloned = cluster.clone(/*cloneRelations=*/false);
    clusterRemap.emplace(cluster.getObjectID(), cloned);
    out.clusters.push_back(cloned);
  }

  // Phase 3: rewire the cluster -> hit edges, resolving each hit through the hit
  // remap (a *different* collection's table). Edges to removed hits are dropped;
  // under Cascade the owning cluster is already gone, so none are.
  for (auto cluster : inClusters) {
    const auto self = clusterRemap.find(cluster.getObjectID());
    if (self == clusterRemap.end()) {
      continue;
    }
    auto newCluster = self->second;
    for (auto hit : cluster.Hits()) {
      if (const auto target = hitRemap.find(hit.getObjectID()); target != hitRemap.end()) {
        newCluster.addHits(target->second);
      }
    }
  }

  return out;
}

/// Build two clusters over four hits:
///   cluster 0 -> { hit0 (E=1), hit1 (E=2) }
///   cluster 1 -> { hit2 (E=3), hit3 (E=0.5) }
std::pair<ExampleHitCollection, ExampleClusterCollection> makeHitsAndClusters() {
  ExampleHitCollection hits;
  auto h0 = hits.create();
  h0.energy(1.0);
  auto h1 = hits.create();
  h1.energy(2.0);
  auto h2 = hits.create();
  h2.energy(3.0);
  auto h3 = hits.create();
  h3.energy(0.5);

  ExampleClusterCollection clusters;
  auto c0 = clusters.create();
  c0.addHits(h0);
  c0.addHits(h1);
  auto c1 = clusters.create();
  c1.addHits(h2);
  c1.addHits(h3);

  return {std::move(hits), std::move(clusters)};
}

} // namespace

TEST_CASE("cross-collection filter leaves dangling hit edges unconnected", "[filtering]") {
  const auto [inHits, inClusters] = makeHitsAndClusters();

  // Drop the low-energy hit (E=0.5); clusters survive but lose the dropped edge.
  const auto out = filterHits(
      inHits, inClusters, [](const ExampleHit& h) { return h.energy() >= 1.0; },
      OnDangling::LeaveUnconnected);

  REQUIRE(out.hits.size() == 3);
  REQUIRE(out.clusters.size() == 2);

  // cluster 0 keeps both of its hits; they now live in the new hit collection.
  REQUIRE(out.clusters[0].Hits().size() == 2);
  REQUIRE(out.clusters[0].Hits()[0] == out.hits[0]);
  // cluster 1 referenced the removed hit, so only the surviving one (E=3) remains.
  REQUIRE(out.clusters[1].Hits().size() == 1);
  REQUIRE(out.clusters[1].Hits()[0].energy() == 3.0);
}

TEST_CASE("cross-collection filter cascades from hit to cluster", "[filtering]") {
  const auto [inHits, inClusters] = makeHitsAndClusters();

  // Same hit removed, but now removing it cascades to the cluster that used it.
  const auto out = filterHits(
      inHits, inClusters, [](const ExampleHit& h) { return h.energy() >= 1.0; }, OnDangling::Cascade);

  REQUIRE(out.hits.size() == 3);
  // cluster 1 referenced the removed hit and is gone; cluster 0 is intact.
  REQUIRE(out.clusters.size() == 1);
  REQUIRE(out.clusters[0].Hits().size() == 2);
}

// ---------------------------------------------------------------------------
// The full prototype: filter across three collections at once, covering every
// relation kind - a cross-collection OneToMany (cluster -> hit), a self
// OneToMany (cluster -> sub-cluster) and a OneToOne (ref -> cluster) - with a
// removal closure that propagates across collections (hit -> cluster -> ref).

namespace {

struct EventPolicies {
  OnDangling clusterHits;     ///< cluster -> hit (OneToMany, cross-collection)
  OnDangling clusterClusters; ///< cluster -> sub-cluster (OneToMany, self)
  OnDangling refCluster;      ///< ref -> cluster (OneToOne, cross-collection)
  OnDangling linkFrom;        ///< link -> hit  (Link "From" endpoint)
  OnDangling linkTo;          ///< link -> cluster (Link "To" endpoint)
};

struct FilteredEvent {
  ExampleHitCollection hits;
  ExampleClusterCollection clusters;
  ExampleWithOneRelationCollection refs;
  TestLinkCollection links;
};

FilteredEvent filterEvent(const ExampleHitCollection& inHits, const ExampleClusterCollection& inClusters,
                          const ExampleWithOneRelationCollection& inRefs, const TestLinkCollection& inLinks,
                          const std::function<bool(const ExampleHit&)>& keepHit, EventPolicies policy) {
  using IdSet = std::unordered_set<podio::ObjectID>;

  // Phase 1: removal closure. The predicate seeds the removed hits; from there
  // removal propagates along every relation configured to cascade, across
  // collections, until the sets stop changing.
  IdSet hitsRemoved;
  for (auto hit : inHits) {
    if (!keepHit(hit)) {
      hitsRemoved.insert(hit.getObjectID());
    }
  }

  IdSet clustersRemoved;
  IdSet refsRemoved;
  for (bool changed = true; changed;) {
    changed = false;

    for (auto cluster : inClusters) {
      if (clustersRemoved.count(cluster.getObjectID())) {
        continue;
      }
      bool remove = false;
      if (policy.clusterHits == OnDangling::Cascade) {
        for (auto hit : cluster.Hits()) {
          remove |= static_cast<bool>(hitsRemoved.count(hit.getObjectID()));
        }
      }
      if (policy.clusterClusters == OnDangling::Cascade) {
        for (auto sub : cluster.Clusters()) {
          remove |= static_cast<bool>(clustersRemoved.count(sub.getObjectID()));
        }
      }
      if (remove) {
        changed |= clustersRemoved.insert(cluster.getObjectID()).second;
      }
    }

    for (auto ref : inRefs) {
      if (refsRemoved.count(ref.getObjectID()) || policy.refCluster != OnDangling::Cascade) {
        continue;
      }
      const auto cluster = ref.cluster();
      if (cluster.isAvailable() && clustersRemoved.count(cluster.getObjectID())) {
        changed |= refsRemoved.insert(ref.getObjectID()).second;
      }
    }
  }

  // Phase 2: clone survivors of every collection, one remap per collection.
  FilteredEvent out;
  Remap<MutableExampleHit> hitRemap;
  for (auto hit : inHits) {
    if (hitsRemoved.count(hit.getObjectID())) {
      continue;
    }
    auto cloned = hit.clone(/*cloneRelations=*/false);
    hitRemap.emplace(hit.getObjectID(), cloned);
    out.hits.push_back(cloned);
  }
  Remap<MutableExampleCluster> clusterRemap;
  for (auto cluster : inClusters) {
    if (clustersRemoved.count(cluster.getObjectID())) {
      continue;
    }
    auto cloned = cluster.clone(/*cloneRelations=*/false);
    clusterRemap.emplace(cluster.getObjectID(), cloned);
    out.clusters.push_back(cloned);
  }
  Remap<MutableExampleWithOneRelation> refRemap;
  for (auto ref : inRefs) {
    if (refsRemoved.count(ref.getObjectID())) {
      continue;
    }
    auto cloned = ref.clone(/*cloneRelations=*/false);
    refRemap.emplace(ref.getObjectID(), cloned);
    out.refs.push_back(cloned);
  }

  // Phase 3: rewire every relation, resolving each target through the remap of
  // the collection that owns it. OneToMany edges to removed targets are simply
  // skipped; a OneToOne to a removed target is left unset.
  for (auto cluster : inClusters) {
    const auto self = clusterRemap.find(cluster.getObjectID());
    if (self == clusterRemap.end()) {
      continue;
    }
    auto newCluster = self->second;
    for (auto hit : cluster.Hits()) {
      if (const auto target = hitRemap.find(hit.getObjectID()); target != hitRemap.end()) {
        newCluster.addHits(target->second);
      }
    }
    for (auto sub : cluster.Clusters()) {
      if (const auto target = clusterRemap.find(sub.getObjectID()); target != clusterRemap.end()) {
        newCluster.addClusters(target->second);
      }
    }
  }
  for (auto ref : inRefs) {
    const auto self = refRemap.find(ref.getObjectID());
    if (self == refRemap.end()) {
      continue;
    }
    const auto cluster = ref.cluster();
    if (cluster.isAvailable()) {
      if (const auto target = clusterRemap.find(cluster.getObjectID()); target != clusterRemap.end()) {
        self->second.cluster(target->second);
      }
    }
  }

  // Links live in their own collection and reference objects in two others. A
  // link is dropped when an endpoint was removed under a Cascade policy;
  // otherwise it is kept, its weight preserved, and each surviving endpoint
  // rewired through that endpoint's remap (a removed endpoint is left unset).
  // Links are terminal - nothing references them - so they need no remap.
  for (auto link : inLinks) {
    const auto from = link.getFrom();
    const auto to = link.getTo();
    const bool fromRemoved = from.isAvailable() && hitsRemoved.count(from.getObjectID());
    const bool toRemoved = to.isAvailable() && clustersRemoved.count(to.getObjectID());
    if ((fromRemoved && policy.linkFrom == OnDangling::Cascade) ||
        (toRemoved && policy.linkTo == OnDangling::Cascade)) {
      continue;
    }
    auto newLink = link.clone(/*cloneRelations=*/false); // copies the weight only
    if (from.isAvailable()) {
      if (const auto target = hitRemap.find(from.getObjectID()); target != hitRemap.end()) {
        newLink.setFrom(target->second);
      }
    }
    if (to.isAvailable()) {
      if (const auto target = clusterRemap.find(to.getObjectID()); target != clusterRemap.end()) {
        newLink.setTo(target->second);
      }
    }
    out.links.push_back(newLink);
  }

  return out;
}

/// Build a three-level event to filter:
///   hits:     h0 (E=1)  h1 (E=2)  h2 (E=0.5)
///   clusters: c0 -> {h0, h1}        c1 -> {h2},  c0 also has sub-cluster c1
///   refs:     r0 -> c0              r1 -> c1
///   links:    l0: h0 -> c0          l1: h2 -> c1
FilteredEvent makeEvent() {
  FilteredEvent ev;
  auto h0 = ev.hits.create();
  h0.energy(1.0);
  auto h1 = ev.hits.create();
  h1.energy(2.0);
  auto h2 = ev.hits.create();
  h2.energy(0.5);

  auto c0 = ev.clusters.create();
  c0.addHits(h0);
  c0.addHits(h1);
  auto c1 = ev.clusters.create();
  c1.addHits(h2);
  c0.addClusters(c1);

  auto r0 = ev.refs.create();
  r0.cluster(c0);
  auto r1 = ev.refs.create();
  r1.cluster(c1);

  auto l0 = ev.links.create();
  l0.setFrom(h0);
  l0.setTo(c0);
  l0.setWeight(1.0f);
  auto l1 = ev.links.create();
  l1.setFrom(h2);
  l1.setTo(c1);
  l1.setWeight(2.0f);

  return ev;
}

} // namespace

TEST_CASE("full filter leaves all dangling relations unconnected", "[filtering]") {
  const auto in = makeEvent();

  // Drop h2 (E=0.5). With every policy LeaveUnconnected, nothing else is removed;
  // only the edges to h2 disappear.
  const auto out = filterEvent(
      in.hits, in.clusters, in.refs, in.links, [](const ExampleHit& h) { return h.energy() >= 1.0; },
      {OnDangling::LeaveUnconnected, OnDangling::LeaveUnconnected, OnDangling::LeaveUnconnected,
       OnDangling::LeaveUnconnected, OnDangling::LeaveUnconnected});

  REQUIRE(out.hits.size() == 2);
  REQUIRE(out.clusters.size() == 2);
  REQUIRE(out.refs.size() == 2);

  // c0 keeps both hits and its sub-cluster; c1 lost its only hit but survives.
  REQUIRE(out.clusters[0].Hits().size() == 2);
  REQUIRE(out.clusters[0].Clusters().size() == 1);
  REQUIRE(out.clusters[0].Clusters()[0] == out.clusters[1]);
  REQUIRE(out.clusters[1].Hits().empty());
  // OneToOne edges still point at the surviving clusters.
  REQUIRE(out.refs[1].cluster() == out.clusters[1]);

  // Both links survive; l1's "From" hit was removed, so that endpoint is unset
  // while its "To" cluster and the preserved weight remain intact.
  REQUIRE(out.links.size() == 2);
  REQUIRE(out.links[0].getFrom() == out.hits[0]);
  REQUIRE(out.links[0].getTo() == out.clusters[0]);
  REQUIRE_FALSE(out.links[1].getFrom().isAvailable());
  REQUIRE(out.links[1].getTo() == out.clusters[1]);
  REQUIRE(out.links[1].getWeight() == 2.0f);
}

TEST_CASE("full filter cascades across all three collections", "[filtering]") {
  const auto in = makeEvent();

  // Drop h2 (E=0.5) and cascade every relation: removing h2 removes c1 (it used
  // h2), removing c1 removes both c0 (its sub-cluster is gone) and r1 (its
  // cluster is gone), and removing c0 removes r0.
  const auto out = filterEvent(
      in.hits, in.clusters, in.refs, in.links, [](const ExampleHit& h) { return h.energy() >= 1.0; },
      {OnDangling::Cascade, OnDangling::Cascade, OnDangling::Cascade, OnDangling::Cascade,
       OnDangling::Cascade});

  REQUIRE(out.hits.size() == 2);
  REQUIRE(out.clusters.empty());
  REQUIRE(out.refs.empty());
  // l0's "To" cluster (c0) and l1's "From" hit (h2) were both removed under a
  // cascading endpoint policy, so both links are dropped.
  REQUIRE(out.links.empty());
}

TEST_CASE("full filter cascades to cluster but leaves its ref unconnected", "[filtering]") {
  const auto in = makeEvent();

  // Cascade hit -> cluster, but leave ref -> cluster unconnected: c1 is removed,
  // and r1 survives with its OneToOne relation left unset.
  const auto out = filterEvent(
      in.hits, in.clusters, in.refs, in.links, [](const ExampleHit& h) { return h.energy() >= 1.0; },
      {OnDangling::Cascade, OnDangling::LeaveUnconnected, OnDangling::LeaveUnconnected,
       OnDangling::LeaveUnconnected, OnDangling::LeaveUnconnected});

  // c1 (used h2) is gone; c0's sub-cluster edge to it is dropped but c0 survives.
  REQUIRE(out.clusters.size() == 1);
  REQUIRE(out.clusters[0].Hits().size() == 2);
  REQUIRE(out.clusters[0].Clusters().empty());

  // Both refs survive; r1's cluster was removed so its relation is now unset.
  REQUIRE(out.refs.size() == 2);
  REQUIRE(out.refs[0].cluster() == out.clusters[0]);
  REQUIRE_FALSE(out.refs[1].cluster().isAvailable());

  // Both links survive under the leave-unconnected endpoint policies. l1 lost
  // both endpoints (h2 removed, c1 cascaded away), so it is fully unset.
  REQUIRE(out.links.size() == 2);
  REQUIRE(out.links[0].getFrom() == out.hits[0]);
  REQUIRE(out.links[0].getTo() == out.clusters[0]);
  REQUIRE_FALSE(out.links[1].getFrom().isAvailable());
  REQUIRE_FALSE(out.links[1].getTo().isAvailable());
}

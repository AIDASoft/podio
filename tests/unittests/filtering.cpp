// Prototype: filtering objects out of a (self-referential) collection while
// handling the relations that point at the removed objects. This lives in
// user space on purpose - it is a worked example of what a filtering utility
// could do, built only on the public generated API (clone + relation adders)
// and podio::ObjectID. See ExampleMC (parents/daughters) as a stand-in for a
// sim-particle collection.

#include "datamodel/ExampleMCCollection.h"

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

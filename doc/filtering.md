# Filtering Frames

`podio::FrameFilter` produces a new `Frame` from an existing one by selecting a
subset of the objects in its collections and **rewiring all relations** so that
the result is internally consistent. It is the right tool when you want to drop
some objects (e.g. low-energy hits, or MC particles outside a region of
interest) while keeping the surviving relation graph valid.

Filtering is inherently a *Frame-level* operation: because relations cross
collections, dropping an object in one collection can leave dangling references
in another. `FrameFilter` therefore rebuilds every collection of the input
`Frame` and returns a new, fully-rewired `Frame`.

## Basic usage

```cpp
#include "podio/FrameFilter.h"

podio::Frame out = podio::FrameFilter{inFrame}
    .keep("MCParticles", [](const ExampleMC& p) { return p.energy() > 1.0; })
    .keepReferenced("hits")
    .cascade("ExampleCluster.Hits")
    .run();
```

- `keep(name, predicate)` — keep only the objects of the named collection for
  which `predicate` returns `true`. The collection type is deduced from the
  predicate's argument type.
- `keepReferenced(name)` — keep an object of the named collection only if some
  surviving object still references it (an *orphan sweep*).
- `drop(name)` — remove the named collection entirely; it is omitted from the
  output Frame. Unlike `keep`, this needs only the name, not the collection's
  type. (To keep an empty collection instead, use an always-false `keep`
  predicate.)
- `cascade("Type.relation")` — mark a relation as mandatory (see below).
- `run()` — execute and return the new `Frame`.

Collections of the input `Frame` that are not mentioned are kept in full, but
are still rebuilt so their relations into filtered collections are corrected.
Frame parameters are copied over verbatim.

## The two axes

The behaviour is controlled by two orthogonal axes.

### Retention mode (per collection)

How an object earns its place in the output:

- **keep-all** (default) — every object survives.
- **predicate** (`keep`) — an object survives iff the predicate holds.
- **drop-all** (`drop`) — no object survives *and* the collection is omitted from
  the output Frame. Type-free, since it never inspects an object. (An always-false
  `keep` predicate is the variant that leaves an empty collection in place.)
- **keep-if-referenced** (`keepReferenced`) — an object survives iff a surviving
  object references it. This is an orphan sweep, computed as a mark-and-sweep
  reachability from the surviving *roots* (the predicate/keep-all survivors).

Orphan sweep is shared-aware: if two collections reference the same `hit` and
only one of the referrers survives, the `hit` is kept. That is precisely why
`keepReferenced` is preferred over a "containment cascade" (deleting an object
deletes everything it points at) — the latter would wrongly drop objects that
are still shared with a survivor.

### Edge policy (per relation)

What happens to a single relation whose target was removed:

- **leave-unconnected** (default) — drop the dangling edge. A
  `OneToManyRelation` simply omits the removed target; a `OneToOneRelation` is
  left unset.
- **cascade** (`cascade("Type.relation")`) — treat the relation as mandatory:
  if the target is removed, remove the object that holds the relation as well
  (*integrity cascade*). This propagates transitively across collections.

A relation is named `<value-type-name>.<relation-name>`, e.g.
`ExampleCluster.Hits` or `ExampleMC.daughters`. For links the value type name is
the templated `podio::Link<From, To>` name, so a link endpoint is cascaded as
e.g. `cascade(std::string(MyLinkCollection::valueTypeName) + ".to")`.

## How `run()` works

The orchestration is type-agnostic and proceeds in phases:

1. **Seed** — apply the predicates to mark the initially-removed objects.
2. **Cascade closure** — for every relation flagged `cascade`, if its target is
   removed, remove the holder; iterate to a fixpoint (cycle-safe).
3. **Mark** — forward reachability from the surviving roots, marking the objects
   that keep-referenced collections should retain.
4. **Sweep** — remove unmarked objects from keep-referenced collections.
5. **Subset fix-up** — a subset collection only references objects owned
   elsewhere, so drop any entry whose target was removed.
6. **Clone & remap** — clone the surviving objects (data members only) into new
   collections and record an `ObjectID → new location` map.
7. **Rewire** — walk every relation of every survivor and re-point it through
   the remap; targets that were removed are dropped (or already cascaded away).
8. **Assemble** — put the rebuilt collections into a new `Frame` and copy the
   parameters. Collections marked `drop` are skipped here, so they do not appear
   in the output `Frame` at all.

## Design: a generic core plus generated hooks

The orchestration above is entirely type-erased — `FrameFilter` only ever sees
`podio::CollectionBase`. The parts that genuinely need concrete type knowledge
are generated once per datatype and registered, mirroring the
`CollectionBufferFactory` pattern:

- `podio::detail::FilterHooks` (in `podio/detail/FilterHooks.h`) is the
  per-datatype interface with three operations:
  - `edges` — report an object's outgoing relation targets as `ObjectID`s
    (used by the cascade and mark phases).
  - `cloneSurvivors` — clone the surviving objects into a fresh collection,
    copying data members and vector members but no relations.
  - `rewire` — re-point the cloned survivors' relations through the remap.
- `podio::detail::FilterHookRegistry` is the singleton (one per process, living
  in the core library) that maps a datatype name to its hooks. Generated code
  registers into it at library-load time.
- `podio::detail::resolveTarget<CollT>` looks a remapped target up and returns
  the concrete handle to hand to a setter/adder.

For each datatype and link the generator emits a `<Type>FilterHooks.cc` that
implements and registers these hooks. The implementation is uniform: each
relation carries a *resolver expression*, so the generated code does not branch
on whether a relation is concrete or an interface.

### Interface relations

A relation to an interface type is polymorphic — its target may live in any of
the interface's member collections. Because the remap is keyed by `ObjectID`
(type-agnostic), the target can always be located; recovering the concrete type
to build the interface handle is done by a generated resolver, one per
interface:

```cpp
std::optional<EnergyInterface>
EnergyInterface::resolveFilteredTarget(const podio::detail::FilterRemap& remap, podio::ObjectID id);
```

It dispatches on the target collection's `getValueTypeName()` over the
interface's member types and constructs the interface from the concrete handle.
It is declared on the interface (in the generated interface header) and defined
once in a generated `<Interface>FilterResolver.cc`, so every relation that uses
the interface shares a single resolver.

## Limitations

- Interface relations are resolved by a linear match over the interface's member
  types (one comparison per candidate concrete type).
- Subset *link* collections are not handled; link collections are assumed to own
  their endpoints.
- A subset collection's synthetic membership edge has no relation name, so it
  cannot be targeted by `cascade(...)`.

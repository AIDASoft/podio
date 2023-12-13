# Writing extra data outside an EDM
In some cases it can be necessary to write some additional data that can not
(yet) be stored in the EDM. PODIO offers a possibility to store such extra *user
data* via the `podio::UserDataCollection`. It gives the user access to a
`std::vector` of a set of limited fundamental types, which is written alongside
the data stored in the EDM classes for each event.

## Example usage
Creating or getting a `UserDataCollection` via the `Frame` works the same
as with any other collection of the EDM via the `put` or `get` functions:

```cpp
#include "podio/UserDataCollection.h"

// Create a collection and put it into a Frame
userFloats = podio::UserDataCollection<float>();
frame.put(std::move(userFloats), "userFloats");

// get a collection
const auto& userData = frame.get<podio::UserDataCollection<float>>("userFloats");
```

The interface of the `UserDataCollection` is similar to a basic version of the
`std::vector`, i.e. `push_back`, `resize` and the basic functions for accessing
elements are present:

```cpp
userFloats.push_back(3.14f); // add elements
userFloats.resize(10);       // make the collection have 10 elements
auto value = userFloats[0];  // access elements by index

for (auto&& value : userFloats) {
    // necessary functionality for range-based for-loops
}

for (size_t i = 0; i < userFloats.size(); ++i) {i
    // but also for index based loops
}
```

For cases where this subset interface is not enough it is also possible to get a
reference to the underlying `std::vector` via the `UserDataCollection::vec()`
method.

## Some limitations
Since adding additional fields to an EDM type is almost trivial for PODIO
generated EDMs the `UserDataCollection` capabilities are deliberately kept
limited. Here we list what we consider to be the biggest limitations:

### No relations to other collections
Since a `UserDataCollection` is really just a list of numbers it is not possible
to form relations to other objects. This also means that users are responsible
for keeping a `UserDataCollection` in sync with an EDM collection if is used to
store additional information, e.g.
```cpp
auto& hits = store.create<HitCollection>("hits");
auto& hitTags = store.create<podio::UserDataCollection<uint16_t>>("hitTags");

for (/* some loop */) {
    auto hit = hits.create();
    // To be able to loop over these in parallel, you have to fill the user data manually
    uint16_t hitTag;
    hitTags.push_back(hitTag);
}
```

### Limited supported types
As mentioned above the possible types that can be stored in a
`UserDataCollection` is currently (deliberately) limited to the following
fundamental types

- fixed width integers: `intN_t` and `uintN_t`, with `N={8, 16, 32, 64}`
- `float` and `double`

Trying to store different types in a `UserDataCollection` will lead to a
compilation error. Note that the usual integer types might be the same as the
fixed width version on some platforms. However, the equivalences of different
types might be different on different platforms. E.g.
```cpp
static_assert(std::is_same_v<unsigned long, uint64_t>);
```
might work on some platforms, but might not work on others.

# The `Frame` concept
The `podio::Frame` is a general data container for collection data of podio generated EDMs.
Additionally, it offers the functionality to store some (limited) data outside of an EDM.
The basic idea of the `Frame` is to give users of podio the possibility to organize EDM data into logical units and to potentially build a hierarchy of different `Frame`s.
Common examples would be the organisation of data into *Events* and *Runs*.
However, it is important to note that podio does really not impose any meaning on any `Frame` and each `Frame` is essentially defined by its contents.

## Basic functionality of a `Frame`
The main functionality of a `Frame` is to store and aggregate EDM collection data and it also offers the possibility to store some generic data alongside.
To ensure thread safety and const-correctness a `Frame` takes ownership of any data that is put into it and only gives read access to immutable data.
This is mandated by the interface for collection data (simplified here for better readability):
```cpp
struct Frame {
template<typename CollT>
const CollT& put(CollT&& coll, const std::string& name);

void put(std::unique_ptr<podio::CollectionBase> coll, const std::string& name);

template<typename CollT>
const CollT& get(const std::string& name) const;

template<typename T>
void putParameter(const std::string& name, T value);

template<typename T>
const T& getParameter(const std::string);
};
```
In this case there are two ways to get collection data into the `Frame`
1. By passing a concrete collection (of type `CollT`) into the `Frame` as an [`rvalue`](https://en.cppreference.com/w/cpp/language/value_category). There are two ways to achieve this, either by passing the return value of a function directly into `Frame::put` or by explicitly moving it in the call via `std::move` if you are using a named variable.
2. By passing a `std::unique_ptr` to a collection. Similar to the first case, this can either be the return value of a function call, or has to be done via `std::move` (as mandated by the `std::unique_ptr` interface).

In both cases, if you passed in a named variable, the user is left with a *moved-from object*, which has to be in a *valid but indefinite* state, and cannot be used afterwards.
Some compilers and static code analysis tools are able to detect the accidental usage of *moved-from* objects.

For putting in parameters the basic principle is very similar, with the major difference being, that for *trivial* types `getParameter` will actually return by value.

For all use cases there is a concept requirement to ensure that only valid collections and valid parameter types can actually be used.
Additional checks also make sure that it is impossible to put in collections without handing over ownership to the `Frame`.

### Usage examples for collection data
These are a few very basic usage examples that highlight the main functionality (and potential pitfalls).

#### Putting collection data into the `Frame`
In all of the following examples, the following basic setup is assumed:
```cpp
#include "podio/Frame.h"

#include "edm4hep/MCParticleCollection.h" // just to have a concrete example

// create an empty Frame
auto frame = podio::Frame();
```

Assuming there is a function that creates an `MCParticleCollection` putting the return value into the `Frame` is very simple
```cpp
edm4hep::MCParticleCollection createMCParticles(); // implemented somewhere else

// put the return value of a function into the Frame
frame.put(createMCParticles(), "particles");

// put the return value into the Frame but keep the const reference
auto& particles = frame.put(createMCParticles(), "moreParticles");
```

If working with named variables it is necessary to use `std::move` to put collections into the `Frame`.
The `Frame` will refuse to compile in case a named variable is not moved.
Assuming the same `createMCParticles` function as above, this looks like the following

```cpp
auto coll = createMCParticles();
// potentially still modify the collection

// Need to use std::move now that the collection has a name
frame.put(std::move(coll), "particles");

// Keeping a const reference is also possible
// NOTE: We are explicitly using a new variable name here
auto coll2 = createMCParticles();
auto& particles = frame.put(std::move(coll2), "MCParticles");
```
At this point only `particles` is in a valid and **defined** state.

#### Getting collection (references) from the `Frame`
Obtaining immutable (`const`) references to collections stored in the `Frame` is trivial.
Here we are assuming that the collections are actually present in the `Frame`.
```cpp
auto& particles = frame.get<edm4hep::MCParticleCollection>("particles");
```

### Usage for Parameters
Parameters are using the `podio::GenericParameters` class behind the scene.
Hence, the types that can be used are `int`, `float`, and `std::string` as well as as `std::vectors` of those.
For better usability, some overloads for `putParameter` exist to allow for an *in-place* construction, like, e.g.
```cpp
// Passing in a const char* for a std::string
frame.putParameter("aString", "a string value");

// Creating a vector of ints on the fly
frame.putParameter("ints", {1, 2, 3, 4});
```

## I/O basics and philosophy
podio offers all the necessary functionality to read and write `Frame`s.
However, it is not in the scope of podio to organize them into a hierarchy, nor
to maintain such a hierarchy. When writing data to file `Frame`s are written to
the file in the order they are passed to the writer. For reading them back podio
offers random access to stored `Frame`s, which should make it possible to
restore any hierarchy again. The Writers and Readers of podio are supposed to be
run on and accessed by only one single thread.

### Writing a `Frame`
For writing a `Frame` the writers can ask each `Frame` for `CollectionWriteBuffers` for each collection that should be written.
In these buffers the underlying data is still owned by the collection, and by extension the `Frame`.
This makes it possible to write the same collection with several different writers.
Writers can access a `Frame` from several different threads, even though each writer is assumed to be on only one thread.
For writing the `GenericParameters` that are stored in the `Frame` and for other necessary data, similar access functionality is offered by the `Frame`.

### Reading a `Frame`
When reading a `Frame` readers do not have to return a complete `Frame`.
Instead they return a more or less arbitrary type of `FrameData` that simply has to provide the following public interface.
```cpp
struct FrameData {
  /// Get a (copy) of the internal collection id table
  podio::CollectionIDTable getIDTable() const;

  /// Get the buffers to construct the collection with the given name
  std::optional<podio::CollectionReadBuffers> getCollectionBuffers(const std::string& name);

  /// Get the still available, i.e. yet unpacked, collections from the raw data
  std::vector<std::string> getAvailableCollections() const;

  /// Get the parameters that are stored in the raw data
  std::unique_ptr<podio::GenericParameters> getParameters();
};
```
A `Frame` is constructed with a (`unique_ptr` of such) `FrameData` and handles everything from there.
Note that the `FrameData` type of any I/O backend is a free type without inheritance as the `Frame` constructor is templated on this.
This splitting of reading data from file and constructing a `Frame` from it later has some advantages:
- Since podio assumes that reading is done single threaded the amount of time that is actually spent in a reader is minimized, as only the file operations need to be done on a single thread. All further processing (potential decompression, unpacking, etc.) can be done on a different thread where the `Frame` is actually constructed.
- It gives different backends the necessary freedom to exploit different optimization strategies and does not force them to conform to an implementation that is potentially detrimental to performance.
- It also makes it possible to pass around data from which a `Frame` can be constructed without having to actually construct one.
- Readers do not have to know how to construct collections from the buffers, as they are only required to provide the buffers themselves.

### Schema evolution
Schema evolution happens on the `CollectionReadBuffers` when they are requested from the `FrameData` inside the `Frame`.
It is possible for the I/O backend to handle schema evolution before the `Frame` sees the buffers for the first time.
In that case podio schema evolution becomes a simple check.

# Frame implementation and design
One of the main concerns of the `Frame` is to offer one common, non-templated, interface while still supporting different I/O backends and potentially different *policies*.
The "classic" approach would be to have an abstract `IFrame` interface with several implementations that offer the desired functionality (and their small differences).
One problem with that approach is that a purely abstract interface cannot have templated member functions. Hence, the desired type-safe behavior of `get` and `put` would be very hard to implement.
Additionally, policies ideally affect orthogonal aspects of the `Frame` behavior.
Implementing all possible combinations of behaviors through implementations of an abstract interface would lead to quite a bit of code duplication and cannot take advantage of the factorization of the problem.
To solve these problems, we chose to implement the `Frame` via [*Type Erasure*](https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Type_Erasure).
This also has the advantage that the `Frame` also has *value semantics* in line with the design of podio.

## Supported Interface

The data model library supports various use cases. The following snippets should serve as examples.

### Object creation and storage

Objects and collections can be created via factories, which ensure proper object ownership:

    auto& hits = store.create<HitCollection>("hits")
    auto hit1 = hits.create(1.4,2.4,3.7,4.2);  // init with values
    auto hit2 = hits.create(); // default-construct object
    hit2.energy(42.23);

In addition, individual objects can be created in the free. If they aren't attached to a collection, they are automatically garbage-collected:

    auto hit1 = Hit();
    auto hit2 = Hit();
    ...
    hits.push_back(hit1);
    ...
    <automatic deletion of hit2>

In this respect all objects behave like objects in Python.

### Object references

The library supports the creation of one-to-many relations:

    auto& hits = store.create<HitCollection>("hits");
    auto hit1 = hits.create();
    auto hit2 = hits.create();
    auto& clusters = store.create<ClusterCollection>("clusters");
    auto cluster = clusters.create();
    cluster.addHit(hit1);
    cluster.addHit(hit2);

The references can be accessed via iterators on the referencing objects

    for (auto i = cluster.Hits_begin(), \
         end = cluster.Hits_end(); i!=end; ++i){
      std::cout << i->energy() << std::endl;
    }

or via direct accessors

    auto size = cluster.Hits_size();
    auto hit  = cluster.Hits(<aNumber>);

If asking for an entry outside bounds, a std::out_of_range exception is thrown.


### Object retrieval

Collections can be retrieved explicitly:

    const HitCollection* hits;
    bool success = store.get("hits",hits);

Or implicitly when following an object reference. In both cases the access to data that has been retrieved is `const`.


### Looping through collections
Looping through collections is supported in two ways. Via iterators:

    for(auto i = hits.begin(), end = hits.end(); i != end; ++i) {
      std::cout << i->energy() << std::endl;
    }

and via direct object access:

    for(int i = 0, end = hits.size(), i != end, ++i){
      std::cout << hit[i].energy() << std::endl;
    }


### Support for Notebook-Pattern

The `notebook pattern` uses the assumption that it is better to create a small
copy of only the data that are needed for a particular calculation. This
pattern is supported by providing access like

    auto x_array = hits.x<10>(); // returning values of
    auto y_array = hits.y<10>(); // the first 10 elements

The resulting `std::array` can then be used in (auto-)vectorizable code.
If less objects than requested are contained in the collection, the remaining entries are default initialized.


### Object Ownership

Every data created is either explicitly owned by collections or automatically garbage-collected. There is no need for any `new` or `delete` call on user side.

### EventStore functionality

The event store contained in the package is for educational purposes and kept very minimal. It has two main methods:

    /// create a new collection
    template<typename T>
    T& create(const std::string& name);

    /// access a collection. returns true if successful
    template<typename T>
    bool get(const std::string& name, const T*& collection);

Please note that a `put` method for collections is not foreseen.

#### Python Interface

The class `EventStore` provides all the necessary (read) access to event files.


# Technical Details

The following sections give some more technical details and explanations for the design choices.

## Layout of Objects
The data model is based on four different kind of objects:
 1. User visible (physics) classes (e.g. `Hit`). These act as transparent references to the underlying data.
 2. A transient object knowing about all data for a certain physics object, including inter-object references (e.g. `HitObject`).
 3. A plain-old-data (POD) type holding the persistent object information (e.g. `HitData`)
 4. A user-visible collection containing the physics objects (e.g. `HitCollection`)

More details to be written.

## Thread-safety and multi-store support
The design avoids the usage of globals and statics. Thus multiple instances of the EventStore and Registry can trivially co-exist.
On-demand reading is limited to the level of the event store. This allows frameworks to do a full pre-fetch of all necessary data before passing control to the user.

Neither DataStore nor Registry are thread-safe at the moment.


## Persistency

The library is build such that it can support multiple storage backends. However, the tested storage system being used is ROOT.


### Adding a new writing back-end

There is no interface a writing class has to fulfill. It only needs to take advantage of the interfaces provided.
On writing out a collection, the method "prepareForWrite" is being invoked. In the case of trivial POD members this results in a no-op. In case, the collection contains references to other collections, the pointers are being translated into collID:objIndex pairs.

future: The data corresponding to a single collection is stored in a `CollectionBuffer` object, simply containing vectors of PODs.

In addition, the name-to-collectionID information in the `Registry` has to be stored alongside the data.

### Adding a new reading back-end

To support following of object references the data reader has to implement the `ICollectionProvider` interface.

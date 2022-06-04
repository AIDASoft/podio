# Advanced Topics

## Writing extra data outside the EDM
[See here](userdata.md)

## Changing / creating new templates
[See here](templates.md)

## Persistency

The library is build such that it can support multiple storage backends. However, the tested storage system being used is ROOT.
The following explains the requirements for a user-provided storage back-end.

### Writing Back-End

There is no interface a writing class has to fulfill. It only needs to take advantage of the interfaces provided in PODIO. To persistify a collection, three pieces of information have to be stored:

  1. the ID of the collection,
  2. the vector of PODs in the collection, and
  3. the relation information in the collection

Before writing out a collection, the data need to be put into the proper structure. For this, the method `prepareForWrite` needs to be invoked. In the case of trivial POD members this results in a no-op. In case, the collection contains references to other collections, the pointers are being translated into `collID:objIndex` pairs. A serialization solution would - in principle - then look like this:

```cpp
     collection->prepareForWrite();
     void* buffer = collection->getBufferAddress();
     auto refCollections = collection->referenceCollections();
     // ...
     //   write buffer, collection ID, and refCollections
     // ...
```

### Reading Back-End

There are two possibilities to implement a reading-back end. In case one uses the `podio::EventStore`, one simply has to implement the `IReader` interface.

If not taking advantage of this implementation, the data reader or the event store have to implement the `ICollectionProvider` interface. Reading of a collection happens then similar to:

```cpp
    // ...
    // your creation of the collection and reading of the PODs from disk
    // ...
    collection->setBuffer(buffer);
    auto refCollections = collection->referenceCollections();
    // ...
    // your filling of refCollections from disk
    // ...
    collection->setID( <collection ID read from disk> );
    collection->prepareAfterRead();
    // ...
    collection->setReferences( &collectionProvider );
```

The strong assumption here is that all references are being followed up directly and no later on-demand reading is done.

## Thread-safety

PODIO was written with thread-safety in mind and avoids the usage of globals and statics.
However, a few assumptions about user code and use-patterns were made.
The following lists the caveats of the library when it comes to parallelization.

### Changing user data

As explained in the section about mutability of data, thread-safety is only guaranteed if data are considered read-only after creation.

### Serialization
During the calls of `prepareForWriting` and `prepareAfterReading` on collections other operations like object creation or addition will lead to an inconsistent state.

### Not-thread-safe components
The example event store provided with PODIO is as of writing not thread-safe. Neither is the chosen serialization.

## Implementing a transient Event Class

PODIO contains one example `podio::EventStore` class.
To implement your own transient event store, the only requirement is to set the collectionID of each collection to a unique ID on creation.

## Running pre-commit

 - Install [homebrew](https://brew.sh/)

 - Install pre-commit

    `$ brew install pre-commit`

 - Run pre-commit manually

    `$ pre-commit run --all-files`

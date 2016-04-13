# Introduction

PODIO, or plain-old-data I/O, is a C++ library to support the creation and handling of data models in particle physics. It is based on the idea of employing plain-old-data (POD) data structures wherever possible, while avoiding deep-object hierarchies and virtual inheritance. This is to both improve runtime performance and simplify the implementation of persistency services.

At the same time it provides the necessary high-level functionality to the physicist, such as support for inter-object relations, and automatic memory-management. In addition, it provides a (ROOT assisted) Python interface. To simplify the creation of efficient data models, PODIO employs code generation from a simple yaml-based markup syntax.

To support the usage of modern software technologies, PODIO was written with concurrency in mind and gives basic support for vectorization technologies.

This document describes first how to define and create your own data model, then how to use the created data. 

In the 

# Quick-start

An up-to-date installation and quick start guide for the impatient user can be found on the [PODIO github page](https://github.com/hegner/podio).

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


## Thread-safety and multi-store support
The design avoids the usage of globals and statics. Thus multiple instances of the EventStore and Registry can trivially co-exist.
On-demand reading is limited to the level of the event store. This allows frameworks to do a full pre-fetch of all necessary data before passing control to the user.

Neither DataStore nor Registry are thread-safe at the moment.


## Persistency

The library is build such that it can support multiple storage backends. However, the tested storage system being used is ROOT.

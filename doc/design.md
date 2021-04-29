# Design and Implementation Details

The driving considerations for the PODIO design are:

  1. the concrete data are contained within plain-old-data structures (PODs)
  1. user-exposed data types are concrete and do not use inheritance
  1. The C++ and Python interface should look as close as possible
  1. The user does not do any explicit memory management
  1. Classes are generated using a higher-level abstraction and code generators

The following sections give some more technical details and explanations for the design choices.
More concrete implementation details can be found in the doxygen documentation.

## Layout of Objects
The data model is based on four different kind of objects and layers, namely

 1. user visible (physics) classes (e.g. `Hit`). These act as transparent references to the underlying data,
 2. a transient object knowing about all data for a certain physics object, including inter-object references (e.g. `HitObject`),
 3. a plain-old-data (POD) type holding the persistent object information (e.g. `HitData`), and
 4. a user-visible collection containing the physics objects (e.g. `HitCollection`).

These layers are described in the following.

### The User Layer

The user visible objects (e.g. `Hit`) act as light-weight references to the underlying data, and provide the necessary user interface. For each of the data-members and one-to-one relations declared in the data model definition, corresponding setters and getters are created. For each of the one-to-many relations a vector-like interface is provided.

With the chosen interface, the code written in C++ and Python looks almost identical, if taking proper advantage of the `auto` keyword.

### The Internal Data Layer

The internal objects give access to the object data, i.e. the POD, and the references to other objects.
These objects inherit from `podio::ObjBase`, which takes care of object identification (`podio::ObjectID`), and object-ownership. The `ObjectID` consists of the index of the object and an ID of the collection it belongs to. If the object does not belong to a collection yet, the data object owns the POD containing the real data, otherwise the POD is owned by the respective collection. For details about the inter-object references and their handling within the data objects please see below.

### The POD Layer
The plain-old-data (POD) contain just the data declared in the `Members` section of the datamodel definition. Ownership and lifetime of the PODs is managed by the other parts of the infrastructure, namely the data objects and the data collections.

### The Collections

The collections created serve three purposes:

  1. giving access to or creating the data items
  2. preparing objects for writing into PODs or preparing them after reading
  3. support for the so-called notebook pattern

### Vectorization support / notebook pattern

As an end-user oriented library, PODIO provides only a limited support for struct-of-arrays (SoA) memory layouts. In the vision, that the data used for heavy calculations is best copied locally, the library provides convenience methods for extracting the necessary information from the collections. More details can be found in the examples section of this document.

## Handling mutability

Depending on the policy of the client of PODIO, data collections may be read-only after creation, or may be altered still.
While the base assumption of PODIO is that once-created collections are immutable once leaving the scope of its creator,
it still allows for explicit `unfreezing` collections afterwards.
This feature has to handled with care, as it heavily impacts thread-safety.


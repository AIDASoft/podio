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

The main requirement for a reading backend is its capability of reading back all
the necessary data from which a collection can be constructed in the form of
`podio::CollectionReadBuffers`. From thes buffers collections can then be
constructed. Each instance has to contain the (type erased) POD buffers (as a
`std::vector`), the (possibly empty) vectors of `podio::ObjectID`s that contain
the relation information as well the (possibly empty) vectors for the vector
member buffers, which are currently stored as pairs of the type (as a
`std::string`) and (type erased) data buffers in the form of `std::vector`s.

### Dumping JSON

It is possible to turn on an automatic conversion to JSON for podio generated datamodels using the [nlohmann/json](https://github.com/nlohmann/json) library.
To enable this feature the **core datamodel library** has to be compiled with the `PODIO_JSON_OUTPUT` and linked against the nlohmann/json library (needs at least version 3.10).
In cmake the necessary steps are (assuming that `datamodel` is the datamodel core library)
```cmake
find_library(nlohmann_json 3.10 REQUIRED)
target_link_library(datamodel PUBLIC nlohmann_json::nlohmann_json)
target_compile_definitions(datamodel PUBLIC PODIO_JSON_OUTPUT)
```

With JSON support enabled it is possible to convert collections (or single objects) to JSON simply via
```cpp
#include "nlohmann/json.hpp"

auto collection = // get collection from somewhere

nlohmann::json json{
   {"collectionName", collection}
};
```

Each element of the collection will be converted to a JSON object, where the keys are the same as in the datamodel definiton.
Components contained in the objects will similarly be similarly converted.

**JSON is not foreseen as a mode for persistency, i.e. there is no plan to add the conversion from JSON to the in memory representation of the datamodel.**

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

 - Install [pre-commit](https://pre-commit.com/)

    `$ pip install pre-commit`

 - Run pre-commit manually

    `$ pre-commit run --all-files`

## Retrieving the EDM definition from a data file
It is possible to get the EDM definition(s) that was used to generate the
datatypes that are stored in a data file. This makes it possible to re-generate
the necessary code and build all libraries again in case they are not easily
available otherwise. To see which EDM definitions are available in a data file
use the `podio-dump` utility

```bash
podio-dump <data-file>
```
which will give an (exemplary) output like this
```
input file: <data-file>

EDM model definitions stored in this file: edm4hep

[...]
```

To actually dump the model definition to stdout use the `--dump-edm` option
and the name of the datamodel you want to dump:

```bash
podio-dump --dump-edm edm4hep <data-file> > dumped_edm4hep.yaml
```

Here we directly redirected the output to a yaml file that can then again be
used by the `podio_class_generator.py` to generate the corresponding c++ code
(or be passed to the cmake macros).

**Note that the dumped EDM definition is equivalent but not necessarily exactly
the same as the original EDM definition.** E.g. all the datatypes will have all
their fields (`Members`, `OneToOneRelations`, `OneToManyRelations`,
`VectorMembers`) defined, and defaulted to empty lists in case they were not
present in the original EDM definition. The reason for this is that the embedded
EDM definition is the pre-processed and validated one [as described
below](#technical-details-on-edm-definition-embedding)

### Accessing the EDM definition programmatically
The EDM definition can also be accessed programmatically via the
`[ROOT|SIO]FrameReader::getEDMDefinition` method. It takes an EDM name as its
single argument and returns the EDM definition as a JSON string. Most likely
this has to be decoded into an actual JSON structure in order to be usable (e.g.
via `json.loads` in python to get a `dict`).

### Technical details on EDM definition embedding
The EDM definition is embedded into the core EDM library as a raw string literal
in JSON format. This string is generated into the `DatamodelDefinition.h` file as

```cpp
namespace <package_name>::meta {
static constexpr auto <package_name>__JSONDefinition = R"EDMDEFINITION(<json encoded definition>)EDMDEFINITION";
}
```

where `<package_name>` is the name of the EDM as passed to the
`podio_class_generator.py` (or the cmake macro). The `<json encoded definition>`
is obtained from the pre-processed EDM definition that is read from the yaml
file. During this pre-processing the EDM definition is validated, and optional
fields are filled with empty defaults. Additionally, the `includeSubfolder`
option will be populated with the actual include subfolder, in case it has been
set to `True` in the yaml file. Since the json encoded definition is generated
right before the pre-processed model is passed to the class generator, this
definition is equivalent, but not necessarily equal to the original definition.

#### The `DatamodelRegistry`
To make access to information about currently loaded and available datamodels a
bit easier the `DatamodelRegistry` (singleton) keeps a map of all loaded
datamodels and provides access to this information possible. In this context we
refer to an *EDM* as the shared library (and the corresponding public headers)
that have been compiled from code that has been generated from a *datamodel
definition* in the original YAML file. In general whenever we refer to a
*datamodel* in this context we mean the enitity as a whole, i.e. its definition
in a YAML file, the concrete implementation as an EDM, as well as other related
information that is related to it.

Currently the `DatamodelRegistry` provides mainly access to the original
definition of available datamodels via two methods:
```cpp
const std::string_view getDatamodelDefinition(const std::string& edmName) const;

const std::string_view getDatamodelDefinition(size_t index) const;
```

where `index` can be obtained from each collection via
`getDatamodelRegistryIndex`. That in turn simply calls
`<package_name>::meta::DatamodelRegistryIndex::value()`, another singleton like object
that takes care of registering an EDM definition to the `DatamodelRegistry`
during its static initialization. It is also defined in the
`DatamodelDefinition.h` header.

Since the datamodel definition is embedded as a raw string literal into the core
EDM shared library, it is in principle also relatively straight forward to
retrieve it from this library by inspecting the binary, e.g. via
```bash
readelf -p .rodata libedm4hep.so | grep options
```

which will result in something like

```
  [   300]  {"options": {"getSyntax": true, "exposePODMembers": false, "includeSubfolder": "edm4hep/"}, "components": {<...>}, "datatypes": {<...>}}
```

#### I/O helpers for EDM definition storing
The `podio/utilities/DatamodelRegistryIOHelpers.h` header defines two utility
classes, that help with instrumenting readers and writers with functionality to
read and write all the necessary EDM definitions.

- The `DatamodelDefinitionCollector` is intended for usage in writers. It
  essentially collects the datamodel definitions of all the collections it encounters.
  The `registerDatamodelDefinition` method it provides should be called with every collection
  that is written. The `getDatamodelDefinitionsToWrite` method returns a vector of all
  datamodel names and their definition that were encountered during writing. **It is
  then the writers responsibility to actually store this information into the
  file**.
-  The `DatamodelDefinitionHolder` is intended to be used by readers. It
  provides the `getDatamodelDefinition` and `getAvailableDatamodels` methods.
  **It is again the readers property to correctly populate it with the data it
  has read from file.** Currently the `SIOFrameReader` and the `ROOTFrameReader`
  use it and also offer the same functionality as public methods with the help
  of it.

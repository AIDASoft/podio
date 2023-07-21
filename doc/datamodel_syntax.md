# Data Models and Data Model Definitions

To describe a data model PODIO provides a data model definition syntax.
This is to ease the creation of optimised data formats, which may take advantage of a so-called `struct-of-array` data layout.
From the user-provided data model description PODIO creates all the files necessary to use this data model.

## Basic Concepts and Supported Features
PODIO encourages the usage of composition, rather than inheritance.
One of the reasons for doing so is the focus on efficiency friendly `plain-old-data`.
For this reason, PODIO does not support inheritance within the defined data model.
Instead, users can combine multiple `components` to build a to be used `datatype`.

To allow the datatypes to be real PODs, the data stored within the data model are constrained to be
POD-compatible data. Those are

 1. basic types like int, double, etc as well as a limited set of fixed width
    integers from `<cstdint>` (the `_leastN` or `_fastN` types as well as all
    `8` bit types are not allowed).
 1. components built up from basic types or other components
 1. Fixed sized arrays of those types

In addition, PODIO supports the storage of one-to-one and one-to-many relations between objects.
In the following the syntax for defining a given data model is explained.
A later section contains more details about the code being created from this.

## Definition of custom components
A component is just a flat struct containing data. it can be defined via:

```yaml
    components:
      # My example component
      MyComponent:
        Members:
          - float x
          - float y
          - float z
          - AnotherComponent a
```

## Definition of custom data classes
Here an excerpt from "datamodel.yaml" for a simple class, just containing one member of the type `int`.

```yaml
    datatypes :
      EventInfo :
        Description : "My first data type"
        Author : "It's me"
        Members :
        - int Number // event number
```

Using this definition, three classes will be created: `EventInfo`, `EventInfoData` and `EventInfoCollection`. These have the following signature:

```cpp
    class EventInfoData {
      public:
        int Number;
    }

    class EventInfo {
      public:
      ...
        int Number() const;
        void Number(int);
      ...
    }
```

### Defining members

The definition of a member is done in the `Members` section in the form:

```yaml
    Members:
      <type> <name> // <comment>
```

where `type` can be any builtin-type or a `component`.

It is also possible to specify default values for members via
```yaml
    Members:
      <type> <name>{<default-value>} // <comment>
```
Note that in this case it is extremely expensive to check whether the provided `default-value` results in valid c++.
Hence, there is only a very basic syntax check, but no actual type check, and wrong default values will be caught only when trying to compile the datamodel.

For describing physics quantities it is important to know their units. Thus it is possible to add the units to the member definition:

```yaml
    Members:
      <type> <name>{<default-value>} [<unit>] // <comment>
```


### Definition of references between objects:
There can be one-to-one-relations and one-to-many relations being stored in a particular class. This happens either in the `OneToOneRelations` or `OneToManyRelations` section of the data definition. The definition has again the form:

```yaml
    OneToOneRelations:
      <type> <name> // <comment>
    OneToManyRelations:
      <type> <name> // <comment>
```

### Explicit definition of methods
In a few cases, it makes sense to add some more functionality to the created classes. Thus this library provides two ways of defining additional methods and code. Either by defining them inline or in external files. Extra code has to be provided separately for immutable and mutable additions.
Note that the immutable (`ExtraCode`) will also be placed into the mutable classes, so that there is no need for duplicating the code.
Only if some additions should only be available for the mutable classes it is necessary to fill the `MutableExtraCode` section.
The `includes` will be add to the header files of the generated classes.

```yaml
    ExtraCode:
      includes: <newline separated list of strings (optional)>
      declaration: <string>
      implementation : <string>
      declarationFile: <string> (to be implemented!)
      implementationFile: <string> (to be implemented!)
    MutableExtraCode:
      includes: <newline separated list of strings (optional)>
      declaration: <string>
      implementation : <string>
      declarationFile: <string> (to be implemented!)
      implementationFile: <string> (to be implemented!)
```

The code being provided has to use the macro `{name}` in place of the concrete name of the class.


## Global options
Some customization of the generated code is possible through flags. These flags are listed in the section `options`:

```yaml
    options:
      getSyntax: False
      exposePODMembers: True
    components:
      # My simple component
      ExampleComponent:
        Members:
          - int x
    datatypes:
      ExampleType:
        Description: "My datatype with a component member"
        Author: "Mr me"
        Members:
         - ExampleComponent comp // component from above
```

- `getSyntax`: steers the naming of get and set methods. If set to true, methods are prefixed with `get` and `set` following the capitalized member name, otherwise the member name is used for both.
- `exposePODMembers`: whether get and set methods are also generated for members of a member-component. In the example corresponding methods would be generated to directly set / get `x` through `ExampleType`.


## Extending a datamodel / using types from an upstream datamodel
It is possible to extend another datamodel with your own types, resp. use some datatypes or components from an upstream datamodel in your own datamodel.
This can be useful for prototyping new datatypes or for accomodating special requirements without having to reimplement / copy a complete datamodel.

To pass an upstream datamodel to the class generator use the `--upstream-edm` option that takes the package name as well as the yaml definition file of the upstream datamodel separated by a colon (':').
This will effectively make all components and datatypes of the upstream datamodel available to the current definition for validation and generation of the necessary includes.
Nevertheless, only the code for the datatypes and components defined in the current yaml file will be generated.
The podio `PODIO_GENERATE_DATAMODEL` cmake macro has gained an additional parameter `UPSTREAM_EDM` to pass the arguments to the generator via the cmake macros.

### Potential pitfalls
- The cmake macros do not handle linking against an upstream datamodel automatically. It is the users responsibility to explicitly link against the upstream datamodel.
- When generating two datamodels side-by-side and into the same output directory and using the `ROOT` backend, the generated `LinkDef.h` file might be overwritten if both datamodels are generated into the same output directory.

Limiting this functionality to one upstream datamodel is a conscious choice to limit the potential for abuse of this feature.

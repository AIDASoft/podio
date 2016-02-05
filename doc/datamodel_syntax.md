## Definition of custom components

A component is just a flat struct containing data. it can be defined via:

    components:
      # My example component
      MyComponent:
        x : float
        y : float
        z : float

Syntax to be changed!

The puropose of components is to support the patter of composition rather than inheritance when building higher level data classes.

## Definition of custom data classes
This package allows the definition of data types using a simple syntax. This is to ease the creation of optimised data formats. Here an excerpt from "datamodel.yaml" for a simple class, just containing one member of the type `int`.

    datatypes :
      EventInfo :
        Description : "My first data type"
        Author : "It's me"
        Members :
        - int Number // event number

Using this definition, three classes will be created: `EventInfo`, `EventInfoData` and `EventInfoCollection`. These have the following signature:

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

### Defining members

The definition of a member is done in the `Members` section in the form:

    <type> <name> // <comment>

where `type` can be any buildin-type or a `component`.

### Definition of references between objects:
There can be one-to-one-relations and one-to-many relations being stored in a particular class. This happens either in the `OneToOneRelations` or `OneToManyRelations` section of the data definition. The definition has again the form:

    <type> <name> // <comment>

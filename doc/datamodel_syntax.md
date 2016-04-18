# PODIO class definition syntax

PODIO uses a user-written data model definition file

## Definition of custom components

A component is just a flat struct containing data. it can be defined via:

    components:
      # My example component
      MyComponent:
        x : float
        y : float
        z : float
        a : AnotherComponent

The purpose of components is to support the patter of composition rather than inheritance when building higher level data classes. Components can only contain simple data types and other components. 

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

    Members:
      <type> <name> // <comment>

where `type` can be any buildin-type or a `component`.

### Definition of references between objects:
There can be one-to-one-relations and one-to-many relations being stored in a particular class. This happens either in the `OneToOneRelations` or `OneToManyRelations` section of the data definition. The definition has again the form:

    OneToOneRelations:
      <type> <name> // <comment>
    OneToManyRelations:
      <type> <name> // <comment>

### Explicit definition of methods
In a few cases, it makes sense to add some more functionality to the created classes. Thus this library provides two ways of defining additional methods and code. Either by defining them inline or in external files. Extra code has to be provided separately for const and non-const additions.

    ExtraCode:
      declaration: <string>
      implementation : <string>
      declarationFile: <string> (to be implemented!)
      implementationFile: <string> (to be implemented!)
    ConstExtraCode:
      declaration: <string>
      implementation : <string>
      declarationFile: <string> (to be implemented!)
      implementationFile: <string> (to be implemented!)

The code being provided has to use the macro `{name}` in place of the concrete name of the class.





  
        

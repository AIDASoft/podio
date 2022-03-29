# Changing / creating new templates

PODIO uses the [Jinja2](https://jinja.palletsprojects.com/en/3.1.x/) template engine to generate the c++ code from the yaml description.
This document here gives an overview of how PODIO uses the Jinja2 engine and how the yaml file is processed before it is passed to it.
We don't go into many details of Jinja2 templates here, please refer to the [Template Designer Document](https://jinja.palletsprojects.com/en/3.1.x/templates/) of Jinja2.
PODIO only makes use of rather basic Jinja2 templates, so it should in principle be possible to pick up the basics just by looking at some existing templates.

## Preprocessing of yaml file

The entry point for reading yaml files is the [`python/podio_config_reader.py`](/python/podio_config_reader.py).
When reading the yaml file a basic validation is run and the data members, relations and vector members of components and datatypes are parsed into `MemberVariable` objects (defined in [`python/generator_utils.py`](/python/generator_utils.py)).
The reader itself will be created as part of the `ClassGenerator` in [`python/podio_class_generator.py`](/python/podio_class_generator.py), which is the main entry point.
It takes care of 
- Configuring the Jinja2 template engine. At the moment this is mainly making the templates known to the engine.
- The necessary preprocessing of all the datatypes and components. This includes collecting necessary include directories and forward declaration, as well as digesting `ExtraCode` snippets.
- Putting all the necessary information into a `dict` that can be easily used in the Jinja2 templates. See [below](#available-information-in-the-templates) for what is available in the templates
- Calling the template engine to fill the necessary templates for each datatype or component and making sure to only write to disk if the filled template actually changed. Optionally run `clang-format` on them before writing.
- Producing a list of generated c++ files for consumption by the cmake macros of PODIO.

## Existing templates

Currently PODIO loads templates that are placed in [`<prefix>/python/templates`](/python/templates).
They are broadly split along the classes that are generated for each datatype or component from the EDM definition:

| template file(s)                | content                                                                                                 | generated file(s)                                                                     |
|---------------------------------|---------------------------------------------------------------------------------------------------------|---------------------------------------------------------------------------------------|
| `Component.h.jinja2`            | Definition for each component                                                                           | `[<package>/]<component-name>.h`                                                      |
| `Data.h.jinja2`                 | POD struct of each datatype (living in the POD layer)                                                   | `[<package>/]<datatype-name>Data.h`                                                   |
| `Obj.{h,cc}.jinja2`             | `Obj` class for each datatype (linving in the object layer) and managing resources                      | `[<package>/]<datatype-name>Obj.h`,  `src/<datatype-name>Obj.cc`                      |
| `[Mutable]Object.{h,cc}.jinja2` | The user facing interfaces for each datatype (living in the user layer)                                 | `[<package>/][Mutable]<datatype-name>.h`, `src/[Mutable]<datatype-name>.cc`           |
| `Collection.{h,cc}.jinja2`      | The user facing collection interface (living in the user layer)                                         | `[<package>/]<datatype-name>Collection.h`, `src/<datatype-name>Collection.cc`         |
| `CollectionData.{h,cc}.jinja2`  | The classes managing the collection storage (not user facing!)                                          | `[<package>/]<datatype-name>CollectionData.h`, `src/<datatype-name>CollectionData.cc` |
| `selection.xml.jinja2`          | The `selection.xml` file that is necessary for generating a root dictionary for the generated datamodel | `src/selection.xml`                                                                   |
| `SIOBlock.{h,cc}.jinja2`        | The SIO blocks that are necessary for the SIO backend                                                   | `[<package>/]<datatype-name>SIOBlock.h`, `src/<datatype-name>SIOBlock.cc`             |

The presence of a `[<package>]` subdirectory for the header files is controlled by the `includeSubfolder` option in the yaml definition file.

Jinja allows the definition of additional macros and supports importing them similar to python modules.
These are stored in the [`macros`](/python/templates/macros) subfolder and are imported directly by the main templates where necessary.

## Adding a new template
All templates that are placed in the templates directory mentioned [above](#existing-templates) become immediately available to the template engine if it ends on `.jinja2`
However, it is still necessary to actively fill them from the class generator.
If the available information for the new templates is already enough and no further pre-processing is necessary, than they need to be added to `_get_filenames_templates` function in the `ClassGenerator`.
The `prefix` and `postfix` dictionaries define how the template filename will be mapped to the generated files: `<prefix><template-filename><postfix>`.
By default a `.h` and a `.cc` file will be generated, but this can be overridden by adding the template to the `endings` dictionary.
With that in place it is now only neccessary to call `_fill_templates` with the appropriate template name and the pre processed data.
Note that for most templates this means that they have to be filled for each datatype or component individually.

If additional preprocessing is necessary, it will be necessary to also add that to the `ClassGenerator`.
The main entry point to the generation is the `process` method which essentially just delegates to other methods.

## Available information in the templates

The following gives an overview of the information that is available from the dictionary that is passed to the templates from the `ClassGenerator`. 
Each (top level) key in this dict is directly available as a variable in the Jinja2 templates, e.g.
```python
component['include'] = # list of includes
```
will become available as
```jinja2
{% for include in includes %}
{{ include }}
{% endfor %}
```

### General information
The following keys / variables are always available
| key / variable name | content                                                                                  |
|---------------------|------------------------------------------------------------------------------------------|
| `package_name`      | The package name of the datamodel (passed to the generator as argument)                  |
| `use_get_syntax`    | The value of the `getSyntax` option from the yaml definition file                        |
| `incfolder`         | The `[<package>/]` part of the generated header files (See [above](#existing-templates)) |

### Components
The following keys are filled for each component
| key / variable | content                                                            |
|----------------|--------------------------------------------------------------------|
| `class`        | The class of the component  as `DataType` (see [below](#datatype)) |
| `Members`      | The members of the component as `MemberVariable`s                  |
| `includes`     | All the necessary includes for this component                      |
| `ExtraCode`    | Optionally present extra code                                      |

### Datatypes
The following keys / variables are filled for each datatype
| key / variable                | content                                                                                                                                                  |
|-------------------------------|----------------------------------------------------------------------------------------------------------------------------------------------------------|
| `class`                       | The (immutable, user-facing) class as `DataType` (see [below](#datatype))                                                                                |
| `Members`                     | The members of the datatype as a list of `MemberVariable`s (see [below](#membervariable))                                                                                                        |
| `OneToOneRelations`           | The one-to-one relation members of the datatype as  a list of `MemberVariable`s                                                                                     |
| `OneToManyRelations`          | The one-to-many relation members of the datatype as a list of `MemberVariable`s                                                                                    |
| `VectorMembers`               | The vector members of the datatype as a list of `MemberVariable`s                                                                                                  |
| `includes`                    | The include directives for the the user facing classes header files                                                                                      |
| `includes_cc`                 | The include directives for the implemenations of the user facing classes                                                                                 |
| `includes_data`               | The necessary include directives for the `Data` POD types                                                                                                |
| `includes_obj`                | The include directives for the `Obj` classes headers.                                                                                                    |
| `includes_cc_obj`             | The include directives for the implementation files of the `Obj` classes.                                                                                |
| `includes_coll_cc`            | The include directives for the implementation of the `Collection` classes                                                                                |
| `include_coll_data`           | The include directives for the header `CollectionData` header file                                                                                       |
| `forward_declarations`        | The forward declarations for the user facing classes header files. This is a nested dict, where the keys are namespaces and the leaf values are classes. |
| `forward_declarations_obj`     | The forward declarations for the `Obj` classes header files.                                                                                             |
| `is_pod`                      | Flag value indicating whether the `Data` class of this datatype is a POD or if it contains an STL member                                                 |
| `obj_needs_destructor`        | Flag indicating whether the `Obj` class needs a dedicated destructor for the datatype or whether `= default` is enough.                                  |
| `ostream_collection_settings` | A dict with a single `header_contents` key that is necessary for the output stream overload implementation of collections                                |


### `MemberVariable`
Defined in [`python/generator_utils.py`](/python/generator_utils.py). 
The string representation gives the definition of the member, including a potentially present description string.
In principle all members are accessible in the templates, however, the most important ones are:

| field         | description                                                                                                                                                                 |
|---------------|-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `name`        | The name of the member                                                                                                                                                      |
| `namespace`   | The (potentially empty) namespace of the member                                                                                                                             |
| `bare_type`   | The type of the member without namespace                                                                                                                                    |
| `full_type`   | The full, namespace qualified, type of the member, essentially `{{ namespace }}::{{ bare_type }}`                                                                           |
| `description` | The (optional) description string of the member                                                                                                                             |
| `is_builtin`  | Flag for indicating that a member is a builtin type                                                                                                                         |
| `is_array`    | Flag for indicating that a member is a `std::array`                                                                                                                         |
| `array_type`  | The type of the array if the member is a `std::array`                                                                                                                       |
| `array_size`  | The size of the array if the member is a `std::array`                                                                                                                       |
| `getter_name` | Method for generating the correct name for getter functions, depending on the `getSyntax` option in the yaml definition file.                                               |
| `setter_name` | Method for generating the correct name for setter functions, depending on the `getSyntax` option in the yaml definition file and on whether the member is a relation or not |
| `signature`   | The signature of a data member that can be used in function signatures, corresponds to `{{ full_type }} {{ name }}`                                                         |

### `DataType`
Defined in [`python/generator_utils.py`](/python/generator_utils.py). 
This is essenitally a stripped down version of the `MemberVariable` with the major difference being that the string representation returns the fully qualified type instead.
The available fields are

| field       | description                                                                    |
|-------------|--------------------------------------------------------------------------------|
| `bare_type` | The type without the namespace                                                 |
| `namespace` | The (potentially empty) namespace                                              |
| `full_type` | The fully qualified type, corresponding to `{{ namespace }}::{{ bare_type }}`. |

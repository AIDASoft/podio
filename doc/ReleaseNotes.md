# v00-13

* 2020-12-03 Marko Petric ([PR#153](https://github.com/aidasoft/podio/pull/153))
  - Set rpath for macOS and externalize compiler and linker flags
  - Search for the same version of python as was used for building ROOT

* 2020-12-03 Joseph C Wang ([PR#152](https://github.com/aidasoft/podio/pull/152))
  - Make EventStore non-copyable

* 2020-12-03 tmadlener ([PR#144](https://github.com/aidasoft/podio/pull/144))
  - Decouple the writers and the EventStore to allow to write collections that have previously been read from a file.

* 2020-11-24 Valentin Volkl ([PR#149](https://github.com/aidasoft/podio/pull/149))
  - [cmake] add find_package_handle_standard_args() to podio config

* 2020-11-18 Frank Gaede ([PR#147](https://github.com/aidasoft/podio/pull/147))
  - fix for MacOs when using SIO I/O with podio
        - need to link edm-core library to edm-sioBlocks library

* 2020-11-10 Thomas Madlener ([PR#130](https://github.com/aidasoft/podio/pull/130))
  - Add SIO as a second I/O backend (as alternative to ROOT) that can be enabled with `ENABLE_SIO`. If enabled, a separate `podioSioIO` library is built that allows reading and writing sio files. For serializing the different datatypes, additional code is generated to build an `SioBlocks` library that is loaded at runtime (if found somewhere on `LD_LIBRARY_PATH`). To facilitate the whole process at the cmake level, **new cmake functions** are provided to generate the core datamodel library **`PODIO_ADD_DATAMODEL_CORE_LIBRARY`**, to (conditionally) define the ROOT dictionary target **`PODIO_ADD_ROOT_IO_DICT`** and to (conditionally) define the Sio Blocks library target **`PODIO_ADD_SIO_IO_BLOCKS`**. The I/O backends that are supported by podio are exported via the **`PODIO_IO_HANDLERS`** list variable.
  - `podio_generate_datamodel.py` now additionally takes the I/O handlers that should be generated as arguments. This is also reflected in an additional argument to `PODIO_GENERATE_DATAMODEL`. To have backwards compatibility, this additional argument defaults to ROOT in both cases and downstream packages should work as usual without changes.

* 2020-10-06 tmadlener ([PR#133](https://github.com/aidasoft/podio/pull/133))
  - Make `ROOTReader` handle file switches properly for meta data reading.

* 2020-09-29 tmadlener ([PR#141](https://github.com/aidasoft/podio/pull/141))
  - Update CI actions to use LCG 96, 97, 98 for mac, centos7 and ubuntu1804
  - Make python bindings work with root 6.22 (and onwards)
  - Make sure that root has been built with c++17 at the cmake stage
  - Require at least CMake 3.12

* 2020-09-18 tmadlener ([PR#134](https://github.com/aidasoft/podio/pull/134))
  - Make the EventStore actually take ownership of the metadata passed to it by the readers. (see #139)
  - Make the collections properly clean up data of VectorMembers (see #139)
  - Fix small memory leak in the EventStore for the CollectionIDTable.

* 2020-09-04 tmadlener ([PR#128](https://github.com/aidasoft/podio/pull/128))
  - Fix a possible nullptr access in the Writers and make registerForWrite return a boolean to make it easier to check from the calling site.

* 2020-09-04 tmadlener ([PR#127](https://github.com/aidasoft/podio/pull/127))
  - cleanup of `GenericParameters` for meta data
        - remove mutable from internal maps

* 2020-09-04 tmadlener ([PR#126](https://github.com/aidasoft/podio/pull/126))
  - No longer install python unittest files

* 2020-09-04 tmadlener ([PR#125](https://github.com/aidasoft/podio/pull/125))
  - improve CollectionIDTable
         - previously used a std::recursive_mutex which is unnecessary in this case
          - made whatever can be made const const and now use std::distance to calculate the index.

* 2020-08-28 tmadlener ([PR#129](https://github.com/aidasoft/podio/pull/129))
  - Reduce memory footprint by clearing intermediately used I/O buffers.

# v00-12

* 2020-08-11 Frank Gaede ([PR#124](https://github.com/aidasoft/podio/pull/124))
  - minor bug fix for macos:
        - add `<sstream>` to write.cpp
        - fixes #123

* 2020-08-11 tmadlener ([PR#122](https://github.com/aidasoft/podio/pull/122))
  - Update README to reflect the new requirements (i.e. `jinja2`)

* 2020-08-11 tmadlener ([PR#121](https://github.com/aidasoft/podio/pull/121))
  - Update Ubuntu CI script to install the python requirements via pip.

* 2020-08-11 tmadlener ([PR#120](https://github.com/aidasoft/podio/pull/120))
  - Use `jinja2` template engine to generate c++ code. This makes it possible to more cleanly separate the generation of the c++ code and all the necessary pre-processing. Now only the pre-processing is done in python, while the complete generation is done using `jinja2`. This should make it much easier to make changes to the generated c++ code.

* 2020-08-05 tmadlener ([PR#117](https://github.com/aidasoft/podio/pull/117))
  - Refactoring of the podio class generator to improve its readability. This refactoring is mainly focusing on reducing the size of some methods to make them easier to grasp as well as trying to more cleanly separate the validation of the inputs and the c++ code generation.  **The validation is stricter than the current version and will potentially break things. Specifically, the handling of `ExtraCode` and `ConstExtraCode` is now implemented according to the documentation.** In the previous version this has somewhat diverged from there. **The yaml declaration of `components` has been made equivalent to the ones of the `datatypes` but "old-style" definitions are still handled properly.** In the latter case a deprecation warning is emitted. **The generated c++ code is functionally equivalent, but not equal character by character.** The differences are mainly whitespace, but at some places also some of the generated code has been updated to more recent c++ capabilities. Additionally, some tests for the validation are added. They are not really exhaustive and should be considered to serve mainly documenting purposes for the moment.

* 2020-08-03 tmadlener ([PR#107](https://github.com/aidasoft/podio/pull/107))
  - Introduce range wrapper for OneToManyRelations and VectorMembers to allow range-based for loops

* 2020-08-03 tmadlener ([PR#106](https://github.com/aidasoft/podio/pull/106))
  - Fix the possibility of running into infinite loops in the ostream operator with cyclical references

* 2020-08-03 Frank Gaede ([PR#102](https://github.com/aidasoft/podio/pull/102))
  - add test example `read_and_write.cpp`
       - simple use case for reading in an event an writing (parts of it) to another file
       - currently fails with segmentation fault  (to be fixed)

* 2020-08-03 Valentin Volkl ([PR#98](https://github.com/aidasoft/podio/pull/98))
  - add ubuntu ci build

# v00-11

* 2020-07-21 tmadlener ([PR#101](https://github.com/aidasoft/podio/pull/101))
  - Fix bug where `ConstObject` with empty `OneToManyRelations` give an invalid iterator range, #100

* 2020-06-23 Andre Sailer ([PR#99](https://github.com/aidasoft/podio/pull/99))
  - Point to libpodioDict.so instead of libpodio.so in rootmap file

* 2020-06-03 Frank Gaede ([PR#92](https://github.com/aidasoft/podio/pull/92))
  - implement reading/writing of meta data for runs, events and collections
       - based on GenericParameters that hold named parameters of type `int, float, string` or vectors if these (copied from `lcio::LCParameters`)
       - meta data for the three types is always written 
       - it is read only on request
  - example for writing:
  ```
     auto& evtMD = store.getEventMetaData() ;
      evtMD.setValue( "UserEventWeight" , (float) 100.*i ) ;
  ```
  - example for reading:
  ```
    auto& evtMD = store.getEventMetaData() ;
    float evtWeight = evtMD.getFloatVal( "UserEventWeight" ) ;
  
  ```
  - addresses #49

* 2020-05-26 Andre Sailer ([PR#91](https://github.com/aidasoft/podio/pull/91))
  - Ensure podioRootIO is linked against podioDict even when linker uses as-needed by default, fixes #90

* 2020-05-26 Thomas Madlener ([PR#89](https://github.com/aidasoft/podio/pull/89))
  - Updated README and env.sh to reflect changes in install process

* 2020-05-12 Valentin Volkl ([PR#88](https://github.com/aidasoft/podio/pull/88))
  - change add relation naming addXxxx -> addToXxxxs

* 2020-05-12 Valentin Volkl ([PR#87](https://github.com/aidasoft/podio/pull/87))
  - update minimum required CMake version to 3.8
  - remove obsolete Jenkinsfile
  - make CPack config optional: cmake variable ENABLE_CPACK defaults to OFF
  - update Readme with Spack instructions

* 2020-04-14 Frank Gaede ([PR#83](https://github.com/aidasoft/podio/pull/83))
  - allow automatic change of version in CMakeLists.txt
    - fixes #82

# v00-10

* 2020-04-03 Frank Gaede ([PR#81](https://github.com/aidasoft/podio/pull/81))
  - make compatible with macos and clang
    - use `-Wl,-undefined,dynamic_lookup` w/ AppleClang
        - make the same, default behavior on linux explicit w/ `-Wl,--allow-shlib-undefined`
    -  add ROOT::Core to TestDataModel library

* 2020-04-03 Marko Petric ([PR#80](https://github.com/aidasoft/podio/pull/80))
  - add a macOS test to GitHub actions

* 2020-04-03 Valentin Volkl ([PR#75](https://github.com/aidasoft/podio/pull/75))
  - Fixes in python code for 2-3 compatibility

* 2020-03-23 Andre Sailer ([PR#74](https://github.com/aidasoft/podio/pull/74))
  - Fix exception when podio_class_generator is called with -q/--quiet
  - Printout about includes only in verbose mode
  - Do not print warnings in quiet mode
  - Print each warning only once

* 2020-03-23 Andre Sailer ([PR#73](https://github.com/aidasoft/podio/pull/73))
  * Added PODIO_GENERATE_DATAMODEL Cmake macro to call the class generator at the right time. see podioMacros for the signature

* 2020-03-11 Marko Petric ([PR#72](https://github.com/aidasoft/podio/pull/72))
  - Update CI to use GitHub actions
  - Add test against a Python 3 LCG view
  - Remove travis
  - Add tests for Python 3 compatibility and flake8

* 2020-03-11 Andre Sailer ([PR#70](https://github.com/aidasoft/podio/pull/70))
  - ClassGenerator: add option "includeSubfolder", to always use `#include "\<packagename\>/\<object\>.h" etc. if set to "True"
  - Added sorting and "uniquing" of include lists. Some duplicates still occur because two different lists are used on occasion 
  - Added $PACKAGE_NAME_ to include guards

* 2020-03-11 Andre Sailer ([PR#69](https://github.com/aidasoft/podio/pull/69))
  -  Move all Root dependencies (RootReader, RootWriter) of the podio Library into podioRootIO, rename podioDict to podioDict

* 2020-02-17 Andre Sailer ([PR#68](https://github.com/aidasoft/podio/pull/68))
  - Add podioMacros.cmake to contain PODIO_GENERATE_DICTIONARY

* 2020-02-17 Andre Sailer ([PR#67](https://github.com/aidasoft/podio/pull/67))
  - Change Yaml loader to preserve order of members as given in the yaml

* 2020-02-17 Andre Sailer ([PR#65](https://github.com/aidasoft/podio/pull/65))
  - podio_class_generator:  only write files if their content changed

* 2019-10-15 Valentin Volkl ([PR#64](https://github.com/aidasoft/podio/pull/64))
  - Add `BUILD_TESTS` CMake option for building tests

* 2019-10-15 Pere Mato ([PR#63](https://github.com/aidasoft/podio/pull/63))
  - Fix to avoid deprecated STL classes in C++17

# v00-09-02

* 2019-09-27 Frank Gaede ([PR#60](https://github.com/AIDASoft/podio/pull/60))
  - fixed the code generation for members with multi word types (long long, unsigned long,...) 
        - add example to ExampleHit:
          `- unsigned long long cellID // cellID`

* 2019-09-27 Ben Morgan ([PR#59](https://github.com/AIDASoft/podio/pull/59))
  - move `templates` inside the `python` directory when installing
        - fixes: #58

* 2019-09-27 Ben Morgan ([PR#59](https://github.com/AIDASoft/podio/pull/59))
  /

* 2019-09-27 Frank Gaede ([PR#57](https://github.com/AIDASoft/podio/pull/57))
  - allow for numbers in namespace and class names
        - generate type name string for CollectionBase::getValueTypeName()  
        - fixes #56

* 2019-09-26 Frank Gaede ([PR#55](https://github.com/AIDASoft/podio/pull/55))
  -  cache collection pointers for fast access in `EventStore::get()`
        - added `EventStore::getFast(int id)` 
        - considerably speeds up XXCollection::setReferences()
           when reading back collections

* 2019-09-26 Ben Morgan ([PR#54](https://github.com/AIDASoft/podio/pull/54))
  - improve the CMake 
       1. Consistent use of CMake usage requirements to propagate include and link dependencies
       2. Full use of the CMakePackageConfigHelpers module to generate the `podioConfig.cmake`      file and associated `podioTargets.cmake`
       3. Automatically refind the ROOT dependency
       4. Standardize install paths for CMake and template files
  A podio client can, in CMake, do
  ```cmake
  find_package(podio REQUIRED)
  add_executable(foo foo.cc)
  target_link_libraries(foo podio::podio)
  ```
  and all include/link paths will be set correctly.

* 2019-08-21 Javier Cervantes Villanueva ([PR#51](https://github.com/AIDASoft/podio/pull/51))
  Do not install tests
  - Allow tests to run after the build phase
  - Paths have been modified to point to binary or source directories
  - Before, tests had to be run only after running make install
  - Test are not installed anymore
  - Fail tests if any error is reported (ROOT Interpreter error may not be considered by CMake)

* 2019-05-10 Frank Gaede ([PR#45](https://github.com/AIDASoft/podio/pull/45))
  - updated documentation
      - add guidelines for contributing
  - reverted some name changes in tests/examples (fixes #44)
      - `read-one` now again called `read`
  - enamble `dryrun` again for generate-edm test

* 2019-04-09 Marko Petric ([PR#43](https://github.com/AIDASoft/podio/pull/43))
  - Implementation of CI based on LCG views, as suggested in #42

* 2019-04-09 Graeme A Stewart ([PR#41](https://github.com/AIDASoft/podio/pull/41))
  - Improve convenience setup scripts and build instructions
         - remove FCC specific code from init.sh
         - factorise environment setup to env.sh
         - updated README.md

* 2019-03-24 Javier Cervantes ([PR#40](https://github.com/AIDASoft/podio/pull/40))
  - ROOTReader now supports multiple inputs thanks to new implementation based on TChain 
  - ROOTReader now supports opening files via xrootd (`root:///eospublic.cern.ch//eos...` for example)
  - Improved CMake and CPack configuration, sticking more closely to HSF template

# v00-09

* 2018-12-20 Frank Gaede ([PR#39](https://github.com/aidasoft/podio/pull/39))
  - add some fixes and improvements
       - fix forward declarations in Object template when using a namespace for the EDM
       - fix array getter names when using the get/set syntax
       -  add missing treatment for include statements  in component's header files
       - handle array members in ostream operators
   - add CollectionBase::size() member function
        - allows to access collection size w/o knowing the concrete type
        - method is already generated in implementation classes

* 2018-12-06 Frank Gaede ([PR#38](https://github.com/aidasoft/podio/pull/38))
  - add code generation for I/O of vector members
         - vector members are treated analogous to the reference vectors,i.e.
            streamed as one large vector per collection
  - updated tests/datamodel  accordingly  (using clang-format)

* 2018-11-30 Frank Gaede ([PR#37](https://github.com/aidasoft/podio/pull/37))
  - handle references and vector members in collection's  ostream operators

* 2018-11-30 Frank Gaede ([PR#36](https://github.com/aidasoft/podio/pull/36))
  - add github templates for releaase notes, issues and contribution guidelines
  - add ReleaseNotes.md 
        - contains all commit logs so far (v00-08)

# v00-08

* 2017-11-02 hegner 
  - Merge pull request #32 from HEP-FCC/master
  - Merge branch 'master' into master

* 2017-09-13 Colin Bernet 
  - Merge pull request #60 from cbernet/nofile_fix

* 2017-09-12 Colin 
  - fixed segv when opening a non accessible input file
  - fix test in interactive mode. write program had been moved

* 2017-06-21 Colin Bernet 
  - Merge pull request #59 from cbernet/pyeventstore_filename

* 2017-06-20 Colin 
  - can now get the name of the current file (useful when reporting exceptions)

* 2017-05-29 hegner 
  - Merge pull request #58 from jlingema/clangf

* 2017-05-29 Joschka Lingemann 
  - Fix bug introduced with clang format class gen would crash if clang-format was not used

* 2017-05-23 hegner 
  - Merge pull request #53 from jlingema/array
  - Merge pull request #55 from jlingema/clangf
  - Merge pull request #57 from jlingema/fixWarnings

* 2017-05-22 Joschka Lingemann 
  - Regenerate test datamodel
  - Add override statements

* 2017-04-20 Joschka Lingemann 
  - Only call clang-format if it is in PATH

* 2017-04-11 Joschka Lingemann 
  - Add language hints for syntax highlighting in docs
  - Add options of class generator to README
  - Add test for clang format
  - Add clang to path in init
  - Add clang-format application as option, closes #54

* 2017-04-07 Joschka Lingemann 
  - Regenerated datamodel
  - Fix test in read and write for array of struct
  - Fixes in parser for arrays of struct
  - Add test for array of struct
  - Add includes for arrays of PODs

* 2017-04-04 Benedikt Hegner 
  - temporarily disable outputstream for arrays

* 2017-04-04 hegner 
  - Merge pull request #26 from gaede/add_ascii_io
  - Merge branch 'master' into add_ascii_io

* 2017-03-24 zaborowska 
  - Merge pull request #52 from jlingema/array

* 2017-03-24 Joschka Lingemann 
  - Add ROOT version requirement

* 2017-03-21 hegner 
  - Merge pull request #30 from HEP-FCC/master
  - Merge branch 'master' into master

* 2017-03-20 hegner 
  - Merge pull request #50 from jlingema/array

* 2017-03-13 Joschka Lingemann 
  - Increase version requirement in README
  - Removing FindROOT macro, using standard use-file
  - Migrate to LCG_88
  - silence warning of unsigned -> int
  - Merge remote branch 'origin/master' into array

* 2017-03-02 Joschka Lingemann 
  - Add implementation for array members

* 2017-02-15 Joschka Lingemann 
  - Add test for datatype with array-member.

* 2017-02-21 zaborowska 
  - Merge pull request #49 from jlingema/cvmfs

* 2017-02-21 Joschka Lingemann 
  - Remove unused variable, add check if PODIO is set.
  - Move to cvmfs
  - Set ROOT_INCLUDE_PATH of pyunittest to avoid failure if FCC env is set.

* 2016-12-12 zaborowska 
  - Merge pull request #47 from jlingema/master

* 2016-12-09 Joschka Lingemann 
  - Regenerated datamodel with check against invalid collections.
  - Add check against OneToManyRelations to invalid collections
  - Move to LCG_87

* 2016-12-09 hegner 
  - Merge pull request #46 from jlingema/master

* 2016-12-06 Joschka Lingemann 
  - Require C++14.

* 2016-11-07 hegner 
  - Merge pull request #45 from jlingema/master

* 2016-10-17 Benedikt Hegner 
  - add method to register collection
  - remove need for templating on registering for writing

* 2016-09-30 Frank Gaede 
  - remove debug printout for found components
  - add example for using ASCIWriter - write_ascii
  - fixed const_relation getters w/ getSyntax
  - regenerated EDM, ostream operator<<, id() and o.->
  - add ASCIIWriter
  - improved creation of ostream operators etc.

* 2016-09-26 Joschka Lingemann 
  - Add non-const accessors for non-const collections.

* 2016-08-23 hegner 
  - Merge pull request #44 from jlingema/docs
  - Merge pull request #43 from jlingema/fix_cpy_relations

* 2016-08-23 Joschka Lingemann 
  - Change option to be the same across projects.

* 2016-08-16 Joschka Lingemann 
  - Improve readability of code base name and version.
  - Allow to customize version for doxygen via cli.
  - Use readme as title page for doxygen.
  - Change version to tags currently used.

* 2016-08-15 Joschka Lingemann 
  - Add unsigned long to the list of builtins for consistency.
  - add test that regenerates datamodel in dry-run mode.
  - Add dry-run option.

* 2016-08-09 Joschka Lingemann 
  - Remove unnecessary dict.

* 2016-07-29 Joschka Lingemann 
  - Add support for (unsigned) long long
  - Fix indentation

* 2016-07-28 hegner 
  - Merge pull request #25 from jlingema/fix_cpy_relations

* 2016-07-28 Joschka Lingemann 
  - Regenerated datamodel.
  - Add a paragraph on options in the datamodel description files.
  - Add checks for clashing getter and setter names.
  - Add doxygen comments.

* 2016-07-27 hegner 
  - Merge pull request #42 from jlingema/fix_cpy_relations

* 2016-07-27 Joschka Lingemann 
  - Regenerated datamodel.
  - Add tests for direct accessors and setters of POD members.
  - Merge with changes for doxygen comments and member exposure
  - Merge pull request #24 from jlingema/fix_cpy_relations
  - Add missing ; and switch to operator==
  - Code cleanup.

* 2016-07-26 Joschka Lingemann 
  - Regenerated datamodel.
  - Removing print info generation, as that creates compilation issues.
  - Merge with recent changes from hegner/podio.
  - Remove print outs
  - Regenerated datamodel.
  - Extend tests to have empty references.
  - Initialize relations on copy only if they are set.

* 2016-07-25 Joschka Lingemann 
  - Regenerated datamodel.
  - Add test for deep copy.
  - Bug fix: Initialisation of OneToOneRelations when copying.

* 2016-07-22 Joschka Lingemann 
  - Merge remote branch 'origin/master'
  - Add tests for direct getter and setter of POD members.
  - Bug fix: option value already parsed to bool.
  - Add getters for POD members in Const objects.
  - Regenerated datamodel.
  - Remove trailing ; to silence compiler warning.
  - Add options to yaml files and parser for get syntax and pod data exposure.
  - Expose POD member members in objects.
  - Adding comments that show up in doxygen.

* 2016-07-12 hegner 
  - Merge pull request #40 from jlingema/master

* 2016-07-12 Joschka Lingemann 
  - Revert to operator[], add function checking presence of a collection.
  - Adding way to only clear the chaches (needed for FCCSW).
  - Change way to access IDs and names to allow catching exceptions.

* 2016-06-30 Benedikt Hegner 
  - update doc as preparation for milestone document

* 2016-06-17 Joschka Lingemann 
  - Add test for OneToManyRelations in namespaces. Fix class-generator.

* 2016-06-16 hegner 
  - Merge pull request #22 from Steinweg/eike_operators_new

* 2016-06-15 Eike Steinweg 
  - implemented ostream operators   - std::ostream operator<<() created for     components, objects and collections   - create PrintInfo classes for     formatted (tabular) printout of     collections

* 2016-06-14 Benedikt Hegner 
  - add convenience methods to python interface

* 2016-06-13 Benedikt Hegner 
  - fix problem of wrong references being stored

* 2016-06-13 hegner 
  - Merge pull request #21 from gaede/reference_bug_test

* 2016-06-13 Frank Gaede 
  - fixed error message for test case
  - add test case for bug with reading back references

* 2016-06-03 hegner 
  - Merge pull request #19 from jlingema/pyinterface

* 2016-05-24 Joschka Lingemann 
  - Fix close method.
  - Improving py interface.
  - Merge remote branch 'hegner/master'

* 2016-05-19 hegner 
  - Merge pull request #39 from jlingema/master

* 2016-05-19 Joschka Lingemann 
  - Remove hard-coded build-type.

* 2016-05-18 hegner 
  - Merge pull request #38 from jlingema/master

* 2016-05-18 Joschka Lingemann 
  - Increase write-test event number to 2000.
  - Merge pull request #18 from jlingema/master
  - Fixing indentation errors.
  - Reverting to LCG_83 and afs.

* 2016-05-12 Benedikt Hegner 
  - style changes

* 2016-04-26 hegner 
  - Merge pull request #17 from gaede/improve_doc

* 2016-04-26 Frank Gaede 
  - made the ink to doxygen explicit
  -  add more doc links to README
  - dde some documenation links to Readme

* 2016-04-22 hegner 
  - Merge pull request #16 from gaede/relation_bug

* 2016-04-22 Frank Gaede 
  - added ExampleMC to show bug in storing relations
  - bug fix in {ref}_end() for empty relation

* 2016-04-21 Frank Gaede 
  - Merge remote-tracking branch 'hegner/master'

* 2016-04-20 hegner 
  - Merge pull request #15 from gaede/add_getters_setters

* 2016-04-20 Frank Gaede 
  - add unittest "AssociativeContainer" set/map
  - remove incorrect access to reference vector method
  - Merge remote-tracking branch 'hegner/master'
  - Merge pull request #13 from gaede/fix_extracode
  - optional get/set syntax; operator<(); acces to reference vector

* 2016-04-20 Benedikt Hegner 
  - clean up references at better place
  - Merge pull request #14 from jlingema/memleaks

* 2016-04-20 Joschka Lingemann 
  - Merge remote branch 'origin/master' into memleaks
  - Fixing memory leak.

* 2016-04-19 Frank Gaede 
  - fixed extra code generation for ConstObject

* 2016-04-19 hegner 
  - Merge pull request #12 from gaede/extracode_in_components
  - add unit test for extracode in component

* 2016-04-19 Benedikt Hegner 
  - add link to bug tracker
  - clear references properly
  - test for adding extra code to components
  -  fix for handling components w/o extra code
  - allow for extra code declarations in components
  - fix object ownership and leak at deletion of collection

* 2016-04-18 Benedikt Hegner 
  - add support for strings as members
  - removing wrong file
  - adding more docs
  - extend datamodel validation
  - add gitignore file

* 2016-04-17 Benedikt Hegner 
  - move examples into tests to make space for real end-user examples
  - Merge branch 'master' of github.com:hegner/podio into catch
  - migrate from gtest to catch
  - move component cross-checking into validator
  - add hook for datamodel validator
  - fix PODIO-6
  - add support for user specified code snippets
  - add pointerless read interface
  - fix case when multiple many-relations are defined; fix writing example

* 2016-04-13 hegner 
  - Merge pull request #11 from gaede/master

* 2016-04-13 Frank Gaede 
  -  added clusters in clusters as test for infinite recursion
  - Merge branch 'master' of https://github.com/gaede/podio
  - Merge branch 'master' of https://github.com/hegner/podio

* 2016-04-13 Benedikt Hegner 
  - move to using LCG releases from CVMFS
  - set BUILD_TYPE to Debug for now
  - break infinite recursion in EventStore::get
  -  add 'long' to built in types
  - set BUILD_TYPE to Debug for now
  - break infinite recursion in EventStore::get
  -  add 'long' to built in types
  - Merge branch 'master' of github.com:hegner/podio
  - remove unneeded parameter from processEvent
  - Merge pull request #10 from gaede/master
  - starting to add more documentation

* 2016-04-12 Frank Gaede 
  - big fix: clear vector of internal references in Collcection::clear()
  -  set podio_LIBRARIES in podioConfig.cmake

* 2016-04-11 Frank Gaede 
  - creare source directories for class generator if needed

* 2016-04-07 clementhelsens 
  - Merge pull request #37 from jlingema/fix_eos

* 2016-03-17 hegner 
  - Merge pull request #9 from jlingema/fix_eos

* 2016-03-17 Joschka Lingemann 
  - Using TFile::Open allows to also read eos files.

* 2016-03-14 hegner 
  - Merge pull request #8 from jlingema/master

* 2016-03-14 Joschka Lingemann 
  - Add templates file.
  - Fixing whitespace in templates according to FCC style.
  - First round of code clean-up. Moving code templates out of the generator. Move re-used functionality in functions.

* 2016-03-10 Joschka Lingemann 
  - Moving member parsing in separate function.

* 2016-03-09 Joschka Lingemann 
  - Add missing class from example datamodel.

* 2016-03-03 hegner 
  - Merge pull request #34 from cbernet/tutorial_2

* 2016-03-03 Joschka Lingemann 
  - Merge pull request #36 from hegner/master

* 2016-03-01 Benedikt Hegner 
  - help cmake finding the proper gtest
  - use LCG 83 view

* 2016-03-01 Colin 
  - operator== for ObjectID, for python equality tests
  - chaining of files really working

* 2016-02-22 Colin 
  - can now use several input files in python

* 2016-02-19 Joschka Lingemann 
  - Adding dependencies for tests of examples.

* 2016-02-19 Colin 
  - re-added python unittest
  - removed python test temporarily

* 2016-02-18 Joschka Lingemann 
  - Slightly neater way of picking up headers.
  - Replacing fwd declaration with include for EventStore.

* 2016-02-11 Colin 
  - removed read.py which is in fact a unit test. could add a read.py script

* 2016-02-10 Colin 
  - re-adding EventStore unittest from albers-core and adapted to new tree
  - back to right python path

* 2016-03-01 Colin Bernet 
  - Merge pull request #35 from cbernet/master
  - operator== for ObjectID, for python equality tests
  - chaining of files really working

* 2016-02-23 Colin 
  - updated README
  - updated README
  - updated README
  - improved README
  - added python read example

* 2016-02-23 Colin Bernet 
  - Merge pull request #33 from cbernet/master
  - Merge remote-tracking branch 'official/master'
  - can now use several input files in python

* 2016-02-19 Colin Bernet 
  - Merge pull request #32 from jlingema/master
  - Adding dependencies for tests of examples.
  - Merge pull request #31 from cbernet/master
  - re-added python unittest
  - removed python test temporarily
  - Merge pull request #30 from jlingema/fix_include
  - Merge pull request #28 from cbernet/master
  - Slightly neater way of picking up headers.
  - Replacing fwd declaration with include for EventStore.
  - Merge pull request #29 from hegner/master

* 2016-02-18 Benedikt Hegner 
  - add missing ID setting when free-floating collections are added to the store
  - removed read.py which is in fact a unit test. could add a read.py script
  - re-adding EventStore unittest from albers-core and adapted to new tree
  - back to right python path

* 2016-02-09 hegner 
  - Merge pull request #7 from HEP-FCC/master

* 2016-02-09 Joschka Lingemann 
  - Merge pull request #26 from cbernet/unified_env
  - Merge pull request #27 from hegner/master
  - Merge pull request #5 from zaborowska/fileZombie
  - Merge pull request #6 from zaborowska/rename

* 2016-02-09 Benedikt Hegner 
  - beautify generated doxygen

* 2016-02-09 Colin 
  - improving readme

* 2016-01-08 Colin 
  - pick up soft on afs only at cern

* 2016-01-07 Colin 
  - unified environment initialization

* 2016-02-08 zaborowska 
  - Renaming Albers to PODIO

* 2016-02-08 Anna 
  - Checking if file opened correctly

* 2016-02-05 Benedikt Hegner 
  - remove deprecated files after merge
  - Merge branch 'hegner-master'
  - resolve conflicts

* 2016-02-05 hegner 
  - Merge pull request #3 from jlingema/master

* 2016-02-04 Joschka Lingemann 
  - Adding install target for templates.
  - Merge with remote.
  - Change to new options. Add how to list all options.
  - Adding options for doxygen and gtest.

* 2016-02-01 Joschka Lingemann 
  - Merge with remote master.
  - Adding examples to library paths.
  - Changing install destination for pcm / rootmap.
  - Updating instructions to new test folder structure

* 2016-01-29 Joschka Lingemann 
  - Small cosmetic fixes to read-me.

* 2016-01-28 Joschka Lingemann 
  - Adding how to run tests.
  - Changing testing structure to make unit-tests optional.
  - Adapting to Apple specifics.
  - Re-adding Find-ROOT macro.

* 2016-01-26 Joschka Lingemann 
  - Fixing namespaces of members of objects.
  - Fixing namespacing of members for components.
  - Fix: Namespaced component in another component.

* 2016-01-18 Joschka Lingemann 
  - Fixing path for python environment

* 2016-01-15 Joschka Lingemann 
  - Adding python environment and pytools for PyYAML.
  - Splitting implementation and header files.
  - Newly generated datamodel.
  - Adding tests for namespaced data.
  - Fix relations for namespaced datamembers.
  - Removing trailing ; to silence warning.
  - First iteration of namespace support.
  - Adding handling of non-builtin members and fixing compile warnings

* 2016-01-13 Joschka Lingemann 
  - Updating externals according to current FCCSW.

* 2015-11-18 Benedikt Hegner 
  - make vector member code compile
  - add more forward declarations handling class A->B->A reference cases
  - move setter/getter implementation into CC file'

* 2015-10-22 Benedikt Hegner 
  - install dictionaries explicitly

* 2015-10-21 Benedikt Hegner 
  - fix bug for storing unitialized references
  - add new unit test covering non-filled relations

* 2015-10-19 Benedikt Hegner 
  - add more single-ref unit tests; fix infinite loop

* 2015-10-16 Benedikt Hegner 
  - allow chaining of components

* 2015-10-16 hegner 
  - Merge pull request #1 from zaborowska/master

* 2015-10-16 Anna 
  - Merge branch 'master' into test
  - Rename of examples->tests in init and README
  - Missing include of gtest added
  - added 'setter' for components

* 2015-10-15 Benedikt Hegner 
  - add tests for components
  - move data definition syntax into separate markdown file

* 2015-10-14 Benedikt Hegner 
  - new iteration of data model w/ new reference accessors
  - add tests for python interface
  - enable tests for python interface
  - remove accidentally added file
  - install python files
  - move PythonEventStore into podio namespace
  - add at() to methods of the collection
  - renaming package creation script

* 2015-10-13 Benedikt Hegner 
  - update to new datamodel; fix mem leak
  - major refurbishment of podio; new interfaces in doc/doc.md

* 2015-05-12 Benedikt Hegner 
  - adjust scripts to new naming convention

* 2015-05-11 Benedikt Hegner 
  - rename albers into podio

* 2015-04-24 Benedikt Hegner 
  - add CollectionBase to dictionaries

* 2015-04-22 Benedikt Hegner 
  - add new tests; feature complete for lcio
  - next step towards lcio modelling
  - prepare for future vector member feature
  - removing deprecated method for supporting XYZHandle convention
  - upgrading the lcio layout
  - add more buildin types
  - removed code duplication in code generation
  - complete testing of single references
  - remove debug printout for the garbage collection

* 2015-04-21 Benedikt Hegner 
  - adding std::array support
  - update code templates for new refs

* 2015-04-20 Benedikt Hegner 
  - streamline definition syntax
  - Merge branch 'master' of github.com:hegner/albers
  - remove wrongly committed files

* 2015-04-20 Pere Mato 
  - Minor changes in CMakeLists.txt to cope with XCode generation
  - don't use the factory of the collection to create hits and refs
  - clarify namespace
  - update examples w/ new code generation
  - properly set relation vectors in push_back
  - create constructor with full signature

* 2015-04-19 Benedikt Hegner 
  - vector member code #1
  - whitespace fix
  - first step towards lcio yaml file

* 2015-04-17 Benedikt Hegner 
  - trivial python wrapper for event store
  - add simple non-existence check
  - add some doc strings
  - start using IReader
  - use method names acquire/release in ObjBase
  - add more docs
  - unify ref-counting behaviour
  - let XXXObj create relation collections themselves
  - add cloning and better initialization; aligning method implementation order with declaration order

* 2015-04-16 Benedikt Hegner 
  - add proper destructor for XXXObj
  - add header needed for gcc
  - Merge branch 'master' of https://github.com/hegner/albers
  - introducing ObjBase

* 2015-04-16 Pere Mato 
  - test target is reserved from CMake > 3.0
  - Simplify CMakeLists files making use of ROOTConfig.cmake (ROOT build with CMake)
  - move delete command for Obj into the ref counting method
  - inline trivial setters and getters

* 2015-04-15 Benedikt Hegner 
  - improve documentation
  - add doxygen target
  - whitespace
  - remove unneeded headers
  - fix gcc warnings
  - move to ROOT 6 on linux

* 2015-04-14 Benedikt Hegner 
  - allow cyclic dependencies between collections and objects
  - setting collection ID on read
  - deprecate registry
  - first step towards deprecation of Registry
  - rename FooEntry classes to FooObj
  - fix cmake
  - next simplification of class generator; move more code into templates
  - first step towards simplified class generator
  - break circular header dependency in case types have references to the same type

* 2015-04-13 Benedikt Hegner 
  - add handling of improper refs on writing
  - move implementations into cc file
  - add documentation draft; whitespace cleanup
  - add some comments
  - add some comments
  - rename event store methods
  - remove deprecated methods from Registry
  - fixing wrong whitespace
  - upgrade to new API supporting free-floating objects

* 2015-04-01 Benedikt Hegner 
  - add newline at the end of the file

* 2014-12-11 Colin Bernet 
  - Merge pull request #24 from cbernet/clone

* 2014-12-11 Colin 
  - can clone an existing handle into a new collection.

* 2014-12-04 Colin Bernet 
  - Merge pull request #23 from cbernet/edm_1

* 2014-12-03 Colin 
  - removed getters and accessors from Handle
  - edm v1 compiles and runs.
  - class generation: can now have components inside components
  - uniform syntax for members

* 2014-12-02 Colin 
  - edm_1 done, but need to review naming scheme and simplify a bit
  - started to work on first detailed edm prototype.

* 2014-12-02 Benedikt Hegner 
  - remove incomplete relation code; was accidental commit

* 2014-11-26 Benedikt Hegner 
  - adjusted to new structure of headers

* 2014-10-26 Benedikt Hegner 
  - make friend declaration confirm standard; needed by genreflex

* 2014-10-24 hegner 
  - Merge pull request #22 from hegner/master

* 2014-10-24 Benedikt Hegner 
  - fix warnings
  - Merge pull request #21 from hegner/master
  - fix merge problems at runtime
  - revert to original data model setup
  - merge HEAD
  - towards n-to-n relation improvements; make class generator create other directory structure

* 2014-10-10 Colin 
  - fixes for python import

* 2014-10-10 Colin Bernet 
  - Merge pull request #20 from cbernet/python
  - added python specific stuff to install scripts
  - Merge pull request #19 from cbernet/python

* 2014-10-09 Colin 
  - reader can now navigate directly to a given event number

* 2014-10-07 Colin Bernet 
  - Merge pull request #18 from cbernet/python

* 2014-10-07 Colin 
  - python read example ready
  - Merge branch 'python' of github.com:cbernet/albers into python
  - code more pythonic; unittests
  - python event store working

* 2014-10-06 Colin Bernet 
  - Merge pull request #17 from cbernet/python
  - Update README.md

* 2014-10-06 Colin 
  - handles working in python
  - added python test file

* 2014-10-02 Colin Bernet 
  - Merge pull request #16 from cbernet/leptons

* 2014-10-02 Colin 
  - added a muon to the event, and computing its isolation.
  - added print operator for ParticleHandle in utilities
  - Merge pull request #15 from cbernet/more_utility
  - bug fix in Collections. particle overlap checking based on handles

* 2014-10-01 Colin 
  - write and read on 100000 events. solved rare problems by setting an eta cut in the generator.

* 2014-10-01 Colin Bernet 
  - Merge pull request #14 from cbernet/dummy_generator
  - Dummy generator generates two jets
  - on-demand reading tested in read example
  - now using only the jets and the particles from the DummyGenerator
  - Merge pull request #13 from cbernet/coll_in_registry
  - CollectionBase pointers now registered in Registry. Changed writer interface
  - added first version of a dummy multijet event generator

* 2014-09-30 Colin 
  - new utility functions. < operator for handles.

* 2014-09-29 Colin 
  - Jet and vector utility functions used in write and read
  - print functions for Collections, and first utility functions

* 2014-09-28 Benedikt Hegner 
  - take last element and increment by one for end iterator
  - fix bug of invalid references in user land

* 2014-09-27 Benedikt Hegner 
  - next step towards one-to-many-relations; collection has proper members now
  - update and clean templates
  - add first part of one-to-many capabilities to POD and Handle

* 2014-09-25 Colin 
  - using the new datamodel in read and write. Reading problem at first event!
  - developed data model

* 2014-09-25 Colin Bernet 
  - Merge pull request #12 from cbernet/few_easy_mods
  - macos X initialization script
  - Merge branch 'master' into merge_attempt

* 2014-09-24 Benedikt Hegner 
  - change syntax for data generation; part 1

* 2014-09-24 Colin 
  - a bit of clean up

* 2014-09-23 Benedikt Hegner 
  - simplify the structure of the PODs; now really look like structs
  - add functionality of defining components; simplify datamodel example and writer code
  - fix pedantic compilation warning

* 2014-09-23 Colin Bernet 
  - Merge pull request #11 from cbernet/event_loop

* 2014-09-23 Colin 
  - event loop in reader working, but messy ! overall design to be clarified
  - bug fix: not writing to TFile anymore just after filling the tree
  - Merge pull request #10 from cbernet/event_loop
  - implemented event loop in writer. big performance issue
  - Merge pull request #9 from cbernet/simple_edm

* 2014-09-22 Colin 
  - example mains adapted to new EDM
  - bug fix for multiple includes
  - realistic data model

* 2014-09-22 Colin Bernet 
  - Merge pull request #8 from cbernet/auto_handle_ptr
  - buffer address in embedded handles set in prepareAfterRead automatically
  - initialed collectionID to 0, and improved const-correctness
  - collection id is now unsigned
  - Merge pull request #7 from cbernet/macos_build_merge
  - Merge branch 'macos_build' into macos_build_merge
  - Merge branch 'master' of github.com:HEP-FCC/albers
  - conditional for mac os x
  - able to build on macos, need to test lxplus6

* 2014-09-21 Colin Bernet 
  - reformatted README properly

* 2014-09-21 Colin 
  - commented the code: Reader, Handle

* 2014-09-19 Colin 
  - Merge branch 'documentation' of github.com:cbernet/albers into documentation

* 2014-09-18 Colin 
  - documentation and questions: EventStore, Collection, Registry, Writer
  - documentation and questions: EventStore, Collection, Registry
  - added an environment setup script

* 2014-09-17 Colin 
  - more specific installation instructions
  - added a particle class, aiming for a Z boson example

* 2014-09-17 Benedikt Hegner 
  - fix for the fact that yaml parsing doesn't preserve ordering

* 2014-09-16 Benedikt Hegner 
  - rearrange the example

* 2014-09-15 Benedikt Hegner 
  - fix md
  - Merge branch 'master' of https://github.com/hegner/albers
  - add initial descriptions
  - make it so

* 2014-09-15 hegner 
  - Initial commit

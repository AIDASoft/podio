# v01-07

* 2026-01-14 Juan Miguel Carceller ([PR#921](https://github.com/AIDASoft/podio/pull/921))
  - Fix the name of the fixture for the read_garbage test

* 2026-01-14 Juan Miguel Carceller ([PR#916](https://github.com/AIDASoft/podio/pull/916))
  - Fix multiple issues in `checkConsistentColls` and add a test.

* 2026-01-12 Juan Miguel Carceller ([PR#917](https://github.com/AIDASoft/podio/pull/917))
  - Make sure a file is closed after opening it by using an `std::unique_ptr` to make sure the TFile is deleted.

* 2026-01-12 Juan Miguel Carceller ([PR#915](https://github.com/AIDASoft/podio/pull/915))
  - Remove the llvm-header-guard check for clang-tidy, seems to be buggy with LLVM 21.

* 2026-01-12 Juan Miguel Carceller ([PR#897](https://github.com/AIDASoft/podio/pull/897))
  - Make the python writers have the same behavior as the C++ writers to decide what to do when no collections are passed

* 2026-01-10 Juan Miguel Carceller ([PR#913](https://github.com/AIDASoft/podio/pull/913))
  - Use a cache for compiled jinja templates to reduce template compilation time.

* 2026-01-10 Wouter Deconinck ([PR#911](https://github.com/AIDASoft/podio/pull/911))
  - fix: install into free-threaded python3.14t/site-packages prefixes

* 2026-01-10 Juan Miguel Carceller ([PR#910](https://github.com/AIDASoft/podio/pull/910))
  - Forward-declare PyObject not to have to include Python.h

* 2025-12-23 Wouter Deconinck ([PR#909](https://github.com/AIDASoft/podio/pull/909))
  - Move static constexpr to inline constexpr to avoid exposed TU-local entities

* 2025-12-22 Wouter Deconinck ([PR#908](https://github.com/AIDASoft/podio/pull/908))
  - Move Pythonizations detail implementations to separate source file

* 2025-12-22 Thomas Madlener ([PR#906](https://github.com/AIDASoft/podio/pull/906))
  - `podio-dump`: Make `-e`/`--entries` accept `-1` to dump all of the entries of a given category

* 2025-12-18 Thomas Madlener ([PR#904](https://github.com/AIDASoft/podio/pull/904))
  - Create a per ref concurrency group for doc building

* 2025-12-18 Thomas Madlener ([PR#902](https://github.com/AIDASoft/podio/pull/902))
  - Add a cmake option (`PODIO_ENABLE_SCHEMA_EVOLUTION_TESTS`) to toggle the schema evolution tests that involve code generation as these more than double the time it takes to run `cmake`.

* 2025-12-18 Aimilianos Koulouris ([PR#901](https://github.com/AIDASoft/podio/pull/901))
  - podio-merge-files: add progress bar and time measurement per step; add short option for output files argument

* 2025-12-17 Thomas Madlener ([PR#903](https://github.com/AIDASoft/podio/pull/903))
  - Move black to earlier in the pre-commit config

* 2025-12-08 Juan Miguel Carceller ([PR#896](https://github.com/AIDASoft/podio/pull/896))
  - Fix rpaths in podio, not set correctly because ${LIBDIR} is empty
  - Remove the linker flags that force to use `RPATH` instead of the more modern and recommended `RUNPATH` (the difference being `RPATH` is searched before `LD_LIBRARY_PATH` and `RUNPATH` is searched after `LD_LIBRARY_PATH`).

* 2025-12-08 Thomas Madlener ([PR#890](https://github.com/AIDASoft/podio/pull/890))
  - Do not read `ExtraCode` for old schemas when doing schema evolution

* 2025-12-08 Thomas Madlener ([PR#889](https://github.com/AIDASoft/podio/pull/889))
  - Make schema evolution code generation handle dropped components

* 2025-12-05 Thomas Madlener ([PR#898](https://github.com/AIDASoft/podio/pull/898))
  - Update documentation requirements and make doc building run on pull requests

* 2025-12-04 Juan Miguel Carceller ([PR#895](https://github.com/AIDASoft/podio/pull/895))
  - Fix warnings, tabs, anchors in the documentation

* 2025-12-04 Thomas Madlener ([PR#828](https://github.com/AIDASoft/podio/pull/828))
  - Allow to pass multiple old datamodel schemas to `podio_class_generator.py`

* 2025-12-02 Thomas Madlener ([PR#894](https://github.com/AIDASoft/podio/pull/894))
  - Make sure to generate the Collection headers to the front of the list of header file that is passed to root dictionary generation.

* 2025-12-01 Juan Miguel Carceller ([PR#893](https://github.com/AIDASoft/podio/pull/893))
  - Add a --gdb switch to the podio-dump wrapper to simplify debugging with `podio-dump`

* 2025-12-01 Thomas Madlener ([PR#845](https://github.com/AIDASoft/podio/pull/845))
  - Change the grammar of the schema evolution files to be more descriptive and also can handle multiple schema evolutions
  - **This is a breaking change for code generation if you are using multiple schema and need manual intervention**

* 2025-11-28 Juan Miguel Carceller ([PR#885](https://github.com/AIDASoft/podio/pull/885))
  - Add some documentation about subset collections

* 2025-11-27 Thomas Madlener ([PR#888](https://github.com/AIDASoft/podio/pull/888))
  - Fix misleading warning about a *potential rename* when adding and dropping unrelated members to a datatype

* 2025-11-27 Thomas Madlener ([PR#887](https://github.com/AIDASoft/podio/pull/887))
  - Make the test that checks if renaming members works more comprehensive

* 2025-11-27 Thomas Madlener ([PR#886](https://github.com/AIDASoft/podio/pull/886))
  - Make sure that array members are generated correctly in older versions of the `Data` PODs

* 2025-11-25 Juan Miguel Carceller ([PR#882](https://github.com/AIDASoft/podio/pull/882))
  - Fix memory leak in the TTree Reader.
  - Run several tests that use TTrees with the sanitizers

* 2025-11-24 Mateusz Jakub Fila ([PR#881](https://github.com/AIDASoft/podio/pull/881))
  - Rename `podio_test_hashes` to `podio-test-hashes` and move to tools directory

* 2025-11-21 Juan Miguel Carceller ([PR#878](https://github.com/AIDASoft/podio/pull/878))
  - Add a column in the output of `podio-dump` with information about the total size that a collection takes on disk and compression, only for TTrees. The compression factor here is `sum of total bytes for all branches / sum of compressed bytes for all branches` for a given collection.

* 2025-11-20 Thomas Madlener ([PR#880](https://github.com/AIDASoft/podio/pull/880))
  - Make sure that the `DataSource` does not read unnecessary collections if the collections have been limited.

* 2025-11-20 Thomas Madlener ([PR#868](https://github.com/AIDASoft/podio/pull/868))
  - Make `Frame::get<T>` throw on non-existing collections instead of silently returning an empty collection. **Note that this is a behavioral change with respect to previous versions of podio for non-existing collections.**

* 2025-11-18 Thomas Madlener ([PR#863](https://github.com/AIDASoft/podio/pull/863))
  - Make the readers skip unreadable collections instead of either crashing badly or returning entirely empty FrameData if a single collection cannot be read.

* 2025-11-11 Thomas Madlener ([PR#872](https://github.com/AIDASoft/podio/pull/872))
  - Add a CI workflow that builds all the way to EDM4eic

* 2025-11-10 Juan Miguel Carceller ([PR#877](https://github.com/AIDASoft/podio/pull/877))
  - Do not allocate dynamically in the ROOTReader when it's not necessary, make the TFile be part of the ROOTReader instead of storing a smart pointer. Don't create a TTree with `new` that never gets destroyed.

* 2025-11-05 Thomas Madlener ([PR#876](https://github.com/AIDASoft/podio/pull/876))
  - Remove unreachable code from `Frame::put` methods and clarify docs that it might throw `std::invalid_argument`

* 2025-11-05 Thomas Madlener ([PR#875](https://github.com/AIDASoft/podio/pull/875))
  - Remove an unnecessary (and wrong) `hasID` check in the `podio::DataSource`.

* 2025-11-05 Thomas Madlener ([PR#874](https://github.com/AIDASoft/podio/pull/874))
  - Use an actually existing member function in documentation of interfaces

* 2025-11-05 Juan Miguel Carceller ([PR#873](https://github.com/AIDASoft/podio/pull/873))
  - Do not use ulimit in schema evolution tests, since https://github.com/AIDASoft/podio/pull/846 should catch some cases
  - Remove the unused option `PODIO_NO_MEMLIMIT_SCHEMA_EVOL_TESTS`

* 2025-11-04 Juan Miguel Carceller ([PR#870](https://github.com/AIDASoft/podio/pull/870))
  - Pass a unique_ptr to the frame constructor in Python, making the constructor from any data `FrameData&&` not needed.

* 2025-10-30 Juan Miguel Carceller ([PR#871](https://github.com/AIDASoft/podio/pull/871))
  - Remove value for a header guard not to confuse `clang-format`

# v01-06

* 2025-10-21 Juan Miguel Carceller ([PR#867](https://github.com/AIDASoft/podio/pull/867))
  - Use a handle to a file or stream in the writers to check if we are done

* 2025-10-20 Juan Miguel Carceller ([PR#865](https://github.com/AIDASoft/podio/pull/865))
  - Make `RNTupleReader::getEntries` `const` since it's also `const` in the other readers

* 2025-10-20 Juan Miguel Carceller ([PR#864](https://github.com/AIDASoft/podio/pull/864))
  - Do not load the deprecated pythonization collection_subscript

* 2025-10-20 Juan Miguel Carceller ([PR#862](https://github.com/AIDASoft/podio/pull/862))
  - Make it possible to read RNTuples with removed types, using `SetEmulateUnknownTypes`
  - Add a test that fails without this fix and works with this fix (only RNTuple was failing)

* 2025-10-20 Juan Miguel Carceller ([PR#846](https://github.com/AIDASoft/podio/pull/846))
  - Add ugly code to trigger an error when reading garbage

* 2025-10-14 Juan Miguel Carceller ([PR#860](https://github.com/AIDASoft/podio/pull/860))
  - Deprecate `isValid()` for collections and add `hasID()` to check if a collection has an ID. This allows to find out if `frame.get<type>("name")` didn't find a collection, since it will return an empty one in this case.

* 2025-10-10 Juan Miguel Carceller ([PR#857](https://github.com/AIDASoft/podio/pull/857))
  - podio-merge-files: import podio later for faster help messages and use get_reader

* 2025-10-10 Thomas Madlener ([PR#855](https://github.com/AIDASoft/podio/pull/855))
  - Add an indication whether a collection is a subset collection to the output of `podio-dump`

# v01-05

* 2025-10-08 Juan Miguel Carceller ([PR#854](https://github.com/AIDASoft/podio/pull/854))
  - Add documentation about some of the existing tools

* 2025-10-06 Thomas Madlener ([PR#803](https://github.com/AIDASoft/podio/pull/803))
  - Generate the `XYZData` PODs into a `v{{ old_version }}` namespace in order to have the old definitions available for schema evolution purposes if necessary.
    - The current version of the PODs are not put into such a version namespace.
    - The old versions are generated even if they are identical in their definition to the current namespace.
    - All versions that have been provided to the code generation will be generated.

* 2025-09-29 Thomas Madlener ([PR#852](https://github.com/AIDASoft/podio/pull/852))
  - Make sure using `PODIO_SIOBLOCK_PATH` works without having `LD_LIBRARY_PATH` set appropriately.
    - Without this fix the `SioBlock` libraries also had to be on `LD_LIBRARY_PATH` for the dynamic loading to work. Now `PODIO_SIOBLOCK_PATH` is enough.

* 2025-09-29 Thomas Madlener ([PR#844](https://github.com/AIDASoft/podio/pull/844))
  - Split schema evolution detection into two parts; detecting changes and judging whether they can be handled automatically or not

* 2025-09-24 Juan Miguel Carceller ([PR#849](https://github.com/AIDASoft/podio/pull/849))
  - Remove support for CPack since it is unused, points to wrong information and is not working. See also https://github.com/key4hep/EDM4hep/pull/445.

* 2025-09-23 Juan Miguel Carceller ([PR#841](https://github.com/AIDASoft/podio/pull/841))
  - Set ROOT_LIBRARY_PATH in tests to better control the environment

* 2025-09-12 Thomas Madlener ([PR#843](https://github.com/AIDASoft/podio/pull/843))
  - Randomize the order of the tests in CI to detect implicit test dependencies and avoid introducing new ones
  - Add global timeouts for running tests to fail earlier in case of CVMFS loading issues

* 2025-09-10 Thomas Madlener ([PR#836](https://github.com/AIDASoft/podio/pull/836))
  - Run schema evolution read tests with limited memory to avoid ROOT running away with all the memory.

* 2025-09-09 Juan Miguel Carceller ([PR#840](https://github.com/AIDASoft/podio/pull/840))
  - Delete an extra Python3_INCLUDE_DIRS, forgotten in https://github.com/AIDASoft/podio/pull/839

* 2025-09-09 Juan Miguel Carceller ([PR#839](https://github.com/AIDASoft/podio/pull/839))
  - Add the Python headers to the system includes to avoid warnings

* 2025-09-08 Juan Miguel Carceller ([PR#837](https://github.com/AIDASoft/podio/pull/837))
  - Move some includes where they are used and add `const` in a few places

* 2025-09-04 Thomas Madlener ([PR#817](https://github.com/AIDASoft/podio/pull/817))
  - Refactor the way schema evolution tests are done to make them easier to adapt and understand
    - Introduce a mini testing framework that allows to focus on a single schema change for a given test
    - Remove the duplication of the test datamodel with various schema changes sprinkled across it
  - Enable some schema evolution tests for RNTuple

* 2025-09-03 Thomas Madlener ([PR#835](https://github.com/AIDASoft/podio/pull/835))
  - Remove an unnecessary `depends_on` argument from the tests for `podio-vis` as they do not depend on any other test and all the necessary inputs are part of the source code.

* 2025-09-03 Juan Miguel Carceller ([PR#834](https://github.com/AIDASoft/podio/pull/834))
  - Add CMake fixtures for tests that read and write files so that reading doesn't happen if writing fails and running the reading tests requires the write tests

* 2025-08-28 Juan Miguel Carceller ([PR#832](https://github.com/AIDASoft/podio/pull/832))
  - Add types with all the collection types of a datamodel in datamodel.h
  - Add tests for the new types
  - Use single spaces instead of two spaces between types in the list

* 2025-08-27 Thomas Madlener ([PR#831](https://github.com/AIDASoft/podio/pull/831))
  - Fix a minor bug in generation of all collections header file

* 2025-08-27 Thomas Madlener ([PR#830](https://github.com/AIDASoft/podio/pull/830))
  - Add a `__repr__` method to some of the classes in the generator python utils for improved printouts

* 2025-08-27 Juan Miguel Carceller ([PR#829](https://github.com/AIDASoft/podio/pull/829))
  - Add a `podio::UserDataCollectionTypes` type list containing all the available user data collection types.

* 2025-08-26 Mateusz Jakub Fila ([PR#826](https://github.com/AIDASoft/podio/pull/826))
  - Fix typo causing error in non-const overload of `LinkCollection::at(size_t)`

* 2025-08-26 Mateusz Jakub Fila ([PR#824](https://github.com/AIDASoft/podio/pull/824))
  - Add automatic collection subscript pythonization to `UserDataCollection` and `LinkCollection`

* 2025-08-26 Thomas Madlener ([PR#823](https://github.com/AIDASoft/podio/pull/823))
  - Switch to newer LCG release and compilers for sanitizer CI workflows
  - Remove some unnecessary debug output from the unittests that trips UBSan on gcc15

* 2025-08-25 Thomas Madlener ([PR#822](https://github.com/AIDASoft/podio/pull/822))
  - Update CI workflows to also build against root with c++23
  - Bump the minimum required version of Catch2 to 3.5 as that fixes some compiler warnings for c++23

* 2025-08-25 Mateusz Jakub Fila ([PR#810](https://github.com/AIDASoft/podio/pull/810))
  - Add automatic pythonization of collection subscript with C++ callback
  - Deprecate pythonization of collection subscript with Python callback

* 2025-08-22 Thomas Madlener ([PR#821](https://github.com/AIDASoft/podio/pull/821))
  - Fix code generation to allow for types to have a `OneToOneRelation` to itself. This was previously not possible (but seemingly unused)

* 2025-08-22 Mateusz Jakub Fila ([PR#820](https://github.com/AIDASoft/podio/pull/820))
  - Run cppcheck pre-commit hook only for c++ files

* 2025-08-22 Thomas Madlener ([PR#819](https://github.com/AIDASoft/podio/pull/819))
  - Use an existing member function for building includes in components code generation

* 2025-08-20 Thomas Madlener ([PR#816](https://github.com/AIDASoft/podio/pull/816))
  - Fix missed renaming of member names that were missed in [#797](https://github.com/AIDASoft/podio/pull/797)

* 2025-08-20 Thomas Madlener ([PR#815](https://github.com/AIDASoft/podio/pull/815))
  - Remove unused datatype from the original model and re-use another one to do the same test

* 2025-08-14 jmcarcell ([PR#812](https://github.com/AIDASoft/podio/pull/812))
  - Remove the deprecated operator-> in collections

# v01-04

* 2025-08-13 jmcarcell ([PR#809](https://github.com/AIDASoft/podio/pull/809))
  - Simplify constructors by using member initializers and only initialize in the constructor what changes with respect to the defaults

* 2025-08-13 Thomas Madlener ([PR#799](https://github.com/AIDASoft/podio/pull/799))
  - Make sure that members that are builtin types can also be renamed with ROOT

* 2025-08-13 Thomas Madlener ([PR#797](https://github.com/AIDASoft/podio/pull/797))
  - Refactor parts of the code generation that are related to schema evolution
  - Remove some duplication and homogenize information from schema changes detection system

* 2025-08-12 jmcarcell ([PR#813](https://github.com/AIDASoft/podio/pull/813))
  - Add cppcheck to pre-commit, together with a file with the warnings that are suppressed
  - Fix the existing warnings

* 2025-08-12 jmcarcell ([PR#808](https://github.com/AIDASoft/podio/pull/808))
  - Define `typeName`, `valueTypeName` and `dataTypeName` outside of each collection and `UserDataCollection` class, in addition to the existing definition inside the class to prevent an issue in which every library under LD_LIBRARY_PATH is opened when creating a collection in Python

* 2025-08-11 jmcarcell ([PR#811](https://github.com/AIDASoft/podio/pull/811))
  - Deprecate the generation of an `operator->` for collections which made them usable as if they were pointers even when dealing with regular values.
    - **Depending on your usage, the removal of this operator might break code that uses a generated datamodel. The recommended fix is to simply switch to using `.` instead of `->` when working with (non-pointer) collections.**
    - The original purpose for adding this was the (now abandoned) seamless drop-in replacement for LCIO, which uses collection pointers instead of values.

* 2025-07-07 Thomas Madlener ([PR#807](https://github.com/AIDASoft/podio/pull/807))
  - Make `PODIO_GENERATE_DICTIONARY` a thin wrapper around `REFLEX_GENERATE_DICTIONARY` instead of a re-implementation

* 2025-07-01 Thomas Madlener ([PR#804](https://github.com/AIDASoft/podio/pull/804))
  - Fix deprecation warning in pre-commit by updating hook version to latest one (v5.0.0)

* 2025-06-30 Thomas Madlener ([PR#802](https://github.com/AIDASoft/podio/pull/802))
  - Switch back to the standard version of the run-lcg-view action, using docker as container runtime, since the upstream issue with cvmfs has been fixed.

* 2025-06-27 jmcarcell ([PR#798](https://github.com/AIDASoft/podio/pull/798))
  - Allow calling finish() multiple times, with no effect after the first by checking the value of `m_finished`.

* 2025-06-20 Thomas Madlener ([PR#786](https://github.com/AIDASoft/podio/pull/786))
  - Introduce a `FrameDataType` concept and constrain the `Frame` constructors with it

* 2025-06-19 jmcarcell ([PR#784](https://github.com/AIDASoft/podio/pull/784))
  - Change the default setting of the generic writer to RNTuple when `PODIO_DEFAULT_WRITE_RNTUPLE` is set
  - Add a test that writes an RNTuple because `PODIO_DEFAULT_WRITE_RNTUPLE` is set
  - Since there was no documentation about the readers and writers, add documentation about them and include `PODIO_DEFAULT_WRITE_RNTUPLE` in it.

* 2025-06-18 jmcarcell ([PR#785](https://github.com/AIDASoft/podio/pull/785))
  - Add missing override to the destructor of UserDataCollection and LinkCollectionImpl

* 2025-06-13 Thomas Madlener ([PR#791](https://github.com/AIDASoft/podio/pull/791))
  - Disable the julia tests in the sanitizers workflows as they do not test anything meaningful there
  - Use podman for now to run the CI that requires Julia

* 2025-06-13 jmcarcell ([PR#789](https://github.com/AIDASoft/podio/pull/789))
  - Use python packages from the Key4hep stack in the pre-commit workflow
  - Fix pre-commit

* 2025-06-13 jmcarcell ([PR#788](https://github.com/AIDASoft/podio/pull/788))
  - Add the value of `ROOT_INCLUDE_PATH` from the environment to the tests
  - Call `k4_local_repo` in the key4hep workflow

* 2025-05-26 jmcarcell ([PR#783](https://github.com/AIDASoft/podio/pull/783))
  - Use size_t instead of unsigned int for `operator[]`, `at`, `getID` and `setID` for Link collections, since it is the type used for data type collections and UserDataCollections.

* 2025-05-20 jmcarcell ([PR#781](https://github.com/AIDASoft/podio/pull/781))
  - Fix Unity builds with SIO enabled by giving a variable different names

# v01-03

* 2025-05-14 jmcarcell ([PR#780](https://github.com/AIDASoft/podio/pull/780))
  - Read the collection information starting with version 1.2.999 to maintain compatibility with the files produced with podio 1.2.99
  - Bump the version to 1.2.999

* 2025-05-13 jmcarcell ([PR#779](https://github.com/AIDASoft/podio/pull/779))
  - Ignore the datasource tests when using the address sanitizer since the tests that create their input files are also ignored
  - Enable `-DENABLE_DATASOURCE` when building with sanitizer (although all the related tests will be ignored)

* 2025-05-13 jmcarcell ([PR#775](https://github.com/AIDASoft/podio/pull/775))
  - Define `typeName` outside of each object class, in addition to the existing definition inside the class.

* 2025-05-13 Mateusz Jakub Fila ([PR#758](https://github.com/AIDASoft/podio/pull/758))
  - Formalize expected collection interface with a concept
  - Add `at` and `create` to `UserDataCollection`

* 2025-05-08 jmcarcell ([PR#777](https://github.com/AIDASoft/podio/pull/777))
  Clean up the ROOT readers and writers
    - Add const where possible
    - Use functions from `std::ranges` when possible
    - Do not use `new` and `delete` when it's not needed (`ROOTReader.cc`)
    - Clean up includes
    - Change some indexes to size_t instead of int to avoid implicit conversions (doesn't change anything)

* 2025-05-07 Thomas Madlener ([PR#711](https://github.com/AIDASoft/podio/pull/711))
  - Store the collection information in a proper `struct` instead of using a `tuple` to facilitate access for non podio based backends (e.g. Julia)
  - Harmonize the format of RNTuple and TTree based ROOT backends
    - Both now store the collection information into the `podio_metadata` TTree / Model as a `vector<podio::root_utils::CollectionWriteInfo>`, which contains all the necessary information for reading a collection.
    - The collection ID and the name are part of this struct, so the `CollectionIDTable` is no longer written separately.
  - **This is a breaking change in the format if you rely on reading the metadata about the stored collections.**

* 2025-04-29 Thomas Madlener ([PR#773](https://github.com/AIDASoft/podio/pull/773))
  - Make sure compilation with gcc11 is possible again and add a workflow based on LCG_107 and Ubuntu 22 to check compatibility in CI

* 2025-04-28 Thomas Madlener ([PR#774](https://github.com/AIDASoft/podio/pull/774))
  - Cleanup some no longer necessary cmake checks

* 2025-04-28 jmcarcell ([PR#767](https://github.com/AIDASoft/podio/pull/767))
  - Pass by const reference in the object constructor and member setters. Decide based on the the `is_builtin` member in python, that excludes containers of builtin types and datamodel types.
  - Add const when possible for other function parameters
  - Update the documentation for the `signature` member.

* 2025-04-25 jmcarcell ([PR#771](https://github.com/AIDASoft/podio/pull/771))
  - Remove double locking in `prepareForWrite`; acquire the lock only once

* 2025-04-23 Thomas Madlener ([PR#769](https://github.com/AIDASoft/podio/pull/769))
  - Make sure that the sanitizer build workflows also use ccache and avoid concurrent runs on the same git ref

* 2025-04-23 Thomas Madlener ([PR#768](https://github.com/AIDASoft/podio/pull/768))
  - Keep RNTuple support enabled for ROOT 6.32, but emit a warning at CMake stage

* 2025-04-22 Thomas Madlener ([PR#757](https://github.com/AIDASoft/podio/pull/757))
  - Make sure that podio RNTuple readers and writers work with ROOT 6.34 and later versions.
   - **Bump the minimum required version of ROOT for RNTuple support to 6.34**. That is where the file format has been stabilized, but the API has only moved out of the `Experimental` namespace in 6.36.

* 2025-04-15 Thomas Madlener ([PR#764](https://github.com/AIDASoft/podio/pull/764))
  - Use `ccache` in CI to speed up workflows
  - Add more workflows based on Key4hep
  - Cancel concurrent workflows for changes pushed before they finished

* 2025-04-09 Thomas Madlener ([PR#763](https://github.com/AIDASoft/podio/pull/763))
  - Remove a leftover empty CMake variable. The corresponding code has been removed in [#662](https://github.com/AIDASoft/podio/pull/662).

* 2025-04-09 jmcarcell ([PR#761](https://github.com/AIDASoft/podio/pull/761))
  - Generate in `datamodel.h` additional lists with all the data and link types
  - Add tests using this functionality

* 2025-04-08 jmcarcell ([PR#762](https://github.com/AIDASoft/podio/pull/762))
  - Fix `podio::Link::typeName` that was wrongly including "Collection"

* 2025-04-01 Thomas Madlener ([PR#756](https://github.com/AIDASoft/podio/pull/756))
  - Cache *ExternalData* fetched by CMake to avoid failing CI due to potentially flaky or overloaded inrastructure

* 2025-04-01 Mateusz Jakub Fila ([PR#748](https://github.com/AIDASoft/podio/pull/748))
  - Change type of `typeName` members to `std::string_view`. Add utility to concatenate `std::string_view`s at compile time.
  - Add `typeName` static member to link collections. `typeName` can be checked at compilation time.
  - Deprecate `linkCollTypeName` and `linkTypeName` helper functions.

* 2025-03-28 Mateusz Jakub Fila ([PR#759](https://github.com/AIDASoft/podio/pull/759))
  - Fix a few more typos readmes, docstrings, assert messages

* 2025-03-26 Mateusz Jakub Fila ([PR#755](https://github.com/AIDASoft/podio/pull/755))
  - allow `^` in units in datamodel definitions

* 2025-03-26 Mateusz Jakub Fila ([PR#754](https://github.com/AIDASoft/podio/pull/754))
  - Add `front`, `back`, `cbegin`, `cend` and `operator bool` to the `RelationRange` so it has all the methods from `std::ranges::view_interface`

* 2025-03-25 Mateusz Jakub Fila ([PR#752](https://github.com/AIDASoft/podio/pull/752))
  - Update `max_size` reported by collections to include limit due to `ObjectID` index type

* 2025-03-21 jmcarcell ([PR#751](https://github.com/AIDASoft/podio/pull/751))
  - Add const to void* in OrderKey.h

* 2025-03-10 Mateusz Jakub Fila ([PR#749](https://github.com/AIDASoft/podio/pull/749))
  - Fix typo in tests so the usage of links with unordered associative containers is actually tested

* 2025-03-10 scott snyder ([PR#747](https://github.com/AIDASoft/podio/pull/747))
  - Fix a build issue when the python version used is different than the one found by default by cmake.

* 2025-03-10 jmcarcell ([PR#746](https://github.com/AIDASoft/podio/pull/746))
  - Support Unity builds Add missing include guards and make sure some functions such as `createBuffer` have a different name for each collection.

* 2025-03-07 Thomas Madlener ([PR#745](https://github.com/AIDASoft/podio/pull/745))
  - Fix the (potential) import error for `podio.version.build_version` by not relying on ROOTs JIT, but rather parsing the python `__version__` into the correct type.

* 2025-03-06 Thomas Madlener ([PR#744](https://github.com/AIDASoft/podio/pull/744))
  - Add v01-01 and v01-02 input root files for backwards compatibility checks

* 2025-03-06 Mateusz Jakub Fila ([PR#743](https://github.com/AIDASoft/podio/pull/743))
  - Prevent error-prone copy-assignment to temporary datatype and link objects. The compilation error instead of unexpected behaviour when by mistake using the podio collection iterators as "LegacyOutputIterator" is more very likely (e.g. `std::fill` will most likely not compile instead compiling but giving wrong results)

* 2025-02-28 Mateusz Jakub Fila ([PR#738](https://github.com/AIDASoft/podio/pull/738))
  - Add `std::hash` for podio datatype objects, interfaces and links. Datatypes, interfaces and links can be used in unordered associative containers such as `std::unordered_set` or `std::unordered_map`

* 2025-02-28 Mateusz Jakub Fila ([PR#720](https://github.com/AIDASoft/podio/pull/720))
  - Collection iterators fulfill `forward_iterator`, `bidirectional_iterator` and `random_access_iterator`concepts. The collections can be used with more categories of range algorithms - up to `random_access_range`, for example `std::adjacent_find`, `std::lower_bound`, `std::fold_right`, and more views like `std::ranges::views::reverse`. 
  - `LinkCollection` and its iterators fulfill concepts up to `random_access_range` and `random_access_iterator`.
  - Add reverse iterator methods to Collections, `LinkCollection` and `UserDataCollection`.
  - **CollectionIterators of generated datamodels now define `operator<=>` instead of `operator!=`. This will require at least c++20 to compile, but should not affect existing behavior.**

* 2025-02-21 Mateusz Jakub Fila ([PR#742](https://github.com/AIDASoft/podio/pull/742))
  - Fixed argument resolution in the roundtrip test to prevent failures when SIO is not present.

* 2025-02-20 jmcarcell ([PR#741](https://github.com/AIDASoft/podio/pull/741))
  - Remove warning about C++17 that is now obsolete

* 2025-02-19 Dmitry Kalinkin ([PR#733](https://github.com/AIDASoft/podio/pull/733))
  - Added `std::hash<podio::ObjectID>` specialization to allow `std::unordered_map<podio::ObjectID, T>`

* 2025-02-18 Mateusz Jakub Fila ([PR#737](https://github.com/AIDASoft/podio/pull/737))
  - Replace  `sizeof(T) == 0` with `always_false` to fail static assertions in `constexpr if`

* 2025-02-17 Mateusz Jakub Fila ([PR#736](https://github.com/AIDASoft/podio/pull/736))
  - fix setting link with interface without specifying link direction

* 2025-02-17 Mateusz Jakub Fila ([PR#729](https://github.com/AIDASoft/podio/pull/729))
  - POSIX glob patterns can be used in `makeReader` and `CreateDataSource`. Added standalone helper `podio::utilities::expand_glob`to resolve globs.
  - Added passing a list of files to `get_reader`

* 2025-02-12 jmcarcell ([PR#734](https://github.com/AIDASoft/podio/pull/734))
  - Add LANGUAGES CXX to CMakeLists.txt to disable checks for a C compiler

* 2025-02-04 Thomas Madlener ([PR#732](https://github.com/AIDASoft/podio/pull/732))
  - Make sure that `isValid` works the same for `LinkCollection` as it does for other collections
  - Make sure to set the `collectionID` for all links in a collection

* 2025-01-31 Thomas Madlener ([PR#730](https://github.com/AIDASoft/podio/pull/730))
  - Switch to a `LCG_106b` for the sanitizer workflows in CI to pick up a newer version of ROOT.
  - Swtich to `LCG_106b` for tests with RNTuple
  - Add an `LCG_104` (root v6.28.04) based workflow for ensuring compatibility with the minimal version of ROOT

* 2025-01-21 Dmitry Kalinkin ([PR#728](https://github.com/AIDASoft/podio/pull/728))
  - Improved exception message for the case when relation to untracked object is to be persisted.

* 2025-01-21 Mateusz Jakub Fila ([PR#727](https://github.com/AIDASoft/podio/pull/727))
  - Make `RelationRange` fulfill the `std::ranges::view` and `std::ranges::borrowed_range` concepts

* 2025-01-20 Thomas Madlener ([PR#724](https://github.com/AIDASoft/podio/pull/724))
  - Start to refactor the read tests partially to break up the large `processEvent` function into smaller more manageable chunks.
  - Fix a small logic bug in the creation of the test files

* 2025-01-17 Mateusz Jakub Fila ([PR#726](https://github.com/AIDASoft/podio/pull/726))
  - Removed redundant check for C++20
  - Removed iterator test that is no longer required in C++

* 2025-01-17 Mateusz Jakub Fila ([PR#725](https://github.com/AIDASoft/podio/pull/725))
  - Updated `LinkCollectionIterator` to fulifl the same iterator concept and iterator category as other collection iterators. `LinkCollection` fulfills the same range concepts as other collections. Algorithms like `std::ranges::find` or `std::count` are supported.

* 2025-01-16 Thomas Madlener ([PR#723](https://github.com/AIDASoft/podio/pull/723))
  - Replace several repeated map lookups with a single lookup inside RNTupleReader

* 2025-01-09 jmcarcell ([PR#708](https://github.com/AIDASoft/podio/pull/708))
  - Add support for reading several RNtuple files

* 2025-01-08 jmcarcell ([PR#698](https://github.com/AIDASoft/podio/pull/698))
  - **Make podio require c++20 and remove compatibility with c++17**
  - Simplify template code by using `concept` and `require` when possible. In some places like the `UserDataCollection` and `GenericParameters` it seems that cppyy (3.1.2 with ROOT 6.32.06) doesn't like `requires` nor `concept` so they can't be changed for now.
  - Use `consteval` when possible which, unlike `constexpr`, guarantees evaluation at compile-time
  - Remove checks for versions above or below C++20 for the standard
  - Use algorithms from std::ranges like `std::ranges::find` and `std::ranges::sort`
  - Remove the ubuntu workflows since they are built on C++17

* 2025-01-07 Thomas Madlener ([PR#719](https://github.com/AIDASoft/podio/pull/719))
  - Make sure that the `RNTupleReader` still builds with ROOT > 6.34

* 2024-12-19 Mateusz Jakub Fila ([PR#718](https://github.com/AIDASoft/podio/pull/718))
  - Add `mutable_type` typedef to collection to simplify inferring mutable type. Add `mutable_type` typedef to user collection for compatibility with other collections.

# v01-02

* 2024-12-17 Thomas Madlener ([PR#715](https://github.com/AIDASoft/podio/pull/715))
  - Improve the schema changes that `podio_schema_evolution.py` detects and reports on
    - Addition / removal of `VectorMembers`, `OneToOneRelations` and `OneToManyRelations`
  - Make the script exit with a non-zero exit code in case an unsupported schema change is detected
  - Add a test setup and some tests for testing the schema evolution script and make sure it detects unsupported changes

* 2024-12-09 jmcarcell ([PR#716](https://github.com/AIDASoft/podio/pull/716))
  - Mark the collection destructor with override and remove `#include <array>`

* 2024-12-09 Mateusz Jakub Fila ([PR#714](https://github.com/AIDASoft/podio/pull/714))
  - Added possibility to use in the interfaces the datatypes from different podio-based datamodels

* 2024-12-05 tmadlener ([PR#646](https://github.com/AIDASoft/podio/pull/646))
  - Add a `LinkNavigator` utility class that facilitates the lookup of linked objects

* 2024-12-02 Thomas Madlener ([PR#710](https://github.com/AIDASoft/podio/pull/710))
  - Make sure that the weights of the links end up as a `vector<LinkData>` in the ROOT files instead of a `vector<float>`

* 2024-12-02 tmadlener ([PR#691](https://github.com/AIDASoft/podio/pull/691))
  - Introduce a new `links` category into the YAML grammar to automate the declaration of Links.

* 2024-11-20 Thomas Madlener ([PR#709](https://github.com/AIDASoft/podio/pull/709))
  - Remove an option from `.clang-tidy` that has been removed from newer versions of `clang-tidy` (>= 18)

* 2024-11-20 jmcarcell ([PR#681](https://github.com/AIDASoft/podio/pull/681))
  - Add a tool to merge several podio files into a single one:
    - Metadata for every event can be saved or not
    - The same format as the first input file will be used (TTree or RNTuple)
    - Metadata about the input file names will be saved

* 2024-11-19 jmcarcell ([PR#707](https://github.com/AIDASoft/podio/pull/707))
  - Run clang-tidy at the end of pre-commit since other failures are more likely and clang-tidy takes a long time.

* 2024-11-18 jmcarcell ([PR#706](https://github.com/AIDASoft/podio/pull/706))
  - Remove unnecessary code in GenericParameters.h

* 2024-11-11 jmcarcell ([PR#704](https://github.com/AIDASoft/podio/pull/704))
  - Fix tests when SIO is OFF

* 2024-11-08 Thomas Madlener ([PR#702](https://github.com/AIDASoft/podio/pull/702))
  - Make sure that `makeReader` still recognizes files with RNTuple inside after they have been marked non-experimental in ROOT.

* 2024-11-08 jmcarcell ([PR#700](https://github.com/AIDASoft/podio/pull/700))
  - Add a deprecation warning when building with C++17 before removing support for it in https://github.com/AIDASoft/podio/pull/698

* 2024-11-08 Victor Schwan ([PR#699](https://github.com/AIDASoft/podio/pull/699))
  - add compatibility with pathlib.Path objects in root_io and sio_io

* 2024-11-08 tmadlener ([PR#693](https://github.com/AIDASoft/podio/pull/693))
  - Fix small issue in `GenericParameters` where trying to access a single element of a parameter that was stored as an empty vector resulted in a crash. Instead make this return an empty optional now.

* 2024-10-17 Thomas Madlener ([PR#697](https://github.com/AIDASoft/podio/pull/697))
  - Temporarily set the `JULIA_DEPOT_PATH` to a writable directory to make CI pass.

* 2024-10-17 jmcarcell ([PR#696](https://github.com/AIDASoft/podio/pull/696))
  - Remove some includes and fix a couple of URLs

* 2024-10-15 tmadlener ([PR#695](https://github.com/AIDASoft/podio/pull/695))
  - Fix minor memory leaks of consumed buffers in `LinkCollection`s and `UserDataCollection`s and in `SIOBlockUserData`
  - Enable some more complete I/O tests using SIO and RNTuple

* 2024-10-15 jmcarcell ([PR#694](https://github.com/AIDASoft/podio/pull/694))
  - Remove unnecessary `inline` in `std_interoperability.cpp` since the variables are only used in that file.

* 2024-09-30 Leonhard Reichenbach ([PR#692](https://github.com/AIDASoft/podio/pull/692))
  - GenericParameters.h: added missing clear and empty check for double map

* 2024-09-30 Mateusz Jakub Fila ([PR#683](https://github.com/AIDASoft/podio/pull/683))
  - Using enable if instead of static_assert in interface type construct to avoid errors with direct initialization of contianers of interface types

* 2024-09-27 tmadlener ([PR#690](https://github.com/AIDASoft/podio/pull/690))
  - Switch to software stacks with a newer version of ROOT for building and testing EDM4hep in CI, because we need a newer version of ROOT to run all the backwards compatibility tests in EDM4hep.

* 2024-09-26 tmadlener ([PR#689](https://github.com/AIDASoft/podio/pull/689))
  - Use a `unique_ptr` to manage the OneToOneRelation pointers in the `Obj` classes.

* 2024-09-26 ANUSHKA SAXENA ([PR#685](https://github.com/AIDASoft/podio/pull/685))
  - Add advanced CMake usage for `PODIO_GENERATE_DATAMODEL` to docs

* 2024-09-24 tmadlener ([PR#688](https://github.com/AIDASoft/podio/pull/688))
  - Make sure that we pick up the correct version of `pylint` in the CI for pre-commit.

* 2024-09-24 tmadlener ([PR#673](https://github.com/AIDASoft/podio/pull/673))
  - Rework the I/O  handling for relations. This is mainly removing the differences in handling of interface and regular datatypes in relations.
    - Remove the conditional code generation from the jinja2 templates and add functionality that dispatches to the correct handling depending on a (c++) type check
  - Expose an `interfaced_types` member type for interface types and add a type helper to check whether a type is an interface type

* 2024-09-23 tmadlener ([PR#687](https://github.com/AIDASoft/podio/pull/687))
  - Make sure the `ROOT_INCLUDE_PATH` in the test environment points to the correct place.

* 2024-09-23 Benedikt Hegner ([PR#684](https://github.com/AIDASoft/podio/pull/684))
  - Finally removing deprecated ROOTFrame[Reader,Writer] and SIOFrame[Reader,Writer] classes (announced removal was v01-00)

# v01-01

* 2024-09-20 Leonhard Reichenbach ([PR#682](https://github.com/AIDASoft/podio/pull/682))
  - DataSource: implement missing GetLabel method

* 2024-09-19 tmadlener ([PR#678](https://github.com/AIDASoft/podio/pull/678))
  - Add a `podio.version` module exposing `podio::version::Version` also in python.
  - Remove the `version_as_str` method again, since that is handled by `Version` now.

* 2024-09-18 jmcarcell ([PR#680](https://github.com/AIDASoft/podio/pull/680))
  - Revert part of #676 because there is no implicit conversion to std::string when trying to put a parameter that can be converted to std::string, therefore it will fail with the static_assert when compiling

* 2024-09-18 Mateusz Jakub Fila ([PR#679](https://github.com/AIDASoft/podio/pull/679))
  - Fixed missing libpodioDataSourceDict printout on import podio when built without data source

* 2024-09-18 jmcarcell ([PR#676](https://github.com/AIDASoft/podio/pull/676))
  - Remove EnableIf for GenericParameters and add `static_assert` in its place to make errors easier to read and debug.

* 2024-09-17 tmadlener ([PR#677](https://github.com/AIDASoft/podio/pull/677))
  - Make sure that podio is built with all capabilities for the CI tests that use EDM4hep

* 2024-09-16 jmcarcell ([PR#675](https://github.com/AIDASoft/podio/pull/675))
  - Fix a few wrong @params in the documentation of some headers

* 2024-09-12 Mateusz Jakub Fila ([PR#674](https://github.com/AIDASoft/podio/pull/674))
  - Added `podio.data_source.CreateDataFrame` for creating RDataFrames with DataSource in python.

* 2024-09-10 Mateusz Jakub Fila ([PR#671](https://github.com/AIDASoft/podio/pull/671))
  - Fixed typos

* 2024-09-10 tmadlener ([PR#670](https://github.com/AIDASoft/podio/pull/670))
  - Strip strings to avoid having empty includes from ExtraCode that would otherwise generate a warning during code generation.
  - Remove a debug printout from code generation

* 2024-09-10 Mateusz Jakub Fila ([PR#669](https://github.com/AIDASoft/podio/pull/669))
  - Improved documentation for the pythonizations. Test mutability of objects returned by collection subscript in python (mutable without the subscript pythonization/immutable with the pythonization)

* 2024-09-10 jmcarcell ([PR#668](https://github.com/AIDASoft/podio/pull/668))
  - Change C-style casts to static_cast and remove unnecessary semicolons

* 2024-09-10 tmadlener ([PR#651](https://github.com/AIDASoft/podio/pull/651))
  - Make it possible to pass the datamodel version (which does not have to be the schema version!) to the class generator to inject it into the podio internals. This will automatically be stored in all output files, such that this information can also be obtained from the files.

* 2024-09-09 Mateusz Jakub Fila ([PR#667](https://github.com/AIDASoft/podio/pull/667))
  - Fix pythonizations trying to import local modules as pythonizations submodules

* 2024-09-06 jmcarcell ([PR#666](https://github.com/AIDASoft/podio/pull/666))
  - Add two checks not to crash when a file doesn't exist or can't be opened and when an empty list is passed to makeReader.

* 2024-09-06 tmadlener ([PR#664](https://github.com/AIDASoft/podio/pull/664))
  - Remove pre-processor checks that became obsolete with [#662](https://github.com/AIDASoft/podio/pull/662)

* 2024-09-06 Mateusz Jakub Fila ([PR#663](https://github.com/AIDASoft/podio/pull/663))
  - Added pythonization for "freezing" class disallowing setting non-existent attributes

* 2024-09-06 tmadlener ([PR#662](https://github.com/AIDASoft/podio/pull/662))
  - Remove the cmake checks to figure out against which filesystem library to link, since all major compilers have had it properly integrated into their STLs for quite some time now. Also remove the fallback to Boost.

* 2024-09-05 tmadlener ([PR#661](https://github.com/AIDASoft/podio/pull/661))
  - Make the README point to the documentation instead of the markdown files

* 2024-09-05 ANUSHKA SAXENA ([PR#660](https://github.com/AIDASoft/podio/pull/660))
  - Added missing dependency `graphviz` for `podio-vis` functionality in the documentation build process.

* 2024-09-05 tmadlener ([PR#659](https://github.com/AIDASoft/podio/pull/659))
  - Add convenience bindings and formatting utility functionality to retrieve the podio build version from a file via python.

* 2024-08-28 tmadlener ([PR#658](https://github.com/AIDASoft/podio/pull/658))
  - Build EDM4hep with c++20 now that c++17 support has been removed from EDM4hep.

* 2024-08-28 Juraj Smiesko ([PR#593](https://github.com/AIDASoft/podio/pull/593))
  - Add a `podio::DataSource` as an `RDataSource` for working with podio (generated EDM) files and collections. This `RDataSource` exposes the podio collections directly as columns for `RDataFrames`. **We consider this fairly stable, but still in early stages, there might still be some (breaking) changes to this**

* 2024-08-23 tmadlener ([PR#656](https://github.com/AIDASoft/podio/pull/656))
  - Use a virtualenv for the documentation generation to make it more robust

* 2024-08-22 jmcarcell ([PR#652](https://github.com/AIDASoft/podio/pull/652))
  - Remove unnecessary header includes

* 2024-08-21 tmadlener ([PR#654](https://github.com/AIDASoft/podio/pull/654))
  - Use a virtualenv to intall pre-commit in CI workflow to avoid issues with underlying environment

* 2024-08-21 Mateusz Jakub Fila ([PR#653](https://github.com/AIDASoft/podio/pull/653))
  - fix typos in documentation

* 2024-07-31 tmadlener ([PR#650](https://github.com/AIDASoft/podio/pull/650))
  - Make sure that the `env.sh` script sets up the correct `PYTHONPATH`

* 2024-07-31 tmadlener ([PR#649](https://github.com/AIDASoft/podio/pull/649))
  - Introduce a thin wrapper script around `clang-tidy` for usage in `pre-commit` to make sure that the necessary include directory for SIO is available. This allows us to run clang-tidy over all files instead of having to exclude a few where it failed previously.

* 2024-07-25 tmadlener ([PR#648](https://github.com/AIDASoft/podio/pull/648))
  - Add docstrings to the `Reader` and `Writer` interface classes
  - Make sure that `makeReader` throws an exception if multiple SIO files are passed (see [#647](https://github.com/AIDASoft/podio/issues/647))

* 2024-07-25 tmadlener ([PR#640](https://github.com/AIDASoft/podio/pull/640))
  - Make JSON dumping of subset collections only store the `ObjectID`s to the elements they are pointing to instead of duplicating the information.
  - Add JSON conversion support to `podio::ObjectID`
    - Use this to simplify JSON conversion code for relations in objects

* 2024-07-23 jmcarcell ([PR#645](https://github.com/AIDASoft/podio/pull/645))
  - Fix duplicated includes in generated datamodels when `ExtraCode` includes are done via `<>`.

* 2024-07-22 tmadlener ([PR#644](https://github.com/AIDASoft/podio/pull/644))
  - Switch to EL9 and c++20 for running pre-commit

* 2024-07-16 tmadlener ([PR#643](https://github.com/AIDASoft/podio/pull/643))
  - Add test files for read compatibility with v01-00
  - Fix v00-99 root test file that was actually one with v00-17-04

* 2024-07-15 jmcarcell ([PR#641](https://github.com/AIDASoft/podio/pull/641))
  - Remove or change a few headers for objects and collections

* 2024-07-12 tmadlener ([PR#639](https://github.com/AIDASoft/podio/pull/639))
  - Improve the readability of dumped `ObjectID`s from `XXXXXXXi` to `XXXXXXXX|i` (i.e. adding a separator between the hex collection ID and the index)

* 2024-07-10 jmcarcell ([PR#638](https://github.com/AIDASoft/podio/pull/638))
  - Make ROOT load libraries without the .so extension

* 2024-07-08 tmadlener ([PR#635](https://github.com/AIDASoft/podio/pull/635))
  - Fix minor issue in the CMake config to make error message for users more useful.

# v01-00-01

* 2024-06-24 tmadlener ([PR#634](https://github.com/AIDASoft/podio/pull/634))
  - Bump CI runners to run on `ubuntu-latest`

* 2024-06-24 jmcarcell ([PR#633](https://github.com/AIDASoft/podio/pull/633))
  - Minor cleanups for generated EDMs
    - Pass the argument by const reference in the relation setter
    - Remove a few includes that are not needed
    - Don't check for `nullptr` before deleting a (possibly `nullptr`) pointer
    - Remove `if - else` check and use implicit conversion of pointer to bool

* 2024-06-23 jmcarcell ([PR#632](https://github.com/AIDASoft/podio/pull/632))
  - Fix one to one relations for cloned objects by copying the one to one relations too.
  - Add a test that checks values for cloned objects from a non mutable object.

# v01-00

* 2024-06-20 tmadlener ([PR#629](https://github.com/AIDASoft/podio/pull/629))
  - Document how files written by podio look like

* 2024-06-20 tmadlener ([PR#625](https://github.com/AIDASoft/podio/pull/625))
  - Make the TTree based backend also write the GenericParameters as pairs of vectors (keys and values) to make the format the same for the RNTuple and the TTree based backends.

* 2024-06-14 Benedikt Hegner ([PR#622](https://github.com/AIDASoft/podio/pull/622))
  - Detect more impossible schema evolutions
  - Improving printout of schema evolution checker

* 2024-06-12 tmadlener ([PR#627](https://github.com/AIDASoft/podio/pull/627))
  - Fix a bug in the schema evolution generation script that crashes in case a datatype member that is a component is changed.
  - Adapt new datalayout to include such a change
  - Add new tests to run schema evolution script standalone

* 2024-06-12 jmcarcell ([PR#606](https://github.com/AIDASoft/podio/pull/606))
  - Add a header file including all the collections in the datamodel. This header will be generated and installed into `<datamodel>/<datamodel>.h`.

* 2024-06-11 tmadlener ([PR#623](https://github.com/AIDASoft/podio/pull/623))
  - Move the implementation of `ROOTFrameData` member functions into a separate cc file.

* 2024-06-11 tmadlener ([PR#618](https://github.com/AIDASoft/podio/pull/618))
  - Fix minor issues with the `Reader` and `Writer` interface classes
    - Make sure that only the public API is public
    - Fix some const correctness issues by marking `const` methods as such
    - Remove unnecessary and (partially) unused APIs from the `Writer` interface
  - Remove the dictionary generation for `podioIO` as we don't need the dictionaries

* 2024-06-11 tmadlener ([PR#614](https://github.com/AIDASoft/podio/pull/614))
  - Refactor the `RNTupleWriter` internals a bit and reduce some code duplication between the `RNTupleWriter` and the `ROOTWriter`
  - Add `getKeys` and `getValues` methods to `GenericParameters`
  - Remove the `CollectionBranches.h` header and move all root related helpers into `utilities/RootHelpers.h`

* 2024-06-10 tmadlener ([PR#621](https://github.com/AIDASoft/podio/pull/621))
  - Make `GenericParameters::print` const-correct to allow proper usage

* 2024-06-10 tmadlener ([PR#619](https://github.com/AIDASoft/podio/pull/619))
  - Add I/O tests for a type with an interface relation, in order to make sure things work as expected.

* 2024-06-10 Mateusz Jakub Fila ([PR#598](https://github.com/AIDASoft/podio/pull/598))
  - Added documentation on collection compatibility with container named requirement
  - Added container-like methods `cbegin`, `cend`, `max_size` and aliases `size_type`, `difference_type` to collection

* 2024-06-06 tmadlener ([PR#616](https://github.com/AIDASoft/podio/pull/616))
  - Switch to Alma9 for the Key4hep based workflow in CI, since CentOS7 is no longer built regularly

* 2024-06-06 tmadlener ([PR#580](https://github.com/AIDASoft/podio/pull/580))
  - Make the `GenericParameters` return `std::optional` on non-existant keys instead of empty defaults. This allows to differentiate between empty (but set) and unset parameters, as the default value could actually be a valid parameter value for some types (e.g. integers or floats).
    - Rename the `setValue` and `getValue` functions to just `set` and `get`. This is also a breaking change, but as far as we are aware the `GenericParameters` are only used internally in podio.
  - Make the `Frame::getParameter` also return this `std::optional`. **This is a breaking change that might require adaptation of existing code**

* 2024-06-05 tmadlener ([PR#608](https://github.com/AIDASoft/podio/pull/608))
  - Add python bindings for the `Frame::getName` functionality introduced in #586

* 2024-06-04 jmcarcell ([PR#612](https://github.com/AIDASoft/podio/pull/612))
  - Fix Python->Python3 when `PODIO_RELAX_PYVER` is ON

* 2024-06-04 jmcarcell ([PR#609](https://github.com/AIDASoft/podio/pull/609))
  - Add a parameter `cloneRelations` for the `clone` method of the user facing handle classes to be able to clone without the relation information.

* 2024-06-04 Benedikt Hegner ([PR#607](https://github.com/AIDASoft/podio/pull/607))
  - Add documentation for CMake macros

* 2024-06-04 tmadlener ([PR#602](https://github.com/AIDASoft/podio/pull/602))
  - Remove the dynamic determination of the supported parameter types for the python bindings to work around https://github.com/root-project/root/issues/14232 and https://github.com/key4hep/EDM4hep/issues/246.

* 2024-06-04 Dmitry Kalinkin ([PR#599](https://github.com/AIDASoft/podio/pull/599))
  - The Python library is installed to `lib/python3.XX/site-packages` instead of `python` by default. This is a more standard prefix for python packages
    - Use the `podio_PYTHON_INSTALLDIR` cmake variable to change this. 
  - Move python setup and compatibility checks with ROOT python version into a dedicated cmake macro (`podio_python_setup`)

* 2024-06-04 tmadlener ([PR#595](https://github.com/AIDASoft/podio/pull/595))
  - Deprecate the `getValue` method in favor of the `as` method for generated interface types.

* 2024-05-28 jmcarcell ([PR#610](https://github.com/AIDASoft/podio/pull/610))
  - Drop unused comparison templates

* 2024-05-24 Mateusz Jakub Fila ([PR#605](https://github.com/AIDASoft/podio/pull/605))
  - Added .clang-format config files to avoid formatting jinja2 templates and tests' extra_code

* 2024-05-22 Mateusz Jakub Fila ([PR#601](https://github.com/AIDASoft/podio/pull/601))
  - Added  ExtraCode declarationFile and implementationFile directives

* 2024-05-22 jmcarcell ([PR#522](https://github.com/AIDASoft/podio/pull/522))
  - Add a reader and writer interface and tests using those
  - Add a new library `podioIO` with the interface readers and writers

* 2024-05-14 Mateusz Jakub Fila ([PR#570](https://github.com/AIDASoft/podio/pull/570))
  - Added mechanism to load cppyy pythonizations
  - Added pythonization for bound-check subscript operation in collections

* 2024-05-13 jmcarcell ([PR#600](https://github.com/AIDASoft/podio/pull/600))
  - Add include to fix building with GCC 14

* 2024-05-07 tmadlener ([PR#597](https://github.com/AIDASoft/podio/pull/597))
  - Update CI workflows to run on `LCG_105` using `gcc13` and `clang16` as well as bumping the OS to EL9. Build RNTuple by default
  - Update the Ubuntu based workflows to run on ubuntu 22.04

* 2024-05-02 Andre Sailer ([PR#591](https://github.com/AIDASoft/podio/pull/591))
  - CI: alma9-gcc13 dev stack

* 2024-05-02 tmadlener ([PR#568](https://github.com/AIDASoft/podio/pull/568))
  - Switch to gcc13 and clang16 for sanitizer workflows. Also switch to EL9 as OS to run on.

* 2024-04-22 jmcarcell ([PR#583](https://github.com/AIDASoft/podio/pull/583))
  - Fix OneToManyRelations and VectorMembers in cloned objects. Currently, pushing back these fields to a cloned object does not give the expected result, because the objects that we get after reading are not the same as we had before writing.
  - Add some code testing this behavior: pushing back to cloned objects that have been read from a file and also after cloning the cloned object.

* 2024-04-19 jmcarcell ([PR#589](https://github.com/AIDASoft/podio/pull/589))
  - Fix leak in the buffer vectorMembers when reading SIO frames

* 2024-04-19 jmcarcell ([PR#588](https://github.com/AIDASoft/podio/pull/588))
  - Set the right path for `PODIO_SIOBLOCK_PATH`, fixing unit tests that read and write SIO files (there aren't any at the moment)
  - Add an error message when `PODIO_SIOBLOCK_PATH` is set but none of the SioBlock libraries are found.

* 2024-04-19 tmadlener ([PR#586](https://github.com/AIDASoft/podio/pull/586))
  - Add a `Frame::getName` method to retrieve the name of a collection via it's collectionID (if the collection is known to the Frame).

* 2024-04-19 jmcarcell ([PR#585](https://github.com/AIDASoft/podio/pull/585))
  - Pass arguments by const reference to avoid making copies in a few places where passing by value was used

* 2024-04-16 jmcarcell ([PR#587](https://github.com/AIDASoft/podio/pull/587))
  - Change "Collcetion" to "Collection". It's an aesthetic change

* 2024-04-15 jmcarcell ([PR#584](https://github.com/AIDASoft/podio/pull/584))
  - Change a few permissions to 644 for a few text files that have executable permissions
  - Add a couple of files to the .gitignore

* 2024-04-11 jmcarcell ([PR#578](https://github.com/AIDASoft/podio/pull/578))
  - Check if the data passed to the frame model is not a nullptr before doing anything with it.

* 2024-04-10 jmcarcell ([PR#577](https://github.com/AIDASoft/podio/pull/577))
  - Improve error message for push_back when using an immutable type

* 2024-04-10 tmadlener ([PR#573](https://github.com/AIDASoft/podio/pull/573))
  - Remove outdated information from top level README

* 2024-04-09 tmadlener ([PR#574](https://github.com/AIDASoft/podio/pull/574))
  - Make all docstrings of c++ classes use the `///` style (instead of the previous mix of `///` and /**/`.
  - Extend and improve existing docstrings (e.g. add `@params` and `@returns` tags, add information or reword).

* 2024-04-03 tmadlener ([PR#575](https://github.com/AIDASoft/podio/pull/575))
  - Make sure that the `as_type` argument is respected in `Frame.put_parameter` also for lists of doubles / floats.
  - Fix test to actually check for this.

* 2024-04-02 tmadlener ([PR#547](https://github.com/AIDASoft/podio/pull/547))
  - Fix the RNTupleWriter after ROOT has slightly changed the API for the RNTupleModel. Fixes #545

* 2024-03-20 tmadlener ([PR#572](https://github.com/AIDASoft/podio/pull/572))
  - Add the necessary infrastructure to run documentation generation on CERNs gitlab infrastructure via a mirror of this repository
    - Deployed to https://key4hep.web.cern.ch/podio
  - Update Doxyfile to have no warnings with newer doxygen versions

* 2024-03-19 Mateusz Jakub Fila ([PR#571](https://github.com/AIDASoft/podio/pull/571))
  - Fixed typos

* 2024-03-11 Wouter Deconinck ([PR#561](https://github.com/AIDASoft/podio/pull/561))
  - support podio-vis for extension data models

* 2024-02-28 tmadlener ([PR#566](https://github.com/AIDASoft/podio/pull/566))
  - Use `tabulate` to make the overview table of the categories in a file more consistent with the rest of the output.
    - Also fixes a small issue with long category names.

* 2024-02-24 Mateusz Jakub Fila ([PR#564](https://github.com/AIDASoft/podio/pull/564))
  - Fix mutable to immutable object conversion with python bindings

* 2024-02-22 Wouter Deconinck ([PR#562](https://github.com/AIDASoft/podio/pull/562))
  - upgrade github-action-cvmfs@v3 to github-action-cvmfs@v4
  - upgrade actions/checkout to v4

* 2024-02-21 tmadlener ([PR#560](https://github.com/AIDASoft/podio/pull/560))
  - Generate `operator!=` for all user facing handle classes to make constructs like `!(lhs == rhs)` obsolete.

* 2024-02-19 jmcarcell ([PR#559](https://github.com/AIDASoft/podio/pull/559))
  - Improve error messages when opening a bad file; instead of saying that the file couldn't be found say also that the tree that podio looks for couldn't be read

* 2024-02-15 jmcarcell ([PR#558](https://github.com/AIDASoft/podio/pull/558))
  - Improve the deprecated message

* 2024-02-15 tmadlener ([PR#553](https://github.com/AIDASoft/podio/pull/553))
  - Generate mutable reference getters for all members on the `Mutable` types
    - Previously this was only generated for non-builtin (i.e. non-trivial) members. 
    - Additionally, deprecate the API that does not respect the `getSyntax` option.
    - Fixes https://github.com/AIDASoft/podio/issues/551

* 2024-02-13 tmadlener ([PR#557](https://github.com/AIDASoft/podio/pull/557))
  - Remove deprecated `collections` property from Frame python bindings
  - Remove deprecated `getParametersForWrite` method from Frame interface

* 2024-02-13 jmcarcell ([PR#556](https://github.com/AIDASoft/podio/pull/556))
  - Add a schema_version for the extension datamodel, this fixes a warning at configure time

* 2024-02-06 jmcarcell ([PR#555](https://github.com/AIDASoft/podio/pull/555))
  - Change FCC stack to Key4hep in the README

# v00-99

* 2024-02-06 tmadlener ([PR#554](https://github.com/AIDASoft/podio/pull/554))
  - Bring back `vector<Data>` and `vector<Component>` into the dictionaries to allow for better interoperability with RNTuple. 
    - See [#464](https://github.com/AIDASoft/podio/issues/464) for some related discussion.

* 2024-02-06 tmadlener ([PR#552](https://github.com/AIDASoft/podio/pull/552))
  - Introduce an `operator<` to the interface types to make it possible to use them in STL containers that require that (e.g. `std::map` and `std::set`).

* 2024-02-06 tmadlener ([PR#549](https://github.com/AIDASoft/podio/pull/549))
  - Rename `{ROOT,SIO}Frame{Reader,Writer}` to `{ROOT,SIO}{Reader,Writer}` since these names are now no longer taken by the deprecated `EventStore` based ones and Frame based I/O is the default now.
    - Keep the old names around as deprecated aliases to not break everything immediately. But **plan to remove the aliases for v1.0**.

* 2024-02-02 Mateusz Jakub Fila ([PR#550](https://github.com/AIDASoft/podio/pull/550))
  - Fixed typos in tests and interface template

* 2024-01-30 tmadlener ([PR#548](https://github.com/AIDASoft/podio/pull/548))
  - Remove an unnecessary usage of the `TClass` machinery inside the `RNTupleReader` as all the necessary information is also available from metadata that we carry around in any case.

* 2024-01-30 Graeme A Stewart ([PR#544](https://github.com/AIDASoft/podio/pull/544))
  Add CMake targets for running  black and flake8 on Python source files

* 2024-01-30 tmadlener ([PR#528](https://github.com/AIDASoft/podio/pull/528))
  - Use `black` to format all python source files
  - Add a pre-commit hook for running black in CI

* 2024-01-25 jmcarcell ([PR#541](https://github.com/AIDASoft/podio/pull/541))
  - Add tool to transform between TTrees and RNTuples

* 2024-01-24 jmcarcell ([PR#543](https://github.com/AIDASoft/podio/pull/543))
  - Change ROOTNTuple{Reader,Writer} to RNTuple{Reader,Writer}

* 2024-01-22 tmadlener ([PR#516](https://github.com/AIDASoft/podio/pull/516))
  - Add a new category of types that can be generated by podio: `interfaces`. These can be used to provide a type that can be initialized from several other datatypes and offers some common functionality. These interface types can be used in `OneToOneRelation`s and in `OneToManyRelation`s.
    - `interfaces` need to provide a list of types which they interface. Other types cannot be used with them.

* 2024-01-18 Mateusz Jakub Fila ([PR#542](https://github.com/AIDASoft/podio/pull/542))
  - Fixed typos in documentation and comments, updated .gitignore

* 2024-01-18 jmcarcell ([PR#540](https://github.com/AIDASoft/podio/pull/540))
  - Add a check for the C++ standard using `ROOT_CXX_STANDARD` that was introduced yesterday in ROOT (https://github.com/root-project/root/commit/d487a42b311c5d0c7544031e3071a388c488c429)

* 2024-01-16 jmcarcell ([PR#539](https://github.com/AIDASoft/podio/pull/539))
  - Fix crash when a writer is in the global namespace by adding a class that will manage all the writers and finish them before exiting

* 2024-01-16 jmcarcell ([PR#538](https://github.com/AIDASoft/podio/pull/538))
  - Use getAvailableCollections() instead of the deprecated .collections

* 2024-01-12 jmcarcell ([PR#536](https://github.com/AIDASoft/podio/pull/536))
  - Add a `getAvailableCollections` method in python that does the same thing as in C++

* 2023-12-19 tmadlener ([PR#535](https://github.com/AIDASoft/podio/pull/535))
  - Implement the suggestions from coverity to move in places where it is easily possible.
  - Fix a small resource leak in SIO.
  - Fix a small copy-paste error in test output (only triggered in case test fails)
  - Restore ostream state after altering it for formatting.

* 2023-12-19 tmadlener ([PR#534](https://github.com/AIDASoft/podio/pull/534))
  - Update the coverity workflow to use EL9

* 2023-12-18 Andre Sailer ([PR#533](https://github.com/AIDASoft/podio/pull/533))
  - rootUtils: include sstream, fixes build on macOS 12 / 13

* 2023-12-18 tmadlener ([PR#532](https://github.com/AIDASoft/podio/pull/532))
  - Remove some benchmark utilities that became unused with the removal of the `TimedReader` and `TimedWriter` classes in #485

* 2023-12-15 tmadlener ([PR#531](https://github.com/AIDASoft/podio/pull/531))
  - Remove the last few deprecated accessors from `GenericParameters`.

# v00-17-04

* 2023-12-14 tmadlener ([PR#527](https://github.com/AIDASoft/podio/pull/527))
  - Split the `ClassGenerator` into a base class (mixin) and two specific c++ and julia code generators that only deal with their language specific needs.
    - Instantiate and configure the correct reader in the `podio_class_generator.py` main script depending on the desired language.
  - Slightly cleanup the `MemberVariable` to declutter its `__init__` method a bit.

* 2023-12-13 tmadlener ([PR#530](https://github.com/AIDASoft/podio/pull/530))
  - Remove the reading of the deprecated old-style format of component definitions in the YAML files.

* 2023-12-13 tmadlener ([PR#485](https://github.com/AIDASoft/podio/pull/485))
  - **Remove the deprecated EventStore functionality as announced in #429**

* 2023-12-12 tmadlener ([PR#529](https://github.com/AIDASoft/podio/pull/529))
  - Switch the relation range tests to use Frame based I/O.

* 2023-12-06 tmadlener ([PR#526](https://github.com/AIDASoft/podio/pull/526))
  - Switch the edm4hep workflows to an LCG stack with a recent enough version of CMake. Necessary after key4hep/EDM4hep#235

* 2023-12-05 jmcarcell ([PR#523](https://github.com/AIDASoft/podio/pull/523))
  - Remove comment with file name and line number. It's very unlikely it remains up to date when either the name or the content of the files changes

* 2023-12-04 jmcarcell ([PR#521](https://github.com/AIDASoft/podio/pull/521))
  - Do not import ROOT when using `podio-dump --help`, otherwise it can take a while depending on the system only to print the help.

* 2023-12-04 tmadlener ([PR#514](https://github.com/AIDASoft/podio/pull/514))
  - Introduce the `MaybeSharedPtr` to manage the `Obj*` in the user facing handle classes.
    - This splits the control block and the managed object into two distinct entities with potentially different lifetimes, which allows to fix #174 and #492.
    - **This increases the size of the user facing handle classes by a factor two**, since they are now effectively two pointers instead of one, even if the control block will not be initialized in case a handle is obtained from a collection.
  - Remove the `ObjBase` base class and make the `ObjectID` a member of the `Obj` classes.
  - Make the user facing handle class constructors from an `Obj*` private as users will not have access to raw `Obj*` in any case.
    - Introduce a static `makeEmpty` method for the generated classes in order to create an empty handle, which is also used internally to handle unpersisted relations.
  - Enable more existing test cases in sanitizer workflows now that it has become possible to do so.

* 2023-12-04 Ananya Gupta ([PR#473](https://github.com/AIDASoft/podio/pull/473))
  - Added Julia code generation support in the existing Python interface.
  - Implemented a new design structure for generated Julia code.
  - Added default parameters in constructor definitions with support for Abstract types (for builtins).
  - Created _sort_components_and_datatypes function to perform topological sort on components and datatypes.
  - Created _has_static_arrays_import to check for the need of using Static Arrays in the generated julia code.
  - Added --lang (-l) programming language argument to specify the programming language for code generation, current choices: cpp and julia, default: cpp.
  - Added --upstream-edm code generation support for julia.
  - Added tests in the unit test suite, covering the Julia code generation of the example data models.
  - Added documentation for julia code generation.
  - Added ENABLE_JULIA toggle option. By default it is OFF.

* 2023-12-01 jmcarcell ([PR#520](https://github.com/AIDASoft/podio/pull/520))
  - Add an error message when there is an std::bad_function_call, which currently shows
    a stacktrace and is quite uninformative.

* 2023-12-01 tmadlener ([PR#519](https://github.com/AIDASoft/podio/pull/519))
  - Make generated member getter functions return by value for builtin types. Keep return by const reference for all other types. Fixes #518

* 2023-12-01 tmadlener ([PR#488](https://github.com/AIDASoft/podio/pull/488))
  - Add python bindings for the RNTuple reader and writer 
  - Make `podio-dump` understand RNTuple based files
  - Fix missing storage of datamodel definitions for RNTuple based files

# v00-17-03

* 2023-11-14 tmadlener ([PR#513](https://github.com/AIDASoft/podio/pull/513))
  - Introduce checks in `ROOTFrameWriter::writeFrame` and `ROOTNTupleWriter::writeFrame` that ensure consistent contents for all Frames of a given category. If inconsistent contents are found an exception is thrown. Before these changes this might lead to a crash or to unreadable files. Fixes #382 
  - Refactor `ROOTNTupleWriter` internals to have only one map that keeps track of categories instead of two maps and a set that need to be kept consistent.

# v00-17-02

* 2023-11-08 jmcarcell ([PR#511](https://github.com/AIDASoft/podio/pull/511))
  - Decouple generation tools and files from the rest of the podio python files by creating a new folder called `podio_gen`. **This is a transparent change for users that only use the generator script.**
    - This makes the configuration / generation times negligible again, because we don't load libraries unnecessarily any longer for the generation.
  - Simplify the python bindings (`podio`) `__init__.py` and remove the `test_utils` from it.
  - Move the tests for writing frames in python to the `tests` folder, where they belong and also test the SIO python writer.

* 2023-11-06 jmcarcell ([PR#510](https://github.com/AIDASoft/podio/pull/510))
  - Fix legacy tests; an extra argument was being passed and default in the .cpp file `example_frame.root` was being used which (almost) always exists (because there is a another test creating it) so it was hard to notice.

* 2023-11-06 jmcarcell ([PR#509](https://github.com/AIDASoft/podio/pull/509))
  - Add an option for using clang format and set it to off by default. It is significantly slower to run cmake with this option on.

* 2023-11-02 jmcarcell ([PR#508](https://github.com/AIDASoft/podio/pull/508))
  - Use the cmake ExternalData module to manage test data. This lets cmake take care of downloading the tests by hash so they can be version controlled. In addition, it's possible to set up a local store using `-DExternalData_OBJECT_STORES=/path/to/store` and it will download the test files there if they are not there but otherwise use them from there, so building from scratch won't download the test files again.

* 2023-10-16 jmcarcell ([PR#507](https://github.com/AIDASoft/podio/pull/507))
  - Copy .clang-format to the dumpmodel test directory and fix some tests when the build directory is not a subdirectory of the main directory. In some tests `clang-format` will try to find a `.clang-format` looking in the directories above and if it doesn't exist it will format files differently and some tests will fail.

# v00-17-01

* 2023-10-12 tmadlener ([PR#505](https://github.com/AIDASoft/podio/pull/505))
  - Bump the pylint version for CI to 2.17.7

* 2023-10-11 tmadlener ([PR#502](https://github.com/AIDASoft/podio/pull/502))
  - Add a `deleteBuffers` function that is populated at generation time to the `CollectionReadBuffers` to make it possible to dispose of unconsumed buffers. This fixes the main leaks described in #500 
  - Make the `ROOTFrameData` clean up all unconsumed buffers at desctruction.
  - Make sure to delete buffers that are no longer necessary in the CollectionData constructor. This fixes some more leaks described in #500

* 2023-10-08 Wouter Deconinck ([PR#503](https://github.com/AIDASoft/podio/pull/503))
  - Install podio-vis

* 2023-10-04 jmcarcell ([PR#497](https://github.com/AIDASoft/podio/pull/497))
  - Move podio_PYTHON_DIR to the top level CMakeLists so that it is set even when BUILD_TESTING is off

* 2023-10-02 jmcarcell ([PR#496](https://github.com/AIDASoft/podio/pull/496))
  - Add an operator != to fix negative comparisons since after https://github.com/AIDASoft/podio/pull/493 now `id()` returns a `podioObjectID`

* 2023-09-29 tmadlener ([PR#493](https://github.com/AIDASoft/podio/pull/493))
  - Make `[Mutable]Object::id()` return a `podio::ObjectID` instead of `unsigned`, since the latter has become useless with #412. Fixes #438 
  - Add an `operator<<(std::ostream&)` for `podio::ObjectID`

# v00-17

* 2023-09-22 Juraj Smiesko ([PR#491](https://github.com/AIDASoft/podio/pull/491))
  - podio-dump: print warning if requested entry not present in the file

* 2023-09-22 tmadlener ([PR#490](https://github.com/AIDASoft/podio/pull/490))
  - Fix bugs in python imports when podio is built without SIO support. Fixes #489

* 2023-09-22 tmadlener ([PR#486](https://github.com/AIDASoft/podio/pull/486))
  - Make sure to initialize `ObjectID`s to *untracked* to properly track whether they have been added to a Frame or not
  - Change `CollectionIDTable` interfaces of `name` and `collectionID` to return `optional` to signal whether a collection (ID) is known to the table. **This is a breaking change if you use the `CollectionIDTable`!**
    - Avoids having to do the lookup twice to check existence and a subsequent retrieval
  - Fix bug of overly shared CollectionIDTable in `ROOTNTupleReader` that was uncovered by the CollectionIDTable switch to optional returns.
  - Switch tests from EventStore to Frame based I/O.
  - Fix bug in Frame based I/O that lead to crashes when trying to resolve relations to unpersisted objects.

* 2023-09-18 tmadlener ([PR#484](https://github.com/AIDASoft/podio/pull/484))
  - Make the podio python bindings import structure "feel more pythonic"

* 2023-09-15 Benedikt Hegner ([PR#483](https://github.com/AIDASoft/podio/pull/483))
  - Clarify error message in case of not implemented schema changes

* 2023-09-15 Benedikt Hegner ([PR#482](https://github.com/AIDASoft/podio/pull/482))
  - rename CMake macro createBuffers into create_buffers

* 2023-09-13 jmcarcell ([PR#481](https://github.com/AIDASoft/podio/pull/481))
  - Rename the cmake executable or target `unittest` to `unittest_podio`, to avoid possible collisions since the `unittest` name is relatively common

* 2023-09-13 Benedikt Hegner ([PR#480](https://github.com/AIDASoft/podio/pull/480))
  - Move the code generation of buffers into the 'create_buffers' macro

* 2023-09-13 Thomas Madlener ([PR#472](https://github.com/AIDASoft/podio/pull/472))
  - Allow comparison of data schemata across versions
  - Provide syntax to clarify user intentions in schema evolution
  - Provide schema evolution implementation based on ROOT backend
  - Include infrastructure for future support for schema evolution in other backends
  - Documentation for schema evolution functionality

* 2023-09-11 tmadlener ([PR#477](https://github.com/AIDASoft/podio/pull/477))
  - Use `nlohmann/json_fwd.hpp` in headers to reduce unnecessary template instantiations. Fixes #475

* 2023-09-08 tmadlener ([PR#478](https://github.com/AIDASoft/podio/pull/478))
  - Add `empty` method to `CollectionBase`
  - Add `operator==` to the collection iterators

* 2023-09-08 Dmitry Kalinkin ([PR#465](https://github.com/AIDASoft/podio/pull/465))
  - Introduce member typedefs to the user facing classes.
    - Object's collection type can now be referenced as `Object::collection_type`. Conversely, the object type is reachable as `ObjectCollection::value_type`. The mutable objects can be reached via `Object::mutable_type`.

* 2023-08-30 tmadlener ([PR#471](https://github.com/AIDASoft/podio/pull/471))
  - Initialize the branch names to be index based for reading (i.e. legacy behavior) for all releases of the v00-16 series.

* 2023-08-22 Andre Sailer ([PR#469](https://github.com/AIDASoft/podio/pull/469))
  - Tests: update required catch2 version to 3.4 for builds with c++20
  - CI: use clang16 on el9 (alma9), instead of clang12 on cs7, using c++20
  - CI: disable key4hep-release-based tests (`tabulate` available)

* 2023-08-22 Benedikt Hegner ([PR#445](https://github.com/AIDASoft/podio/pull/445))
  - Allow to specify units as part of the datamodel definition

* 2023-07-26 tmadlener ([PR#463](https://github.com/AIDASoft/podio/pull/463))
  - Make sure to only access vector member buffers of collections of datatypes with `VectorMembers` if they actually exist. This is necessary to make subset collections of such datatypes work in I/O. Fixes [#462](https://github.com/AIDASoft/podio/issues/462)
  - Add a test that reproduces the original issue and is fixed by this.

* 2023-07-25 tmadlener ([PR#461](https://github.com/AIDASoft/podio/pull/461))
  - Make sure the `ROOTFrameReader` on the master branch can read `v00-16-06` files
  - Add `v00-16-06` to the legacy versions that are tested

* 2023-07-25 tmadlener ([PR#447](https://github.com/AIDASoft/podio/pull/447))
  - Add a python wrapper around the different available Frame writers.
  - Add a `put` method to the `Frame` wrapper that allows to add collections to the Frame without having to explicitly use `cppyy.gbl.std.move`. Fixes #432 
  - Add test cases that write via python bindings and read via c++.

* 2023-07-20 tmadlener ([PR#457](https://github.com/AIDASoft/podio/pull/457))
  - Simplify the test setup for SIO in CMake and make it explicit on the `ENABLE_SIO` option rather than on the presence of targets.

* 2023-07-18 jmcarcell ([PR#456](https://github.com/AIDASoft/podio/pull/456))
  - Cache podio_PYTHON_DIR

* 2023-07-18 jmcarcell ([PR#455](https://github.com/AIDASoft/podio/pull/455))
  - Rename `CMAKE_BINARY_DIR` to `PROJECT_BINARY_DIR`

* 2023-07-18 tmadlener ([PR#439](https://github.com/AIDASoft/podio/pull/439))
  - Introduce the `FrameCategories.h` header that puts a few of the conventions and otherwise hardcoded strings into variables / functions.

* 2023-07-14 jmcarcell ([PR#454](https://github.com/AIDASoft/podio/pull/454))
  - Rename `CMAKE_{SOURCE,BIN}_DIR` to `PROJECT_{SOURCE,BIN}_DIR`

* 2023-07-14 tmadlener ([PR#452](https://github.com/AIDASoft/podio/pull/452))
  - Extend the pre-processor condition for the `to_json` functionality to not be visible in `rootcling` or the root interpreter. Fixes #435

* 2023-07-13 Benedikt Hegner ([PR#450](https://github.com/AIDASoft/podio/pull/450))
  - Add optional description and author fields to component definition

* 2023-07-13 tmadlener ([PR#449](https://github.com/AIDASoft/podio/pull/449))
  - Fix the `pre-commit` workflow by making it run on top of the key4hep nightlies that come with a recent enough root version to be able to work with the RNTuple addition (#395)
  - Fix a few minor complaints from newer versions of `clang-format`, `clang-tidy` and `pylint`
  - Enable building the RNTuple backend for more workflows (anything that comes with a new enough ROOT essentially).

* 2023-07-13 tmadlener ([PR#448](https://github.com/AIDASoft/podio/pull/448))
  - Remove the lcio datalayout which has been untouched (and unbuilt) for quite a few years.

* 2023-07-13 tmadlener ([PR#446](https://github.com/AIDASoft/podio/pull/446))
  - Make calling `finish` for the `SIOFrameWriter` non-mandatory. See #442 for other related changes.

* 2023-07-11 jmcarcell ([PR#442](https://github.com/AIDASoft/podio/pull/442))
  - Allow not calling `finish()` when using the writers

* 2023-07-11 jmcarcell ([PR#395](https://github.com/AIDASoft/podio/pull/395))
  - Add support for the new RNTuple format by adding a writer, reader and tests.

* 2023-06-30 Ananya Gupta ([PR#437](https://github.com/AIDASoft/podio/pull/437))
  - Modified parse function definition to incorporate `missing description in member definition` error message. Fixes #436

* 2023-06-27 Thomas Madlener ([PR#413](https://github.com/AIDASoft/podio/pull/413))
  - Introduce `podio::SchemaEvolution` that can hold schema evolution functions and that offers an `evolveBuffers` method that does the schema evolution on these buffers before collections are created.
  - Add hooks in `podio::Frame` to call this when collections are read from the FrameData.

* 2023-06-23 tmadlener ([PR#434](https://github.com/AIDASoft/podio/pull/434))
  - Properly handle the slightly different branch contents before `v00-16-04`. Fixes #433 
  - Add tests that use the `ROOTLegacyReader` to actually read the downloaded legacy files

* 2023-06-15 tmadlener ([PR#428](https://github.com/AIDASoft/podio/pull/428))
  - Split the tests directory into several (more or less) topical sub-directories to declutter the main test `CMakeLists.txt` a bit
  - Move commonly used functionality into `cmake/podioTests.cmake` (e.g. setting up a test environment)
  - Move python unittests config to the `python` directory

* 2023-06-15 tmadlener ([PR#427](https://github.com/AIDASoft/podio/pull/427))
  - Delay library loading as long as possible, mainly for quicker responses for `--help`
  - Add a `--version` flag for dumping the podio version
  - Display collections and parameters in alphabetical order and automatically adjust column widths to fit contents (using the `tabulate` package).

* 2023-06-09 Thomas Madlener ([PR#402](https://github.com/AIDASoft/podio/pull/402))
  - Add public `static constexpr char*` type names to the collections and make the `getXXXName()` methods return `string_view`s to these strings. **This is a breaking change to the interface of the collections if you explicitly rely on them being `std::string`**
    - `typeName`: the full type name of the collection (returned also by `getTypeName`)
    - `valueTypeName`: the (immutable) type name of the objects of the collection (returned by `getValueTypeName`)
    - `dataTypeName`: the type name of the data PODs (returned by `getDataTypeName`)
  - Make unittest environment properly use `PODIO_SIOBLOCK_PATH`
  - `USE_EXTERNAL_CATCH2` now can also be set to `AUTO` to look for a suitable version of Catch2 before falling back and fetching and building it's own version instead of a hard fail.

* 2023-06-08 tmadlener ([PR#426](https://github.com/AIDASoft/podio/pull/426))
  - Check if `PODIO_SIOBLOCK_PATH` exists in the environment and use that to look for SIO Blocks libraries before falling back to `LD_LIBRARY_PATH`. This makes it possible to make slightly more robust environments if several (incompatible) podio installations are visible on `LD_LIBRARY_PATH`

* 2023-06-08 tmadlener ([PR#425](https://github.com/AIDASoft/podio/pull/425))
  - Add a `SKIP_CATCH_DISCOVERY` cmake option to skip the unittest discovery of Catch2 to avoid running the catch discovery in an unsuitable environment.
  - Make environment for unittests more specific to avoid catching too much of the underlying environment.

* 2023-06-08 tmadlener ([PR#412](https://github.com/AIDASoft/podio/pull/412))
  - Using string hashes as CollectionID based on MurmurHash

* 2023-06-05 tmadlener ([PR#423](https://github.com/AIDASoft/podio/pull/423))
  - Add some more structure to make it easier to add more legacy tests.
    - Use this to download more legacy files automatically
    - Restructure CMake config to make this possible
  - Add tests for Frame based root I/O reading files that have been produced with prior versions of podio
  - Add more tests for EventStore based root I/O reading files that have been produced with prior versions of podio

* 2023-06-05 tmadlener ([PR#421](https://github.com/AIDASoft/podio/pull/421))
  - Make the collections appear in alphabetical order in root files, using a case insensitive sorting of the collections that are written.

* 2023-06-05 Thomas Madlener ([PR#405](https://github.com/AIDASoft/podio/pull/405))
  - Make the branch names for relations and vector members more legible and valid c++ variable names to improve interoperability with RDataFrame. Fixes #169 
    - The branch names will have the following structure: `_<collection-name>_<relation-name>`, resp. `_<collection-name>_<vectormember-name>`, where `relation-name`, resp.`vectormember-name` are taken from the YAML definitions.
    - Subset collections will have a single branch with `<collection-name>_objIdx`. This makes it easier to disambiguate them from normal collections.
  - **This is a breaking change if you use the root files directly! If you use the podio Readers/Writers everything should be transparent**

* 2023-05-30 tmadlener ([PR#422](https://github.com/AIDASoft/podio/pull/422))
  - Fix small bug in Frame python bindings where set but empty parameters could crash `podio-dump` when trying to access a non-existent element

# v00-16-05

* 2023-05-23 tmadlener ([PR#420](https://github.com/AIDASoft/podio/pull/420))
  - Fix a version check inside the `ROOTReader` to avoid segmentation violations

# v00-16-04

* 2023-05-23 tmadlener ([PR#417](https://github.com/AIDASoft/podio/pull/417))
  - Fix an issue with reading multiple files via the `ROOTFrameReader` ([#411](https://github.com/AIDASoft/podio/issues/411))
    - Add documentation for API of opening file(s)
    - Add tests for reading multiple files

* 2023-05-22 tmadlener ([PR#418](https://github.com/AIDASoft/podio/pull/418))
  - Bring back the public templated `getMap` functionality for `podio::GenericParameters` as they are already used in DD4hep (see [AIDASoft/DD4hep#1112](https://github.com/AIDASoft/DD4hep/pull/1112)). 
    - Mark the existing `getXYZMap` as deprecated but keep them for a brief transition period.
    - These have been removed in [#415](https://github.com/AIDASoft/podio/pull/415).

* 2023-05-19 jmcarcell ([PR#416](https://github.com/AIDASoft/podio/pull/416))
  - Remove selection rules for classes that don't exist anymore

* 2023-05-15 jmcarcell ([PR#415](https://github.com/AIDASoft/podio/pull/415))
  - Remove the deprecated getters and setters from the generic parameters

* 2023-05-15 jmcarcell ([PR#410](https://github.com/AIDASoft/podio/pull/410))
  - Remove the square that is run when cmake runs

* 2023-05-09 tmadlener ([PR#414](https://github.com/AIDASoft/podio/pull/414))
  - Fix off-by-one error in `UserDataCollection::print` that caused the first element to be printed twice.

* 2023-05-09 Thomas Madlener ([PR#394](https://github.com/AIDASoft/podio/pull/394))
  - Introduce a `CollectionBufferFactory` that can create the necessary buffers from a collection type, a schema version and a subset collection flag.
    - Use this factory throughout all existing Readers
    - Remove `createBuffers` and `createSchemaEvolvableBuffers` from `podio::CollectionBase` interface
  - Make the minimum allowed `schema_version` 1 in the yaml definition files. Default to 1 if no `schema_version` is provided
  - Add a `schemaVersion` to the `DatamodelDefinition.h` header that is generated and that can be accessed via `{{ package_name }}::meta::schemaVersion`. Use this to propagate schema information to the necessary places.
  - Make `SIOBlocks` write the current schema version, such that on reading they can generate the appropriate buffers for the version on file.

* 2023-04-22 Christopher Dilks ([PR#408](https://github.com/AIDASoft/podio/pull/408))
  - fix type inconsistency between `Collection::size()` and index for const object accessors

* 2023-04-21 jmcarcell ([PR#387](https://github.com/AIDASoft/podio/pull/387))
  - Make sure that the dump model round trip tests work without `ENABLE_SIO`
  - Actually test the extension model dumping

* 2023-04-12 Thomas Madlener ([PR#400](https://github.com/AIDASoft/podio/pull/400))
  - Fix a bug in `SIOFrameData::getAvailableCollections` to also work with Frames where some of the collections have not been written and that could lead to a seg fault.
  - Add a test for this in c++ (previously only covered in python unittests of Frame).

* 2023-04-05 Thomas Madlener ([PR#399](https://github.com/AIDASoft/podio/pull/399))
  - Add `PODIO_ENABLE_SIO=1` to the public `target_compile_definitions` for `podioSioIO` so that all dependent targets automatically get it as well. This should make it easier to use SIO dependent features in dependencies. 
  - Consistently use a scope for `target_link_libraries` in tests.

* 2023-04-03 Paul Gessinger-Befurt ([PR#398](https://github.com/AIDASoft/podio/pull/398))
  - Do not reject building if ROOT was built with C++20 (instead of C++17).

* 2023-04-03 Thomas Madlener ([PR#397](https://github.com/AIDASoft/podio/pull/397))
  - Remove the `GENERATED` property from generated files in CMake to avoid inconsistent removal of headers and source files with the `clean` target. Fixes [#396](https://github.com/AIDASoft/podio/issues/396)

* 2023-03-15 Benedikt Hegner ([PR#341](https://github.com/AIDASoft/podio/pull/341))
  - Adding infrastructure for schema evolution
  - Added explicit version tracking to the metadata
  - Data model comparison tool w/ simple heuristics to identify potential omissions / mistakes (e.g. checking for the limits of the ROOT backend)
  - Changed handling of backwards compatibility for the collection info metadata

# v00-16-03

* 2023-03-14 jmcarcell ([PR#391](https://github.com/AIDASoft/podio/pull/391))
  - Catch an exception when a `clang-format` flag is not found

* 2023-03-14 jmcarcell ([PR#390](https://github.com/AIDASoft/podio/pull/390))
  - Modify the initial `clang-format` check to try to run with all the arguments that will be used later

* 2023-03-13 jmcarcell ([PR#389](https://github.com/AIDASoft/podio/pull/389))
  - Add .cache to the gitignore

* 2023-03-07 Thomas Madlener ([PR#358](https://github.com/AIDASoft/podio/pull/358))
  - Embed the EDM definition in JSON format into the shared core datamodel libraries
    - Generate an additional `DatamodelDefinition.h` header file containing the string literal json encoded definition
    - Statically register this to the newly introduced `DatamodelRegistry` and make collections aware of which datamodel they belong to
  - Collect all EDM definitions from all collections that are written with a writer and write all these definitions to the resulting file
    - Currently only done for the `FrameWriter`s
  - Give `podio-dump` the necessary functionality to retrieve the stored models and dump them in YAML format again
    - Add roundtrip tests that compare the generated code from the original model and the one that has been dumped from a data file to ensure that all components work as intended.
  - See the [advanced topics documentation](https://github.com/tmadlener/podio/blob/store-model-def/doc/advanced_topics.md#retrieving-the-edm-definition-from-a-data-file) for more details.

* 2023-03-06 Dmitry Kalinkin ([PR#384](https://github.com/AIDASoft/podio/pull/384))
  - Added an operator for conversion to std::string for podio::version::Version

* 2023-03-01 Thomas Madlener ([PR#378](https://github.com/AIDASoft/podio/pull/378))
  - Introduce deprecation warnings for the **EventStore based I/O model** as it **will be removed in favor of the `Frame` based one**

* 2023-03-01 Thomas Madlener ([PR#372](https://github.com/AIDASoft/podio/pull/372))
  - Make `double` a supported type of `GenericParameters`. A similar thing has been added to LCIO in [iLCSoft/LCIO#143](https://github.com/iLCSoft/LCIO/pull/143) to support storing event weights that need double precision.
  - Add more unittests to the `GenericParameters` covering also the available constructors.

* 2023-02-27 Thomas Madlener ([PR#380](https://github.com/AIDASoft/podio/pull/380))
  - Add `getParameters` method to the `Frame` and deprecate `getGenericParametersForWrite` which offered the exact same functionality.
    - Make it easily possible to get all parameters that are currently stored in a Frame via an "official" channel
    - Replace all internal usages.
  - Add a `getParameterKeys` templated method to get the keys for different parameter types that are currently stored in the Frame.

* 2023-02-22 jmcarcell ([PR#377](https://github.com/AIDASoft/podio/pull/377))
  - Add a visualization tool that converts a YAML description to a graph

* 2023-02-21 jmcarcell ([PR#376](https://github.com/AIDASoft/podio/pull/376))
  - Fix tests without SIO

* 2023-02-14 Thomas Madlener ([PR#375](https://github.com/AIDASoft/podio/pull/375))
  - Fix the `PODIO_VERSION` preprocessor macro to be actually usable in a preprocessor context. Fixes [#374](https://github.com/AIDASoft/podio/issues/374)
  - Make `podio_VERSION` preprocessor constant something that can be used in a preprocessor context (now the same as `PODIO_BUILD_VERSION`
  - Add test that ensures that the macro and the constant are actually used in a preprocessor context.

* 2023-02-13 Juraj Smiesko ([PR#373](https://github.com/AIDASoft/podio/pull/373))
  - Adding ID to the short podio-dump output

* 2023-02-06 Nathan Brei ([PR#369](https://github.com/AIDASoft/podio/pull/369))
  - Mark non-templated definitions of `Frame::Frame`, `Frame::get`, `Frame::put` and `Frame::putParameters` as `inline` to fix linker errors.

* 2023-02-02 jmcarcell ([PR#364](https://github.com/AIDASoft/podio/pull/364))
  - Make workflows not trigger twice on pushes to PRs

* 2023-01-26 jmcarcell ([PR#368](https://github.com/AIDASoft/podio/pull/368))
  - CMAKE: Add option PODIO_RELAX_PYVER to allow relaxing the required match of python version with the one that ROOT has been built with to only check major and minor versions

* 2023-01-16 Thomas Madlener ([PR#363](https://github.com/AIDASoft/podio/pull/363))
  - Move sio utility functionality defined in `SIOFrameWriter.cc` to private `sioUtils.h` header and use it also in the legacy `SIOWriter`.
  - Fix cmake configure dependencies (missed in #343) for datamodel generation macro.
  - Use `defaultdict` instead of hand rolling one in class generator.

* 2023-01-16 Thomas Madlener ([PR#361](https://github.com/AIDASoft/podio/pull/361))
  - Add basic I/O tests for datatypes defined in the extension datamodel. Fixes #319

* 2023-01-11 jmcarcell ([PR#355](https://github.com/AIDASoft/podio/pull/355))
  - Change the readers so that when the file is missing they won't crash

* 2023-01-10 jmcarcell ([PR#365](https://github.com/AIDASoft/podio/pull/365))
  - Fix the pre-commit workflow

* 2022-12-23 jmcarcell ([PR#362](https://github.com/AIDASoft/podio/pull/362))
  - Rename the variable `match` to avoid collisions with a python keyword from Python 3.10 onwards

# v00-16-02

* 2022-12-19 Thomas Madlener ([PR#360](https://github.com/AIDASoft/podio/pull/360))
  - Make the log output of loading the SIOBlock libraries more informative by also providing the absolute paths to the loaded (and rejected) shared libraries.

* 2022-12-16 Thomas Madlener ([PR#333](https://github.com/AIDASoft/podio/pull/333))
  - Initialize the `unique_ptr<mutex>` in the constructor initializer list instead of in the member variable declaration. This is more likely a bug in nvcc (or maybe a c++17 feature not yet supported by nvcc). Fixes key4hep/k4Clue#34
  - Pass `--disable-new-dtags` to the linker when using `PODIO_SET_RPATH`, to set `RPATH` and not `RUNPATH` in the binaries.
  - Pin the ubuntu version for runners that build on ubuntu to not accidentally go out of sync with the underlying LCG releases.
  - Disable the podio tests in the edm4hep workflows (see #359).

# v00-16-01

* 2022-12-06 jmcarcell ([PR#356](https://github.com/AIDASoft/podio/pull/356))
  - Fix path in the README
  - Use the functionality in argparse to choose between options

* 2022-12-06 Benedikt Hegner ([PR#346](https://github.com/AIDASoft/podio/pull/346))
  - Switched tp Apache 2.0 license to facilitate integration in experiment stacks.

* 2022-12-05 Thomas Madlener ([PR#357](https://github.com/AIDASoft/podio/pull/357))
  - Put `<prefix>/bin` onto `PATH` in order to make `podio-dump` available from environments created with `env.sh`

* 2022-12-02 jmcarcell ([PR#354](https://github.com/AIDASoft/podio/pull/354))
  - Make `env.sh` setup script POSIX compliant to run in shells other than bash
    - Change `==` to `=`
    - Change tabs to spaces (two) to avoid mix of spaces and tabs for indenting
    - Add `<prefix>/include` to `ROOT_INCLUDE_PATH` (as it is required since #343)

* 2022-11-16 Thomas Madlener ([PR#351](https://github.com/AIDASoft/podio/pull/351))
  -  Fix bug in Frame python bindings where empty collections were considered as non-existing. Replacing the original check relying on some implicit boolean conversions (which also caught empty collections) to an explicit check against `nullptr`.
  - Make `podio-dump` more robust in installations without SIO support, by guarding the corresponding import.

* 2022-11-14 Thomas Madlener ([PR#344](https://github.com/AIDASoft/podio/pull/344))
  - Make `podio-dump` work with new Frame based I/O (fixes #339)
  - Keep existing functionality intact by using the legacy readers introduced in #345.

* 2022-11-11 Thomas Madlener ([PR#345](https://github.com/AIDASoft/podio/pull/345))
  - Add a `ROOTLegacyReader` and a `SIOLegacyReader` that read files that have been written prior to #287 into `podio::Frame`s and offers the same interface as the frame readers
    - Also including python bindings for it

* 2022-11-10 Thomas Madlener ([PR#349](https://github.com/AIDASoft/podio/pull/349))
  - Fix bug in setting relations in nested get calls in `podio::Frame`. Fixes #348 
  - Adapt the read test to actually check this. Previously this went unnoticed, because the necessary relations were already set in a previous call.

* 2022-11-10 Thomas Madlener ([PR#343](https://github.com/AIDASoft/podio/pull/343))
  - Add python bindings for `Frame` based I/O
    - Available from `podio.root_io` and `podio.sio_io`, where a `Reader` and a `Writer` is implemented for each.
    - Wrapper around `podio::Frame`. **Requires that the `podio/Frame.h` header is available somewhere on the `ROOT_INCLUDE_PATH`**.
  - Add necessary functionality for python bindings to C++ API
    - untyped `Frame::get` method for getting collections
    - New constructor from `FrameDataT&&`
    - functionality to inspect file and `Frame` contents more easily
  - Reorganize python code into structure that follows the usual python packaging conventions a bit more closely
    - Introduce the `podio` module. Make CMake generate the `__init__.py` with the correct version
    - Move everything except the generator script into `module`. Additionally also keep an `EventStore` wrapper to not break existing code. 
  - Refactor the `CMakeLists.txt` that is responsible for building the core and all required I/O libraries
    - Build more dictionaries for more python bindings.

* 2022-11-02 Thomas Madlener ([PR#342](https://github.com/AIDASoft/podio/pull/342))
  - Migrate to `actions/checkout@v3` as advised by [github](https://github.blog/changelog/2022-09-22-github-actions-all-actions-will-begin-running-on-node16-instead-of-node12/)
  - Use the checkout action to clone the dependencies in the edm4hep workflow instead of doing an explicit clone in the body of the action

* 2022-11-02 Dmitry Kalinkin ([PR#327](https://github.com/AIDASoft/podio/pull/327))
  - fix typo in documentation

* 2022-10-24 Juraj Smiesko ([PR#340](https://github.com/AIDASoft/podio/pull/340))
  - Adding reading of specific entry from frame

* 2022-10-21 Thomas Madlener ([PR#335](https://github.com/AIDASoft/podio/pull/335))
  - Update the `github-action-cvmfs` and `run-lcg-view` actions to their latest available version to pick up the latest improvements (caching of dependencies, log groups)
  - Introduce log groups in github actions for easier to interpret outputs
  - Switch to LCG_102 for lcg based build environments
  - Add a workflow that builds and tests EDM4hep after building podio

# v00-16

* 2022-10-04 Thomas Madlener ([PR#337](https://github.com/AIDASoft/podio/pull/337))
  - Make the notebook pattern functionality return `std::vector`s instead of `std::array` to avoid having to specify a static size. Fixes #332 
  - **Backwards incompatible change** as the return type as well as the call signature for the notebook pattern change.

* 2022-09-27 Andre Sailer ([PR#336](https://github.com/AIDASoft/podio/pull/336))
  - podioConfig.cmake: silence warning about cmake policy CMP00012
  - CMake: explicitly look for catch2 version 3 and fail at cmake instead of compile step

* 2022-09-27 Thomas Madlener ([PR#334](https://github.com/AIDASoft/podio/pull/334))
  - Fix a warning/error message from ROOT from attempts to stream the `std::mutex` members of `GenericParameters` by marking them as transient for the dictionary generation.

* 2022-09-16 Thomas Madlener ([PR#323](https://github.com/AIDASoft/podio/pull/323))
  - Add a `podio-dump` python script (installed to `<prefix>/bin` that can be used to dump event contents to stdout. By default prints an overview over the collections and their types, but can also be used to dump full events, via the `-d` or `--detailed` flag. Use `--help` to get all available options and their descriptions.
  - To allow `podio-dump` to work with all available backends also add support for reading SIO via the `PythonEventStore`.
    - Split off the necessary c++ functionality into a separate `podioPythonStore` library (+ necessary ROOT dictionaries).
  - Add a `print` function to the collections for easier dumping from the python side.
  - Add a `print` function to the `GenericParameters`
  - Make `goToEvent` is a part of the `IReader` interface and correctly implemented it for the `SIOReader`.

* 2022-09-16 Thomas Madlener ([PR#287](https://github.com/AIDASoft/podio/pull/287))
  - Introduce the `podio::Frame` as a generalized, thread-safe (event) data container.
    - This first version offers all necessary functionality and an almost finalized interface, i.e. we plan to keep this as stable as possible, but we might still change things if it turns out that there are better ways to do some things
    - For details about the basic interface and the underlying design considerations please consult the corresponding [documentation](https://github.com/AIDASoft/podio/doc/frame.md)
  - **This will be the only way to work with podio data starting from version 1.0**
    - For now the current I/O implementations remain in place unchanged, but they will be deprecated (and removed) in the not too distant future

# v00-15

* 2022-08-09 Thomas Madlener ([PR#312](https://github.com/AIDASoft/podio/pull/312))
  - Add support for converting objects and collections to JSON using [nlohmann/json](https://github.com/nlohmann/json).
    - To enable JSON support it is necessary to build the datamodel with `PODIO_JSON_OUTPUT` and to link against the nlohmann/json library.

* 2022-08-05 Wouter Deconinck ([PR#318](https://github.com/AIDASoft/podio/pull/318))
  - CMake: PODIO_ADD_ROOT_IO_DICT: Bugfix for data models in `OUTPUT_FOLDER` not equal to source dir in root dictionary generation cmake macro. 
    - Now `SELECTION_XML` can be passed either as absolute path or relative to `OUTPUT_FOLDER`.

* 2022-08-03 Thomas Madlener ([PR#317](https://github.com/AIDASoft/podio/pull/317))
  - Make it possible to pass an upstream datamodel to the class generator such that datatypes and components defined there can be used in an unrelated datamodel. This makes it possible to extend datamodels and to prototype new datatypes with the aim of upstreaming them eventually without having to redefine all the necessary components.
  - Refactor the internals of the config reader / class generator slightly to make it possible to hold multiple datamodels in memory

* 2022-08-02 Thomas Madlener ([PR#316](https://github.com/AIDASoft/podio/pull/316))
  - Remove macOS CI workflows because github hosted runners will deprecate macOS 10.15 ([announcement](https://github.com/actions/virtual-environments/issues/5583)) and later versions of macOS no longer support fuse and as a consequence CVMFS.

* 2022-07-27 Thomas Madlener ([PR#315](https://github.com/AIDASoft/podio/pull/315))
  - Make the `is_trivial_type` flag available in the template engine behave as expected (it behaved exactly oppositely to what was documented and what one would intuitively expect). The flag was originally introduced in #288

* 2022-07-27 Thomas Madlener ([PR#283](https://github.com/AIDASoft/podio/pull/283))
  - Allow users to define default values for member variables, instead of default initializing all of them.
    - The syntax for specifying a default value is `- <type> <name>{<init-value>} // <description>`.
    - The passed value is not validated in any way. **Apart from a very basic syntax check, there is no validation that the provided default initialization values are actually valid**. This means that generated code might not compile.
  - Remove some of the python2 compatibility and do some cleanup

* 2022-07-27 Thomas Madlener ([PR#276](https://github.com/AIDASoft/podio/pull/276))
  - Remove support for having std::string members in datatypes and components, as they break PODness and it seems that this feature was not in use in any case.
  - Make ROOTReader slightly more robust against missing datatypes in dictionaries when reading files.

* 2022-06-22 Valentin Volkl ([PR#307](https://github.com/AIDASoft/podio/pull/307))
  - hotfix for https://github.com/AIDASoft/podio/issues/290: revert a clang-tidy change to make sure that there are no unknown symbols in podioDict

* 2022-06-21 Thomas Madlener ([PR#282](https://github.com/AIDASoft/podio/pull/282))
  - Add a `PODIO_USE_CLANG_FORMAT` option to the cmake configuration to toggle the autodiscovery of `clang-format` and a `.clang-format` configuration file. This option is also available for downstream packages that use podio to generate their EDM. 
    - The default is `AUTO`, where we try to discover a suitable `clang-format` version as well as a `.clang-format` file and use it if we find it.
    - If set to `OFF` podio will not try to see whether `clang-format` and a `.clang-format` file are available and will also not try to format the code accordingly.
    - If set to `ON` podio will actually require a suitable `clang-format` version and the presence of a `.clang-format` file and will fail at the cmake stage if not present.

* 2022-06-16 Thomas Madlener ([PR#305](https://github.com/AIDASoft/podio/pull/305))
  - Make sure generator warnings are printed
  - Add a deprecation warning for the upcoming removal of support of `std::string` in data types. (See also #276)

* 2022-06-16 Thomas Madlener ([PR#294](https://github.com/AIDASoft/podio/pull/294))
  - Remove the `EventStore`, `CollectionIDTable` and `version::Version` members from the `SIOCollectionIDTableBlock` to make it easier to use in the `Frame` context
  - Move the `podio:version::build_version` into its own `SIOVersionBlock`
  - **This is a breaking change for the SIO backend and it will not be able to read files that have been written prior to this**

# v00-14-02

* 2022-06-15 Thomas Madlener ([PR#304](https://github.com/AIDASoft/podio/pull/304))
  - Use the releases v3.0.1 version of Catch2 instead of an unreleased commit

* 2022-06-15 Thomas Madlener ([PR#303](https://github.com/AIDASoft/podio/pull/303))
  - Default initialize the array for the vectorized access.

* 2022-06-14 soumil ([PR#296](https://github.com/AIDASoft/podio/pull/296))
  - Add instructions on how to run pre-commit locally to the documentation

* 2022-06-14 Thomas Madlener ([PR#295](https://github.com/AIDASoft/podio/pull/295))
  - Mark `CollectionBase::prepareForWrite` as `const` and make sure that the generated implementations are thread safe.

* 2022-06-14 Thomas Madlener ([PR#286](https://github.com/AIDASoft/podio/pull/286))
  - Make sure that vector member buffers for writing point to the correct place even if a collection has been moved, by resetting them when the buffers are requested.
  - Add checks for this to the unittests, as this is sort of an interface for I/O backends.

* 2022-06-13 Kalina Stoimenova ([PR#301](https://github.com/AIDASoft/podio/pull/301))
  - Fixed the text in the cmake message for code generation to point to the correct readme file

* 2022-06-13 Thomas Madlener ([PR#300](https://github.com/AIDASoft/podio/pull/300))
  - Newer versions of pylint have removed a few options and a few checks that aimed at python2-python3 compatibility.

* 2022-06-13 Thomas Madlener ([PR#299](https://github.com/AIDASoft/podio/pull/299))
  - Explicitly add constructors to `CollectionBase`
  - Make sure to not use an unset `LD_LIBRARY_PATH` for detecting sio blocks shared libraries.

* 2022-06-02 soumil ([PR#293](https://github.com/AIDASoft/podio/pull/293))
  - Removing python2 compatibility imports
  - Removing ordered loading (obsolete) function

* 2022-06-01 Thomas Madlener ([PR#285](https://github.com/AIDASoft/podio/pull/285))
  - Fix potential bug in setting the collection ID for subset collections

* 2022-05-30 soumil ([PR#291](https://github.com/AIDASoft/podio/pull/291))
  - Replace the `obj_needs_destructor` flag in the generator code and templates with the `is_trivial_type` flag, since that is the more appropriate name. (Fixes #288)

* 2022-05-27 Thomas Madlener ([PR#274](https://github.com/AIDASoft/podio/pull/274))
  - Add documentation for the Jinja2 templates and the code generation process in general to make working with these parts of PODIO easier.

* 2022-05-23 Thomas Madlener ([PR#262](https://github.com/AIDASoft/podio/pull/262))
  - Make the getters and setters for the `GenericParameters` templated functions and add a deprecation warning for the untemplated ones.
  - Define a `SupportedGenericDataTypes` tuple defining the types (and vectors of those) that can be stored in `GenericParameters`
  - Add a `podio/utilities/TypeHelpers.h` header with some type handling helpers.

* 2022-05-20 Thomas Madlener ([PR#277](https://github.com/AIDASoft/podio/pull/277))
  - Avoid fetching the (remote) legacy input file for tests unnecessarily every time cmake is run.

* 2022-05-17 Thomas Madlener ([PR#284](https://github.com/AIDASoft/podio/pull/284))
  - Make sure the `EventStore` doesn't try to read event meta data multiple times per event
  - Add a `empty` method to `GenericParameters` to check if any parameters are stored.

* 2022-04-04 Thomas Madlener ([PR#280](https://github.com/AIDASoft/podio/pull/280))
  - Only use `--color` option for diff in `clang-format` wrapper script if it is supported by the underlying `diffutils`.

* 2022-04-02 Thomas Madlener ([PR#254](https://github.com/AIDASoft/podio/pull/254))
  - Add a `.clang-format` and `.clang-tidy` config file for consistent formatting and following a few coding guidelines.
  - Add pre-commit hooks that run `clang-tidy` and `clang-format` 
  - Make all currently present files follow the formatting and guidelines of the present configuration.
  - Make the `PODIO_GENERATE_DATAMODEL` macro look for a `.clang-format` file and the presence of `clang-format`and automatically format all the generated files if both are there.

* 2022-04-01 Thomas Madlener ([PR#279](https://github.com/AIDASoft/podio/pull/279))
  - Fix test environment to work again in newest Key4hep release by unsetting `ROOT_INCLUDE_PATH` in the test environment to avoid potential clashes with existing other installations in the environment.
  - Add CI build against the Key4hep nightlies.
  - Switch to use the Catch2 installation from Key4hep for the workflows.

* 2022-03-31 Thomas Madlener ([PR#253](https://github.com/AIDASoft/podio/pull/253))
  - Add a basic setup for [pre-commit](https://pre-commit.com) and replace the python linting github workflow with one that is run via pre-commit.
    - Add additional checks for consistent line-endings and removal of trailing whitespaces.
  - Update pylint and flake8 config to no longer check for python2/python3 compatibility but instead follow the same guidelines as e.g. in ILCDirac.
  - Fix all issues that were uncovered.

* 2022-03-23 Thomas Madlener ([PR#270](https://github.com/AIDASoft/podio/pull/270))
  - Remove duplicated printing of component members in the `std::ostream& operator<<` overloads of the datatypes. Fixes #269 
  - Add an example datatype that broke compilation before these fixes.

* 2022-03-18 Andre Sailer ([PR#265](https://github.com/AIDASoft/podio/pull/265))
  -  CI: use clang12 and gcc11 for tests based on dev stacks

# v00-14-01

* 2022-03-04 Thomas Madlener ([PR#261](https://github.com/aidasoft/podio/pull/261))
  - Make the datamodel validation accept arrays of fixed width integer types.

* 2022-02-09 Placido Fernandez Declara ([PR#259](https://github.com/aidasoft/podio/pull/259))
  - Filter files with regex based on file name, not complete path

* 2022-02-08 Thomas Madlener ([PR#238](https://github.com/aidasoft/podio/pull/238))
  - Extend the `podioVersion.h` header that is configured by cmake to hold some version utilities.
    - `podio::version::Version` class holding three `uint16_t`s for major, minor and patch version, plus `constexpr` comparison operators.
    - static const(expr) `podio::version::build_version` that holds the current (i.e. last tag) version of podio
    - Add preprocessor macros with similar functionality
      - `PODIO_VERSION` takes a major, minor and a patch version number and encodes it into a 64 bit version constant. 
      - `PODIO_[MAJOR|MINOR|PATCH]_VERSION` macros can extracts these values again from a 64 bit encoded version. 
      - `PODIO_BUILD_VERSION` holds the 64 bit encoded current (i.e. last tag) version of podio
  - Reorder the read tests slightly and make some sections version dependent
  - Add legacy file read test from #230

* 2022-01-28 Thomas Madlener ([PR#256](https://github.com/aidasoft/podio/pull/256))
  - Ignore the test introduced in #235 in sanitizer builds as it currently breaks.

* 2022-01-24 Placido Fernandez Declara ([PR#235](https://github.com/aidasoft/podio/pull/235))
  - Fix crashes that happen when reading collections that have related objects in collections that have not been persisted.
  - Fix similar crashes for subset collections where the original collection has not been persisted.
    - The expected behavior in both cases is that podio does not crash when reading such collections, but only once the user tries to actually access such a missing object. Each object has an `isAvailable` function to guard against such crashes if need be.
  - Add a test that makes sure that the expected behavior is the one that is observed.
  - Fix a somewhat related bug in `setReferences` which was mistakenly a no-op for collections of a type without relations. Since this is the mechanism we use for restoring subset collections it obviously has to be present for all types.

* 2022-01-21 Thomas Madlener ([PR#252](https://github.com/aidasoft/podio/pull/252))
  - Make the `CollectionData` classes use `unique_ptr` instead of raw pointers, wherever they actually own the pointer.
  - Implement move constructors and move assignment operators for collections. Thanks to the usage of `unique_ptr` for ownership management in the `CollectionData`, these can be `default`ed in `Collection` and `CollectionData`.
  - Add a few tests to check that moving collections actually works.

* 2022-01-20 Thomas Madlener ([PR#251](https://github.com/aidasoft/podio/pull/251))
  - Make sure that collections of types without relations can still be used properly as subset collections. Previous to these changes, the necessary functionality was not generated if a datatype had no relations (i.e. not a single `OneToOneRelation` or `OneToManyRelation`).
  - Add a check of this functionality to the write/read tests.

* 2022-01-20 Thomas Madlener ([PR#249](https://github.com/aidasoft/podio/pull/249))
  - Add a `USE_SANITIZER` build option to more easily build podio with sanitizers for testing. Curently `Address`, `Memory[WithOrigin]`, `Undefined` and `Thread` are available as options. Given the limitations of the sanitizers these are more or less mutually exlusive.
  - Label all the Catch2 test cases which makes it easier to run them selectively.
  - For builds with sanitizers enabled, by default ignore tests with known failures, but add a `FORCE_RUN_ALL_TESTS` cmake option that overrides this for local development.
  - Run CI workflows with a selection of sanitizers enabled (on a limited list of tests).

* 2022-01-20 hegner ([PR#209](https://github.com/aidasoft/podio/pull/209))
  - Remove mention of Python 2 compatibility

* 2021-12-03 Thomas Madlener ([PR#245](https://github.com/aidasoft/podio/pull/245))
  - Make it possible to call `prepareForWrite` multiple times on collections by rendering all but the first call no-ops. Fixes #241
    - Collections are marked as prepared, either if they are read from file or once `prepareForWrite` has been called on them.

* 2021-12-03 Thomas Madlener ([PR#205](https://github.com/aidasoft/podio/pull/205))
  - Make the default classes immutable and mark mutable classes explictly via their class name (e.g. `Hit` and `MutableHit`). See a brief discussion in #204 for more details on the reasons for this **breaking change**.
  - After these changes collections return mutable objects via their `create` functionality, and will only give access to the default (immutable) objects when they are `const` (e.g. when they are read from file).
  - In general these changes should make it easier for users to write interface that behave as expected, and also make it very obvious where objects are actually mutated already from looking at an interface definition.

* 2021-10-22 Thomas Madlener ([PR#239](https://github.com/aidasoft/podio/pull/239))
  - Fix a typo in the cmake config for finding the correct python version when cmake is used in downstream packages.

* 2021-10-21 Thomas Madlener ([PR#237](https://github.com/aidasoft/podio/pull/237))
  - Mistakenly dropped colon in #236

* 2021-10-14 Thomas Madlener ([PR#236](https://github.com/aidasoft/podio/pull/236))
  - Fix problem in python tests that appears in spack builds

# v00-14-01

* 2022-03-04 Thomas Madlener ([PR#261](https://github.com/aidasoft/podio/pull/261))
  - Make the datamodel validation accept arrays of fixed width integer types.

* 2022-02-09 Placido Fernandez Declara ([PR#259](https://github.com/aidasoft/podio/pull/259))
  - Filter files with regex based on file name, not complete path

* 2022-02-08 Thomas Madlener ([PR#238](https://github.com/aidasoft/podio/pull/238))
  - Extend the `podioVersion.h` header that is configured by cmake to hold some version utilities.
    - `podio::version::Version` class holding three `uint16_t`s for major, minor and patch version, plus `constexpr` comparison operators.
    - static const(expr) `podio::version::build_version` that holds the current (i.e. last tag) version of podio
    - Add preprocessor macros with similar functionality
      - `PODIO_VERSION` takes a major, minor and a patch version number and encodes it into a 64 bit version constant. 
      - `PODIO_[MAJOR|MINOR|PATCH]_VERSION` macros can extracts these values again from a 64 bit encoded version. 
      - `PODIO_BUILD_VERSION` holds the 64 bit encoded current (i.e. last tag) version of podio
  - Reorder the read tests slightly and make some sections version dependent
  - Add legacy file read test from #230

* 2022-01-28 Thomas Madlener ([PR#256](https://github.com/aidasoft/podio/pull/256))
  - Ignore the test introduced in #235 in sanitizer builds as it currently breaks.

* 2022-01-24 Placido Fernandez Declara ([PR#235](https://github.com/aidasoft/podio/pull/235))
  - Fix crashes that happen when reading collections that have related objects in collections that have not been persisted.
  - Fix similar crashes for subset collections where the original collection has not been persisted.
    - The expected behavior in both cases is that podio does not crash when reading such collections, but only once the user tries to actually access such a missing object. Each object has an `isAvailable` function to guard against such crashes if need be.
  - Add a test that makes sure that the expected behavior is the one that is observed.
  - Fix a somewhat related bug in `setReferences` which was mistakenly a no-op for collections of a type without relations. Since this is the mechanism we use for restoring subset collections it obviously has to be present for all types.

* 2022-01-21 Thomas Madlener ([PR#252](https://github.com/aidasoft/podio/pull/252))
  - Make the `CollectionData` classes use `unique_ptr` instead of raw pointers, wherever they actually own the pointer.
  - Implement move constructors and move assignment operators for collections. Thanks to the usage of `unique_ptr` for ownership management in the `CollectionData`, these can be `default`ed in `Collection` and `CollectionData`.
  - Add a few tests to check that moving collections actually works.

* 2022-01-20 Thomas Madlener ([PR#251](https://github.com/aidasoft/podio/pull/251))
  - Make sure that collections of types without relations can still be used properly as subset collections. Previous to these changes, the necessary functionality was not generated if a datatype had no relations (i.e. not a single `OneToOneRelation` or `OneToManyRelation`).
  - Add a check of this functionality to the write/read tests.

* 2022-01-20 Thomas Madlener ([PR#249](https://github.com/aidasoft/podio/pull/249))
  - Add a `USE_SANITIZER` build option to more easily build podio with sanitizers for testing. Curently `Address`, `Memory[WithOrigin]`, `Undefined` and `Thread` are available as options. Given the limitations of the sanitizers these are more or less mutually exlusive.
  - Label all the Catch2 test cases which makes it easier to run them selectively.
  - For builds with sanitizers enabled, by default ignore tests with known failures, but add a `FORCE_RUN_ALL_TESTS` cmake option that overrides this for local development.
  - Run CI workflows with a selection of sanitizers enabled (on a limited list of tests).

* 2022-01-20 hegner ([PR#209](https://github.com/aidasoft/podio/pull/209))
  - Remove mention of Python 2 compatibility

* 2021-12-03 Thomas Madlener ([PR#245](https://github.com/aidasoft/podio/pull/245))
  - Make it possible to call `prepareForWrite` multiple times on collections by rendering all but the first call no-ops. Fixes #241
    - Collections are marked as prepared, either if they are read from file or once `prepareForWrite` has been called on them.

* 2021-12-03 Thomas Madlener ([PR#205](https://github.com/aidasoft/podio/pull/205))
  - Make the default classes immutable and mark mutable classes explictly via their class name (e.g. `Hit` and `MutableHit`). See a brief discussion in #204 for more details on the reasons for this **breaking change**.
  - After these changes collections return mutable objects via their `create` functionality, and will only give access to the default (immutable) objects when they are `const` (e.g. when they are read from file).
  - In general these changes should make it easier for users to write interface that behave as expected, and also make it very obvious where objects are actually mutated already from looking at an interface definition.

* 2021-10-22 Thomas Madlener ([PR#239](https://github.com/aidasoft/podio/pull/239))
  - Fix a typo in the cmake config for finding the correct python version when cmake is used in downstream packages.

* 2021-10-21 Thomas Madlener ([PR#237](https://github.com/aidasoft/podio/pull/237))
  - Mistakenly dropped colon in #236

* 2021-10-14 Thomas Madlener ([PR#236](https://github.com/aidasoft/podio/pull/236))
  - Fix problem in python tests that appears in spack builds

# v00-14

* 2021-10-13 Thomas Madlener ([PR#234](https://github.com/AIDASoft/podio/pull/234))
  - Make sure that `#include <cstdint>` is present when using fixed with integers in datatypes

* 2021-10-12 Thomas Madlener ([PR#232](https://github.com/AIDASoft/podio/pull/232))
  - Make it possible to read "old" podio data files that have been written with podio < 0.13.1 (i.e. before #197) was merged.
    - For ROOT: Reconstruct the `"CollectionInfoType"` branch that as introduced there via other means and simply assume that all collections are proper collections (since subset collections didn't exist prior).
    - For SIO: Bump the version of the `SIOCollectionIDTableBlock` to 0.2 and only read the subset collection bits when they are available.

* 2021-10-12 Valentin Volkl ([PR#231](https://github.com/AIDASoft/podio/pull/231))
  - Add regression test for mutable clones of const objects

* 2021-10-11 Thomas Madlener ([PR#223](https://github.com/AIDASoft/podio/pull/223))
  - Add brief documentation for the newly added `UserDataCollection` added in #213

# v00-14-00

* 2021-10-12 Thomas Madlener ([PR#232](https://github.com/AIDASoft/podio/pull/232))
  - Make it possible to read "old" podio data files that have been written with podio < 0.13.1 (i.e. before #197) was merged.
    - For ROOT: Reconstruct the `"CollectionInfoType"` branch that as introduced there via other means and simply assume that all collections are proper collections (since subset collections didn't exist prior).
    - For SIO: Bump the version of the `SIOCollectionIDTableBlock` to 0.2 and only read the subset collection bits when they are available.

* 2021-10-12 Valentin Volkl ([PR#231](https://github.com/AIDASoft/podio/pull/231))
  - Add regression test for mutable clones of const objects

* 2021-10-11 Thomas Madlener ([PR#223](https://github.com/AIDASoft/podio/pull/223))
  - Add brief documentation for the newly added `UserDataCollection` added in #213

# v00-13-02

* 2021-10-08 Thomas Madlener ([PR#224](https://github.com/AIDASoft/podio/pull/224))
  - Make the `clone` function always return a mutable object, also when called on an immutable object (Fixes #219)

* 2021-09-22 Thomas Madlener ([PR#214](https://github.com/AIDASoft/podio/pull/214))
  - Make the CMake datamodel generation macro use the python interpreter that is also found by CMake to avoid accidentally picking up an unsuitable system provided version that might be on `PATH`.

* 2021-09-21 Frank Gaede ([PR#213](https://github.com/AIDASoft/podio/pull/213))
  - add possibility to store additional user data as collections of fundamental types in PODIO files
       -  uses `std::vector<basic_type>`
       - stored in simple branch in root (and simple block in SIO)
       - all fundamental types supported in PODIO (except bool) can be written
  - example code:
  ```c++
    auto& usrInts = store.create<podio::UserDataCollection<uint64_t> >("userInts");
    auto& usrDoubles = store.create<podio::UserDataCollection<double> >("userDoubles");
    // ...
  
    // add some unsigned ints
    usrInts.resize( i + 1 ) ;
    int myInt = 0 ;
    for( auto& iu : usrInts ){
      iu = myInt++  ;
    }
    // and some user double values
    unsigned nd = 100 ;
    usrDoubles.resize( nd ) ;
    for(unsigned id=0 ; id<nd ; ++id){
      usrDoubles[id] = 42. ;
    }
  ```
  
  - should replace https://github.com/key4hep/EDM4hep/pull/114 in a more efficient way

* 2021-09-21 tmadlener ([PR#143](https://github.com/AIDASoft/podio/pull/143))
  - Generate an additional `podio_generated_files.cmake` file containing all generated source files as a `header` and `sources` list and make the code generation macro include this file to get the headers and source files.
    - Now only the files generated for the current settings are picked up by cmake
    - Makes it possible to have additional files in the folders where the generated files are placed, since these are no longer globbed over.

* 2021-09-10 Thomas Madlener ([PR#217](https://github.com/AIDASoft/podio/pull/217))
  - Make the `Obj` destructors `= default` where possible, i.e. if a datatype has no relations to handle
  - Make the assignment operators of the user facing classes use the "copy-and-swap" idiom
  - Fix the problem where `OneToOneRelations` needed to be from the same namespace as the datatype they are used in (#216)

* 2021-09-06 Thomas Madlener ([PR#211](https://github.com/AIDASoft/podio/pull/211))
  - Fix test dependencies to allow running tests in parallel via `ctest -jN`

* 2021-08-18 Thomas Madlener ([PR#210](https://github.com/AIDASoft/podio/pull/210))
  - Fix a few small issues in the datamodel yaml file validation. **These do not change the behavior of code generation, they just try to catch problems earlier**
    - Make sure that `OneToManyRelations` and `OneToOneRelations` have the same restrictions
    - Only allow `components`, builtins and arrays of those as `Members`
  - Make the API of `validate` slightly more generic by taking a dict instead of multiple arguments.
  - Make the generator exit with an easier to read error message in case of a validation problem instead of printing a full backtrace.

* 2021-08-18 Thomas Madlener ([PR#197](https://github.com/AIDASoft/podio/pull/197))
  - Introduce a `podio::CollectionBuffers` class that contains everything that is necessary for I/O of a given collection. **This is a breaking change in the collection interface** 
  - Introduce  and generate a `CollectionData` class for each datatype that only manages the storage of a given collection.
    - Exposes only the `Obj` entries of each collection as well as the necessary functionality to add a new object (and its relations) to the collection.
  - Implement "subset" collections that behave exactly the same as normal collections apart from an additional function call when creating them.

* 2021-08-13 Thomas Madlener ([PR#206](https://github.com/AIDASoft/podio/pull/206))
  - Switch to Catch2 v3 test library and by default assume that it is available. Use the 'USE_EXTERNAL_CATCH2` cmake option to control whether podio should use an external installation or if it should fetch and build it internally instead.
  - Remove `catch.hpp` header that was previously shipped, since it is no longer needed.

* 2021-08-13 Thomas Madlener ([PR#201](https://github.com/AIDASoft/podio/pull/201))
  - Make assignment operator increase the reference count to avoid possible heap-after-free usage. (Fixes #200)

# v00-13-01

* 2021-06-03 Thomas Madlener ([PR#195](https://github.com/aidasoft/PODIO/pull/195))
  - Fix possible circular and self-includes in generated header files.

* 2021-06-03 Thomas Madlener ([PR#194](https://github.com/aidasoft/PODIO/pull/194))
  - Make it possible to do indexed access on a `RelationRange`, making the interface more akin to a `const std::vector`

* 2021-05-31 tmadlener ([PR#193](https://github.com/aidasoft/PODIO/pull/193))
  - Make collection element access const correct.

* 2021-05-31 Thomas Madlener ([PR#192](https://github.com/aidasoft/PODIO/pull/192))
  - Fix const-correctness problems of meta data access via EventStore.

* 2021-05-28 Benedikt Hegner ([PR#191](https://github.com/aidasoft/PODIO/pull/191))
  - Fix bug in validity check so that transient and persistent collections are treated the same

* 2021-05-28 Thomas Madlener ([PR#186](https://github.com/aidasoft/PODIO/pull/186))
  - Add support for [fixed width integer type](https://en.cppreference.com/w/cpp/types/integer) members in components and datatypes.
    - Now possible to use `int16_t`, `int32_t`, `int64_t`, `uint16_t`, `uint32_t` and `uint64_t` as members. Other fixed width integer types that are potentially defined in `<cstdint>` are not considered valid as the intended use case is really only fixed width integers for now. These are rejected at the datamodel validation step.
    - Fixed width integers are considered to be "builtin" types for podio.

* 2021-05-04 Valentin Volkl ([PR#189](https://github.com/aidasoft/PODIO/pull/189))
  - [cmake] fix test dependencies: `read_and_write.cpp` reads the file `example.root` that is created by the write test. If the dependency is not declared, running the tests concurrently can lead to spurious test failures.

* 2021-04-28 tmadlener ([PR#180](https://github.com/aidasoft/PODIO/pull/180))
  - Improve the branch look-up logic in ROOTReader and ROOTWriter. Triggered by a performance degradation in v6.22/06, where this logic was changed inside ROOT and our use case was affected badly. All ROOT versions profit from these changes as it is in general more efficient than the previous implementation.

* 2021-03-30 tmadlener ([PR#182](https://github.com/aidasoft/PODIO/pull/182))
  - Use `run-lcg-view` github action and switch to more recent LCG releases to run CI.
  - Update README to include status of CI

* 2021-03-23 Valentin Volkl ([PR#185](https://github.com/aidasoft/PODIO/pull/185))
  - extended .gitignore

* 2021-03-23 Valentin Volkl ([PR#184](https://github.com/aidasoft/PODIO/pull/184))
  - Clean up AsciiWriter comments

* 2021-03-23 tmadlener ([PR#183](https://github.com/aidasoft/PODIO/pull/183))
  - Use SIO targets in cmake, which are exported starting with v00-01 (iLCSoft/SIO#15)

* 2021-02-23 Marko Petric ([PR#181](https://github.com/aidasoft/PODIO/pull/181))
  - Add coverity nightly scan based on `run-lcg-view` action

* 2021-02-23 tmadlener ([PR#175](https://github.com/aidasoft/PODIO/pull/175))
  - Fully qualify return types for `OneToOneRelation` getters in generated .cc file for objects and `Const` objects. This fixes a bug described in https://github.com/AIDASoft/podio/issues/168#issuecomment-770751871 and now allows to mix different namespaces in the generated code. This allows to more easily extend already existing datamodels by compiling and linking against them.

* 2021-02-23 Dmitry Romanov ([PR#173](https://github.com/aidasoft/PODIO/pull/173))
  - Added IO Handler argument to schema generation example in README

* 2021-02-23 tmadlener ([PR#171](https://github.com/aidasoft/PODIO/pull/171))
  - Fix compiler warnings, that were uncovered by #153 and described in #170. Fix them in the core classes and also in the generated ones.
  - Enforce no new warnings with `Werror` in the CI builds.

* 2021-02-15 Joseph C Wang ([PR#156](https://github.com/aidasoft/PODIO/pull/156))
  - Readers/writers are now noncopyable

* 2021-02-02 Joseph C Wang ([PR#154](https://github.com/aidasoft/PODIO/pull/154))
  - Disable operator = for collections so that it maintains one copy of collections, fixes #111

* 2021-01-26 tmadlener ([PR#172](https://github.com/aidasoft/PODIO/pull/172))
  - Fix deprecated `brew install` commands in mac workflow

* 2020-12-18 tmadlener ([PR#165](https://github.com/aidasoft/PODIO/pull/165))
  - Add a convenience `RelationRange::emtpy` function for easily checking whether a range is empty.

* 2020-12-18 tmadlener ([PR#162](https://github.com/aidasoft/PODIO/pull/162))
  - Fix cmake problem #161 on Ubuntu

* 2020-12-18 tmadlener ([PR#155](https://github.com/aidasoft/PODIO/pull/155))
  - Add some benchmarking tools, including `TimedReader` and `TimedWriter` decorators that allow to wrap (interface conforming) readers and writers and record the times different operations take. The times are recorded on two levels: setup times, like constructing a reader or "one-time" calls and per event times, for things that happen each event (e.g. `writeEvent` or `readCollection`). Additionally the `BenchmarkRecorder` in principle also allows to track additional things outside of these decorators.

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

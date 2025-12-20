set(CATCH2_MIN_VERSION 3.5.0)
if(USE_EXTERNAL_CATCH2)
  if (USE_EXTERNAL_CATCH2 STREQUAL AUTO)
    find_package(Catch2 ${CATCH2_MIN_VERSION})
  else()
    find_package(Catch2 ${CATCH2_MIN_VERSION} REQUIRED)
  endif()
endif()

if(NOT Catch2_FOUND)
  message(STATUS "Fetching local copy of Catch2 library for unit-tests...")
  # Build Catch2 with the default flags, to avoid generating warnings when we
  # build it
  set(CXX_FLAGS_CMAKE_USED ${CMAKE_CXX_FLAGS})
  set(CMAKE_CXX_FLAGS ${CXX_FLAGS_CMAKE_DEFAULTS})
  Include(FetchContent)
  FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v${CATCH2_MIN_VERSION}
    )
  FetchContent_MakeAvailable(Catch2)
  set(CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras ${CMAKE_MODULE_PATH})

  # Disable clang-tidy on external contents
  set_target_properties(Catch2 PROPERTIES CXX_CLANG_TIDY "")

  # Hack around the fact, that the include directories are not declared as
  # SYSTEM for the targets defined this way. Otherwise warnings can still occur
  # in Catch2 code when templates are evaluated (which happens quite a bit)
  get_target_property(CATCH2_IF_INC_DIRS Catch2 INTERFACE_INCLUDE_DIRECTORIES)
  set_target_properties(Catch2 PROPERTIES INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${CATCH2_IF_INC_DIRS}")

  # Reset the flags
  set(CMAKE_CXX_FLAGS ${CXX_FLAGS_CMAKE_USED})
endif()

find_package(Threads REQUIRED)
add_executable(unittest_podio unittest.cpp frame.cpp buffer_factory.cpp interface_types.cpp std_interoperability.cpp links.cpp)
target_link_libraries(unittest_podio PUBLIC TestDataModel ExtensionDataModel InterfaceExtensionDataModel PRIVATE Catch2::Catch2WithMain Threads::Threads podio::podioRootIO)
if (ENABLE_SIO)
  target_link_libraries(unittest_podio PRIVATE podio::podioSioIO)
endif()

# Add module-specific tests when modules are enabled
if(PODIO_ENABLE_CXX_MODULES)
  # Direct import tests require CMake 3.30+ for proper module consumption support
  # NOTE: As of CMake 3.31, importing modules in regular .cpp files is experimental.
  # This test demonstrates the intended usage pattern when CMake support matures.
  # CMake 3.28+ added CXX_MODULE file sets, but consuming them in regular .cpp files
  # requires dependency scanning to work correctly, which is still evolving.
  if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.30)
    message(STATUS "CMake ${CMAKE_VERSION} - adding module import demonstration test")
    message(STATUS "  Note: Direct 'import' in .cpp files requires experimental CMake features")
    message(STATUS "  This test documents the intended usage when CMake module support matures")
    
    # This test serves as:
    # 1. Documentation of how modules SHOULD be consumed when CMake support is complete
    # 2. A canary to detect when CMake module support reaches maturity
    # 3. Validation that our module interfaces are correctly structured
    add_executable(unittest_podio_import cxx_modules_import.cpp)
    target_link_libraries(unittest_podio_import PRIVATE 
      TestDataModel 
      podio::podio 
      Catch2::Catch2WithMain
    )
    target_compile_features(unittest_podio_import PRIVATE cxx_std_20)
    
    # Add the test but mark it as experimental
    add_test(NAME unittest_import COMMAND unittest_podio_import)
    PODIO_SET_TEST_ENV(unittest_import)
    set_tests_properties(unittest_import PROPERTIES
      LABELS "modules;import;experimental"
      # This test is expected to fail until CMake module dependency scanning matures
      # When it starts passing without code changes, CMake support has improved!
      WILL_FAIL TRUE
    )
  else()
    message(STATUS "CMake ${CMAKE_VERSION} - skipping module import test (requires 3.30+)")
  endif()
endif()

# The unittests can easily be filtered and they are labelled so we can put together a
# list of labels that we want to ignore
set(filter_tests "")
if (NOT FORCE_RUN_ALL_TESTS)
  if(USE_SANITIZER MATCHES "Address")
    set(filter_tests "~[LEAK-FAIL]~[ASAN-FAIL]")
  elseif(USE_SANITIZER MATCHES "Leak")
    set(filter_tests "~[LEAK-FAIL]")
  elseif(USE_SANITIZER MATCHES "Thread")
    set(filter_tests "~[THREAD-FAIL]")
  elseif(USE_SANITIZER MATCHES "Undefined")
    set(filter_tests "~[UBSAN-FAIL]")
  endif()
endif()

if (USE_SANITIZER MATCHES "Memory(WithOrigin)?")
  # Automatic test discovery fails with Memory sanitizers due to some issues in
  # Catch2. So in that case we skip the discovery step and simply run the thing
  # directly in the tests.
  if (FORCE_RUN_ALL_TESTS)
    # Unfortunately Memory sanitizer seems to be really unhappy with Catch2 and
    # it fails to successfully launch the executable and execute any test. Here
    # we just include them in order to have them show up as failing
    add_test(NAME unittest COMMAND unittest_podio ${filter_tests})
    PODIO_SET_TEST_ENV(unittest)
  endif()
else()
  include(Catch)
  catch_discover_tests(unittest_podio
      WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
      TEST_PREFIX "UT_" # make it possible to filter easily with -R ^UT
      TEST_SPEC ${filter_tests} # discover only tests that are known to not fail
        PROPERTIES
        ENVIRONMENT
        PODIO_SIOBLOCK_PATH=${PROJECT_BINARY_DIR}/tests
        ENVIRONMENT
        ROOT_LIBRARY_PATH=${PROJECT_BINARY_DIR}/tests
        ENVIRONMENT
        LD_LIBRARY_PATH=${CMAKE_CURRENT_BINARY_DIR}:${PROJECT_BINARY_DIR}/src:${PROJECT_BINARY_DIR}/tests:$<TARGET_FILE_DIR:ROOT::Tree>:$<$<TARGET_EXISTS:SIO::sio>:$<TARGET_FILE_DIR:SIO::sio>>:$ENV{LD_LIBRARY_PATH}
      ENVIRONMENT
      LSAN_OPTIONS=suppressions=${PROJECT_SOURCE_DIR}/tests/root_io/leak_sanitizer_suppressions.txt
  )
endif()

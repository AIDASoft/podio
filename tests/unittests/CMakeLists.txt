if(CMAKE_CXX_STANDARD GREATER_EQUAL 20)
  set(CATCH2_MIN_VERSION 3.4)
else()
  set(CATCH2_MIN_VERSION 3.1)
endif()
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
    GIT_TAG        v3.4.0
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
add_executable(unittest unittest.cpp frame.cpp)
target_link_libraries(unittest PUBLIC TestDataModel PRIVATE Catch2::Catch2WithMain Threads::Threads podio::podioRootIO)
if (ENABLE_SIO)
  target_link_libraries(unittest PRIVATE podio::podioSioIO TestDataModelSioBlocks)
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

option(SKIP_CATCH_DISCOVERY "Skip the Catch2 test discovery" OFF)

# To work around https://github.com/catchorg/Catch2/issues/2424 we need the
# DL_PATH argument for catch_discoer_tests which requires CMake 3.22 at least
# The whole issue can be avoied if we skip the catch test discovery and set the
# environment on our own
if (CMAKE_VERSION VERSION_LESS 3.22)
  set(SKIP_CATCH_DISCOVERY ON)
endif()

if (USE_SANITIZER MATCHES "Memory(WithOrigin)?" OR SKIP_CATCH_DISCOVERY)
  # Automatic test discovery fails with Memory sanitizers due to some issues in
  # Catch2. So in that case we skip the discovery step and simply run the thing
  # directly in the tests.
  if (FORCE_RUN_ALL_TESTS OR SKIP_CATCH_DISCOVERY)
    # Unfortunately Memory sanitizer seems to be really unhappy with Catch2 and
    # it fails to succesfully launch the executable and execute any test. Here
    # we just include them in order to have them show up as failing
    add_test(NAME unittest COMMAND unittest ${filter_tests})
    PODIO_SET_TEST_ENV(unittest)
  endif()
else()
  include(Catch)
  catch_discover_tests(unittest
      WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
      TEST_PREFIX "UT_" # make it possible to filter easily with -R ^UT
      TEST_SPEC ${filter_tests} # discover only tests that are known to not fail
      DL_PATHS ${CMAKE_CURRENT_BINARY_DIR}:${PROJECT_BINARY_DIR}/src:$<TARGET_FILE_DIR:ROOT::Tree>:$<$<TARGET_EXISTS:SIO::sio>:$<TARGET_FILE_DIR:SIO::sio>>:$ENV{LD_LIBRARY_PATH}
      PROPERTIES
        ENVIRONMENT
        PODIO_SIOBLOCK_PATH=${CMAKE_CURRENT_BINARY_DIR}
  )
endif()

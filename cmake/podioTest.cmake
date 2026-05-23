#--- small utility helper function to set a consistent test environment for the passed test

# We need to preload the sanitizer runtime so that cling can load sanitizer-instrumented
# shared libraries via dlopen (Python tests). Only one sanitizer is assumed to be active.
if(USE_SANITIZER MATCHES "Address")
  set(_PODIO_SANITIZER_SHORT_NAME asan)
elseif(USE_SANITIZER MATCHES "Thread")
  set(_PODIO_SANITIZER_SHORT_NAME tsan)
endif()

if(_PODIO_SANITIZER_SHORT_NAME)
  if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(_PODIO_SANITIZER_LIB_NAME "libclang_rt.${_PODIO_SANITIZER_SHORT_NAME}.so")
  else()
    set(_PODIO_SANITIZER_LIB_NAME "lib${_PODIO_SANITIZER_SHORT_NAME}.so")
  endif()
  execute_process(
    COMMAND ${CMAKE_CXX_COMPILER} -print-file-name=${_PODIO_SANITIZER_LIB_NAME}
    OUTPUT_VARIABLE _PODIO_SANITIZER_SO
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  if(IS_ABSOLUTE "${_PODIO_SANITIZER_SO}" AND EXISTS "${_PODIO_SANITIZER_SO}")
    set(PODIO_SANITIZER_LIBRARY "${_PODIO_SANITIZER_SO}")
  elseif(NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    find_library(PODIO_SANITIZER_LIBRARY NAMES ${_PODIO_SANITIZER_SHORT_NAME})
  endif()
  if(PODIO_SANITIZER_LIBRARY)
    message(STATUS "Found sanitizer runtime library for test environment: ${PODIO_SANITIZER_LIBRARY}")
  else()
    message(WARNING "Could not find sanitizer runtime library; Python tests may fail under ${USE_SANITIZER} sanitizer")
  endif()
endif()

# PODIO_SET_TEST_ENV(test [PYTHON])
#   Sets the standard test environment for <test>.
#   Pass PYTHON for tests that invoke python3: the sanitizer runtime will be
#   preloaded via LD_PRELOAD so that cling can dlopen instrumented shared
#   libraries in case a sanitizer has been enabled for the build.
function(PODIO_SET_TEST_ENV test)
  cmake_parse_arguments(PARSE_ARGV 1 ARG "PYTHON" "" "")
  # We need to convert this into a list of arguments that can be used as environment variable
  list(JOIN PODIO_IO_HANDLERS " " IO_HANDLERS)
  set(test_environment
    ROOT_LIBRARY_PATH=${PROJECT_BINARY_DIR}/tests
    LD_LIBRARY_PATH=${PROJECT_BINARY_DIR}/src:$<TARGET_FILE_DIR:ROOT::Tree>:$<$<TARGET_EXISTS:SIO::sio>:$<TARGET_FILE_DIR:SIO::sio>>:$ENV{LD_LIBRARY_PATH}
      PYTHONPATH=${PROJECT_SOURCE_DIR}/python:$ENV{PYTHONPATH}
      PODIO_SIOBLOCK_PATH=${PROJECT_BINARY_DIR}/tests
      ROOT_INCLUDE_PATH=${PROJECT_SOURCE_DIR}/tests:${PROJECT_SOURCE_DIR}/include:$ENV{ROOT_INCLUDE_PATH}:$<TARGET_FILE_DIR:fmt::fmt>/../include
      SKIP_SIO_TESTS=$<NOT:$<BOOL:${ENABLE_SIO}>>
      IO_HANDLERS=${IO_HANDLERS}
      PODIO_USE_CLANG_FORMAT=${PODIO_USE_CLANG_FORMAT}
      PODIO_BASE=${PROJECT_SOURCE_DIR}
      PODIO_BUILD_BASE=${PROJECT_BINARY_DIR}
      ENABLE_SIO=${ENABLE_SIO}
      PODIO_BUILD_BASE=${PROJECT_BINARY_DIR}
  )
  if(USE_SANITIZER MATCHES "Address")
    list(APPEND test_environment
      "LSAN_OPTIONS=suppressions=${PROJECT_SOURCE_DIR}/tests/lsan_suppressions.txt:$ENV{LSAN_OPTIONS}"
    )
  elseif(USE_SANITIZER MATCHES "Thread")
    list(APPEND test_environment
      "TSAN_OPTIONS=suppressions=${PROJECT_SOURCE_DIR}/tests/tsan_suppressions.txt:$ENV{TSAN_OPTIONS}"
    )
  endif()
  # Preload the sanitizer runtime so cling can dlopen instrumented libraries
  if(ARG_PYTHON AND PODIO_SANITIZER_LIBRARY)
    list(APPEND test_environment "LD_PRELOAD=${PODIO_SANITIZER_LIBRARY}:$ENV{LD_PRELOAD}")
  endif()
  set_property(TEST ${test}
    PROPERTY ENVIRONMENT "${test_environment}"
  )
endfunction()

#--- small utility helper function to allow for a more terse definition of tests below
function(CREATE_PODIO_TEST sourcefile additional_libs)
  string( REPLACE ".cpp" "" name ${sourcefile} )
  add_executable( ${name} ${sourcefile} )
  add_test(NAME ${name} COMMAND ${name})

  target_link_libraries(${name} PRIVATE TestDataModel ExtensionDataModel InterfaceExtensionDataModel ${additional_libs})
  PODIO_SET_TEST_ENV(${name})
endfunction()

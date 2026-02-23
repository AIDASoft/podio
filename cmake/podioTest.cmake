#--- small utility helper function to set a consistent test environment for the passed test

# We need to preload the ASan runtime so that cling can load ASan-instrumented
# shared libraries via dlopen (Python tests)
if(USE_SANITIZER MATCHES "Address")
  if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    execute_process(
      COMMAND ${CMAKE_CXX_COMPILER} -print-file-name=libclang_rt.asan.so
      OUTPUT_VARIABLE _PODIO_ASAN_SO
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(IS_ABSOLUTE "${_PODIO_ASAN_SO}" AND EXISTS "${_PODIO_ASAN_SO}")
      set(PODIO_ASAN_LIBRARY "${_PODIO_ASAN_SO}")
    endif()
  else()
    execute_process(
      COMMAND ${CMAKE_CXX_COMPILER} -print-file-name=libasan.so
      OUTPUT_VARIABLE _PODIO_ASAN_SO
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(IS_ABSOLUTE "${_PODIO_ASAN_SO}" AND EXISTS "${_PODIO_ASAN_SO}")
      set(PODIO_ASAN_LIBRARY "${_PODIO_ASAN_SO}")
    else()
      find_library(PODIO_ASAN_LIBRARY NAMES asan)
    endif()
  endif()
  if(PODIO_ASAN_LIBRARY)
    message(STATUS "Found ASan runtime library for test environment: ${PODIO_ASAN_LIBRARY}")
  else()
    message(WARNING "Could not find ASan runtime library; Python tests may fail under Address sanitizer")
  endif()
endif()

if(USE_SANITIZER MATCHES "Thread")
  if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    execute_process(
      COMMAND ${CMAKE_CXX_COMPILER} -print-file-name=libclang_rt.tsan.so
      OUTPUT_VARIABLE _PODIO_TSAN_SO
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(IS_ABSOLUTE "${_PODIO_TSAN_SO}" AND EXISTS "${_PODIO_TSAN_SO}")
      set(PODIO_TSAN_LIBRARY "${_PODIO_TSAN_SO}")
    endif()
  else()
    execute_process(
      COMMAND ${CMAKE_CXX_COMPILER} -print-file-name=libtsan.so
      OUTPUT_VARIABLE _PODIO_TSAN_SO
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(IS_ABSOLUTE "${_PODIO_TSAN_SO}" AND EXISTS "${_PODIO_TSAN_SO}")
      set(PODIO_TSAN_LIBRARY "${_PODIO_TSAN_SO}")
    else()
      find_library(PODIO_TSAN_LIBRARY NAMES tsan)
    endif()
  endif()
  if(PODIO_TSAN_LIBRARY)
    message(STATUS "Found TSan runtime library for test environment: ${PODIO_TSAN_LIBRARY}")
  else()
    message(WARNING "Could not find TSan runtime library; Python tests may fail under Thread sanitizer")
  endif()
endif()

# PODIO_SET_TEST_ENV(test [PYTHON])
#   Sets the standard test environment for <test>.
#   Pass PYTHON for tests that invoke python3: the sanitizer runtime will be
#   preloaded via LD_PRELOAD so that cling can dlopen instrumented shared
#   libraries.
function(PODIO_SET_TEST_ENV test)
  cmake_parse_arguments(PARSE_ARGV 1 ARG "PYTHON" "" "")
  # We need to convert this into a list of arguments that can be used as environment variable
  list(JOIN PODIO_IO_HANDLERS " " IO_HANDLERS)
  set(test_environment
    ROOT_LIBRARY_PATH=${PROJECT_BINARY_DIR}/tests
    LD_LIBRARY_PATH=${PROJECT_BINARY_DIR}/src:$<TARGET_FILE_DIR:ROOT::Tree>:$<$<TARGET_EXISTS:SIO::sio>:$<TARGET_FILE_DIR:SIO::sio>>:$ENV{LD_LIBRARY_PATH}
      PYTHONPATH=${PROJECT_SOURCE_DIR}/python:$ENV{PYTHONPATH}
      PODIO_SIOBLOCK_PATH=${PROJECT_BINARY_DIR}/tests
      ROOT_INCLUDE_PATH=${PROJECT_SOURCE_DIR}/tests:${PROJECT_SOURCE_DIR}/include:$ENV{ROOT_INCLUDE_PATH}
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
  if(ARG_PYTHON AND PODIO_ASAN_LIBRARY)
    list(APPEND test_environment "LD_PRELOAD=${PODIO_ASAN_LIBRARY}:$ENV{LD_PRELOAD}")
  elseif(ARG_PYTHON AND PODIO_TSAN_LIBRARY)
    list(APPEND test_environment "LD_PRELOAD=${PODIO_TSAN_LIBRARY}:$ENV{LD_PRELOAD}")
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

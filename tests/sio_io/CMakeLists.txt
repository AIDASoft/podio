set(sio_dependent_tests
  read_frame_sio.cpp
  write_frame_sio.cpp
  read_and_write_frame_sio.cpp
  read_python_frame_sio.cpp
)
set(sio_libs podio::podioSioIO)
foreach( sourcefile ${sio_dependent_tests} )
  CREATE_PODIO_TEST(${sourcefile} "${sio_libs}")
endforeach()

set_tests_properties(
  read_frame_sio
  read_and_write_frame_sio

  PROPERTIES
    DEPENDS
    write_frame_sio
)

#--- Write via python and the SIO backend and see if we can read it back in in
#--- c++
add_test(NAME write_python_frame_sio COMMAND python3 ${PROJECT_SOURCE_DIR}/tests/write_frame.py example_frame_with_py.sio sio_io.Writer)
PODIO_SET_TEST_ENV(write_python_frame_sio)
set_property(TEST read_python_frame_sio PROPERTY DEPENDS write_python_frame_sio)

add_executable(read_frame_legacy_sio read_frame_legacy_sio.cpp)
target_link_libraries(read_frame_legacy_sio PRIVATE "${sio_libs}" TestDataModel)

# If the variable is cached and defined now, we have inputs and can add the
# legacy file read test
if (DEFINED CACHE{PODIO_TEST_INPUT_DATA_DIR})
  message(STATUS "Using test inputs stored in: "  ${PODIO_TEST_INPUT_DATA_DIR})

  foreach(version IN LISTS "00-16-05;00-16-06")
    ADD_PODIO_LEGACY_TEST(${version} read_frame_sio example_frame.sio legacy_test_cases)
    ADD_PODIO_LEGACY_TEST(${version} read_frame_legacy_sio example.sio legacy_test_cases)
  endforeach()
endif()

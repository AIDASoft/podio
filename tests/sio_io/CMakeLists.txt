set(sio_dependent_tests
  read_frame_sio.cpp
  write_frame_sio.cpp
  read_and_write_frame_sio.cpp
  read_python_frame_sio.cpp
  write_interface_sio.cpp
  read_interface_sio.cpp
  selected_colls_roundtrip_sio.cpp
)
set(sio_libs podio::podioSioIO podio::podioIO)
foreach( sourcefile ${sio_dependent_tests} )
  CREATE_PODIO_TEST(${sourcefile} "${sio_libs}")
endforeach()

set_tests_properties(write_frame_sio PROPERTIES FIXTURES_SETUP podio_sio_write_fixture)
set_tests_properties(write_interface_sio PROPERTIES FIXTURES_SETUP podio_sio_write_interface_fixture)

set_tests_properties(
  read_frame_sio
  read_and_write_frame_sio
  selected_colls_roundtrip_sio

  PROPERTIES
    DEPENDS
    write_frame_sio
    FIXTURES_REQUIRED podio_sio_write_fixture
)

set_tests_properties(read_interface_sio PROPERTIES FIXTURES_REQUIRED podio_sio_write_interface_fixture)

#--- Write via python and the SIO backend and see if we can read it back in in
#--- c++
add_test(NAME write_python_frame_sio COMMAND python3 ${PROJECT_SOURCE_DIR}/tests/write_frame.py example_frame_with_py.sio sio_io.Writer)
PODIO_SET_TEST_ENV(write_python_frame_sio)
set_tests_properties(write_python_frame_sio PROPERTIES FIXTURES_SETUP podio_sio_write_fixture)
set_tests_properties(read_python_frame_sio PROPERTIES FIXTURES_REQUIRED podio_sio_write_fixture)

add_executable(read_frame_legacy_sio read_frame_legacy_sio.cpp)
target_link_libraries(read_frame_legacy_sio PRIVATE "${sio_libs}" TestDataModel)

foreach(version IN LISTS sio_legacy_test_versions)
  ADD_PODIO_LEGACY_TEST(${version} read_frame_sio ${version}-example_frame.sio)
  if (version MATCHES "^v00-16")
    ADD_PODIO_LEGACY_TEST(${version} read_frame_legacy_sio ${version}-example.sio)
  endif()
endforeach()

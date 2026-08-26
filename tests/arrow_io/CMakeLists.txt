set(arrow_dependent_tests
  read_frame_arrow.cpp
  write_frame_arrow.cpp
  read_and_write_frame_arrow.cpp
  read_python_frame_arrow.cpp
  write_interface_arrow.cpp
  read_interface_arrow.cpp
  selected_colls_roundtrip_arrow.cpp
  write_frame_arrow_multithreaded.cpp
  read_frame_arrow_multithreaded.cpp
)
set(arrow_libs podio::podioArrow podio::podioIO TestDataModelPodioArrow ExtensionDataModelPodioArrow InterfaceExtensionDataModelPodioArrow)
foreach( sourcefile ${arrow_dependent_tests} )
  CREATE_PODIO_TEST(${sourcefile} "${arrow_libs}")
endforeach()

set_tests_properties(write_frame_arrow PROPERTIES FIXTURES_SETUP podio_write_arrow_fixture)
set_tests_properties(write_frame_arrow_multithreaded PROPERTIES FIXTURES_SETUP podio_write_arrow_mt_fixture)
set_tests_properties(write_interface_arrow PROPERTIES FIXTURES_SETUP podio_write_interface_arrow_fixture)

set_tests_properties(
  read_frame_arrow
  read_and_write_frame_arrow
  selected_colls_roundtrip_arrow

  PROPERTIES
    FIXTURES_REQUIRED podio_write_arrow_fixture
)

set_tests_properties(read_interface_arrow PROPERTIES FIXTURES_REQUIRED podio_write_interface_arrow_fixture)
set_tests_properties(read_frame_arrow_multithreaded PROPERTIES FIXTURES_REQUIRED podio_write_arrow_mt_fixture)

#--- Write via python and the Arrow backend and see if we can read it back in in
#--- c++
add_test(NAME write_python_frame_arrow COMMAND python3 ${PROJECT_SOURCE_DIR}/tests/write_frame.py example_frame_with_py.podio_parquet arrow_io.Writer)
PODIO_SET_TEST_ENV(write_python_frame_arrow PYTHON)
set_tests_properties(write_python_frame_arrow PROPERTIES FIXTURES_SETUP podio_write_python_arrow_fixture)
set_tests_properties(read_python_frame_arrow PROPERTIES FIXTURES_REQUIRED podio_write_python_arrow_fixture)

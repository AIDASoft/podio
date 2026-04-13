set(root_dependent_tests
  relation_range.cpp
  read_and_write_associated.cpp
  read_frame_root.cpp
  write_frame_root.cpp
  read_python_frame_root.cpp
  read_frame_root_multiple.cpp
  read_and_write_frame_root.cpp
  write_interface_root.cpp
  read_interface_root.cpp
  read_glob.cpp
  read_frame_root_lazy.cpp
  selected_colls_roundtrip_root.cpp
  write_empty_collections_root.cpp
  )
if(ENABLE_RNTUPLE)
  set(root_dependent_tests
      ${root_dependent_tests}
      write_rntuple.cpp
      read_rntuple.cpp
      read_rntuple_lazy.cpp
      read_python_frame_rntuple.cpp
      write_interface_rntuple.cpp
      read_interface_rntuple.cpp
      selected_colls_roundtrip_rntuple.cpp
     )
endif()
if(ENABLE_DATASOURCE)
  set(root_dependent_tests
      ${root_dependent_tests}
      read_with_rdatasource_root.cpp
  )
endif()
set(root_libs TestDataModelDict ExtensionDataModelDict InterfaceExtensionDataModelDict podio::podioRootIO podio::podioIO)
if(ENABLE_DATASOURCE)
  list(APPEND root_libs podio::podioDataSource)
endif()
foreach( sourcefile ${root_dependent_tests} )
  CREATE_PODIO_TEST(${sourcefile} "${root_libs}")
endforeach()

set_tests_properties(write_frame_root PROPERTIES FIXTURES_SETUP podio_write_root_fixture)
set_tests_properties(write_interface_root PROPERTIES FIXTURES_SETUP podio_write_interface_root_fixture)
set_tests_properties(read_interface_root PROPERTIES FIXTURES_REQUIRED podio_write_interface_root_fixture)


if(ENABLE_RNTUPLE)
  set_tests_properties(write_rntuple PROPERTIES FIXTURES_SETUP podio_write_rntuple_fixture)
  set_tests_properties(write_interface_rntuple PROPERTIES FIXTURES_SETUP podio_write_interface_rntuple_fixture)
endif()

set_tests_properties(
  read_frame_root
  read_frame_root_multiple
  read_and_write_frame_root
  read_glob
  read_frame_root_lazy
  selected_colls_roundtrip_root

  PROPERTIES
    FIXTURES_REQUIRED podio_write_root_fixture
)

add_test(NAME read_python_multiple COMMAND python3 ${PROJECT_SOURCE_DIR}/tests/root_io/read_multiple.py)
PODIO_SET_TEST_ENV(read_python_multiple PYTHON)
set_tests_properties(read_python_multiple PROPERTIES FIXTURES_REQUIRED podio_write_root_fixture)

add_test(NAME write_python_empty_colls_root COMMAND python3 ${PROJECT_SOURCE_DIR}/tests/write_empty_collections.py empty_colls_frame_with_py.root)
PODIO_SET_TEST_ENV(write_python_empty_colls_root PYTHON)
set_tests_properties(write_python_empty_colls_root PROPERTIES FIXTURES_SETUP podio_write_python_empty_colls_root_fixture)

if(ENABLE_RNTUPLE)
  set_tests_properties(
    read_rntuple
    read_rntuple_lazy
    selected_colls_roundtrip_rntuple

    PROPERTIES
      FIXTURES_REQUIRED podio_write_rntuple_fixture
  )
  set_tests_properties(read_interface_rntuple PROPERTIES FIXTURES_REQUIRED podio_write_interface_rntuple_fixture)

  add_test(NAME write_interface_default_rntuple COMMAND write_interface_root example_frame_interface_default_rntuple.root)
  PODIO_SET_TEST_ENV(write_interface_default_rntuple)
  get_test_property(write_interface_default_rntuple ENVIRONMENT ENV)
  set_property(TEST write_interface_default_rntuple PROPERTY ENVIRONMENT "${ENV};PODIO_DEFAULT_WRITE_RNTUPLE=1")
  set_tests_properties(write_interface_default_rntuple PROPERTIES FIXTURES_SETUP podio_write_interface_default_rntuple_fixture)
  add_test(NAME read_interface_default_rntuple COMMAND read_rntuple example_frame_interface_default_rntuple.root)
  set_tests_properties(read_interface_default_rntuple PROPERTIES ENVIRONMENT "${ENV}" FIXTURES_REQUIRED podio_write_interface_default_rntuple_fixture)

endif()

if(ENABLE_DATASOURCE)
  set_tests_properties(read_with_rdatasource_root PROPERTIES FIXTURES_REQUIRED podio_write_root_fixture)
endif()

if(ENABLE_DATASOURCE)
  add_test(NAME use_python_with_rdatasource_root
           COMMAND python3 ${PROJECT_SOURCE_DIR}/tests/root_io/use_datasource.py)
  PODIO_SET_TEST_ENV(use_python_with_rdatasource_root PYTHON)
  set_tests_properties(use_python_with_rdatasource_root PROPERTIES
    FIXTURES_REQUIRED podio_write_root_fixture
    FIXTURES_SETUP    podio_datasource_snapshot_fixture
  )

  add_executable(check_datasource_output check_datasource_output.cpp)
  target_link_libraries(check_datasource_output PRIVATE ROOT::Core ROOT::RIO ROOT::Tree)
  add_test(NAME check_datasource_output COMMAND check_datasource_output)
  PODIO_SET_TEST_ENV(check_datasource_output)
  set_tests_properties(check_datasource_output PROPERTIES
    FIXTURES_REQUIRED podio_datasource_snapshot_fixture
  )
endif()

if(ENABLE_DATASOURCE AND ENABLE_RNTUPLE)
  add_test(NAME use_python_with_rdatasource_rntuple
           COMMAND python3 ${PROJECT_SOURCE_DIR}/tests/root_io/use_datasource.py
                   example_rntuple.root datasource_snapshot_rntuple.root)
  PODIO_SET_TEST_ENV(use_python_with_rdatasource_rntuple PYTHON)
  set_tests_properties(use_python_with_rdatasource_rntuple PROPERTIES
    FIXTURES_REQUIRED podio_write_rntuple_fixture
    FIXTURES_SETUP    podio_datasource_rntuple_snapshot_fixture
  )

  add_test(NAME check_datasource_output_rntuple
           COMMAND check_datasource_output datasource_snapshot_rntuple.root)
  PODIO_SET_TEST_ENV(check_datasource_output_rntuple)
  set_tests_properties(check_datasource_output_rntuple PROPERTIES
    FIXTURES_REQUIRED podio_datasource_rntuple_snapshot_fixture
  )
endif()

add_executable(read_frame_legacy_root read_frame_legacy_root.cpp)
target_link_libraries(read_frame_legacy_root PRIVATE "${root_libs}")

foreach(version IN LISTS root_legacy_test_versions)
  ADD_PODIO_LEGACY_TEST(${version} read_frame_root ${version}-example_frame.root)
  if (version MATCHES "^v00-16")
    ADD_PODIO_LEGACY_TEST(${version} read_frame_legacy_root ${version}-example.root)
  endif()
endforeach()

#--- Write via python and the ROOT backend and see if we can read it back in in
#--- c++
add_test(NAME write_python_frame_root COMMAND python3 ${PROJECT_SOURCE_DIR}/tests/write_frame.py example_frame_with_py.root root_io.Writer)
PODIO_SET_TEST_ENV(write_python_frame_root PYTHON)
set_tests_properties(write_python_frame_root PROPERTIES FIXTURES_SETUP podio_write_python_root_fixture)
set_tests_properties(read_python_frame_root PROPERTIES FIXTURES_REQUIRED podio_write_python_root_fixture)

if (ENABLE_RNTUPLE)
  add_test(NAME write_python_frame_rntuple COMMAND python3 ${PROJECT_SOURCE_DIR}/tests/write_frame.py example_frame_with_py_rntuple.root root_io.RNTupleWriter)
  PODIO_SET_TEST_ENV(write_python_frame_rntuple PYTHON)
  set_tests_properties(write_python_frame_rntuple PROPERTIES FIXTURES_SETUP podio_write_python_rntuple_fixture)

  set_tests_properties(read_python_frame_rntuple PROPERTIES FIXTURES_REQUIRED podio_write_python_rntuple_fixture)
endif()

add_test(NAME param_reading_rdataframe COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/param_reading_rdataframe.py example_frame.root)
PODIO_SET_TEST_ENV(param_reading_rdataframe PYTHON)
set_tests_properties(param_reading_rdataframe PROPERTIES FIXTURES_REQUIRED podio_write_root_fixture)



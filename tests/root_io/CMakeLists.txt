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
  selected_colls_roundtrip_root.cpp
  )
if(ENABLE_RNTUPLE)
  set(root_dependent_tests
      ${root_dependent_tests}
      write_rntuple.cpp
      read_rntuple.cpp
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

set_property(TEST read_interface_root PROPERTY DEPENDS write_interface_root)

set_tests_properties(
  read_frame_root
  read_frame_root_multiple
  read_and_write_frame_root
  read_glob
  selected_colls_roundtrip_root

  PROPERTIES
    DEPENDS write_frame_root
)

add_test(NAME read_python_multiple COMMAND python3 ${PROJECT_SOURCE_DIR}/tests/root_io/read_multiple.py)
PODIO_SET_TEST_ENV(read_python_multiple)
set_property(TEST read_python_multiple PROPERTY DEPENDS write_frame_root)

if(ENABLE_RNTUPLE)
  set_tests_properties(
    read_rntuple
    selected_colls_roundtrip_rntuple

    PROPERTIES
      DEPENDS write_rntuple
  )
  set_property(TEST read_interface_rntuple PROPERTY DEPENDS write_interface_rntuple)
endif()

if(ENABLE_DATASOURCE)
  set_property(TEST read_with_rdatasource_root PROPERTY DEPENDS write_frame_root)
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
PODIO_SET_TEST_ENV(write_python_frame_root)
set_property(TEST read_python_frame_root PROPERTY DEPENDS write_python_frame_root)

if (ENABLE_RNTUPLE)
  add_test(NAME write_python_frame_rntuple COMMAND python3 ${PROJECT_SOURCE_DIR}/tests/write_frame.py example_frame_with_py_rntuple.root root_io.RNTupleWriter)
  PODIO_SET_TEST_ENV(write_python_frame_rntuple)

  set_property(TEST read_python_frame_rntuple PROPERTY DEPENDS write_python_frame_rntuple)
endif()

add_test(NAME param_reading_rdataframe COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/param_reading_rdataframe.py example_frame.root)
PODIO_SET_TEST_ENV(param_reading_rdataframe)
set_property(TEST param_reading_rdataframe PROPERTY DEPENDS write_frame_root)

if(ENABLE_DATASOURCE)
  add_test(NAME read_python_with_rdatasource_root COMMAND python3 ${PROJECT_SOURCE_DIR}/tests/root_io/read_datasource.py)
  PODIO_SET_TEST_ENV(read_python_with_rdatasource_root)
  set_property(TEST read_python_with_rdatasource_root PROPERTY DEPENDS read_with_rdatasource_root)
endif()

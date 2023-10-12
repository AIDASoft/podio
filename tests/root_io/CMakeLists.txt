set(root_dependent_tests
  relation_range.cpp
  read_and_write_associated.cpp
  read_frame_root.cpp
  write_frame_root.cpp
  read_python_frame_root.cpp
  read_frame_root_multiple.cpp
  read_and_write_frame_root.cpp
  )
if(ENABLE_RNTUPLE)
  set(root_dependent_tests
      ${root_dependent_tests}
      write_rntuple.cpp
      read_rntuple.cpp
      read_python_frame_rntuple.cpp
     )
endif()
set(root_libs TestDataModelDict ExtensionDataModelDict podio::podioRootIO)
foreach( sourcefile ${root_dependent_tests} )
  CREATE_PODIO_TEST(${sourcefile} "${root_libs}")
endforeach()

set_tests_properties(
  read_frame_root
  read_frame_root_multiple
  read_and_write_frame_root

  PROPERTIES
    DEPENDS write_frame_root
)

if(ENABLE_RNTUPLE)
  set_property(TEST read_rntuple PROPERTY DEPENDS write_rntuple)
endif()

add_executable(read_frame_legacy_root read_frame_legacy_root.cpp)
target_link_libraries(read_frame_legacy_root PRIVATE "${root_libs}")

message(STATUS "Test inputs will be stored in: ${ExternalData_OBJECT_STORES} if they are not already present")
add_executable(read-legacy-files-root read-legacy-files-root.cpp)
target_link_libraries(read-legacy-files-root PRIVATE TestDataModel TestDataModelDict podio::podioRootIO)

# Add a legacy test case based on a base executable and a version for which an
# input file exists
macro(ADD_PODIO_LEGACY_TEST version base_test input_file)
  ExternalData_Add_Test(legacy_test_cases
    NAME ${base_test}_${version}
    COMMAND ${base_test} DATA{${PROJECT_SOURCE_DIR}/tests/input_files/${input_file}}
  )
  set_property(TEST ${base_test}_${version} PROPERTY ENVIRONMENT
    LD_LIBRARY_PATH=${PROJECT_BINARY_DIR}/tests:${PROJECT_BINARY_DIR}/src:$ENV{LD_LIBRARY_PATH}
    # Clear the ROOT_INCLUDE_PATH for the tests, to avoid potential conflicts
    # with existing headers from other installations
    ROOT_INCLUDE_PATH=
  )
endmacro()

ADD_PODIO_LEGACY_TEST(v00-13 read-legacy-files-root v00-13-example.root legacy_test_cases)

foreach(version IN LISTS legacy_test_versions)
  ADD_PODIO_LEGACY_TEST(${version} read-legacy-files-root ${version}-example.root legacy_test_cases)
  ADD_PODIO_LEGACY_TEST(${version} read_frame_root ${version}-example_frame.root legacy_test_cases)
  ADD_PODIO_LEGACY_TEST(${version} read_frame_legacy_root ${version}-example.root legacy_test_cases)
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

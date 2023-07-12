set(root_dependent_tests
  write.cpp
  read.cpp
  read-multiple.cpp
  relation_range.cpp
  read_and_write.cpp
  read_and_write_associated.cpp
  write_timed.cpp
  read_timed.cpp
  read_frame_root.cpp
  write_frame_root.cpp
  read_frame_legacy_root.cpp
  read_frame_root_multiple.cpp
  )
if(ENABLE_RNTUPLE)
  set(root_dependent_tests
      ${root_dependent_tests}
      write_rntuple.cpp
      read_rntuple.cpp
     )
endif()
set(root_libs TestDataModelDict ExtensionDataModelDict podio::podioRootIO)
foreach( sourcefile ${root_dependent_tests} )
  CREATE_PODIO_TEST(${sourcefile} "${root_libs}")
endforeach()



#--- set some dependencies between the different tests to ensure input generating ones are run first
set_property(TEST read PROPERTY DEPENDS write)
set_property(TEST read-multiple PROPERTY DEPENDS write)
set_property(TEST read_and_write PROPERTY DEPENDS write)
set_property(TEST read_frame_legacy_root PROPERTY DEPENDS write)
set_property(TEST read_timed PROPERTY DEPENDS write_timed)
set_property(TEST read_frame_root PROPERTY DEPENDS write_frame_root)
set_property(TEST read_frame_root_multiple PROPERTY DEPENDS write_frame_root)
if(ENABLE_RNTUPLE)
  set_property(TEST read_rntuple PROPERTY DEPENDS write_rntuple)
endif()

add_test(NAME check_benchmark_outputs COMMAND check_benchmark_outputs write_benchmark_root.root read_benchmark_root.root)
set_property(TEST check_benchmark_outputs PROPERTY DEPENDS read_timed write_timed)


# If the variable is cached and defined now, we have inputs and can add the
# legacy file read test
if (DEFINED CACHE{PODIO_TEST_INPUT_DATA_DIR})
  message(STATUS "Using test inputs stored in: "  ${PODIO_TEST_INPUT_DATA_DIR})
  add_executable(read-legacy-files-root read-legacy-files-root.cpp)
  target_link_libraries(read-legacy-files-root PRIVATE TestDataModel TestDataModelDict podio::podioRootIO)

  # Add a legacy test case based on a base executable and a version for which an
  # input file exists
  macro(ADD_PODIO_LEGACY_TEST version base_test input_file)
    add_test(NAME ${base_test}_${version} COMMAND ${base_test} ${PODIO_TEST_INPUT_DATA_DIR}/${version}/${input_file})
    set_property(TEST ${base_test}_${version} PROPERTY ENVIRONMENT
      LD_LIBRARY_PATH=${PROJECT_BINARY_DIR}/tests:${PROJECT_BINARY_DIR}/src:$ENV{LD_LIBRARY_PATH}
      # Clear the ROOT_INCLUDE_PATH for the tests, to avoid potential conflicts
      # with existing headers from other installations
      ROOT_INCLUDE_PATH=
    )
  endmacro()

  ADD_PODIO_LEGACY_TEST(v00-13 read-legacy-files-root example.root legacy_test_cases)

  foreach(version IN LISTS legacy_test_versions)
    ADD_PODIO_LEGACY_TEST(${version} read-legacy-files-root example.root legacy_test_cases)
    ADD_PODIO_LEGACY_TEST(${version} read_frame_root example_frame.root legacy_test_cases)
    ADD_PODIO_LEGACY_TEST(${version} read_frame_legacy_root example.root legacy_test_cases)
  endforeach()
endif()

#--- Write via python and the ROOT backend and see if we can read it back in in
#--- c++
add_test(NAME write_frame_root_python COMMAND python3 ${CMAKE_CURRENT_LIST_DIR}/write_frame_root.py)
PODIO_SET_TEST_ENV(write_frame_root_python)

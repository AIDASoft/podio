# Some of the tests currently fail when run under some sanitizers, ignore them
# for now (but report that they have been ignored). This allows us to still run
# this in CI

# See: https://gitlab.kitware.com/cmake/community/-/wikis/doc/ctest/Testing-With-CTest#customizing-ctest

# We define some lists here to avoid having to repeat them below
set(failing_with_address_sanitizer
  # Technically most of the write tests here succeed, but there is no point in
  # only running them, since they are practically only the setup step for the
  # actual read tests
  schema_evol:code_gen:datatypes_new_member:read_garbage
  schema_evol:code_gen:datatypes_rename_relation:write_old
  schema_evol:code_gen:datatypes_rename_relation:read
  schema_evol:code_gen:datatypes_rename_relation:write_old_rntuple
  schema_evol:code_gen:datatypes_rename_relation:read_rntuple
)

# This will only apply for clang based builds and is currently the superset of
# the address sanitizer failures
set(failing_with_undefined_sanitizer
  ${failing_with_address_sanitizer}
)

if ((NOT "@FORCE_RUN_ALL_TESTS@" STREQUAL "ON") AND (NOT "@USE_SANITIZER@" STREQUAL ""))

  # ostream_operator is working with Memory sanitizer (at least locally)
  if("@USE_SANITIZER@" MATCHES "Memory(WithOrigin)?")
    list(REMOVE_ITEM CTEST_CUSTOM_TESTS_IGNORE ostream_operator)
  endif()

  if("@USE_SANITIZER@" MATCHES "Address")
    set(CTEST_CUSTOM_TESTS_IGNORE
      ${CTEST_CUSTOM_TESTS_IGNORE}

      ${failing_with_address_sanitizer}
  )
  endif()

  if("@USE_SANITIZER@" MATCHES "Thread")
    set(CTEST_CUSTOM_TESTS_IGNORE
      ${CTEST_CUSTOM_TESTS_IGNORE}

      read_rntuple
  )
  endif()

  if("@USE_SANITIZER@" MATCHES "Undefined" AND "@CMAKE_CXX_COMPILER_ID@" STREQUAL "Clang" AND @CMAKE_CXX_COMPILER_VERSION@ VERSION_LESS_EQUAL "20.0.0")
    set(CTEST_CUSTOM_TESTS_IGNORE
      ${CTEST_CUSTOM_TESTS_IGNORE}
      relation_range

      read_glob
      read_and_write_associated
      write_frame_root
      read_frame_root
      read_frame_root_multiple
      read_and_write_frame_root
      selected_colls_roundtrip_root
      write_empty_collections_root

      podio-dump-root
      podio-dump-detailed-root
      podio-dump-all-events

      write_interface_root
      read_interface_root

      write_rntuple
      read_rntuple
      write_interface_rntuple
      write_interface_default_rntuple
      read_interface_rntuple
      read_interface_default_rntuple
      selected_colls_roundtrip_rntuple

      # Tests failing due to UBSAN DEADLYSIGNAL in
      # ROOT::Detail::TCollectionProxyInfo::Generate
      read_with_rdatasource_root
      read_python_with_rdatasource_root
      param_reading_rdataframe

      write_frame_root_multithreaded
      read_frame_root_multithreaded
      write_frame_rntuple_multithreaded
      read_frame_rntuple_multithreaded

      read_python_multiple
      write_python_empty_colls_root
      write_python_frame_root
      read_python_frame_root
      write_python_frame_rntuple
      read_python_frame_rntuple
      write_python_frame_sio
      read_python_frame_sio
      pyunittest

      datamodel_def_store_roundtrip_root
      datamodel_def_store_roundtrip_root_extension

      # podio-dump of legacy files also triggers the podioDict UBSAN crash
      podio-dump-legacy_root_v00-16-06
      podio-dump-legacy_root-detailed_v00-16-06
      podio-dump-legacy_sio_v00-16-06
      podio-dump-legacy_sio-detailed_v00-16-06

      podio-dump-rntuple
      podio-dump-detailed-rntuple

      datamodel_def_store_roundtrip_rntuple
      datamodel_def_store_roundtrip_rntuple_extension

      schema_evol:code_gen:components_new_member:write_old
      schema_evol:code_gen:components_new_member:read
      schema_evol:code_gen:components_rename_member:write_old
      schema_evol:code_gen:components_rename_member:read
      schema_evol:code_gen:datatypes_new_member:write_old
      schema_evol:code_gen:datatypes_new_member:read
      schema_evol:code_gen:datatypes_rename_member:write_old
      schema_evol:code_gen:datatypes_rename_member:read
      schema_evol:code_gen:implicit_floating_point_change:write_old
      schema_evol:code_gen:implicit_floating_point_change:read

      schema_evol:code_gen:no_change:write_old
      schema_evol:code_gen:no_change:read
      schema_evol:code_gen:no_change_array_member:write_old
      schema_evol:code_gen:no_change_array_member:read
      schema_evol:code_gen:array_component_new_member:write_old
      schema_evol:code_gen:array_component_new_member:read
      schema_evol:code_gen:datatypes_remove_type:write_old
      schema_evol:code_gen:datatypes_remove_type:read

      schema_evol:code_gen:no_change:write_old_rntuple
      schema_evol:code_gen:no_change:read_rntuple
      schema_evol:code_gen:drop_component:write_old_rntuple
      schema_evol:code_gen:drop_component:read_rntuple
      schema_evol:code_gen:multi_schema_drop_component:write_old_rntuple
      schema_evol:code_gen:multi_schema_drop_component:write_oldest_rntuple
      schema_evol:code_gen:multi_schema_drop_component:read_rntuple
      schema_evol:code_gen:implicit_floating_point_change:write_old_rntuple

      schema_evol:code_gen:multi_schema_component_new_member:write_oldest
      schema_evol:code_gen:multi_schema_component_new_member:write_old
      schema_evol:code_gen:multi_schema_component_new_member:read
      schema_evol:code_gen:multi_schema_datatypes_new_member:write_oldest
      schema_evol:code_gen:multi_schema_datatypes_new_member:write_old
      schema_evol:code_gen:multi_schema_datatypes_new_member:read

      schema_evol:code_gen:multi_schema_components_rename_member:write_old
      schema_evol:code_gen:multi_schema_components_rename_member:write_oldest
      schema_evol:code_gen:multi_schema_components_rename_member:read

      schema_evol:code_gen:multi_schema_component_new_member:write_oldest_rntuple
      schema_evol:code_gen:multi_schema_datatypes_new_member:write_oldest_rntuple

      schema_evol:code_gen:drop_component:write_old
      schema_evol:code_gen:drop_component:read
      schema_evol:code_gen:multi_schema_drop_component:write_oldest
      schema_evol:code_gen:multi_schema_drop_component:write_old
      schema_evol:code_gen:multi_schema_drop_component:read

      schema_evol:code_gen:datatypes_new_member:write_old_rntuple
      schema_evol:code_gen:components_new_member:write_old_rntuple
      schema_evol:code_gen:array_component_new_member:write_old_rntuple
      schema_evol:code_gen:datatypes_remove_type:write_old_rntuple
      schema_evol:code_gen:multi_schema_component_new_member:write_old_rntuple
      schema_evol:code_gen:multi_schema_datatypes_new_member:write_old_rntuple

      schema_evol:code_gen:implicit_floating_point_change:read_rntuple
      schema_evol:code_gen:datatypes_new_member:read_rntuple
      schema_evol:code_gen:components_new_member:read_rntuple
      schema_evol:code_gen:array_component_new_member:read_rntuple
      schema_evol:code_gen:datatypes_remove_type:read_rntuple
      schema_evol:code_gen:multi_schema_component_new_member:read_rntuple
      schema_evol:code_gen:multi_schema_datatypes_new_member:read_rntuple


      ${failing_with_undefined_sanitizer}
    )
    foreach(version in @root_legacy_test_versions@)
        list(APPEND CTEST_CUSTOM_TESTS_IGNORE read_frame_root_${version})
        list(APPEND CTEST_CUSTOM_TESTS_IGNORE read_frame_legacy_root_${version})
    endforeach()
  endif()

endif()

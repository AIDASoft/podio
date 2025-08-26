# Some of the tests currently fail when run under some sanitizers, ignore them
# for now (but report that they have been ignored). This allows us to still run
# this in CI

# See: https://gitlab.kitware.com/cmake/community/-/wikis/doc/ctest/Testing-With-CTest#customizing-ctest

# "Integration style tests" pretty much all have problems at the moment with any
# sanitizer

# We define some lists here to avoid having to repeat them below
set(failing_with_address_sanitizer
  # Technically most of the write tests here succeed, but there is no point in
  # only running them, since they are practically only the setup step for the
  # actual read tests
  schema_evol:code_gen:components_new_member:write
  schema_evol:code_gen:components_new_member:read
  schema_evol:code_gen:components_rename_member:write
  schema_evol:code_gen:components_rename_member:read
  schema_evol:code_gen:datatypes_new_member:write
  schema_evol:code_gen:datatypes_new_member:read
  schema_evol:code_gen:datatypes_rename_member:write
  schema_evol:code_gen:datatypes_rename_member:read
  schema_evol:code_gen:implicit_floating_point_change:write
  schema_evol:code_gen:implicit_floating_point_change:read
)

set(failing_with_thread_sanitizer
  # Technically most of the write tests here succeed, but there is no point in
  # only running them, since they are practically only the setup step for the
  # actual read tests
  schema_evol:code_gen:components_new_member:write_rntuple
  schema_evol:code_gen:components_new_member:read_rntuple
  schema_evol:code_gen:datatypes_new_member:write_rntuple
  schema_evol:code_gen:datatypes_new_member:read_rntuple
  schema_evol:code_gen:implicit_floating_point_change:write_rntuple
  schema_evol:code_gen:implicit_floating_point_change:read_rntuple
)

# This will only apply for clang based builds and is currently the superset of
# the two above
set(failing_with_undefined_sanitizer
  ${failing_with_address_sanitizer}
  ${failing_with_thread_sanitizer}
)

if ((NOT "@FORCE_RUN_ALL_TESTS@" STREQUAL "ON") AND (NOT "@USE_SANITIZER@" STREQUAL ""))
  set(CTEST_CUSTOM_TESTS_IGNORE
    ${CTEST_CUSTOM_TESTS_IGNORE}

    read_and_write_associated
    check_benchmark_outputs
    read_frame_legacy_root
    read_frame_root_multiple
    write_python_frame_root
    read_python_frame_root
    read_and_write_frame_root

    param_reading_rdataframe

    write_frame_root
    read_frame_root
    read_glob
    read_python_multiple
    selected_colls_roundtrip_root

    write_interface_root
    read_interface_root

    write_python_frame_sio
    read_python_frame_sio

    write_python_frame_rntuple
    read_python_frame_rntuple

    relation_range

    pyunittest
    test_strace

    podio-dump-root
    podio-dump-detailed-root
    podio-dump-legacy_root_v00-16-06
    podio-dump-legacy_root-detailed_v00-16-06

    podio-dump-legacy_sio_v00-16-06
    podio-dump-legacy_sio-detailed_v00-16-06

    datamodel_def_store_roundtrip_root
    datamodel_def_store_roundtrip_root_extension

    read_with_rdatasource_root
    read_python_with_rdatasource_root
  )

  foreach(version in @root_legacy_test_versions@)
      list(APPEND CTEST_CUSTOM_TESTS_IGNORE read_frame_root_${version})
      list(APPEND CTEST_CUSTOM_TESTS_IGNORE read_frame_legacy_root_${version})
  endforeach()

  foreach(version in @sio_legacy_test_versions@)
      list(APPEND CTEST_CUSTOM_TESTS_IGNORE read_frame_sio_${version})
      list(APPEND CTEST_CUSTOM_TESTS_IGNORE read_frame_legacy_sio_${version})
  endforeach()

  # ostream_operator is working with Memory sanitizer (at least locally)
  if("@USE_SANITIZER@" MATCHES "Memory(WithOrigin)?")
    list(REMOVE_ITEM CTEST_CUSTOM_TESTS_IGNORE ostream_operator)
  endif()

  # There is still an (indirect) leak somewhere in the collection buffer
  # machinery before they hit the collections.
  # See also https://github.com/AIDASoft/podio/issues/506
  if("@USE_SANITIZER@" MATCHES "Address")
    set(CTEST_CUSTOM_TESTS_IGNORE
      ${CTEST_CUSTOM_TESTS_IGNORE}

      read_frame_legacy_sio
      read_and_write_frame_sio

      ${failing_with_address_sanitizer}
  )
  endif()

  if("@USE_SANITIZER@" MATCHES "Thread")
    set(CTEST_CUSTOM_TESTS_IGNORE
      ${CTEST_CUSTOM_TESTS_IGNORE}

      read_rntuple
      read_interface_rntuple
      read_interface_default_rntuple
      selected_colls_roundtrip_rntuple

      podio-dump-rntuple
      podio-dump-detailed-rntuple

      datamodel_def_store_roundtrip_rntuple
      datamodel_def_store_roundtrip_rntuple_extension

      ${failing_with_thread_sanitizer}
    )
  endif()

  if("@USE_SANITIZER@" MATCHES "Undefined" AND "@CMAKE_CXX_COMPILER_ID@" STREQUAL "Clang")
    set(CTEST_CUSTOM_TESTS_IGNORE
      ${CTEST_CUSTOM_TESTS_IGNORE}

      write_rntuple
      read_rntuple
      write_interface_rntuple
      write_interface_default_rntuple
      read_interface_rntuple
      read_interface_default_rntuple
      selected_colls_roundtrip_rntuple

      podio-dump-rntuple
      podio-dump-detailed-rntuple

      datamodel_def_store_roundtrip_rntuple
      datamodel_def_store_roundtrip_rntuple_extension

      ${failing_with_undefined_sanitizer}
    )
  endif()

endif()

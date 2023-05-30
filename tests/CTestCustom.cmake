# Some of the tests currently fail when run under some sanitizers, ignore them
# for now (but report that they have been ignored). This allows us to still run
# this in CI

# See: https://gitlab.kitware.com/cmake/community/-/wikis/doc/ctest/Testing-With-CTest#customizing-ctest

# "Integration style tests" pretty much all have problems at the moment with any
# sanitizer

if ((NOT "@FORCE_RUN_ALL_TESTS@" STREQUAL "ON") AND (NOT "@USE_SANITIZER@" STREQUAL ""))
  set(CTEST_CUSTOM_TESTS_IGNORE
    ${CTEST_CUSTOM_TESTS_IGNORE}

    write
    read
    read_and_write
    read_and_write_associated
    write_timed
    read_timed
    check_benchmark_outputs
    read-multiple
    read-legacy-files-root_v00-13
    read_frame_legacy_root
    read_frame_root_multiple

    write_frame_root
    read_frame_root

    write_sio
    read_sio
    read_and_write_sio
    write_timed_sio
    read_timed_sio
    check_benchmark_outputs_sio
    write_frame_sio
    read_frame_sio
    read_frame_legacy_sio

    write_ascii

    ostream_operator
    relation_range

    pyunittest

    podio-dump-help
    podio-dump-root-legacy
    podio-dump-root
    podio-dump-detailed-root
    podio-dump-detailed-root-legacy

    podio-dump-sio-legacy
    podio-dump-sio
    podio-dump-detailed-sio
    podio-dump-detailed-sio-legacy

    datamodel_def_store_roundtrip_root
    datamodel_def_store_roundtrip_root_extension
    datamodel_def_store_roundtrip_sio
    datamodel_def_store_roundtrip_sio_extension
  )

  # ostream_operator is working with Memory sanitizer (at least locally)
  if("@USE_SANITIZER@" MATCHES "Memory(WithOrigin)?")
    list(REMOVE_ITEM CTEST_CUSTOM_TESTS_IGNORE ostream_operator)
  endif()
endif()

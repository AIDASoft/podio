option(PODIO_ENABLE_SCHEMA_EVOLUTION_TESTS "Enable the building and running of schema evolution related tests" ON)

add_subdirectory(detection)
if (PODIO_ENABLE_SCHEMA_EVOLUTION_TESTS)
  add_subdirectory(code_gen)
endif()

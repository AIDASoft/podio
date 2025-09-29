# Most sanitizers don't really like to run with limited virtual memory
option(PODIO_NO_MEMLIMIT_SCHEMA_EVOL_TESTS "Run the schema evolution tests without memory limit" OFF)

include(test_utilities.cmake)

ADD_SCHEMA_EVOLUTION_TEST(components_new_member)
ADD_SCHEMA_EVOLUTION_TEST(datatypes_new_member)
ADD_SCHEMA_EVOLUTION_TEST(implicit_floating_point_change)
ADD_SCHEMA_EVOLUTION_TEST(components_rename_member WITH_EVOLUTION)
ADD_SCHEMA_EVOLUTION_TEST(datatypes_rename_member WITH_EVOLUTION)

# RNTuple support for schema evolution capabilities is version dependent
if(ENABLE_RNTUPLE AND ROOT_VERSION VERSION_GREATER_EQUAL 6.34)
  ADD_SCHEMA_EVOLUTION_TEST(implicit_floating_point_change RNTUPLE NO_GENERATE_MODELS)
endif()

if(ENABLE_RNTUPLE AND ROOT_VERSION VERSION_GREATER_EQUAL 6.36)
  ADD_SCHEMA_EVOLUTION_TEST(datatypes_new_member RNTUPLE NO_GENERATE_MODELS)
  ADD_SCHEMA_EVOLUTION_TEST(components_new_member RNTUPLE NO_GENERATE_MODELS)
endif()

# These do not yet work.
# TODO: Investigate why
# if(ENABLE_RNTUPLE AND ROOT_VERSION VERSION_GREATER 6.36)
#   ADD_SCHEMA_EVOLUTION_TEST(datatypes_rename_member WITH_EVOLUTION RNTUPLE NO_GENERATE_MODELS)
#   ADD_SCHEMA_EVOLUTION_TEST(components_rename_member WITH_EVOLUTION RNTUPLE NO_GENERATE_MODELS)
# endif()


# The following are tests that check whether trying to read files that have a
# **newer** version of the datamodel than is available from the environment.
# First we have to write the file with a new version. We will simply use one of
# the check files of the schema evolution checks above
add_executable(write_file_newer_model components_new_member/check.cpp)
target_link_libraries(write_file_newer_model PRIVATE
  components_new_member_newModel
  podio::podioIO
)
target_include_directories(write_file_newer_model PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)
target_compile_definitions(write_file_newer_model PRIVATE
  PODIO_SCHEMA_EVOLUTION_TEST_WRITE
  TEST_CASE="write_file_newer_model"
)
# Write a file with a newer version
add_test(NAME write_file_newer_model COMMAND write_file_newer_model)
set_property(TEST write_file_newer_model
  PROPERTY
  ENVIRONMENT
    ROOT_LIBRARY_PATH=${CMAKE_CURRENT_BINARY_DIR}/components_new_member/new_model
    LD_LIBRARY_PATH=${PROJECT_BINARY_DIR}/src:$<TARGET_FILE_DIR:ROOT::Tree>:$<$<TARGET_EXISTS:SIO::sio>:$<TARGET_FILE_DIR:SIO::sio>>:$ENV{LD_LIBRARY_PATH}

)
set_tests_properties(write_file_newer_model
  PROPERTIES
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/components_new_member
  FIXTURES_SETUP write_file_newer_model_setup
)
# Try to read back this file with an older version
add_test(NAME read_file_older_env_edm_version COMMAND podio-dump-tool write_file_newer_model.root)
set_property(TEST read_file_older_env_edm_version
  PROPERTY
  ENVIRONMENT
    ROOT_LIBRARY_PATH=${CMAKE_CURRENT_BINARY_DIR}/components_new_member/old_model
    LD_LIBRARY_PATH=${PROJECT_BINARY_DIR}/src:$<TARGET_FILE_DIR:ROOT::Tree>:$<$<TARGET_EXISTS:SIO::sio>:$<TARGET_FILE_DIR:SIO::sio>>:$ENV{LD_LIBRARY_PATH}
)
set_tests_properties(read_file_older_env_edm_version
  PROPERTIES
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/components_new_member
  FIXTURES_REQUIRED write_file_newer_model_setup
  PASS_REGULAR_EXPRESSION "WARNING: EDM 'datamodel' exists in file with schema version 2 but podio loaded schema version 1 from the environment"
)

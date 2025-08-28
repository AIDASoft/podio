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

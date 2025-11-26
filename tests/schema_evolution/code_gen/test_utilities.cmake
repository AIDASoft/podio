#--- GENERATE_DATAMODEL(test_case model_version [WITH_EVOLUTION] [NO_EVOLUTION_CHECKS] [OLD_VERSIONS version1 version2 ...])
#
# Arguments:
#   test_case           The name of the test case
#   model_version       which version of the model to generate (old or new)
#   WITH_EVOLUTION      (Optional) pass an evolution.yaml file to the generation of the model
#   NO_EVOLUTION_CHECKS (Optional) skip passing OLD_DESCRIPTION to PODIO_GENERATE_DATAMODEL
#   OLD_VERSIONS   (Optional) list of old model versions to pass to OLD_DESCRIPTION
#
# Generate the necessary code and build all required libraries for the specified
# datamodel and version. Make sure to put all generated and compiled binary
# outputs into a distinct subfolder such that at (test) runtime the models can
# be individually "toggled"
function(GENERATE_DATAMODEL test_case model_version)
  cmake_parse_arguments(PARSED_ARGS "WITH_EVOLUTION;NO_EVOLUTION_CHECKS" "" "OLD_VERSIONS" ${ARGN})
  set(model_base ${test_case}_${model_version}Model)
  set(output_base ${CMAKE_CURRENT_BINARY_DIR}/${test_case}/${model_version}_model)

  # Build the list of old descriptions
  set(old_descriptions)
  if(PARSED_ARGS_OLD_VERSIONS)
    foreach(old_version ${PARSED_ARGS_OLD_VERSIONS})
      list(APPEND old_descriptions ${test_case}/${old_version}.yaml)
    endforeach()
  endif()

  # Validate that WITH_EVOLUTION requires old descriptions
  if(PARSED_ARGS_WITH_EVOLUTION AND NOT old_descriptions)
    message(FATAL_ERROR "WITH_EVOLUTION requires OLD_VERSIONS to be specified")
  endif()

  # Generate the datamodel with appropriate options
  if(PARSED_ARGS_WITH_EVOLUTION)
    PODIO_GENERATE_DATAMODEL(datamodel ${test_case}/${model_version}.yaml headers sources
      IO_BACKEND_HANDLERS ${PODIO_IO_HANDLERS}
      OUTPUT_FOLDER ${output_base}
      OLD_DESCRIPTIONS ${old_descriptions}
      SCHEMA_EVOLUTION ${test_case}/evolution.yaml
    )
  else()
    if(old_descriptions AND NOT PARSED_ARGS_NO_EVOLUTION_CHECKS)
      PODIO_GENERATE_DATAMODEL(datamodel ${test_case}/${model_version}.yaml headers sources
        IO_BACKEND_HANDLERS ${PODIO_IO_HANDLERS}
        OUTPUT_FOLDER ${output_base}
        OLD_DESCRIPTIONS ${old_descriptions}
    )
    else()
      PODIO_GENERATE_DATAMODEL(datamodel ${test_case}/${model_version}.yaml headers sources
        IO_BACKEND_HANDLERS ${PODIO_IO_HANDLERS}
        OUTPUT_FOLDER ${output_base}
      )
    endif()
  endif()

  PODIO_ADD_DATAMODEL_CORE_LIB(${model_base} "${headers}" "${sources}"
    OUTPUT_FOLDER ${output_base}
  )
  PODIO_ADD_ROOT_IO_DICT(${model_base}Dict ${model_base} "${headers}" ${output_base}/src/selection.xml
    OUTPUT_FOLDER ${output_base}
  )

  # Make sure that each model can be "toggled" at runtime separately.
  # Effectively amounts to moving the byproducts of the dictgen above to the
  # appropriate place and making sure libraries are built into the right output
  # directory
  set_target_properties(${model_base} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${output_base})
  set_target_properties(${model_base}Dict PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${output_base})
  add_custom_command(TARGET ${model_base}Dict
    POST_BUILD
    BYPRODUCTS
      ${output_base}/lib${model_base}Dict_rdict.pcm
      ${output_base}/${model_base}DictDict.rootmap

    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_BINARY_DIR}/lib${model_base}Dict_rdict.pcm ${output_base}/lib${model_base}Dict_rdict.pcm
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_BINARY_DIR}/${model_base}DictDict.rootmap ${output_base}/${model_base}DictDict.rootmap

    COMMENT "Moving generated rootmaps for ${test_case}"
    VERBATIM
  )

  # Make sure cmake is aware of how these files came to their final destination
  # so that there is a chance of dependency tracking and we might avoid too
  # frequent triggering of the dictgen step
  add_custom_target(Move_${model_base}Dict_files
    DEPENDS
      ${output_base}/lib${model_base}Dict_rdict.pcm
      ${output_base}/${model_base}DictDict.rootmap
      ${model_base}Dict
  )
  set_target_properties(${model_base}Dict-dictgen PROPERTIES EXCLUDE_FROM_ALL TRUE)
endfunction()

#--- ADD_SCHEMA_EVOLUTION_TEST(test_case [RNTUPLE] [NO_GENERATE_MODELS])
#
# Arguments:
#   test_case           The name of the test case
#   RNTUPLE            (Optional) Use RNTuple backend for testing
#   NO_GENERATE_MODELS (Optional) Skip generation of datamodels
#   WITH_EVOLUTION     (Optional) Mark this evolution as one that needs intervention
#
# Add all the bits and pieces that are necessary to test a certain schema
# evolution case. This includes
# - The generation of the old and new datamodels (unless NO_GENERATE_MODELS is specified)
# - The compilation of the executables to write data in the old format and read
#   them back in the new format
#
# See the README for more details on which parts need to be implemented for
# adding a new test case.
function(ADD_SCHEMA_EVOLUTION_TEST test_case)
  cmake_parse_arguments(PARSED_ARGS "RNTUPLE;NO_GENERATE_MODELS;WITH_EVOLUTION;NO_EVOLUTION_CHECKS" "" "OLD_MODELS" ${ARGN})

  # Default to a single old version unless we get an argument
  set(old_versions "old")
  if(PARSED_ARGS_OLD_MODELS)
    set(old_versions ${PARSED_ARGS_OLD_MODELS})
  endif()

  # Generate datamodels
  if(NOT PARSED_ARGS_NO_GENERATE_MODELS)
    # Generate old model(s)
    foreach(old_version ${old_versions})
      GENERATE_DATAMODEL(${test_case} ${old_version})
    endforeach()

    # Generate new model with the knowledge of all old versions
    if(PARSED_ARGS_WITH_EVOLUTION)
      if(PARSED_ARGS_NO_EVOLUTION_CHECKS)
        GENERATE_DATAMODEL(${test_case} new WITH_EVOLUTION NO_EVOLUTION_CHECKS OLD_VERSIONS ${old_versions})
      else()
        GENERATE_DATAMODEL(${test_case} new WITH_EVOLUTION OLD_VERSIONS ${old_versions})
      endif()
    else()
      if(PARSED_ARGS_NO_EVOLUTION_CHECKS)
        GENERATE_DATAMODEL(${test_case} new NO_EVOLUTION_CHECKS OLD_VERSIONS ${old_versions})
      else()
        GENERATE_DATAMODEL(${test_case} new OLD_VERSIONS ${old_versions})
      endif()
    endif()
  endif()

  set(test_base ${test_case})
  set(suffix "")
  if(PARSED_ARGS_RNTUPLE)
    set(test_base ${test_case}_rntuple)
    set(suffix "_rntuple")
  endif()

  # Executable and test for writing old data
  # Create write executables for each old model version
  set(required_fixtures)
  foreach(old_version ${old_versions})
    # Define datamodel_version based on old_version
    if(old_version STREQUAL "oldest")
      set(datamodel_version 1)
    elseif(old_version STREQUAL "old")
      set(datamodel_version 2)
    else()
      message(WARNING "Using a datamodel version (name) that is not mapped to an integer value, things are likely to break at compile time")
      unset(datamodel_version)
    endif()

    # Executable for writing with this specific old version
    add_executable(write_${test_base}_${old_version} ${test_case}/check.cpp)
    target_link_libraries(write_${test_base}_${old_version} PRIVATE ${test_case}_${old_version}Model podio::podioIO)
    target_compile_definitions(write_${test_base}_${old_version} PRIVATE
      PODIO_SCHEMA_EVOLUTION_TEST_WRITE
      TEST_CASE="${test_case}"
      DATAMODEL_VERSION=${datamodel_version}
    )
    if(PARSED_ARGS_RNTUPLE)
      target_compile_definitions(write_${test_base}_${old_version} PRIVATE PODIO_SCHEMA_EVOLUTION_RNTUPLE)
    endif()
    target_include_directories(write_${test_base}_${old_version} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

    add_test(NAME schema_evol:code_gen:${test_case}:write_${old_version}${suffix} COMMAND write_${test_base}_${old_version})
    set_property(TEST schema_evol:code_gen:${test_case}:write_${old_version}${suffix}
      PROPERTY ENVIRONMENT
        ROOT_LIBRARY_PATH=${CMAKE_CURRENT_BINARY_DIR}/${test_case}/${old_version}_model
        LD_LIBRARY_PATH=${PROJECT_BINARY_DIR}/src:$<TARGET_FILE_DIR:ROOT::Tree>:$<$<TARGET_EXISTS:SIO::sio>:$<TARGET_FILE_DIR:SIO::sio>>:$ENV{LD_LIBRARY_PATH}
    )
    set_tests_properties(schema_evol:code_gen:${test_case}:write_${old_version}${suffix}
      PROPERTIES
        FIXTURES_SETUP ${test_case}_w_${old_version}${suffix}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${test_case}
      )
    list(APPEND required_fixtures ${test_case}_w_${old_version}${suffix})
  endforeach()

  # Executable and test for reading new data
  add_executable(read_${test_base} ${test_case}/check.cpp)
  target_link_libraries(read_${test_base} PRIVATE ${test_case}_newModel podio::podioIO)
  target_compile_definitions(read_${test_base} PRIVATE PODIO_SCHEMA_EVOLUTION_TEST_READ TEST_CASE="${test_case}")
  if(PARSED_ARGS_RNTUPLE)
    target_compile_definitions(read_${test_base} PRIVATE PODIO_SCHEMA_EVOLUTION_RNTUPLE)
  endif()

  target_include_directories(read_${test_base} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

  set(test_command "${CMAKE_CURRENT_BINARY_DIR}/read_${test_base}")

  add_test(NAME schema_evol:code_gen:${test_case}:read${suffix}
        COMMAND bash -c "${test_command}")
  set_property(TEST schema_evol:code_gen:${test_case}:read${suffix}
    PROPERTY ENVIRONMENT
      ROOT_LIBRARY_PATH=${CMAKE_CURRENT_BINARY_DIR}/${test_case}/new_model
      LD_LIBRARY_PATH=${PROJECT_BINARY_DIR}/src:$<TARGET_FILE_DIR:ROOT::Tree>:$<$<TARGET_EXISTS:SIO::sio>:$<TARGET_FILE_DIR:SIO::sio>>:$ENV{LD_LIBRARY_PATH}
  )
  set_tests_properties(schema_evol:code_gen:${test_case}:read${suffix}
    PROPERTIES
      FIXTURES_REQUIRED "${required_fixtures}"
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${test_case}
    )
endfunction()

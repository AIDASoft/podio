#--- GENERATE_DATAMODEL(test_case model_version [WITH_EVOLUTION])
#
# Arguments:
#   test_case      The name of the test case
#   model_version  which version of the model to generate (old or new)
#   WITH_EVOLUTION (Optional) pass an evolution.yaml file to the generation of the model
#
# Generate the necessary code and build all required libraries for the specified
# datamodel and version. Make sure to put all generated and compiled binary
# outputs into a distinct subfolder such that at (test) runtime the models can
# be individually "toggled"
function(GENERATE_DATAMODEL test_case model_version)
  cmake_parse_arguments(PARSED_ARGS "WITH_EVOLUTION" "" "" ${ARGN})
  set(model_base ${test_case}_${model_version}Model)
  set(output_base ${CMAKE_CURRENT_BINARY_DIR}/${test_case}/${model_version}_model)

  if(PARSED_ARGS_WITH_EVOLUTION)
    PODIO_GENERATE_DATAMODEL(datamodel ${test_case}/${model_version}.yaml headers sources
      IO_BACKEND_HANDLERS ${PODIO_IO_HANDLERS}
      OUTPUT_FOLDER ${output_base}
      OLD_DESCRIPTION ${test_case}/old.yaml
      SCHEMA_EVOLUTION ${test_case}/evolution.yaml
    )
  else()
    PODIO_GENERATE_DATAMODEL(datamodel ${test_case}/${model_version}.yaml headers sources
      IO_BACKEND_HANDLERS ${PODIO_IO_HANDLERS}
      OUTPUT_FOLDER ${output_base}
      OLD_DESCRIPTION ${test_case}/old.yaml
    )
  endif()
  PODIO_ADD_DATAMODEL_CORE_LIB(${model_base} "${headers}" "${sources}"
    OUTPUT_FOLDER ${output_base}
  )
  PODIO_ADD_ROOT_IO_DICT(${model_base}Dict ${model_base} "${headers}" ${output_base}/src/selection.xml
    OUTPUT_FOLDER ${output_base}
  )

  # Make sure that each model can be "toggled" at runtime separately.
  # Effecitvely amounts to moving the byproducts of the dictgen above to the
  # appropriate place and making sure libraries are built into the right output
  # directory
  set_target_properties(${model_base} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${output_base})
  set_target_properties(${model_base}Dict PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${output_base})
  add_custom_command(TARGET ${model_base}Dict
    POST_BUILD
    BYPRODUCTS
      ${output_base}/lib${model_base}Dict_rdict.pcm
      ${output_base}/${model_base}DictDict.rootmap

    COMMAND ${CMAKE_COMMAND} -E rename ${CMAKE_CURRENT_BINARY_DIR}/lib${model_base}Dict_rdict.pcm ${output_base}/lib${model_base}Dict_rdict.pcm
    COMMAND ${CMAKE_COMMAND} -E rename ${CMAKE_CURRENT_BINARY_DIR}/${model_base}DictDict.rootmap ${output_base}/${model_base}DictDict.rootmap

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
  cmake_parse_arguments(PARSED_ARGS "RNTUPLE;NO_GENERATE_MODELS;WITH_EVOLUTION" "" "" ${ARGN})
  # Generate datamodels
  if(NOT PARSED_ARGS_NO_GENERATE_MODELS)
    if(PARSED_ARGS_WITH_EVOLUTION)
      GENERATE_DATAMODEL(${test_case} old)
      GENERATE_DATAMODEL(${test_case} new WITH_EVOLUTION)
    else()
      GENERATE_DATAMODEL(${test_case} old)
      GENERATE_DATAMODEL(${test_case} new)
    endif()
  endif()

  set(test_base ${test_case})
  set(suffix "")
  if(PARSED_ARGS_RNTUPLE)
    set(test_base ${test_case}_rntuple)
    set(suffix "_rntuple")
  endif()
  # Executable and test for writing old data
  add_executable(write_${test_base} ${test_case}/check.cpp)
  target_link_libraries(write_${test_base} PRIVATE ${test_case}_oldModel podio::podioIO)
  target_compile_definitions(write_${test_base} PRIVATE PODIO_SCHEMA_EVOLUTION_TEST_WRITE TEST_CASE="${test_case}")
  if(PARSED_ARGS_RNTUPLE)
    target_compile_definitions(write_${test_base} PRIVATE PODIO_SCHEMA_EVOLUTION_RNTUPLE)
  endif()
  target_include_directories(write_${test_base} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

  add_test(NAME schema_evol:code_gen:${test_case}:write${suffix} COMMAND write_${test_base})
  set_property(TEST schema_evol:code_gen:${test_case}:write${suffix}
    PROPERTY ENVIRONMENT
      LD_LIBRARY_PATH=${PROJECT_BINARY_DIR}/src:${CMAKE_CURRENT_BINARY_DIR}/${test_case}/old_model:$<TARGET_FILE_DIR:ROOT::Tree>:$<$<TARGET_EXISTS:SIO::sio>:$<TARGET_FILE_DIR:SIO::sio>>:$ENV{LD_LIBRARY_PATH}
      PODIO_SIO_BLOCK=${CMAKE_CURRENT_BINARY_DIR}/${test_case}/old_model
  )
  set_tests_properties(schema_evol:code_gen:${test_case}:write${suffix} PROPERTIES FIXTURES_SETUP ${test_case}_w${suffix})

  # Executable and test for reading new data
  add_executable(read_${test_base} ${test_case}/check.cpp)
  target_link_libraries(read_${test_base} PRIVATE ${test_case}_newModel podio::podioIO)
  target_compile_definitions(read_${test_base} PRIVATE PODIO_SCHEMA_EVOLUTION_TEST_READ TEST_CASE="${test_case}")
  if(PARSED_ARGS_RNTUPLE)
    target_compile_definitions(read_${test_base} PRIVATE PODIO_SCHEMA_EVOLUTION_RNTUPLE)
  endif()

  target_include_directories(read_${test_base} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

  add_test(NAME schema_evol:code_gen:${test_case}:read${suffix} COMMAND read_${test_base})
  set_property(TEST schema_evol:code_gen:${test_case}:read${suffix}
    PROPERTY ENVIRONMENT
      LD_LIBRARY_PATH=${PROJECT_BINARY_DIR}/src:${CMAKE_CURRENT_BINARY_DIR}/${test_case}/new_model:$<TARGET_FILE_DIR:ROOT::Tree>:$<$<TARGET_EXISTS:SIO::sio>:$<TARGET_FILE_DIR:SIO::sio>>:$ENV{LD_LIBRARY_PATH}
      PODIO_SIO_BLOCK=${CMAKE_CURRENT_BINARY_DIR}/${test_case}/new_model
  )
  set_tests_properties(schema_evol:code_gen:${test_case}:read${suffix} PROPERTIES FIXTURES_REQUIRED ${test_case}_w${suffix})
endfunction()

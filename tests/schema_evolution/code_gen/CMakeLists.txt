PODIO_GENERATE_DATAMODEL(datamodel components_new_member/old.yaml old_headers old_sources
  IO_BACKEND_HANDLERS ${PODIO_IO_HANDLERS}
  OUTPUT_FOLDER ${CMAKE_CURRENT_BINARY_DIR}/components_new_member/old_model
)
PODIO_ADD_DATAMODEL_CORE_LIB(components_new_member_oldModel "${old_headers}" "${old_sources}"
  OUTPUT_FOLDER ${CMAKE_CURRENT_BINARY_DIR}/components_new_member/old_model
)
PODIO_ADD_ROOT_IO_DICT(components_new_member_oldModelDict components_new_member_oldModel "${old_headers}" ${CMAKE_CURRENT_BINARY_DIR}/components_new_member/old_model/src/selection.xml
  OUTPUT_FOLDER ${CMAKE_CURRENT_BINARY_DIR}/components_new_member/old_model
)

# Make sure that we have separable environments for the tests
set_target_properties(components_new_member_oldModel PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/components_new_member/old_model/)
set_target_properties(components_new_member_oldModelDict PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/components_new_member/old_model/)
add_custom_command(TARGET components_new_member_oldModelDict-dictgen
  POST_BUILD
  BYPRODUCTS
    ${CMAKE_CURRENT_BINARY_DIR}/components_new_member/old_model/libcomponents_new_member_oldModelDict_rdict.pcm
    ${CMAKE_CURRENT_BINARY_DIR}/components_new_member/old_model/components_new_member_oldModelDictDict.rootmap

  COMMAND ${CMAKE_COMMAND} -E rename ${CMAKE_CURRENT_BINARY_DIR}/libcomponents_new_member_oldModelDict_rdict.pcm ${CMAKE_CURRENT_BINARY_DIR}/components_new_member/old_model/libcomponents_new_member_oldModelDict_rdict.pcm
  COMMAND ${CMAKE_COMMAND} -E rename ${CMAKE_CURRENT_BINARY_DIR}/components_new_member_oldModelDictDict.rootmap ${CMAKE_CURRENT_BINARY_DIR}/components_new_member/old_model/components_new_member_oldModelDictDict.rootmap

  COMMENT "Moving generated rootmaps to appropriate place"
  VERBATIM
)

add_executable(write_components_new_member components_new_member/check.cpp)
target_link_libraries(write_components_new_member PRIVATE components_new_member_oldModel podio::podioIO)
target_compile_definitions(write_components_new_member PRIVATE PODIO_SCHEMA_EVOLUTION_TEST_WRITE)
add_test(NAME schema_evol:code_gen:components_new_member:write COMMAND write_components_new_member)
set_property(TEST schema_evol:code_gen:components_new_member:write
  PROPERTY ENVIRONMENT
  LD_LIBRARY_PATH=${PROJECT_BINARY_DIR}/src:${CMAKE_CURRENT_BINARY_DIR}/components_new_member/old_model:$<TARGET_FILE_DIR:ROOT::Tree>:$<$<TARGET_EXISTS:SIO::sio>:$<TARGET_FILE_DIR:SIO::sio>>:$ENV{LD_LIBRARY_PATH}
  PODIO_SIO_BLOCK=${CMAKE_CURRENT_BINARY_DIR}/components_new_member/old_model
)


PODIO_GENERATE_DATAMODEL(datamodel components_new_member/new.yaml new_headers new_sources
  IO_BACKEND_HANDLERS ${PODIO_IO_HANDLERS}
  OUTPUT_FOLDER ${CMAKE_CURRENT_BINARY_DIR}/components_new_member/new_model
  OLD_DESCRIPTION components_new_member/old.yaml
)
PODIO_ADD_DATAMODEL_CORE_LIB(components_new_member_newModel "${new_headers}" "${new_sources}"
  OUTPUT_FOLDER ${CMAKE_CURRENT_BINARY_DIR}/components_new_member/new_model
)
PODIO_ADD_ROOT_IO_DICT(components_new_member_newModelDict components_new_member_newModel "${new_headers}" ${CMAKE_CURRENT_BINARY_DIR}/components_new_member/new_model/src/selection.xml
  OUTPUT_FOLDER ${CMAKE_CURRENT_BINARY_DIR}/components_new_member/new_model
)

# Make sure we have a separable test environment
set_target_properties(components_new_member_newModel PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/components_new_member/new_model/)
set_target_properties(components_new_member_newModelDict PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/components_new_member/new_model/)
add_custom_command(TARGET components_new_member_oldModelDict-dictgen
  POST_BUILD
  BYPRODUCTS
    ${CMAKE_CURRENT_BINARY_DIR}/components_new_member/new_model/libcomponents_new_member_newModelDict_rdict.pcm
    ${CMAKE_CURRENT_BINARY_DIR}/components_new_member/new_model/components_new_member_newModelDictDict.rootmap

  COMMAND ${CMAKE_COMMAND} -E rename ${CMAKE_CURRENT_BINARY_DIR}/libcomponents_new_member_newModelDict_rdict.pcm ${CMAKE_CURRENT_BINARY_DIR}/components_new_member/new_model/libcomponents_new_member_newModelDict_rdict.pcm
  COMMAND ${CMAKE_COMMAND} -E rename ${CMAKE_CURRENT_BINARY_DIR}/components_new_member_newModelDictDict.rootmap ${CMAKE_CURRENT_BINARY_DIR}/components_new_member/new_model/components_new_member_newModelDictDict.rootmap

  COMMENT "Moving generated rootmaps to appropriate place"
  VERBATIM
)

add_custom_target(components_new_member_rootmapTarget ALL DEPENDS
  ${CMAKE_CURRENT_BINARY_DIR}/components_new_member/old_model/libcomponents_new_member_oldModelDict_rdict.pcm
  ${CMAKE_CURRENT_BINARY_DIR}/components_new_member/old_model/components_new_member_oldModelDictDict.rootmap
  ${CMAKE_CURRENT_BINARY_DIR}/components_new_member/new_model/libcomponents_new_member_newModelDict_rdict.pcm
  ${CMAKE_CURRENT_BINARY_DIR}/components_new_member/new_model/components_new_member_newModelDictDict.rootmap
)

add_executable(read_components_new_member components_new_member/check.cpp)
target_link_libraries(read_components_new_member PRIVATE components_new_member_newModel podio::podioIO)
target_compile_definitions(read_components_new_member PRIVATE PODIO_SCHEMA_EVOLUTION_TEST_READ)
add_test(NAME schema_evol:code_gen:components_new_member:read COMMAND read_components_new_member)

set_property(TEST schema_evol:code_gen:components_new_member:read
  PROPERTY ENVIRONMENT
  LD_LIBRARY_PATH=${PROJECT_BINARY_DIR}/src:${CMAKE_CURRENT_BINARY_DIR}/components_new_member/new_model:$<TARGET_FILE_DIR:ROOT::Tree>:$<$<TARGET_EXISTS:SIO::sio>:$<TARGET_FILE_DIR:SIO::sio>>:$ENV{LD_LIBRARY_PATH}
  PODIO_SIO_BLOCK=${CMAKE_CURRENT_BINARY_DIR}/components_new_member/new_model
)
set_property(TEST schema_evol:code_gen:components_new_member:read
  PROPERTY DEPENDS schema_evol:code_gen:components_new_member:write
)

#---------------------------------------------------------------------------------------------------
#---PODIO_GENERATE_DICTIONARY( dictionary headerfiles SELECTION selectionfile OPTIONS opt1 opt2 ...
#                               DEPENDS dependency1 dependency2 ...
#                             )
# if dictionary is a TARGET (e.g., created with add_library), we inherit the INCLUDE_DIRECTORES and
# COMPILE_DEFINITIONS properties
#
# This is a copy from RELFEX_GENERATE_DICTIONARY from the RootMacros in Root v6.22
# Copied here to allow creating dictionaries based on targer properties
#---------------------------------------------------------------------------------------------------
function(PODIO_GENERATE_DICTIONARY dictionary)
  CMAKE_PARSE_ARGUMENTS(ARG "" "SELECTION" "OPTIONS;DEPENDS" ${ARGN})
  #---Get List of header files---------------
  set(headerfiles)
  foreach(fp ${ARG_UNPARSED_ARGUMENTS})
    file(GLOB files inc/${fp})
    if(files)
      foreach(f ${files})
        if(NOT f MATCHES LinkDef)
          set(headerfiles ${headerfiles} ${f})
        endif()
      endforeach()
    elseif(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${fp})
      set(headerfiles ${headerfiles} ${CMAKE_CURRENT_SOURCE_DIR}/${fp})
    else()
      set(headerfiles ${headerfiles} ${fp})
    endif()
  endforeach()
  #---Get Selection file------------------------------------
  if(IS_ABSOLUTE ${ARG_SELECTION})
    set(selectionfile ${ARG_SELECTION})
  else()
    set(selectionfile ${CMAKE_CURRENT_SOURCE_DIR}/${ARG_SELECTION})
  endif()

  set(gensrcdict ${dictionary}.cxx)

  #---roottest compability---------------------------------
  if(CMAKE_ROOTTEST_NOROOTMAP)
    set(rootmapname )
    set(rootmapopts )
  elseif(DEFINED CMAKE_ROOTTEST_NOROOTMAP)  # Follow the roottest dictionary library naming
    set(rootmapname ${dictionary}.rootmap)
    set(rootmapopts --rootmap=${rootmapname} --rootmap-lib=${libprefix}${dictionary}_dictrflx)
  else()
    set(rootmapname ${dictionary}Dict.rootmap)
    set(rootmapopts --rootmap=${rootmapname} --rootmap-lib=${libprefix}${dictionary}Dict)
  endif()

  set(include_dirs ${CMAKE_CURRENT_SOURCE_DIR})
  get_directory_property(incdirs INCLUDE_DIRECTORIES)
  foreach(d ${incdirs})
    if(NOT "${d}" MATCHES "^(AFTER|BEFORE|INTERFACE|PRIVATE|PUBLIC|SYSTEM)$")
      list(APPEND include_dirs ${d})
    endif()
  endforeach()

  get_directory_property(defs COMPILE_DEFINITIONS)
  foreach( d ${defs})
   list(APPEND definitions ${d})
  endforeach()

  IF(TARGET ${dictionary})
    LIST(APPEND include_dirs $<TARGET_PROPERTY:${dictionary},INCLUDE_DIRECTORIES>)
    LIST(APPEND definitions $<TARGET_PROPERTY:${dictionary},COMPILE_DEFINITIONS>)
  ENDIF()

  add_custom_command(
    OUTPUT ${gensrcdict} ${rootmapname}
    COMMAND ${ROOT_genreflex_CMD}
    ARGS ${headerfiles} -o ${gensrcdict} ${rootmapopts} --select=${selectionfile}
         --gccxmlpath=${GCCXML_home}/bin ${ARG_OPTIONS}
         "-I$<JOIN:${include_dirs},;-I>"
         "$<$<BOOL:$<JOIN:${definitions},>>:-D$<JOIN:${definitions},;-D>>"
    DEPENDS ${headerfiles} ${selectionfile} ${ARG_DEPENDS}

    COMMAND_EXPAND_LISTS
    )
  IF(TARGET ${dictionary})
    target_sources(${dictionary} PRIVATE ${gensrcdict})
  ENDIF()
  set(gensrcdict ${dictionary}.cxx PARENT_SCOPE)
  set_source_files_properties(${gensrcdict}
    PROPERTIES
    GENERATED TRUE
    COMPILE_FLAGS "-Wno-overlength-strings"
    )

  #---roottest compability---------------------------------
  if(CMAKE_ROOTTEST_DICT)
    ROOTTEST_TARGETNAME_FROM_FILE(targetname ${dictionary})

    set(targetname "${targetname}-dictgen")

    add_custom_target(${targetname} DEPENDS ${gensrcdict} ${ROOT_LIBRARIES})
  else()
    set(targetname "${dictionary}-dictgen")
    # Creating this target at ALL level enables the possibility to generate dictionaries (genreflex step)
    # well before the dependent libraries of the dictionary are build
    add_custom_target(${targetname} ALL DEPENDS ${gensrcdict})
  endif()

endfunction()


#---------------------------------------------------------------------------------------------------
#---PODIO_GENERATE_DATAMODEL( datamodel YAML_FILE RETURN_HEADERS RETURN_SOURCES
#      OUTPUT_FOLDER      output_directory
#      IO_BACKEND_HANDLERS  io_handlers
#   )
#
#   Arguments:
#      datamodel            Name of the datamodel to be created. a TARGET "create${datamodel}" will be created
#      YAML_FILE            The path to the yaml file describing the datamodel
#      RETURN_HEADERS       variable that will be filled with the list of created headers files: ${datamodel}/*.h
#      RETURN_SOURCES       variable that will be filled with the list of created source files : src/*.cc
#   Parameters:
#      OUTPUT_FOLDER        OPTIONAL: The folder in which the output files should be placed
#                           Default is ${CMAKE_CURRENT_SOURCE_DIR}
#      IO_BACKEND_HANDLERS  OPTIONAL: The I/O backend handlers that should be generated. The list is
#                           passed directly to podio_class_generator.py and validated there
#                           Default is ROOT
#  )
#
# Note that the create_${datamodel} target will always be called, but if the YAML_FILE has not changed
# this is essentially a no-op, and should not cause re-compilation.
#---------------------------------------------------------------------------------------------------
function(PODIO_GENERATE_DATAMODEL datamodel YAML_FILE RETURN_HEADERS RETURN_SOURCES)
  CMAKE_PARSE_ARGUMENTS(ARG "" "OUTPUT_FOLDER" "IO_BACKEND_HANDLERS" ${ARGN})
  IF(NOT ARG_OUTPUT_FOLDER)
    SET(ARG_OUTPUT_FOLDER ${CMAKE_CURRENT_SOURCE_DIR})
  ENDIF()
  IF(NOT ARG_IO_BACKEND_HANDLERS)
    # At least build the ROOT selection.xml by default for now
    SET(ARG_IO_BACKEND_HANDLERS "ROOT")
  ENDIF()

  # Make sure that we re run the generation process everytime either the
  # templates or the yaml file changes.
  include(${podio_PYTHON_DIR}/templates/CMakeLists.txt)
  set_property(
    DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
    ${YAML_FILE}
    ${PODIO_TEMPLATES}
    ${podio_PYTHON_DIR}/podio_class_generator.py
    ${podio_PYTHON_DIR}/generator_utils.py
    ${podio_PYTHON_DIR}/podio_config_reader.py
  )

  # we need to boostrap the data model, so this has to be executed in the cmake run
  execute_process(
    COMMAND ${CMAKE_COMMAND} -E echo "Creating \"${datamodel}\" data model"
    COMMAND python ${podio_PYTHON_DIR}/podio_class_generator.py ${YAML_FILE} ${ARG_OUTPUT_FOLDER} ${datamodel} ${ARG_IO_BACKEND_HANDLERS}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )

  # Get the generated headers and source files
  include(${ARG_OUTPUT_FOLDER}/podio_generated_files.cmake)

  set (${RETURN_HEADERS} ${headers} PARENT_SCOPE)
  set (${RETURN_SOURCES} ${sources} PARENT_SCOPE)

endfunction()


#---------------------------------------------------------------------------------------------------
#---PODIO_ADD_DATAMODEL_CORE_LIB( lib_name HEADERS SOURCES
#      OUTPUT_FOLDER output_directory
#   )
#
# Add the core datamodel library linking only to the core podio::podio library
# without any I/O backend specific dependencies.
#
# Arguments:
#    lib_name             Name of the library
#    HEADERS              The list of all header files created by PODIO_GENERATE_DATAMODEL
#    SOURCES              The list of all source files created by PODIO_GENERATE_DATAMODEL
#
# Parameters:
#    OUTPUT_FOLDER        OPTIONAL: The folder in which the output files have been placed by PODIO_GENERATE_DATAMODEL. Defaults to ${CMAKE_CURRENT_SOURCE_DIR}
#---------------------------------------------------------------------------------------------------
function(PODIO_ADD_DATAMODEL_CORE_LIB lib_name HEADERS SOURCES)
  CMAKE_PARSE_ARGUMENTS(ARG "" "OUTPUT_FOLDER" "" ${ARGN})
  IF(NOT ARG_OUTPUT_FOLDER)
    SET(ARG_OUTPUT_FOLDER ${CMAKE_CURRENT_SOURCE_DIR})
  ENDIF()

  # Filter out anything I/O backend related to build the core library
  LIST(FILTER HEADERS EXCLUDE REGEX .*SIOBlock.h)
  LIST(FILTER SOURCES EXCLUDE REGEX .*SIOBlock.cc)

  add_library(${lib_name} SHARED ${SOURCES} ${HEADERS})
  target_link_libraries(${lib_name} PUBLIC podio::podio)
  target_include_directories(${lib_name} PUBLIC
    $<BUILD_INTERFACE:${ARG_OUTPUT_FOLDER}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )
  set_target_properties(${lib_name} PROPERTIES PUBLIC_HEADER "${HEADERS}")
endfunction()


#---------------------------------------------------------------------------------------------------
#---PODIO_ADD_ROOT_IO_DICT( dict_name CORE_LIB HEADERS SELECTION_XML
#      OUTPUT_FOLDER output_directory
#   )
#
# Conditionally add the ROOT dictionary to the targets if the corresponding
# selection xml has been generated by PODIO_GENERATE_DATAMODEL.
#
# Arguments:
#    dict_name            Name of the dictionary
#    CORE_LIB             The core datamodel library (e.g. from PODIO_ADD_DATAMODEL_CORE_LIB)
#    HEADERS              The list of all header files generated by PODIO_GENERATE_DATAMODEL
#    SELECTION_XML        The selection.xml file genertaed by PODIO_GENERATE_DATAMODEL
#
# Parameters:
#    OUTPUT_FOLDER        OPTIONAL: The folder in which the output files have been placed by PODIO_GENERATE_DATAMODEL. Defaults to ${CMAKE_CURRENT_SOURCE_DIR}
#---------------------------------------------------------------------------------------------------
function(PODIO_ADD_ROOT_IO_DICT dict_name CORE_LIB HEADERS SELECTION_XML)
  CMAKE_PARSE_ARGUMENTS(ARG "" "OUTPUT_FOLDER" "" ${ARGN})
  IF(NOT ARG_OUTPUT_FOLDER)
    SET(ARG_OUTPUT_FOLDER ${CMAKE_CURRENT_SOURCE_DIR})
  ENDIF()

  IF (NOT EXISTS ${ARG_OUTPUT_FOLDER}/${SELECTION_XML})
    MESSAGE(STATUS "Not adding the ROOT dictionary to the targets because the selection.xml has not been generated")
    RETURN()
  ENDIF()

  # Filter out anything I/O backend related from the generated headers as ROOT only needs
  # the core headers
  LIST(FILTER HEADERS EXCLUDE REGEX .*SIOBlock.h)

  add_library(${dict_name} SHARED)
  target_link_libraries(${dict_name} PUBLIC
    ${CORE_LIB}
    podio::podio
    ROOT::Core
    )
  target_include_directories(${dict_name} PUBLIC
    $<BUILD_INTERFACE:${ARG_OUTPUT_FOLDER}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )
  PODIO_GENERATE_DICTIONARY(${dict_name} ${HEADERS} SELECTION ${SELECTION_XML}
    OPTIONS --library ${CMAKE_SHARED_LIBRARY_PREFIX}${dict_name}${CMAKE_SHARED_LIBRARY_SUFFIX}
    )
  set_target_properties(${dict_name}-dictgen PROPERTIES EXCLUDE_FROM_ALL TRUE)
  add_dependencies(${dict_name} ${CORE_LIB})
endfunction()

#---------------------------------------------------------------------------------------------------
#---PODIO_ADD_SIO_IO_BLOCKS( CORE_LIB HEADERS SOURCES
#      OUTPUT_FOLDER output_directory
#      )
#
# Conditionally add the SIOBlocks library to the targets if the corresponding
# SIOBlocks code has been generated by PODIO_GENERATE_DATAMODEL. Since the
# runtime loading of the SIOBlocks library follows a naming convention, the name
# of the library cannot be chosen freely, but is instead determined from the
# name of the core datamodel library.
#
# Arguments:
#    CORE_LIB             The name of the core datamodel library. The name of the SIO Block library target will be ${CORE_LIB}SioBlocks
#    HEADERS              The list of all header files created by PODIO_GENERATE_DATAMODEL
#    SOURCES              The list of all source files created by PODIO_GENERATE_DATAMODEL
#
# Parameters:
#    OUTPUT_FOLDER        OPTIONAL: The folder in which the output files have been placed by PODIO_GENERATE_DATAMODEL. Defaults to ${CMAKE_CURRENT_SOURCE_DIR}
#---------------------------------------------------------------------------------------------------
function(PODIO_ADD_SIO_IO_BLOCKS CORE_LIB HEADERS SOURCES)
  CMAKE_PARSE_ARGUMENTS(ARG "" "OUTPUT_FOLDER" "" ${ARGN})
  IF(NOT ARG_OUTPUT_FOLDER)
    SET(ARG_OUTPUT_FOLDER ${CMAKE_CURRENT_SOURCE_DIR})
  ENDIF()

  # Only get the SIOBlock handlers
  LIST(FILTER HEADERS INCLUDE REGEX .*SIOBlock.h)
  LIST(FILTER SOURCES INCLUDE REGEX .*SIOBlock.cc)

  IF(NOT HEADERS)
    MESSAGE(STATUS "Not adding the SIO Blocks library to the targets because the corresponding c++ sources have not been generated")
    RETURN()
  ENDIF()

  add_library(${CORE_LIB}SioBlocks SHARED ${SOURCES} ${HEADERS})
  target_link_libraries(${CORE_LIB}SioBlocks PUBLIC ${CORE_LIB} podio::podio podio::podioSioIO SIO::sio)
  target_include_directories(${CORE_LIB}SioBlocks PUBLIC
    $<BUILD_INTERFACE:${ARG_OUTPUT_FOLDER}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
endfunction()


#---------------------------------------------------------------------------------------------------
function(PODIO_CHECK_CPP_FS FS_LIBS)
  SET(have_filesystem FALSE)
  MESSAGE(STATUS "Checking for filesystem library support of compiler")
  # GNU implementation prior to 9.1 requires linking with -lstdc++fs and LLVM
  # implementation prior to LLVM 9.0 requires linking with -lc++fs
  # After that it should be built-in
  FOREACH(FS_LIB_NAME "" stdc++fs c++fs)
    # MESSAGE(STATUS "Linking against ${FS_LIB_NAME}")
    try_compile(have_filesystem ${CMAKE_BINARY_DIR}/try ${PROJECT_SOURCE_DIR}/cmake/try_filesystem.cpp
      CXX_STANDARD ${CMAKE_CXX_STANDARD}
      CXX_EXTENSIONS False
      OUTPUT_VARIABLE HAVE_FS_OUTPUT
      LINK_LIBRARIES ${FS_LIB_NAME}
      )
    # MESSAGE(STATUS "-----> " ${HAVE_FS_OUTPUT})
    IF(have_filesystem)
      MESSAGE(STATUS "Compiler supports filesystem when linking against '${FS_LIB_NAME}'")
      SET(${FS_LIBS} ${FS_LIB_NAME} PARENT_SCOPE)
      RETURN()
    ENDIF()
    MESSAGE(STATUS "Compiler not compatible when linking against '${FS_LIB_NAME}'")
  ENDFOREACH()

  MESSAGE(STATUS "Compiler does not have filesystem support, falling back to boost::filesystem")
  find_package(Boost REQUIRED COMPONENTS filesystem system)
  SET(${FS_LIBS} Boost::filesystem Boost::system PARENT_SCOPE)
  SET_TARGET_PROPERTIES(Boost::filesystem
    PROPERTIES
    INTERFACE_COMPILE_DEFINITIONS USE_BOOST_FILESYSTEM
    )
endfunction()

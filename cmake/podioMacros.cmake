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
  # we need to boostrap the data model, so this has to be executed in the cmake run
  execute_process(
    COMMAND ${CMAKE_COMMAND} -E echo "Creating \"${datamodel}\" data model"
    COMMAND python ${podio_PYTHON_DIR}/podio_class_generator.py ${YAML_FILE} ${ARG_OUTPUT_FOLDER} ${datamodel} ${ARG_IO_BACKEND_HANDLERS}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )

  file(GLOB headers ${ARG_OUTPUT_FOLDER}/${datamodel}/*.h)
  file(GLOB sources ${ARG_OUTPUT_FOLDER}/src/*.cc)

  set (${RETURN_HEADERS} ${headers} PARENT_SCOPE)
  set (${RETURN_SOURCES} ${sources} PARENT_SCOPE)

  add_custom_target(create_${datamodel}
    COMMENT "Re-Creating \"${datamodel}\" data model"
    DEPENDS ${YAML_FILE}
    BYPRODUCTS ${sources} ${headers}
    COMMAND python ${podio_PYTHON_DIR}/podio_class_generator.py --quiet ${YAML_FILE} ${ARG_OUTPUT_FOLDER} ${datamodel} ${ARG_IO_BACKEND_HANDLERS}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )

endfunction()

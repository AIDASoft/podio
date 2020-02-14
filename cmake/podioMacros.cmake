#---------------------------------------------------------------------------------------------------
#---PODIO_GENERATE_DICTIONARY( dictionary headerfiles SELECTION selectionfile OPTIONS opt1 opt2 ...
#                              DEPENDS dependency1 dependency2 ...
#                              USES target1 target2 ...
#                            )
#
#  USES takes a list of targets. From the targets the properties INCLUDE_DIRECTORIES
#     and COMPILE_DEFINITIONS are used to create the appropriate -I and -D flags
#
#---------------------------------------------------------------------------------------------------
macro(PODIO_GENERATE_DICTIONARY dictionary)
  CMAKE_PARSE_ARGUMENTS(ARG "" "SELECTION" "OPTIONS;DEPENDS;USES" ${ARGN})
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

  set(definitions)
  get_directory_property(defs COMPILE_DEFINITIONS)
  foreach( d ${defs})
   list(APPEND definitions ${d})
  endforeach()

  foreach(DEP ${ARG_USES})
    LIST(APPEND include_dirs $<TARGET_PROPERTY:${DEP},INCLUDE_DIRECTORIES>)
    LIST(APPEND definitions $<TARGET_PROPERTY:${DEP},COMPILE_DEFINITIONS>)
  endforeach()

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

endmacro()

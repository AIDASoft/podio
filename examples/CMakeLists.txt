include_directories(
	${CMAKE_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/datamodel
#        ${CMAKE_CURRENT_SOURCE_DIR}/utilities
)

file(GLOB sources datamodel/*.cc)
file(GLOB headers datamodel/*.h)
file(GLOB sources_utils utilities/*.cc)
file(GLOB headers_utils utilities/*.h)

list(REMOVE_ITEM headers ${CMAKE_CURRENT_SOURCE_DIR}/datamodel/LinkDef.h )
REFLEX_GENERATE_DICTIONARY(DataModelEx ${headers} SELECTION datamodel/selection.xml)

add_library(DataModelExample SHARED ${sources} ${sources_utils} ${headers} ${headers_utils} DataModelEx)
target_link_libraries(DataModelExample albers ${ROOT_LIBRARIES} ${ROOT_COMPONENT_LIBRARIES})

file(GLOB executables RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

foreach( sourcefile ${executables} )
  string( REPLACE ".cpp" "" name ${sourcefile} )
  add_executable( ${name} ${sourcefile} )
  target_link_libraries( ${name} DataModelExample )
  install(TARGETS ${name} DESTINATION examples)
endforeach( sourcefile ${executables} )

install(TARGETS DataModelExample DESTINATION examples)
install(FILES python/eventstore.py DESTINATION examples)

include_directories(
	${CMAKE_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/datamodel
        ${CMAKE_CURRENT_SOURCE_DIR}/utilities
)

file(GLOB sources datamodel/*.cc)
file(GLOB headers datamodel/*.h)
file(GLOB sources_utils utilities/*.cc)
file(GLOB headers_utils utilities/*.h)


list(REMOVE_ITEM headers ${CMAKE_CURRENT_SOURCE_DIR}/datamodel/LinkDef.h )
message(DICTIONARY ${headers})
ROOT_GENERATE_DICTIONARY(ExampleDict ${headers} LINKDEF LinkDef.h )

add_library(DataModelExample SHARED ${sources} ${sources_utils} ${headers} ${headers_utils} ${}ExampleDict.cxx)
target_link_libraries(DataModelExample albers ${ROOT_LIBRARIES} ${ROOT_COMPONENT_LIBRARIES})

file(GLOB executables RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

foreach( sourcefile ${executables} )
  string( REPLACE ".cpp" "" name ${sourcefile} )
  add_executable( ${name} ${sourcefile} )
  target_link_libraries( ${name} DataModelExample )
  install(TARGETS ${name} DESTINATION examples)
endforeach( sourcefile ${executables} )

install(TARGETS DataModelExample DESTINATION examples)

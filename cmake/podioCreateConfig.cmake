include(CMakePackageConfigHelpers)
configure_file(cmake/podioConfig.cmake.in "${PROJECT_BINARY_DIR}/podioConfig.cmake" @ONLY)
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/podioConfigVersion.cmake
                                 VERSION ${podio_VERSION}
                                 COMPATIBILITY SameMajorVersion )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/podioConfig.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/podioConfigVersion.cmake
              DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake )

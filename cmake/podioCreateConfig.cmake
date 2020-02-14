set(CMAKE_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake")

include(CMakePackageConfigHelpers)

# Version file is same wherever we are
write_basic_package_version_file(${PROJECT_BINARY_DIR}/podioConfigVersion.cmake
                                 VERSION ${podio_VERSION}
                                 COMPATIBILITY SameMajorVersion )

# Build tree config
export(EXPORT podioTargets NAMESPACE podio:: FILE ${PROJECT_BINARY_DIR}/podioTargets.cmake)

# Install tree config
configure_package_config_file(${PROJECT_SOURCE_DIR}/cmake/podioConfig.cmake.in
                              ${PROJECT_BINARY_DIR}/podioConfig.cmake
                              INSTALL_DESTINATION ${CMAKE_INSTALL_CMAKEDIR}/${PROJECT_NAME}
                              PATH_VARS CMAKE_INSTALL_INCLUDEDIR CMAKE_INSTALL_LIBDIR)


install(FILES ${CMAKE_CURRENT_BINARY_DIR}/podioConfig.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/podioConfigVersion.cmake
              ${CMAKE_SOURCE_DIR}/cmake/podioMacros.cmake
              DESTINATION ${CMAKE_INSTALL_CMAKEDIR}/${PROJECT_NAME} )
install(EXPORT podioTargets
  DESTINATION ${CMAKE_INSTALL_CMAKEDIR}/${PROJECT_NAME}
  NAMESPACE podio:: )

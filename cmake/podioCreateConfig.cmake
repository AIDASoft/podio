#--- CMake Config Files -----------------------------------------------
# - Use CMake's module to help generating relocatable config files
include(CMakePackageConfigHelpers)

# - Versioning
write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/podioConfigVersion.cmake
  VERSION ${podio_VERSION}
  COMPATIBILITY SameMajorVersion)

# - Install time config and target files
configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/podioConfig.cmake.in
  "${PROJECT_BINARY_DIR}/podioConfig.cmake"
  INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/podio"
  PATH_VARS
    CMAKE_INSTALL_BINDIR
    CMAKE_INSTALL_INCLUDEDIR
    CMAKE_INSTALL_LIBDIR
  )

# - install and export
install(FILES
  "${PROJECT_BINARY_DIR}/podioConfigVersion.cmake"
  "${PROJECT_BINARY_DIR}/podioConfig.cmake"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/podio"
  )
install(EXPORT podioTargets
  NAMESPACE podio::
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/podio"
  )



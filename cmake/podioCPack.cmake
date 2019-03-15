
set(CPACK_PACKAGE_NAME "hep-podio")
set(CPACK_SET_DESTDIR TRUE)
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/AIDASoft/podio")
set(CPACK_PACKAGE_DESCRIPTION "PlainOldData template engine for effective datamodels ")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${CPACK_PACKAGE_DESCRIPTION})
set(CPACK_PACKAGE_VENDOR "AIDASoft")
set(CPACK_PACKAGE_VERSION ${podio_VERSION})
set(CPACK_PACKAGE_VERSION_MAJOR ${podio_MAJOR_VERSION})
set(CPACK_PACKAGE_VERSION_MINOR ${podio_MINOR_VERSION})
set(CPACK_PACKAGE_VERSION_PATCH ${podio_PATCH_VERSION})

# required fields for .deb
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "valentin.volkl@cern.ch")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE ${CPACK_PACKAGE_HOMEPAGE_URL})
set(CPACK_DEBIAN_PACKAGE_RECOMMENDS "hep-root, python-yaml")

set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")

#--- source package settings ---------------------------------------------------
set(CPACK_SOURCE_IGNORE_FILES
    ${PROJECT_BINARY_DIR}
    "~$"
    "/.git/"
    "/\\\\\\\\.git/"
    "/#"
)
set(CPACK_SOURCE_STRIP_FILES "")

#--- translate buildtype -------------------------------------------------------
string( TOLOWER "${CMAKE_BUILD_TYPE}" buildtype_lower )
if(buildtype_lower STREQUAL "release")
  set(HSF_BUILDTYPE "opt")
elseif(buildtype_lower STREQUAL "debug")
  set(HSF_BUILDTYPE "dbg")
elseif(buildtype_lower STREQUAL "relwithbebinfo")
  set(HSF_BUILDTYPE "owd")
endif()


#--- use HSF platform name if possible -----------------------------------------
function(hsf_get_platform _output_var)
  # - Determine arch for target of project build
  set(HSF_ARCH ${CMAKE_SYSTEM_PROCESSOR})

  # - Translate compiler info to HSF format
  string(TOLOWER ${CMAKE_C_COMPILER_ID} HSF_COMPILER_ID)
  if(NOT HSF_COMPILER_ID)
    set(HSF_COMPILER_ID "unknown")
  endif()

  set(HSF_COMPILER_VERSION ${CMAKE_C_COMPILER_VERSION})
  if(NOT HSF_COMPILER_VERSION)
    set(HSF_COMPILER_VERSION "0")
  endif()
  # Strip version to MAJORMINOR (?)
  string(REGEX REPLACE "([a-z0-9]+)(\.|-|_)([a-z0-9]+).*" "\\1\\3" HSF_COMPILER_VERSION ${HSF_COMPILER_VERSION})

  # - Determine OS info
  # NOTE: This derives *HOST* OS info, *NOT* *TARGET* OS
  #       Needs more thought for cross-compile cases, though likely reduces
  #       to check on CMAKE_CROSS_COMPILING and subsequent use of the needed
  #       variables as defined in the toolchain file
  if(APPLE)
    set(HSF_OS_ID "macos")
    execute_process(COMMAND sw_vers -productVersion
      COMMAND cut -d . -f 1-2
      OUTPUT_VARIABLE HSF_OS_VERSION
      OUTPUT_STRIP_TRAILING_WHITESPACE
      )
    string(REPLACE "." "" HSF_OS_VERSION ${HSF_OS_VERSION})
  elseif(WIN32)
    # Should be able to determine gross Windows version from CMAKE_SYSTEM_VERSION
    set(HSF_OS_ID "win")
    if(CMAKE_SYSTEM_VERSION VERSION_EQUAL "6.1")
      set(HSF_OS_VERSION "7")
    elseif(CMAKE_SYSTEM_VERSION VERSION_EQUAL "6.2")
      set(HSF_OS_VERSION "8")
    elseif(CMAKE_SYSTEM_VERSION VERSION_EQUAL "6.3")
      set(HSF_OS_VERSION "8")
    elseif(CMAKE_SYSTEM_VERSION VERSION_EQUAL "10.0")
      set(HSF_OS_VERSION "10")
    else()
      set(HSF_OS_VERSION "unknown")
    endif()
  elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
    # Use /etc/os-release if it's present
    if(EXISTS "/etc/os-release")
      # - Parse based on spec from freedesktop
      # http://www.freedesktop.org/software/systemd/man/os-release.html
      # - ID
      file(STRINGS "/etc/os-release" HSF_OS_ID REGEX "^ID=.*$")
      string(REGEX REPLACE "ID=|\"" "" HSF_OS_ID ${HSF_OS_ID})
      # - VERSION_ID
      file(STRINGS "/etc/os-release" HSF_OS_VERSION REGEX "^VERSION_ID=.*$")
      string(REGEX REPLACE "VERSION_ID=|\"" "" HSF_OS_VERSION ${HSF_OS_VERSION})
      string(REGEX REPLACE "([a-z0-9]+)(\.|-|_)([a-z0-9]+).*" "\\1\\3" HSF_OS_VERSION ${HSF_OS_VERSION})
    else()
      # Workaround for older systems
      # 1. Might be lucky and have lsb_release
      find_program(tricktrack_LSB_RELEASE_EXECUTABLE lsb_release
        DOC "Path to lsb_release program"
        )
      mark_as_advanced(tricktrack_LSB_RELEASE_EXECUTABLE)
      if(tricktrack_LSB_RELEASE_EXECUTABLE)
        # - ID
        execute_process(COMMAND ${tricktrack_LSB_RELEASE_EXECUTABLE} -is
          OUTPUT_VARIABLE HSF_OS_ID
          OUTPUT_STRIP_TRAILING_WHITESPACE
          )
        string(TOLOWER ${HSF_OS_ID} HSF_OS_ID)
        # - Version
        execute_process(COMMAND ${tricktrack_LSB_RELEASE_EXECUTABLE} -ir
          OUTPUT_VARIABLE HSF_OS_VERSION
          OUTPUT_STRIP_TRAILING_WHITESPACE
          )
        string(REGEX REPLACE "([a-z0-9]+)(\.|-|_)([a-z0-9]+).*" "\\1\\3" HSF_OS_VERSION ${HSF_OS_VERSION})
      else()
        # 2. Only mark in general terms, or have to check for possible /etc/VENDOR-release files
        set(HSF_OS_ID "linux")
        string(REGEX REPLACE "([a-z0-9]+)(\.|-|_)([a-z0-9]+).*" "\\1\\3" HSF_OS_VERSION ${CMAKE_SYSTEM_VERSION})
      endif()
    endif()
  else()
    set(HSF_OS_ID "unknown")
    set(HSF_OS_VERSION "0")
  endif()

  set(${_output_var} "${HSF_ARCH}-${HSF_OS_ID}${HSF_OS_VERSION}-${HSF_COMPILER_ID}${HSF_COMPILER_VERSION}-${HSF_BUILDTYPE}" PARENT_SCOPE)
endfunction()

#--- use HSF platform name -----------------------------------------------------
execute_process(
  COMMAND hsf_get_platform.py --buildtype ${HSF_BUILDTYPE}
  OUTPUT_VARIABLE HSF_PLATFORM OUTPUT_STRIP_TRAILING_WHITESPACE)


# If hsf_get_platform isn't available, use CMake function
if(NOT HSF_PLATFORM)
  hsf_get_platform(HSF_PLATFORM)
endif()

set(CPACK_PACKAGE_RELOCATABLE True)
set(CPACK_PACKAGE_INSTALL_DIRECTORY "podio_${podio_VERSION}")
set(CPACK_PACKAGE_FILE_NAME "podio_${podio_VERSION}_${HSF_PLATFORM}")

include(CPack)

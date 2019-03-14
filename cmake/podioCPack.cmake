
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


#--- use HSF platform name -----------------------------------------------------
execute_process(
  COMMAND hsf_get_platform.py --buildtype ${HSF_BUILDTYPE}
  OUTPUT_VARIABLE HSF_PLATFORM OUTPUT_STRIP_TRAILING_WHITESPACE)


set(CPACK_PACKAGE_RELOCATABLE True)
set(CPACK_PACKAGE_INSTALL_DIRECTORY "podio_${podio_VERSION}")
set(CPACK_PACKAGE_FILE_NAME "podio_${podio_VERSION}_${HSF_PLATFORM}")

include(CPack)

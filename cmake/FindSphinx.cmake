find_program(SPHINX_BUILD_EXECUTABLE
  NAMES sphinx-build
  DOC "Path to sphinx-build executable")

find_program(SPHINX_API_DOC_EXECUTABLE
  NAMES sphinx-apidoc
  DOC "Path to sphinx-apidoc executable")

include(FindPackageHandleStandardArgs)

# Handle standard arguments to find_package
find_package_handle_standard_args(Sphinx "Failed to find sphinx-executable"
  SPHINX_BUILD_EXECUTABLE SPHINX_API_DOC_EXECUTABLE)

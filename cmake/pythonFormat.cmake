# Additional targets to run the python formatter and linter on python scripts
#
# Requires ruff to be available in the environment. The configuration is taken
# from .ruff.toml in the source directory.

# Get all our Python files
file(GLOB_RECURSE ALL_PYTHON_FILES ${PROJECT_SOURCE_DIR}/python/*.py)

find_program(RUFF_EXECUTABLE ruff)
if(RUFF_EXECUTABLE)
    add_custom_target(
        ruff-format
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMAND ${RUFF_EXECUTABLE} format ${ALL_PYTHON_FILES}
    )
    set_target_properties(ruff-format PROPERTIES EXCLUDE_FROM_ALL TRUE)

    add_custom_target(
        ruff-check
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMAND ${RUFF_EXECUTABLE} check ${ALL_PYTHON_FILES}
    )
    set_target_properties(ruff-check PROPERTIES EXCLUDE_FROM_ALL TRUE)
else()
    message(STATUS "Failed to find ruff executable - no targets to run ruff can be set")
endif()

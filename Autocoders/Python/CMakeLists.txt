# Set python autocoder directory
set(PYTHON_AUTOCODER_DIR "${CMAKE_CURRENT_LIST_DIR}" CACHE INTERNAL "Directory for Python autocoder")

add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/src/")
# Tests build for UTs only
if (CMAKE_BUILD_TYPE STREQUAL "TESTING")
    add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/templates/")
    add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/test/")
endif()

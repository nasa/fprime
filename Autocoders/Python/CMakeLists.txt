# Set python autocoder directory
set(PYTHON_AUTOCODER_DIR "${CMAKE_CURRENT_LIST_DIR}" CACHE INTERNAL "Directory for Python autocoder")

add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/src/")
# Tests build for UTs only
if (BUILD_TESTING AND NOT __FPRIME_NO_UT_GEN__)
    add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/templates/")
    add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/test/")
endif()

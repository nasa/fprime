# Set python autocoder directory
set(PYTHON_AUTOCODER_DIR "${CMAKE_CURRENT_LIST_DIR}" CACHE INTERNAL "Directory for Python autocoder")

add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/src/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/templates/")
#TODO-MSTARCH add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/test/")

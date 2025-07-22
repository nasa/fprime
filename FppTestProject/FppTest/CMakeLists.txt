###
# FPP Test
#
# Builds unit tests for FPP autocoder
###
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/array/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/component/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/dp/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/state_machine/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/enum/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/interfaces/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/struct/")


if (NOT BUILD_TESTING)
    add_compile_options(
      -DFW_DIRECT_PORT_CALLS
    )
endif()

add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/components")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/ports")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/deployment")

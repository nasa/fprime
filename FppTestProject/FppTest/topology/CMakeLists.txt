if (BUILD_TESTING)
    add_compile_options(
      -DFW_DIRECT_PORT_CALLS=0
    )
else()
    add_compile_options(
      -DFW_DIRECT_PORT_CALLS=1
    )
endif()

add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/components")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/ports")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/deployment")

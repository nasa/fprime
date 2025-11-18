add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/topology")

register_fprime_deployment(
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/main.cpp"
    DEPENDS
        ${PROJECT_NAME}_topology_deployment_topology
)

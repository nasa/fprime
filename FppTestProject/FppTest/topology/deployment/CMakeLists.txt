add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/topology")

register_fprime_deployment(
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/main.cpp"
        # "${CMAKE_CURRENT_LIST_DIR}/../../../../Fw/Comp/ActiveComponentBase.cpp"
        # "${CMAKE_CURRENT_LIST_DIR}/../../../../Fw/Comp/QueuedComponentBase.cpp"
    DEPENDS
        ${PROJECT_NAME}_topology_deployment_topology
)

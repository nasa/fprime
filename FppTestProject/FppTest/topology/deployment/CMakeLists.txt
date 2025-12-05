add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/topology")

register_fprime_deployment(
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/main.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/../../component/types/FormalParamTypes.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/../../utils/Utils.cpp"
    DEPENDS
        ${PROJECT_NAME}_topology_deployment_topology
        STest
)

target_link_libraries(FppTest_topology_deployment PUBLIC gtest gtest_main)

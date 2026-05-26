###
# FPP Test
#
# Builds unit tests for FPP autocoder
###

# FIXME(tumbar) This seems fairly brittle to disable certain UTs with direct port calls
if (FPRIME_ENABLE_DIRECT_PORT_CALLS)
    # Disable unit testing for components that require
    # indirect port calls
    set(__FPRIME_NO_UT_GEN__ ON)
endif ()

add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/array/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/component/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/dp/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/state_machine/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/enum/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/interfaces/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/struct/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/sizeof/")

if (FPRIME_ENABLE_DIRECT_PORT_CALLS)
    # Re-enable unit testing for the topology since it uses direct port calls
    set(__FPRIME_NO_UT_GEN__ OFF)
endif ()

add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/topology/")


register_fprime_deployment(
  SOURCES
    "source.cpp"
  DEPENDS
    ${PROJECT_NAME}_array
    ${PROJECT_NAME}_component_active
    ${PROJECT_NAME}_component_empty
    ${PROJECT_NAME}_component_passive
    ${PROJECT_NAME}_component_queued
    ${PROJECT_NAME}_dp
    ${PROJECT_NAME}_enum
    ${PROJECT_NAME}_interfaces
    ${PROJECT_NAME}_state_machine_external_instance
    ${PROJECT_NAME}_state_machine_internal_initial
    ${PROJECT_NAME}_state_machine_internal_state
    ${PROJECT_NAME}_state_machine_internal_instance_choice
    ${PROJECT_NAME}_state_machine_internal_instance_initial
    ${PROJECT_NAME}_state_machine_internal_instance_state
    ${PROJECT_NAME}_sizeof
    ${PROJECT_NAME}_struct
    ${PROJECT_NAME}_topology_ports
    ${PROJECT_NAME}_topology_components_Comp
    ${PROJECT_NAME}_topology_components_Sender
    ${PROJECT_NAME}_topology_components_Receiver
    ${PROJECT_NAME}_topology_main
    ${PROJECT_NAME}_topology_special_ports
    ${PROJECT_NAME}_topology_top_ports
)


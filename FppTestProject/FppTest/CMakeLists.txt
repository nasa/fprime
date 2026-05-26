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

set(SOURCE_FILES "source.cpp")
set(MOD_DEPS
    ${PROJECT_NAME}/array
    ${PROJECT_NAME}/component/active
    ${PROJECT_NAME}/component/empty
    ${PROJECT_NAME}/component/passive
    ${PROJECT_NAME}/component/queued
    ${PROJECT_NAME}/dp
    ${PROJECT_NAME}/enum
    ${PROJECT_NAME}/interfaces
    ${PROJECT_NAME}/state_machine/external_instance
    ${PROJECT_NAME}/state_machine/internal/initial
    ${PROJECT_NAME}/state_machine/internal/state
    ${PROJECT_NAME}/state_machine/internal_instance/choice
    ${PROJECT_NAME}/state_machine/internal_instance/initial
    ${PROJECT_NAME}/state_machine/internal_instance/state
    ${PROJECT_NAME}/sizeof
    ${PROJECT_NAME}/struct
    ${PROJECT_NAME}/topology/ports
    ${PROJECT_NAME}/topology/components/Comp
    ${PROJECT_NAME}/topology/components/Sender
    ${PROJECT_NAME}/topology/components/Receiver
    ${PROJECT_NAME}/topology/main
    ${PROJECT_NAME}/topology/special_ports
    ${PROJECT_NAME}/topology/top_ports
)

register_fprime_deployment()


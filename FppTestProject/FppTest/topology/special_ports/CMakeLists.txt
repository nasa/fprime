

register_fprime_module(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/topology.fpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/SpecialPortsTopologyDefs.hpp"
  DEPENDS
    ${PROJECT_NAME}_topology_ports
    ${PROJECT_NAME}_topology_components_Comp
    ${PROJECT_NAME}_topology_components_Framework
)

# Declare dependencies on test modules

register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/main.cpp"
  DEPENDS
    STest
)

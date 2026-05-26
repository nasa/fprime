

register_fprime_module(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/instances.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/topology.fpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/FppTestTopologyDefs.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/ReceiverTopologyDefs.hpp"
  DEPENDS
    ${PROJECT_NAME}_topology_ports
    ${PROJECT_NAME}_topology_components_Sender
    ${PROJECT_NAME}_topology_components_Receiver
)

# Declare dependencies on test modules

register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/main.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/../../component/types/FormalParamTypes.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/../../utils/Utils.cpp"
  DEPENDS
    STest
)

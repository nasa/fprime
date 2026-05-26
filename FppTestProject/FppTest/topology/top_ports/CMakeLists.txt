

register_fprime_module(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/Topology.fpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/ReceiverTopTopologyDefs.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/SenderTopTopologyDefs.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/TopPortsTopologyDefs.hpp"
  DEPENDS
    ${PROJECT_NAME}_topology_components_Sender
    ${PROJECT_NAME}_topology_components_Receiver
)

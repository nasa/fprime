

register_fprime_module(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/Receiver.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/Receiver.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/Receiver.hpp"
  DEPENDS
    ${PROJECT_NAME}_topology_ports
)



register_fprime_module(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/Framework.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/Framework.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/Framework.hpp"
  DEPENDS
    ${PROJECT_NAME}_topology_ports
    ${PROJECT_NAME}_topology_types
)

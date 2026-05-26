

register_fprime_module(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/Comp.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/Comp.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/Comp.hpp"
  DEPENDS
    ${PROJECT_NAME}_topology_ports
    ${PROJECT_NAME}_topology_types
)

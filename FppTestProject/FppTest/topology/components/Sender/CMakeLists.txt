register_fprime_module(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/Sender.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/Sender.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/Sender.hpp"
  DEPENDS
    Fw_Comp
    Fw_Types
    Os
    ${PROJECT_NAME}_topology_ports
)

target_link_libraries(FppTest_topology_components_Sender PRIVATE gtest)

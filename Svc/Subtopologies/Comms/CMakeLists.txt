add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/CommsConfig/")

register_fprime_module(
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/Comms.fpp"
    HEADERS
        "${CMAKE_CURRENT_LIST_DIR}/CommsTopologyDefs.hpp"
    INTERFACE
)

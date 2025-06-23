add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/ComCcsdsConfig/")

register_fprime_module(
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/ComCcsds.fpp"
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/SubtopologyTopologyDefs.cpp"
    HEADERS
        "${CMAKE_CURRENT_LIST_DIR}/SubtopologyTopologyDefs.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/PingEntries.hpp"
    DEPENDS 
        ComCcsdsConfig
)

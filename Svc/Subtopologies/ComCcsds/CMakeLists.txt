
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/ComCcsdsConfig/")

register_fprime_module(
    EXCLUDE_FROM_ALL
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/ComCcsds.fpp"
    HEADERS
        "${CMAKE_CURRENT_LIST_DIR}/SubtopologyTopologyDefs.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/PingEntries.hpp"
    DEPENDS
        Svc_Subtopologies_ComCcsds_ComCcsdsConfig
    INTERFACE
)

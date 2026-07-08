
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/CcsdsFramingConfig/")

register_fprime_module(
    EXCLUDE_FROM_ALL
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/CcsdsFraming.fpp"
    HEADERS
        "${CMAKE_CURRENT_LIST_DIR}/SubtopologyTopologyDefs.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/PingEntries.hpp"
    DEPENDS
        Svc_Subtopologies_CcsdsFraming_CcsdsFramingConfig
    INTERFACE
)

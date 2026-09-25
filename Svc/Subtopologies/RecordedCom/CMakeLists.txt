add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/RecordedComConfig/")

register_fprime_module(
    EXCLUDE_FROM_ALL
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/RecordedCom.fpp"
    HEADERS
        "${CMAKE_CURRENT_LIST_DIR}/RecordedComTopologyDefs.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/PingEntries.hpp"
    DEPENDS
        Svc_Subtopologies_RecordedCom_RecordedComConfig
    INTERFACE
)
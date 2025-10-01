add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/CdhCoreConfig/")

register_fprime_module(
    EXCLUDE_FROM_ALL
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/CdhCore.fpp"
    HEADERS
        "${CMAKE_CURRENT_LIST_DIR}/SubtopologyTopologyDefs.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/PingEntries.hpp"
    INTERFACE
    DEPENDS
        Svc_Subtopologies_CdhCore_CdhCoreConfig
)

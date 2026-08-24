
if (FPRIME_ENABLE_TEXT_LOGGERS)
    set(CDHCORE_TOPOLOGY_FPP "${CMAKE_CURRENT_LIST_DIR}/CdhCore.fpp")
else()
    # Svc.PassiveTextLogger is excluded from the build; use the subtopology
    # variant without the textLogger instance.
    set(CDHCORE_TOPOLOGY_FPP "${CMAKE_CURRENT_LIST_DIR}/CdhCoreNoTextLogger.fpp")
endif()
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/CdhCoreConfig/")

register_fprime_module(
    EXCLUDE_FROM_ALL
    AUTOCODER_INPUTS
        "${CDHCORE_TOPOLOGY_FPP}"
    HEADERS
        "${CMAKE_CURRENT_LIST_DIR}/SubtopologyTopologyDefs.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/PingEntries.hpp"
    INTERFACE
    DEPENDS
        Svc_Subtopologies_CdhCore_CdhCoreConfig
)

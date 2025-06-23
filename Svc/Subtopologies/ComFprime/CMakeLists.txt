add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/ComFprimeConfig/")

register_fprime_module(
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/ComFprime.fpp"
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/SubtopologyTopologyDefs.cpp"
    HEADERS
        "${CMAKE_CURRENT_LIST_DIR}/SubtopologyTopologyDefs.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/PingEntries.hpp"
    DEPENDS
        ComFprimeConfig
)

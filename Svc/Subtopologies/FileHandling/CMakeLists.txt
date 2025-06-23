add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/FileHandlingConfig/")

register_fprime_module(
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/FileHandling.fpp"
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/SubtopologyTopologyDefs.cpp"
    HEADERS
        "${CMAKE_CURRENT_LIST_DIR}/SubtopologyTopologyDefs.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/PingEntries.hpp"
)

add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/CommsCCSDSConfig/")

register_fprime_module(
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/CommsCCSDS.fpp"
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/CommsCCSDSTopologyDefs.cpp"
    HEADERS
        "${CMAKE_CURRENT_LIST_DIR}/SubtopologyTopologyDefs.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/PingEntries.hpp"
)

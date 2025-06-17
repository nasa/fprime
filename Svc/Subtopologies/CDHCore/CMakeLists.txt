add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/config/")

register_fprime_module(
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/CDHCore.fpp"
    HEADERS
        "${CMAKE_CURRENT_LIST_DIR}/PingEntries.hpp"
    INTERFACE
)

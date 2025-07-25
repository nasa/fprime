# Clear any variables that might cause conflicts with source files
# Helps with backwards compatibility for old F' build system
unset(SOURCE_FILES)
unset(MOD_DEPS)

register_fprime_module(
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/MySubtopologyInstance.fpp"
    HEADERS
        "${CMAKE_CURRENT_LIST_DIR}/PingEntries.hpp"
    DEPENDS
        Svc_Subtopologies_ComLoggerTee
    INTERFACE
)

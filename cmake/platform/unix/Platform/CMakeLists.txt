####
# unix/Platform/CMakeLists.txt:
#
# This file defines a configuration module for unix platforms.
####
register_fprime_config(
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/PlatformTypes.fpp"
    HEADERS
        "${CMAKE_CURRENT_LIST_DIR}/PlatformTypes.h"
    INTERFACE # No buildable files generated
)

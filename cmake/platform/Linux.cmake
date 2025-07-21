####
# Linux.cmake:
#
# Linux platform file for standard linux targets.
####
FIND_PACKAGE ( Threads REQUIRED )
set(FPRIME_USE_POSIX ON)
set(FPRIME_HAS_SOCKETS ON)
# Add unix include path which is compatible with Linux for PlatformTypes.hpp
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/unix/Platform/")
# Override unix implementations with LINUX specific ones
register_fprime_config(
        PlatformLinux
    INTERFACE # No buildable files generated
    CHOOSES_IMPLEMENTATIONS
        Os_Cpu_Linux
        Os_Memory_Linux
    BASE_CONFIG
)
target_compile_definitions(PlatformLinux INTERFACE -DTGT_OS_TYPE_LINUX)

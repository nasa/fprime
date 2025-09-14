####
# Darwin.cmake:
#
# Darwin based platform file used for Darwin (Mac OS X) targets. Note: this sets some OS X flags before calling into the common
# Linux implementations to use the posix types defined there.
####

FIND_PACKAGE ( Threads REQUIRED )
set(FPRIME_USE_POSIX ON)
set(FPRIME_HAS_SOCKETS ON)
# Set platform default for stubbed drivers
if (NOT DEFINED FPRIME_USE_STUBBED_DRIVERS)
   set(FPRIME_USE_STUBBED_DRIVERS ON)
endif()
# Add unix include path which is compatible with Darwin for PlatformTypes.hpp
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/unix/Platform/")
# Override unix implementations with DARWIN specific ones
register_fprime_config(
      PlatformDarwin
   INTERFACE # No buildable files generated
   CHOOSES_IMPLEMENTATIONS
      Os_Cpu_Darwin
      Os_Memory_Darwin
   BASE_CONFIG
)
target_compile_definitions(PlatformDarwin INTERFACE -DTGT_OS_TYPE_DARWIN)

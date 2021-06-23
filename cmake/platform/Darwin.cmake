####
# Darwin.cmake:
#
# Darwin based platform file used for Darwin (Mac OS X) targets. Note: this sets some OS X flags before calling into the common
# Linux implementations to use the posix types defined there.
####
add_definitions(-DTGT_OS_TYPE_DARWIN)

set(FPRIME_USE_POSIX )
# Set platform default for stubbed drivers
 ( FPRIME_USE_STUBBED_DRIVERS)
   set(FPRIME_USE_STUBBED_DRIVERS )
()

# Set platform default for baremetal scheduler drivers
 ( FPRIME_USE_BAREMETAL_SCHEDULER)
   set(FPRIME_USE_BAREMETAL_SCHEDULER )
   message(STATUS "Requiring thread library")
   FIND_PACKAGE ( Threads )
()

# Add linux include path which is compatible Darwin StandardTypes.hpp
(SYSTEM "${FPRIME_FRAMEWORK_PATH}/Fw/Types/Linux")

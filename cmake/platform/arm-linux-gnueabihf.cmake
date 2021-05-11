##
# arm-linux-gnueabihf.cmake:
#
# A platform file supporting the raspberry pi. This is the F prime platform support file that combines with the
# toolchain file found at [../toolchain/raspberrypi.cmake](../toolchain/raspberrypi.cmake).
##

# Set platform default for stubbed drivers
if (NOT DEFINED FPRIME_USE_STUBBED_DRIVERS)
   set(FPRIME_USE_STUBBED_DRIVERS OFF)
endif()

# Set platform default for baremetal scheduler drivers
if (NOT DEFINED FPRIME_USE_BAREMETAL_SCHEDULER)
   set(FPRIME_USE_BAREMETAL_SCHEDULER OFF)
   message(STATUS "Requiring thread library")
   FIND_PACKAGE ( Threads REQUIRED )
endif()

# RPI is Linux based, so include the common Linux items
include("${CMAKE_CURRENT_LIST_DIR}/Linux-common.cmake")

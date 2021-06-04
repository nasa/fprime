####
# Linux.cmake:
#
# Linux platform file for standard linux targets. Merely defers to [./Linux-common.cmake](Linux-common.cmake).
####

# Set platform default for stubbed drivers
if (NOT DEFINED FPRIME_USE_STUBBED_DRIVERS)
   set(FPRIME_USE_STUBBED_DRIVERS ON)
endif()

# Set platform default for baremetal scheduler drivers
if (NOT DEFINED FPRIME_USE_BAREMETAL_SCHEDULER)
   set(FPRIME_USE_BAREMETAL_SCHEDULE OFF)
   message(STATUS "Requiring thread library")
   FIND_PACKAGE ( Threads REQUIRED )
endif()

# Use common linux setup
include("${CMAKE_CURRENT_LIST_DIR}/Linux-common.cmake")

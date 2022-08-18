####
# Linux.cmake:
#
# Linux platform file for standard linux targets. Merely defers to [./Linux-common.cmake](Linux-common.cmake).
####
# Set platform default for baremetal scheduler drivers
if (NOT DEFINED FPRIME_USE_BAREMETAL_SCHEDULER)
   set(FPRIME_USE_BAREMETAL_SCHEDULER OFF)
   message(STATUS "Requiring thread library")
   FIND_PACKAGE ( Threads REQUIRED )
endif()

# Use common linux setup
add_definitions(-DTGT_OS_TYPE_LINUX)
set(FPRIME_USE_POSIX ON)

# Add Linux specific headers into the system
include_directories(SYSTEM "${CMAKE_CURRENT_LIST_DIR}/types")


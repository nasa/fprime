####
# Linux.cmake:
#
# Linux platform file for standard linux targets.
####
# Set platform default for baremetal scheduler drivers
if (NOT DEFINED FPRIME_USE_BAREMETAL_SCHEDULER)
   set(FPRIME_USE_BAREMETAL_SCHEDULER OFF)
   message(STATUS "Requiring thread library")
   FIND_PACKAGE ( Threads REQUIRED )
endif()
choose_fprime_implementation(Os/File Os/File/Posix)
choose_fprime_implementation(Os/Task Os/Task/Posix)
choose_fprime_implementation(Os/Mutex Os/Mutex/Posix)
# REVIEW NOTE: 
# `/` kind of remind of a file path, but in this case it should maybe
# correspond to the actual path, which is e.g. Os/Posix/Task and not Os/Task/Posix
# I suggest we may want to rename all those targets to match the path in which they are defined
# e.g. Os_Mutex_Posix becomes Os_Posix_Mutex
# Thoughts?

# Use common linux setup
add_definitions(-DTGT_OS_TYPE_LINUX)
set(FPRIME_USE_POSIX ON)

# Add Linux specific headers into the system
include_directories(SYSTEM "${CMAKE_CURRENT_LIST_DIR}/types")


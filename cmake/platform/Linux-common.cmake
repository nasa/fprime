####
# Linux-common.cmake:
#
# Platform setup for all Linux based targets. This can be used for Linux, raspberry pi, and other linux-like hosts. This
# file represents all common linux functions.
####

# Set linux target
add_definitions(-DTGT_OS_TYPE_LINUX)
set(FPRIME_USE_POSIX ON)
# Requires threading library, use cmake to find appropriate library
message(STATUS "Requiring thread library")
FIND_PACKAGE ( Threads REQUIRED )

# Add Linux specific headers into the system
include_directories(SYSTEM "${FPRIME_FRAMEWORK_PATH}/Fw/Types/Linux")

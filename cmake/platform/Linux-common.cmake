
# Define OS build
add_definitions(-DTGT_OS_TYPE_LINUX)

message(STATUS "Requiring thread library")

# Requires threading library
FIND_PACKAGE ( Threads REQUIRED )

# Add linux include path
include_directories(SYSTEM "${FPRIME_CORE_DIR}/Fw/Types/Linux")

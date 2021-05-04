####
# Darwin.cmake:
#
# Darwin based platform file used for Darwin (Mac OS X) targets. Note: this sets some OS X flags before calling into the common
# Linux implementations to use the posix types defined there.
####
add_definitions(-DTGT_OS_TYPE_DARWIN)

# Find an appropriate thread library
message(STATUS "Requiring thread library")
FIND_PACKAGE ( Threads REQUIRED )
set(FPRIME_USE_POSIX ON)

# Darwin specific flags: shared, C, and C++ settings
set(DARWIN_COMMON
  "-Wall -Wextra -fno-builtin -fno-asm -Wno-unused-parameter -Wno-long-long"
)
set(CMAKE_C_FLAGS
  "${CMAKE_C_FLAGS} ${DARWIN_COMMON} -std=c99 -pedantic -Werror-implicit-function-declaration -Wstrict-prototypes"
)
set(CMAKE_CXX_FLAGS
  "${CMAKE_CXX_FLAGS} ${DARWIN_COMMON} -std=c++11"
)
# Add linux include path which is compatible with Darwin for StandardTypes.hpp
include_directories(SYSTEM "${FPRIME_FRAMEWORK_PATH}/Fw/Types/Linux")

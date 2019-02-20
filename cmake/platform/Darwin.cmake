# Define OS build
add_definitions(-DTGT_OS_TYPE_DARWIN)

message(STATUS "Requiring thread library")

# Requires threading library
FIND_PACKAGE ( Threads REQUIRED )

set(DARWIN_COMMON
  "-Wall -Wextra -fno-builtin -fno-asm -Wno-unused-parameter -Wno-long-long -v"
)

set(CMAKE_C_FLAGS
  "${CMAKE_C_FLAGS} ${DARWIN_COMMON} -std=c99 -pedantic -Werror-implicit-function-declaration -Wstrict-prototypes"
)

set(CMAKE_CXX_FLAGS
  "${CMAKE_CXX_FLAGS} ${DARWIN_COMMON}"
)

# Add linux include path which is compatable with Darwin
include_directories(SYSTEM "${FPRIME_CORE_DIR}/Fw/Types/Linux")

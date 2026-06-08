####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
restrict_platforms(Darwin)
add_custom_target("${FPRIME_CURRENT_MODULE}")

register_fprime_module(
    Os_Darwin_Shared
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/error.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/error.hpp"
  DEPENDS
    Fw_Types
)

register_os_implementation(Cpu Darwin)
register_os_implementation(Memory Darwin)
register_os_implementation(CountingSemaphore Darwin Os_Darwin_Shared)

# -----------------------------------------
### Os/Darwin/Cpu Section
# -----------------------------------------
register_fprime_ut(
    DarwinCpuTest
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/../test/ut/cpu/CommonCpuTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/DarwinCpuTests.cpp"
  CHOOSES_IMPLEMENTATIONS
    Os_Cpu_Darwin
  DEPENDS
    Fw_Types
    Fw_Time
    STest
)
# -----------------------------------------
### Os/Darwin/Memory Section
# -----------------------------------------
register_fprime_ut(
    DarwinMemoryTest
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/../test/ut/memory/CommonMemoryTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/DarwinMemoryTests.cpp"
  CHOOSES_IMPLEMENTATIONS
    Os_Memory_Darwin
  DEPENDS
    Fw_Types
    Fw_Time
    STest
)
# -----------------------------------------
### Os/Darwin/CountingSemaphore Section
# -----------------------------------------
register_fprime_ut(
    DarwinCountingSemaphoreTest
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/../test/ut/countingsemaphore/CommonTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/../test/ut/countingsemaphore/CountingSemaphoreRules.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/DarwinCountingSemaphoreTests.cpp"
  CHOOSES_IMPLEMENTATIONS
    Os_CountingSemaphore_Darwin
  DEPENDS
    Fw_Types
    Fw_Time
    STest
)
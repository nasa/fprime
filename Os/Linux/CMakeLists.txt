####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
restrict_platforms(Linux)
add_custom_target("${FPRIME_CURRENT_MODULE}")

register_os_implementation(Cpu Linux Os_File)
register_os_implementation(Memory Linux Os_File)

# -----------------------------------------
### Os/Linux/Cpu Section
# -----------------------------------------
register_fprime_ut(
    LinuxCpuTest
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/../test/ut/cpu/CommonCpuTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/LinuxCpuTests.cpp"
  CHOOSES_IMPLEMENTATIONS
    Os_Cpu_Linux
  DEPENDS
    Fw_Types
    Fw_Time
    STest
)

# -----------------------------------------
### Os/Linux/Memory Section
# -----------------------------------------
register_fprime_ut(
    LinuxMemoryTest
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/../test/ut/memory/CommonMemoryTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/LinuxMemoryTests.cpp"
  CHOOSES_IMPLEMENTATIONS
    Os_Memory_Linux
  DEPENDS
    Fw_Types
    Fw_Time
    STest
)

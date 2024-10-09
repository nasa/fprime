####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
restrict_platforms(Linux)
add_custom_target("${FPRIME_CURRENT_MODULE}")

# -----------------------------------------
### Os/Linux/Cpu Section
# -----------------------------------------
set(SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/Cpu.cpp"
)
set(HEADER_FILES
        "${CMAKE_CURRENT_LIST_DIR}/Cpu.hpp"
)
set(MOD_DEPS
    Fw_Types
)
register_fprime_module(Os_Cpu_Linux)
register_fprime_implementation(Os/Cpu Os_Cpu_Linux "${CMAKE_CURRENT_LIST_DIR}/DefaultCpu.cpp")

set(UT_SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/../test/ut/cpu/CommonCpuTests.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/LinuxCpuTests.cpp"
)
set(UT_MOD_DEPS
        STest
)
choose_fprime_implementation(Os/Cpu Os_Cpu_Linux)
register_fprime_ut(LinuxCpuTest)


# -----------------------------------------
### Os/Linux/Memory Section
# -----------------------------------------
set(SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/Memory.cpp"
)
set(HEADER_FILES
        "${CMAKE_CURRENT_LIST_DIR}/Memory.hpp"
)
set(MOD_DEPS)
register_fprime_module(Os_Memory_Linux)
register_fprime_implementation(Os/Memory Os_Memory_Linux "${CMAKE_CURRENT_LIST_DIR}/DefaultMemory.cpp")

set(UT_SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/../test/ut/memory/CommonMemoryTests.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/LinuxMemoryTests.cpp"
)
set(UT_MOD_DEPS
        STest
)
choose_fprime_implementation(Os/Memory Os_Memory_Linux)
register_fprime_ut(LinuxMemoryTest)

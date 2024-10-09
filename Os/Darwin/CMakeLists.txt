####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
restrict_platforms(Darwin)
add_custom_target("${FPRIME_CURRENT_MODULE}")

# -----------------------------------------
### Os/Darwin/Cpu Section
# -----------------------------------------
set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/Cpu.cpp"
)
set(HEADER_FILES
    "${CMAKE_CURRENT_LIST_DIR}/Cpu.hpp"
)
set(MOD_DEPS)
register_fprime_module(Os_Cpu_Darwin)
register_fprime_implementation(Os/Cpu Os_Cpu_Darwin "${CMAKE_CURRENT_LIST_DIR}/DefaultCpu.cpp")

set(UT_SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/../test/ut/cpu/CommonCpuTests.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/DarwinCpuTests.cpp"
)
set(UT_MOD_DEPS
        STest
)
choose_fprime_implementation(Os/Cpu Os_Cpu_Darwin)
register_fprime_ut(DarwinCpuTest)


# -----------------------------------------
### Os/Darwin/Memory Section
# -----------------------------------------
set(SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/Memory.cpp"
)
set(HEADER_FILES
        "${CMAKE_CURRENT_LIST_DIR}/Memory.hpp"
)
set(MOD_DEPS)
register_fprime_module(Os_Memory_Darwin)
register_fprime_implementation(Os/Memory Os_Memory_Darwin "${CMAKE_CURRENT_LIST_DIR}/DefaultMemory.cpp")

set(UT_SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/../test/ut/memory/CommonMemoryTests.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/DarwinMemoryTests.cpp"
)
set(UT_MOD_DEPS
        STest
)
choose_fprime_implementation(Os/Memory Os_Memory_Darwin)
register_fprime_ut(DarwinMemoryTest)

####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
restrict_platforms(Linux)
add_custom_target("${FPRIME_CURRENT_MODULE}")

add_fprime_supplied_os_module(Cpu Linux)
add_fprime_supplied_os_module(Memory Linux)

# -----------------------------------------
### Os/Linux/Cpu Section
# -----------------------------------------
set(UT_SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/../test/ut/cpu/CommonCpuTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/LinuxCpuTests.cpp"
)
add_fprime_os_test(LinuxCpuTest "${UT_SOURCE_FILES}" "Os_Cpu\;Os_Cpu_Linux")


# -----------------------------------------
### Os/Linux/Memory Section
# -----------------------------------------

set(UT_SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/../test/ut/memory/CommonMemoryTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/LinuxMemoryTests.cpp"
)
add_fprime_os_test(LinuxMemoryTest "${UT_SOURCE_FILES}" "Os_Memory\;Os_Memory_Linux")

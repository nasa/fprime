####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
restrict_platforms(Darwin)
add_custom_target("${FPRIME_CURRENT_MODULE}")

register_os_implementation(Cpu Darwin)
register_os_implementation(Memory Darwin)

# -----------------------------------------
### Os/Darwin/Cpu Section
# -----------------------------------------
set(UT_SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/../test/ut/cpu/CommonCpuTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/DarwinCpuTests.cpp"
)
add_fprime_os_test(DarwinCpuTest "${UT_SOURCE_FILES}" "Os_Cpu\;Os_Cpu_Darwin")
# -----------------------------------------
### Os/Darwin/Memory Section
# -----------------------------------------
set(UT_SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/../test/ut/memory/CommonMemoryTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/DarwinMemoryTests.cpp"
)
add_fprime_os_test(DarwinMemoryTest "${UT_SOURCE_FILES}" "Os_Cpu\;Os_Memory_Darwin")

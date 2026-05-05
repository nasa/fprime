# Opt out of target/test_chained_autocoder. This module exercises the standalone
# target/test_autocoder; without this, the chained target would also run test_target_autocoder
# on it and CMake would emit the same `cmake -E copy` rule into two sub-makefiles, racing
# under `make -jN`. Custom targets are attached inside register_fprime_module, so the flag
# must be set first.
set(SKIP_TEST_CHAINED_AUTOCODER_TARGET TRUE)
register_fprime_module(
        TestTargetAutocoderModule
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/test1.test-target.txt"
        "${CMAKE_CURRENT_LIST_DIR}/test2.test-target.txt"
    EXCLUDE_FROM_ALL
    INTERFACE
)
unset(SKIP_TEST_CHAINED_AUTOCODER_TARGET)
target_include_directories(TestBuildAutocoderModule PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
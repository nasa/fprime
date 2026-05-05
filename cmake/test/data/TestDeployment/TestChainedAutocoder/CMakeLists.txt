# Opt out of target/test_autocoder. target/test_chained_autocoder runs test_target_autocoder
# itself (as the first stage of the chain), so applying both targets to this module would
# duplicate the same `cmake -E copy` rule into two sub-makefiles and race under `make -jN`.
# Custom targets are attached inside register_fprime_module, so the flag must be set first.
set(SKIP_TEST_AUTOCODER_TARGET TRUE)
register_fprime_module(
        TestChainedAutocoderModule
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/test1.test-target.txt"
        "${CMAKE_CURRENT_LIST_DIR}/test2.test-target.txt"
    EXCLUDE_FROM_ALL
    INTERFACE
)
unset(SKIP_TEST_AUTOCODER_TARGET)

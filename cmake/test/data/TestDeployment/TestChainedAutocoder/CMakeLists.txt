register_fprime_module(
        TestChainedAutocoderModule
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/test1.test-target.txt"
        "${CMAKE_CURRENT_LIST_DIR}/test2.test-target.txt"
    EXCLUDE_FROM_ALL
    INTERFACE
)

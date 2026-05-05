register_fprime_module(
        TestChainedAutocoderModule
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/test1.chained-input.txt"
        "${CMAKE_CURRENT_LIST_DIR}/test2.chained-input.txt"
    EXCLUDE_FROM_ALL
    INTERFACE
)

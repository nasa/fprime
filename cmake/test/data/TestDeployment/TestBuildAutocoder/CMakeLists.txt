register_fprime_module(
        TestBuildAutocoderModule
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/test1.test-build.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/test2.test-build.cpp"
    EXCLUDE_FROM_ALL
)

register_fprime_module(
        TestHeaderAutocoderModule
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/test1.test-build.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/test2.test-build.hpp"
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/test.cpp"
    EXCLUDE_FROM_ALL
)
target_include_directories(TestHeaderAutocoderModule PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
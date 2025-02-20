####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####
if (BUILD_TESTING)
    add_compile_options(
        -Wno-conversion
    )
endif()

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/TlmPacketizer.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/TlmPacketizer.cpp"
)

register_fprime_module()


### UTs ###
set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/TlmPacketizer.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/main.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/TlmPacketizerTester.cpp"
)
register_fprime_ut()


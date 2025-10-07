####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####

register_fprime_module(
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/GenericHub.fpp"
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/GenericHub.cpp"
    EXCLUDE_FROM_ALL
)
### UTs ###
register_fprime_ut(
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/GenericHub.fpp"
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/GenericHubTestMain.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/GenericHubTester.cpp"
    DEPENDS
        Fw_Com
        STest
)
set (UT_TARGET_NAME "${FPRIME_CURRENT_MODULE}_ut_exe")
if (TARGET "${UT_TARGET_NAME}")
    target_compile_options("${UT_TARGET_NAME}" PRIVATE -Wno-conversion)
endif()

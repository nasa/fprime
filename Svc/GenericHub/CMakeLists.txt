####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/GenericHub.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/GenericHubComponentImpl.cpp"
)

register_fprime_module()
### UTs ###
set(UT_SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/GenericHub.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/TestMain.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/Tester.cpp"
)
set(UT_MOD_DEPS
    Fw/Com
    STest
)
register_fprime_ut()

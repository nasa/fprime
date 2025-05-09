####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####
set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/ComStub.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/ComStub.cpp"
)
set(MOD_DEPS
    Fw/Logger
)
register_fprime_module()

set(UT_SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/ComStub.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/ComStubTester.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/ComStubTestMain.cpp"
)
set(UT_MOD_DEPS
    STest
)
set(UT_AUTO_HELPERS ON)
register_fprime_ut()

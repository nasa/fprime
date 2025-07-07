####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####
set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/ActivePhaserComponentAi.xml"
    "${CMAKE_CURRENT_LIST_DIR}/ActivePhaserComponentImpl.cpp"
)

register_fprime_module()

set(UT_MOD_DEPS
    STest
)
set(UT_SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/ActivePhaserComponentAi.xml"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/Tester.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/TestMain.cpp"
)
register_fprime_ut()
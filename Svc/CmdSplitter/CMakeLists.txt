set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/CmdSplitter.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/CmdSplitter.cpp"
)
register_fprime_module()

set(UT_SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/CmdSplitter.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/CmdSplitterTestMain.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/CmdSplitterTester.cpp"

)
set(UT_MOD_DEPS STest)
set(UT_AUTO_HELPERS ON)
register_fprime_ut()
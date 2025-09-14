register_fprime_module(
    AUTOCODER_INPUTS
       "${CMAKE_CURRENT_LIST_DIR}/CmdSplitter.fpp"
    SOURCES
       "${CMAKE_CURRENT_LIST_DIR}/CmdSplitter.cpp"
)

register_fprime_ut(
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/CmdSplitter.fpp"
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/CmdSplitterTestMain.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/CmdSplitterTester.cpp"
    UT_AUTO_HELPERS
    DEPENDS
        STest
)

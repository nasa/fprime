set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/DpWriter.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/DpWriter.fpp"
)

set(MOD_DEPS
  Fw/Dp
)

register_fprime_module()

set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/DpWriter.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/AbstractState.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/DpWriterTestMain.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/DpWriterTester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/Rules/BufferSendIn.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/Rules/CLEAR_EVENT_THROTTLE.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/Rules/FileOpenStatus.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/Rules/FileWriteStatus.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/Rules/SchedIn.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/Rules/Testers.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/Scenarios/Random.cpp"
)

set(UT_MOD_DEPS
  STest
)

set(UT_AUTO_HELPERS ON)
choose_fprime_implementation(Os/File Os_File_Test_Stub)
register_fprime_ut()

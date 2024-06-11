set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/DpManager.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/DpManager.cpp"
)

register_fprime_module()

set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/DpManager.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/DpManagerTestMain.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/DpManagerTester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/Rules/BufferGetStatus.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/Rules/CLEAR_EVENT_THROTTLE.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/Rules/ProductGetIn.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/Rules/ProductRequestIn.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/Rules/ProductSendIn.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/Rules/SchedIn.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/Rules/Testers.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/Scenarios/Random.cpp"
)

set(UT_MOD_DEPS
  STest
)

set(UT_AUTO_HELPERS ON)
register_fprime_ut()

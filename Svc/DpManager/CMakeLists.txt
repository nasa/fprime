
register_fprime_module(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/DpManager.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/DpManager.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/DpManager.hpp"
)

register_fprime_ut(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/DpManager.fpp"
  SOURCES
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
  DEPENDS
    STest
  UT_AUTO_HELPERS
)

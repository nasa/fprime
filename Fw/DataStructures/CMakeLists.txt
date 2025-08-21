set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/DataStructures.cpp"
)

register_fprime_module()

set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/ArrayMapTest.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/ArraySetOrMapImplTest.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/ArraySetTest.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/ArrayTest.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/CircularIndexTest.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/DataStructuresTestMain.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/ExternalArrayMapTest.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/ExternalArraySetTest.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/ExternalArrayTest.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/ExternalFifoQueueTest.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/ExternalStackTest.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/FifoQueueTest.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/STest/ArraySetOrMapImplTestRules.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/STest/ArraySetOrMapImplTestScenarios.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/STest/FifoQueueTestRules.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/STest/FifoQueueTestScenarios.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/STest/MapTestRules.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/STest/MapTestScenarios.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/STest/SetTestRules.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/STest/SetTestScenarios.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/STest/StackTestRules.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/STest/StackTestScenarios.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/StackTest.cpp"
)

set(UT_MOD_DEPS
  STest
)

register_fprime_ut()

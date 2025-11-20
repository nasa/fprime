set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/Sender.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/Sender.fpp"
)
set(MOD_DEPS
  Fw/Comp
  Fw/Types
  Os
  ${PROJECT_NAME}/topology/ports
)
register_fprime_module()

set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/Sender.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/SenderTestMain.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/SenderTester.cpp"
)

set(UT_MOD_DEPS
  FppTest/topology/ports
  STest
)

target_link_libraries(FppTest_topology_components_Sender PRIVATE gtest)

set(UT_AUTO_HELPERS ON)
register_fprime_ut()

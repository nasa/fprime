# Register the standard build
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/MathReceiver.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/MathReceiverComponentAi.xml"
)
register_fprime_module()

## Register the unit test build
#set(UT_SOURCE_FILES
#  "${CMAKE_CURRENT_LIST_DIR}/MathReceiverComponentAi.xml"
#  "${CMAKE_CURRENT_LIST_DIR}/test/ut/Tester.cpp"
#  "${CMAKE_CURRENT_LIST_DIR}/test/ut/main.cpp"
#)
#register_fprime_ut()

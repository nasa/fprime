set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/MathSender.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/MathSenderComponentAi.xml"
)

register_fprime_module()

set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/MathSenderComponentAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/Tester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/main.cpp"
)
register_fprime_ut()

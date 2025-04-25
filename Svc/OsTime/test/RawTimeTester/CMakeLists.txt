
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/RawTimeTester.cpp"
)

register_fprime_module()

register_fprime_implementation(RawTime Svc_OsTime_test_RawTimeTester)


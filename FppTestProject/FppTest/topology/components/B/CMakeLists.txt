set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/B.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/B.fpp"
)
register_fprime_module()

set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/B.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/BTestMain.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/BTester.cpp"
)

set(UT_MOD_DEPS
  STest
)

set(UT_AUTO_HELPERS ON)
register_fprime_ut()

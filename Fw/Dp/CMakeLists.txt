set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/Dp.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/DpContainer.cpp"
)
set(MOD_DEPS Utils/Hash)
register_fprime_module()

set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/Dp.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/TestMain.cpp"
)
set(UT_MOD_DEPS STest)
register_fprime_ut()

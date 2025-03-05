####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/FprimeDeframer.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/FprimeDeframer.cpp"
)

set(MOD_DEPS
    Utils/Types
    Svc/FprimeProtocol
)

register_fprime_module()


#### UTs ####
set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/FprimeDeframer.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/FprimeDeframerTester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/FprimeDeframerTestMain.cpp"
)
set(UT_MOD_DEPS
    STest
)
set(UT_AUTO_HELPERS ON)

register_fprime_ut()

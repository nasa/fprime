####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/Framer.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/Framer.cpp"
)

set(MOD_DEPS
    Svc/FramingProtocol
)

register_fprime_module()

#### UTS ###
set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/Framer.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/Tester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/TestMain.cpp"
)
register_fprime_ut()

####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
####
set(MOD_DEPS Os)

set(SOURCE_FILES
"${CMAKE_CURRENT_LIST_DIR}/UartFramer.fpp"
"${CMAKE_CURRENT_LIST_DIR}/UartFramer.cpp"
)
register_fprime_module()

## UTs ###
# Note: this UT expects user input
set(UT_MOD_DEPS
   Os
)
set(UT_SOURCE_FILES
 "${CMAKE_CURRENT_LIST_DIR}/UartFramer.fpp"
 "${CMAKE_CURRENT_LIST_DIR}/test/ut/TestMain.cpp"
 "${CMAKE_CURRENT_LIST_DIR}/test/ut/Tester.cpp"
)
register_fprime_ut()

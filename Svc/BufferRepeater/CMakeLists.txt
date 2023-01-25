####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoded files
# MOD_DEPS: (optional) module dependencies
#
####
set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/BufferRepeater.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/BufferRepeater.cpp"
)

register_fprime_module()

### UTs ###
set(UT_SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/BufferRepeater.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/TestMain.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/Tester.cpp"
)
register_fprime_ut()

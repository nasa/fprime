####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/Buffer.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/Buffer.cpp"
)

set(MOD_DEPS
    Fw/Types
)

register_fprime_module()

set(UT_SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/TestBuffer.cpp"
)
register_fprime_ut()

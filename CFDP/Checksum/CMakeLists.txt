####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/Checksum.cpp"
)

set(MOD_DEPS
  Fw/Cfg
  Fw/Types
)
register_fprime_module()

set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/Main.cpp"
)

set(UT_MOD_DEPS
  "${FPRIME_FRAMEWORK_PATH}/CFDP/Checksum"
  "${FPRIME_FRAMEWORK_PATH}/Fw/Types"
)
register_fprime_ut()
# Add GTest directory
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/GTest")

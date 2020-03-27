####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
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
  "${FPRIME_CORE_DIR}/CFDP/Checksum"
  "${FPRIME_CORE_DIR}/Fw/Types"
)
register_fprime_ut()
# Add GTest directory
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/GTest")

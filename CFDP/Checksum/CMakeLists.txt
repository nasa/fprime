####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####

register_fprime_module(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/Checksum.cpp"
  DEPENDS
    Fw/Types
)

register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/ChecksumMain.cpp"
  DEPENDS
    "${FPRIME_FRAMEWORK_PATH}/CFDP/Checksum"
    "${FPRIME_FRAMEWORK_PATH}/Fw/Types"
)
# Add GTest directory
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/GTest")

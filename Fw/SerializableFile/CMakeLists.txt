####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/SerializableFile.cpp"
)
set(MOD_DEPS
  Fw/Cfg
  Fw/Types
)
register_fprime_module()
##TODO-MSTARCH add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/test/TestSerializable")
#add_unit_test("${AUTOCODER_INPUT_FILES}" "${SOURCE_FILES};${CMAKE_CURRENT_LIST_DIR}/Test.cpp" "${LINK_DEPS};${CMAKE_CURRENT_LIST_DIR}/test/TestSerializable")

####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
if (BUILD_TESTING)
    add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/test/TestSerializable")
endif()
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/SerializableFile.cpp"
)
set(MOD_DEPS
    Fw/Cfg
    Fw/Types
    Os
)
register_fprime_module()
### UTs ###
set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/Test.cpp"
)
set(UT_MOD_DEPS
  "${FPRIME_FRAMEWORK_PATH}/Fw/SerializableFile"
  "${FPRIME_FRAMEWORK_PATH}/Fw/SerializableFile/test/TestSerializable"
  "${FPRIME_FRAMEWORK_PATH}/Fw/Types"
  "${FPRIME_FRAMEWORK_PATH}/Utils/Hash"
  "${FPRIME_FRAMEWORK_PATH}/Os"
)
register_fprime_ut()

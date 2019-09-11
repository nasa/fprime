####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
####
if (CMAKE_BUILD_TYPE STREQUAL "TESTING")
    add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/test/TestSerializable")
endif()
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/SerializableFile.cpp"
)
set(MOD_DEPS
  Fw/Cfg
  Fw/Types
)
# For shared libraries, Os must exist in the module list
if (BUILD_SHARED_LIBS)
    list(APPEND MOD_DEPS "Os")
endif()
register_fprime_module()
### UTs ###
set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/Test.cpp"
)
set(UT_MOD_DEPS
  "${FPRIME_CORE_DIR}/Fw/SerializableFile"
  "${FPRIME_CORE_DIR}/Fw/SerializableFile/test/TestSerializable"
  "${FPRIME_CORE_DIR}/Fw/Types"
  "${FPRIME_CORE_DIR}/Utils/Hash"
  "${FPRIME_CORE_DIR}/Os"
)
register_fprime_ut()

####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/Assert.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/MallocAllocator.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/MemAllocator.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/PolyType.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/SerialBuffer.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/Serializable.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/StringBase.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/StringUtils.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/StringToNumber.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/Types.fpp"
)
set(MOD_DEPS
  Fw/Cfg
)
register_fprime_module()
require_fprime_implementation("Fw_StringFormat")
### UTs ###
set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/ExternalSerializeBufferTest.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/TypesTest.cpp"
)
set(UT_MOD_DEPS
  "${FPRIME_FRAMEWORK_PATH}/Os"
)
register_fprime_ut()

# Non-test directory
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/GTest")

# Create an empty interface library and use it to register an implementation
add_library(snprintf-format ${VALID_EMPTY})
# Must suppress empty library warnings
target_compile_options(snprintf-format PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-Wno-unused-const-variable>)
register_fprime_implementation(Fw_StringFormat snprintf-format "${CMAKE_CURRENT_LIST_DIR}/snprintf_format.cpp")

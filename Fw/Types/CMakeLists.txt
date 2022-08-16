####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####
get_filename_component(PLATFORM_NAME "${CMAKE_TOOLCHAIN_FILE}" NAME)
if (NOT DEFINED FPRIME_PLATFORM_TYPES)
    message(FATAL_ERROR "Platform '${CMAKE_SYSTEM_NAME}.cmake' must define FPRIME_PLATFORM_TYPES")
elseif (NOT EXISTS "${FPRIME_PLATFORM_TYPES}/PlatformTypes.cpp" OR NOT EXISTS "${FPRIME_PLATFORM_TYPES}/PlatformTypes.hpp")
    message(FATAL_ERROR "Platform '${CMAKE_SYSTEM_NAME}.cmake' must define PlatformTypes.cpp and PlatformTypes.hpp in ${FPRIME_PLATFORM_TYPES}")
endif()

set(SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/Types.fpp"
  "${FPRIME_CONFIG_DIR}/FpConfig.cpp"
  "${FPRIME_PLATFORM_TYPES}/PlatformTypes.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/Assert.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/BasicTypes.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/String.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/InternalInterfaceString.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/MallocAllocator.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/MemAllocator.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/PolyType.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/SerialBuffer.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/Serializable.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/StringType.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/StringUtils.cpp"
)
set(MOD_DEPS
  Fw/Cfg
)
register_fprime_module()
### UTs ###
set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/TypesTest.cpp"
)
set(UT_MOD_DEPS
  "${FPRIME_FRAMEWORK_PATH}/Os"
)
register_fprime_ut()

# Non-test directory
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/GTest")

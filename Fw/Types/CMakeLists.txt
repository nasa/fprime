####
# F prime CMakeLists.txt:
####
register_fprime_module(
    Fw_Types
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/Assert.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/ConstStringBase.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/MallocAllocator.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/MemAllocator.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/PolyType.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/SerialBuffer.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Serializable.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/StringBase.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/StringUtils.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/StringToNumber.cpp"
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/Types.fpp"
  DEPENDS
    __fprime_config # Only module that should ever list __fprime_config in DEPENDS. Use Fw_Types instead.
  REQUIRES_IMPLEMENTATIONS
    Fw_StringFormat
    Fw_StringScan
)

register_fprime_implementation(
    Fw_StringFormat_snprintf
  IMPLEMENTS
    Fw_StringFormat
  DEPENDS
    Fw_Types
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/snprintf_format.cpp"
)

register_fprime_implementation(
    Fw_StringScan_sscanf
  IMPLEMENTS
    Fw_StringScan
  DEPENDS
    Fw_Types
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/sscanf_scan.cpp"
)


### UTs ###
register_fprime_ut(
    Fw_Types_ut_exe
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/ExternalSerializeBufferTest.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/AssertTypesTest.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/TypesTest.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/CAssertTest.cpp"
  DEPENDS
    Os
)
if (TARGET Fw_Types_ut_exe)
    target_compile_options(Fw_Types_ut_exe PRIVATE -Wno-conversion)
endif()

register_fprime_ut(
    Fw_StringFormat_snprintf_ut_exe
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/SnPrintfFormatTest.cpp"
  DEPENDS
    Fw_Types
  CHOOSES_IMPLEMENTATIONS
    Fw_StringFormat_snprintf
)


if (FPRIME_PLATFORM STREQUAL "Linux")
    register_fprime_ut(
        Fw_Types_Mmap_ut_exe
      SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/MmapAllocatorTest.cpp"
      DEPENDS
        Fw_Types
    )
endif()
register_fprime_ut(
    Fw_StringScan_sscanf_ut_exe
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/ScanfScanTest.cpp"
  DEPENDS
    Fw_Types
  CHOOSES_IMPLEMENTATIONS
    Fw_StringScan_sscanf
)

# Non-test directory
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/GTest")

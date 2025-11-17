####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####

register_fprime_module(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/FileUplink.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/FileUplink.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/File.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Warnings.cpp"
  DEPENDS
    Os
    Fw/Buffer
    Fw/FilePacket
    Utils/Hash
    CFDP/Checksum
)

### UTs ###

register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/FileUplink.fpp"
    ${CMAKE_CURRENT_LIST_DIR}/test/ut/FileUplinkTester.cpp
    ${CMAKE_CURRENT_LIST_DIR}/test/ut/FileUplinkMain.cpp
)
set (UT_TARGET_NAME "${FPRIME_CURRENT_MODULE}_ut_exe")
if (TARGET "${UT_TARGET_NAME}")
    target_compile_options("${UT_TARGET_NAME}" PRIVATE -Wno-conversion)
endif()

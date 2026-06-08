# Module subdirectories

add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Hash/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Types/")

#Source files


register_fprime_module(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/RateLimiter.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/TokenBucket.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CRCChecker.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/CRCChecker.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/RateLimiter.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/TestUtils.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/TokenBucket.hpp"
  DEPENDS
    Fw_Types
    Fw_Time
    Os
    Utils_Hash
)
register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/main.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/RateLimiterTester.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/TokenBucketTester.cpp"
  DEPENDS
    STest
    Fw_Types
    Fw_Time
    Os
)
set (UT_TARGET_NAME "${FPRIME_CURRENT_MODULE}_ut_exe")
if (TARGET "${UT_TARGET_NAME}")
    target_compile_options("${UT_TARGET_NAME}" PRIVATE -Wno-conversion)
endif()

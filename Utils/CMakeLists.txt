#Source files
set(SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/RateLimiter.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/TokenBucket.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/CRCChecker.cpp"
        )

set(MOD_DEPS
    Fw/Types
    Fw/Time
    Os
    Utils/Hash
   )

register_fprime_module()
set(UT_SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/main.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/RateLimiterTester.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/TokenBucketTester.cpp"
        )
set(UT_MOD_DEPS
        Fw/Types
        Fw/Time
        Os
        )
register_fprime_ut()

# Module subdirectories

add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Hash/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Types/")

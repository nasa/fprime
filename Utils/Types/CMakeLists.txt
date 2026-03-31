####
# F prime CMakeLists.txt:
####

register_fprime_module(
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/CircularBuffer.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Queue.cpp"
    DEPENDS
        Fw_Types

)

set (UT_TARGET_NAME "Types_Circular_Buffer_ut_exe")
register_fprime_ut("${UT_TARGET_NAME}"
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/CircularBuffer/CircularState.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/CircularBuffer/CircularRules.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/CircularBuffer/CircularBufferTester.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/CircularBuffer/Main.cpp"
    DEPENDS
        STest
        Fw_Types
    )
if (TARGET "${UT_TARGET_NAME}")
    target_compile_options("${UT_TARGET_NAME}" PRIVATE -Wno-conversion)
endif()

# Queue unit tests
register_fprime_ut(
    "Utils_Types_Queue_ut_exe"
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/Queue/QueueTest.cpp"
    DEPENDS
        STest
        Fw_Types
)
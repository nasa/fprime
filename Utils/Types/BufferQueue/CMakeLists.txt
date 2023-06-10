set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/BufferQueueCommon.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/PriorityBufferQueue.cpp"
)
set(MOD_DEPS Utils/Types/MaxHeap)
register_fprime_module()

set(UT_SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/BufferQueueTest.cpp"
)
register_fprime_ut()
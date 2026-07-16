register_fprime_module(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/MaxHeap.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/AtomicQueue.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/MaxHeap.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/AtomicQueue.hpp"
  DEPENDS
    Fw_Types
    Fw_Logger
    Os_CountingSemaphore
)

# Rules based unit testing

set(UT_TARGET_NAME "Types_Max_Heap_test")
register_fprime_ut("${UT_TARGET_NAME}"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/MaxHeap/MaxHeapTest.cpp"
  DEPENDS
    STest
    Fw_Types
)
if (TARGET "${UT_TARGET_NAME}")
    target_compile_options("${UT_TARGET_NAME}" PRIVATE -Wno-conversion)
endif()

# AtomicQueue unit test
set(UT_TARGET_NAME "Types_Atomic_Queue_test")
register_fprime_ut("${UT_TARGET_NAME}"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/AtomicQueue/AtomicQueueTest.cpp"
  DEPENDS
    STest
    Fw_Types
    Fw_Time
    Os
    Os_CountingSemaphore
)
if (TARGET "${UT_TARGET_NAME}")
    target_compile_options("${UT_TARGET_NAME}" PRIVATE -Wno-conversion)
endif()

add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Types")
add_custom_target("${FPRIME_CURRENT_MODULE}")

#### Os/Generic/Queue Section ####
register_fprime_module(
    Os_Generic_PriorityQueue_Implementation
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/PriorityQueue.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/PriorityQueue.hpp"
  DEPENDS
    Fw_Types
    Os_Generic_Types
)
register_fprime_implementation(
    Os_Generic_PriorityQueue
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/DefaultPriorityQueue.cpp"
  IMPLEMENTS
    Os_Queue
  DEPENDS
    Fw_Types
    Os_Generic_PriorityQueue_Implementation
)

register_fprime_ut(
    PriorityQueueTest
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/PriorityQueueTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/../test/ut/queue/CommonTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/../test/ut/queue/QueueRules.cpp"
  DEPENDS
    Fw_Types
    Fw_Time
    Os
    STest
  CHOOSES_IMPLEMENTATIONS
    Os_Generic_PriorityQueue
)
if (TARGET PriorityQueueTest)
    target_compile_options(PriorityQueueTest PRIVATE -Wno-conversion)
    target_include_directories(PriorityQueueTest PRIVATE "${CMAKE_CURRENT_LIST_DIR}/test/ut")
endif()

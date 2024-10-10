add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Types")
add_custom_target("${FPRIME_CURRENT_MODULE}")

#### Os/Generic/Queue Section ####
set(SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/PriorityQueue.cpp"
)
set(MOD_DEPS
    Os_Generic_Types
)
set(HEADER_FILES
        "${CMAKE_CURRENT_LIST_DIR}/PriorityQueue.hpp"
)
register_fprime_module(Os_Generic_PriorityQueue)
register_fprime_implementation(Os/Queue Os_Generic_PriorityQueue "${CMAKE_CURRENT_LIST_DIR}/DefaultPriorityQueue.cpp")

set(UT_SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/PriorityQueueTests.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/../test/ut/queue/CommonTests.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/../test/ut/queue/QueueRules.cpp"
)
set(UT_MOD_DEPS
        Fw_Time
        Os
        STest
)
choose_fprime_implementation(Os/Queue Os_Generic_PriorityQueue)
register_fprime_ut(PriorityQueueTest)
if (TARGET PriorityQueueTest)
    target_include_directories(PriorityQueueTest PRIVATE "${CMAKE_CURRENT_LIST_DIR}/test/ut")
endif()
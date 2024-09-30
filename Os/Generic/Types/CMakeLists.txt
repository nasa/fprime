set(SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/MaxHeap.cpp"
)
set(MOD_DEPS
        "Fw/Types"
)
register_fprime_module()

# Rules based unit testing
set(UT_MOD_DEPS
        STest
        Fw/Types
)

set(UT_SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/MaxHeap/MaxHeapTest.cpp"
)
register_fprime_ut("Types_Max_Heap_test")
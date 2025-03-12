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
set(UT_TARGET "Types_Max_Heap_test")
register_fprime_ut("${UT_TARGET}")
if (TARGET "${UT_TARGET}")
    target_compile_options("${UT_TARGET}" PRIVATE -Wno-conversion)
endif()

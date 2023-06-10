set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/MaxHeap.cpp"
)
register_fprime_module()

# Third  UT Pthreads MAX Heap
set(UT_SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/MaxHeapTest.cpp"
)
register_fprime_ut()

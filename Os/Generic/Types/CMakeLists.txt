

register_fprime_module(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/MaxHeap.cpp"
  DEPENDS
    "Fw/Types"
)

# Rules based unit testing
register_fprime_ut(
  "Types_Max_Heap_test"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/MaxHeap/MaxHeapTest.cpp"
  DEPENDS
    STest
    Fw/Types
)
set(UT_TARGET_NAME "Types_Max_Heap_test")
if (TARGET "${UT_TARGET_NAME}")
    target_compile_options("${UT_TARGET_NAME}" PRIVATE -Wno-conversion)
endif()

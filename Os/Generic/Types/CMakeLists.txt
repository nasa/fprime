register_fprime_module(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/MaxHeap.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/MaxHeap.hpp"
  DEPENDS
    Fw_Types
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

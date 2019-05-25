# Add codegen target to cache
set(CODEGEN_TARGET "codegen" CACHE INTERNAL "Target for codegen autocoder")

# Create target for autocoder
add_library(
  ${CODEGEN_TARGET}
  STATIC
  ${EMPTY_C_SRC}
)

include("${CMAKE_CURRENT_LIST_DIR}/channels/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/commands/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/component/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/events/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/gtest/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/html/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/impl/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/md/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/port/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/serialize/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/test/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/test_impl/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/topology/CMakeLists.txt")

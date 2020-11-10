# Add codegen target to cache
set(CODEGEN_TARGET "codegen" CACHE INTERNAL "Target for codegen autocoder")
set_property(GLOBAL PROPERTY CODEGEN_OUTPUTS)

# Create target for autocoder

include("${CMAKE_CURRENT_LIST_DIR}/arrays/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/channels/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/commands/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/component/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/events/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/enums/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/gtest/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/html/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/impl/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/md/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/port/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/serialize/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/test/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/test_impl/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/topology/CMakeLists.txt")
get_property(CODEGEN_OUTPUTS_PYS GLOBAL PROPERTY CODEGEN_OUTPUTS)
add_custom_target(${CODEGEN_TARGET} DEPENDS ${CODEGEN_OUTPUTS_PYS})

# Include the options used as part of the build system
include("${CMAKE_CURRENT_LIST_DIR}/Options.cmake")
# Add platform target_sources
include("${CMAKE_CURRENT_LIST_DIR}/platform/CMakeLists.txt")

# Add custom source directory
include("${CMAKE_CURRENT_LIST_DIR}/src/CMakeLists.txt")

# Add parser directory
include("${CMAKE_CURRENT_LIST_DIR}/parser/CMakeLists.txt")

# Bring in the rest of the build system
include("${CMAKE_CURRENT_LIST_DIR}/Executable.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/Module.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/Utils.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/Unit_Test.cmake")

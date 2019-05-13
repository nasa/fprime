
# set gtest target name
set(GTEST_TARGET "gtest${BUILD_SUFFIX}" CACHE INTERNAL "Target for gtest infrastructure")

# Create gtest static library
add_library(
  ${GTEST_TARGET}
  STATIC
  "${CMAKE_CURRENT_LIST_DIR}/src/gtest-all.cc"
)

# Add include paths for gtest
target_include_directories(
  ${GTEST_TARGET}
  PUBLIC "${CMAKE_CURRENT_LIST_DIR}" "${CMAKE_CURRENT_LIST_DIR}/include"
)

# Module subdirectories

# Port subdirectories
add_library(
    "STest${BUILD_SUFFIX}"
    "${CMAKE_CURRENT_LIST_DIR}/STest/Random/bsd_random.c"
    "${CMAKE_CURRENT_LIST_DIR}/STest/Random/Random.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/STest/Pick/Pick_default.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/STest/Pick/Pick.cpp"
)
target_include_directories(STest${BUILD_SUFFIX} PUBLIC "${CMAKE_CURRENT_LIST_DIR}")

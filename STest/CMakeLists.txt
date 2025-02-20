# Module subdirectories

add_compile_options(
    -Wno-conversion
)

# Port subdirectories
add_library(
    "STest"
    "${CMAKE_CURRENT_LIST_DIR}/STest/Random/bsd_random.c"
    "${CMAKE_CURRENT_LIST_DIR}/STest/Random/Random.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/STest/Pick/Pick_default.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/STest/Pick/Pick.cpp"
)
target_include_directories(STest PUBLIC "${CMAKE_CURRENT_LIST_DIR}")

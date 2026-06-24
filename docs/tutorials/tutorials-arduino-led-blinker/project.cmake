# This CMake file is intended to register project-wide objects so they can be
# reused easily between deployments, but also by other projects.

# Components
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Components/")

# Deployments
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/LedBlinker/")

####
# settings.cmake:
#
# Required build settings for building against the fprime framework. This list **should** be kept small, only setting
# items that are strictly required for building fprime.
####

# fprime framework is build with C++11 and C99 support, project may override these setting after the inclusion of
# Fprime-Code.cmake.
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# fprime unit testing methodology requires the following flags
if (BUILD_TESTING)
    add_compile_options(-g -DBUILD_UT -DPROTECTED=public -DPRIVATE=public -DSTATIC=)
endif()

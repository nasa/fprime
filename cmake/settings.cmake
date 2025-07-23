####
# settings.cmake:
#
# Required build settings for building against the fprime framework. This list **should** be kept small, only setting
# items that are strictly required for building fprime. Settings must be restricted to items that are required to ensure
# fprime builds properly.
#
# Required settings:
# - C standard 99
# - C++ 11 standard, fprime uses C++11 items
# - UT flags overrides
####
include_guard()
# fprime framework is build with C++11 and C99 support, project may override these settings, but results are not guaranteed
# when overriding for F Prime framework code.
if (NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()
if (NOT DEFINED CMAKE_C_STANDARD)
    set(CMAKE_C_STANDARD 99)
    set(CMAKE_C_STANDARD_REQUIRED ON)
    set(CMAKE_C_EXTENSIONS OFF)
endif()
 
# fprime unit testing methodology requires the following flags
if (BUILD_TESTING)
    add_compile_options(-g -DBUILD_UT)
endif()

####
# CMakeLists.txt:
#
# Build core F prime.
####
cmake_minimum_required(VERSION 3.5)
project(FPrime C CXX)
set(FPRIME_FRAMEWORK_PATH "${CMAKE_CURRENT_LIST_DIR}" CACHE PATH "Location of F prime framework" FORCE)
set(FPRIME_PROJECT_ROOT "${CMAKE_CURRENT_LIST_DIR}" CACHE PATH "Root path of F prime project" FORCE)

# This cmake project is only intended to be used by CI and developers to test F-prime. We enable
# -pedantic, -Wall and -Wextra on this particular project as a canary to warn about compilation
# errors without impacting real projects, where a warning from an untested compiler could break the
# build.
#
# -Wno-unused-parameter: Disable the unused parameter warning for now. F' has a lot of interfaces,
# so unused method parameters are common in the F prime code base. Eventually all intentionally
# unused parameters should be annotated to avoid this error.
#
# -Wno-gnu-zero-variadic-macro-arguments: gnu extension required to allow FW_ASSERT to support 0+
# optional arguments
#
# -Wno-vla-extension: Variable length arrays are required to support sending to async serializable
# ports. https://github.com/nasa/fprime/issues/945
#
# -Wno-zero-length-array: maximum port message size calculated by using sizeof on an array the size
# of all the port's arguments. When port has no argument array is zero sized. Array is used at
# compile time only.ß
add_compile_options(
    -pedantic
    -Wall
    -Wextra
    -Werror
    $<$<COMPILE_LANGUAGE:CXX>:-Wold-style-cast>
    -Wno-unused-parameter
    -Wno-gnu-zero-variadic-macro-arguments
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-vla-extension>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-zero-length-array>
)

# For this testing cmake project, enable AddressSanitizer, a runtime memory sanitizer, on all unit tests
set (CMAKE_C_FLAGS_TESTING "${CMAKE_C_FLAGS_TESTING} -fno-omit-frame-pointer -fsanitize=address")
set (CMAKE_CXX_FLAGS_TESTING "${CMAKE_CXX_FLAGS_TESTING} -fno-omit-frame-pointer -fsanitize=address")
set (CMAKE_LINKER_FLAGS_TESTING "${CMAKE_LINKER_FLAGS_TESTING} -fno-omit-frame-pointer -fsanitize=address")

# Include the build for F prime.
include("${CMAKE_CURRENT_LIST_DIR}/cmake/FPrime.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/cmake/FPrime-Code.cmake")

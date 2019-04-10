####
# Linux-UnitTests.cmake:
#
# Linux platform file for standard linux target supporting the unit-test specific flags.
####
# Use common linux setup
include("${CMAKE_CURRENT_LIST_DIR}/Linux-common.cmake")

set(BUILD_FLAGS
    "-DBUILD_UT"
    "-DPROTECTED=public"
    "-DPRIVATE=public"
)
set(BUILD_SUFFIX "_linux_ut")

set(UT_BUILD TRUE)
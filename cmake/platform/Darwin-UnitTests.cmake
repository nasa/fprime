####
# Darwin-UnitTests.cmake:
#
# Darwin based platfrom file used for Darwin (Mac OS X) targets.
####
# Use common linux setup
include("${CMAKE_CURRENT_LIST_DIR}/Darwin.cmake")

set(BUILD_FLAGS
    "-DBUILD_UT"
    "-DPROTECTED=public"
    "-DPRIVATE=public"
)
set(BUILD_SUFFIX "darwin_ut")

set(UT_BUILD TRUE)

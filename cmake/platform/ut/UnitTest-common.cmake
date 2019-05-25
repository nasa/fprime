####
# UnitTest-common.cmake:
#
# Unit tests common package. This sets up the F prime unit test compilation flags, and a suffix for
# compiling the UTs. In addition it must set the "UT_BUILD" flag to "TRUE" to allow registration of
# Unit Tests using the baked-in helpers.
####
set(BUILD_FLAGS
    "-DBUILD_UT"
    "-DPROTECTED=public"
    "-DPRIVATE=public"
)
set(BUILD_SUFFIX "_ut")
set(UT_BUILD TRUE)
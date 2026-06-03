####
# Linux.cmake:
#
# Test platform file provided by lib/test-platform-library.
# Defers to the framework Linux platform and sets a marker to prove this file was loaded.
####
include("${FPRIME_FRAMEWORK_PATH}/cmake/platform/Linux.cmake")
set(TEST_PLATFORM_LIBRARY_LOADED TRUE CACHE INTERNAL "Marker proving the library platform file was loaded")

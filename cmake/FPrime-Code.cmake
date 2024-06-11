####
# FPrime-Code:
#
# FPrime code. This cmake file includes the basic directories that make up the mainline F prime framework. This is
# separated from the CMake includes themselves such that this is built after all CMake setup.
#
# Note: given F prime's historical folder structure, this is not organized as an fprime library.
####
include_guard()

# Ensure that the FPrime build system is setup
include("${CMAKE_CURRENT_LIST_DIR}/FPrime.cmake")

# Setup fprime code followed by all libraries
fprime_setup_included_code()

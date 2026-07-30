####
# fprime_install.cmake:
#
# Wrapper script invoked by POST_BUILD commands to install build artifacts.
# Defaults DESTDIR to FPRIME_INSTALL_DEST (passed via -D) when the user
# has not set DESTDIR in the environment. Sets CMAKE_INSTALL_PREFIX to "/"
# so that DESTDIR alone controls the final install location.
#
# Expected -D arguments:
#   FPRIME_INSTALL_DEST - default install directory (from configure step)
#   FPRIME_BUILD_DIR    - path to the CMake binary directory
#   CMAKE_INSTALL_COMPONENT - install component to process
####
if(NOT DEFINED ENV{DESTDIR})
    set(ENV{DESTDIR} "${FPRIME_INSTALL_DEST}")
endif()
set(CMAKE_INSTALL_PREFIX "/")
include("${FPRIME_BUILD_DIR}/cmake_install.cmake")

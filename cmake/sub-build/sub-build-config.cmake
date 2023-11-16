####
# sub-build-config.cmake:
#
# Contains configuration properties for sub-builds. These settings are global and apply to all sub-builds, but are
# updated occasionally as understanding of sub-builds is developed.
####
include_guard()

# List of types of variables that are excluded from the list passed to sub-builds.
set(TYPES_DISALLOWED_LIST
    INTERNAL
    STATIC
)

# List of cache variables that shall be excluded from the list passed to sub-builds.
set(SUB_BUILD_EXCLUDED_CACHE_VARIABLES
    CMAKE_EDIT_COMMAND
    CMAKE_HOME_DIRECTORY
    CMAKE_INSTALL_NAME_TOOL
    FPRIME_BUILD_LOCATIONS
    FPRIME_SUB_BUILD
    FPRIME_VERSION_SCRIPT
    CMAKE_C_COMPILER_FORCED
    CMAKE_CXX_COMPILER_FORCED
    FPRIME_SKIP_TOOLS_VERSION_CHECK
    CMAKE_HAVE_LIBC_PTHREAD
    CMAKE_HAVE_PTHREAD_H
    CMAKE_SKIP_INSTALL_ALL_DEPENDENCY
    FIND_PACKAGE_MESSAGE_DETAILS_Threads
    BUILD_GMOCK
    CMAKE_INSTALL_BINDIR
    CMAKE_INSTALL_DATAROOTDIR
    CMAKE_INSTALL_INCLUDEDIR
    CMAKE_INSTALL_LIBDIR
    CMAKE_INSTALL_LIBEXECDIR
    CMAKE_INSTALL_LOCALSTATEDIR
    CMAKE_INSTALL_OLDINCLUDEDIR
    CMAKE_INSTALL_SBINDIR
    CMAKE_INSTALL_SHAREDSTATEDIR
    CMAKE_INSTALL_SYSCONFDIR
    INSTALL_GTEST
    VALGRIND
    gmock_build_tests
    gtest_build_samples
    gtest_build_tests
    gtest_disable_pthreads
    gtest_force_shared_crt
    gtest_hide_internal_symbols
)


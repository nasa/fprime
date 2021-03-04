####
# Command Line Options:
#
# Command line options are used to change how the F prime CMake setup builds an F prime deployment. These options
# typically have sensible defaults that configure the build in the most common way.  Most users do not need to use these
# options directly, but may choose to do so.
#
# Note: some deployments may specify their own `-D` cmake flags and these deployments should take care to ensure there
# is no collision with the arguments described here.
#
# Users need not specified any of the options to build F prime, however; when non-standard build behavior is desired,
# then these options can be used. These options are specified with the -D<OPTION>=<VALUE> flag. Usually the value is
# "ON" or "OFF". Other values are documented along side the option.
#
# Note: `fprime-util` will specify some settings for users. Additional settings can be passed through fprime-util with
# the `-D` option there.
#
####

####
# `CMAKE_DEBUG_OUTPUT:`
#
# Turns on the reporting of debug output of the CMake build. Can help refine the CMake system, and repair errors. For
# normal usage, this is not necessary.
#
# **Values:**
# - ON: generate debugging output
# - OFF: (default) do *not* generate debugging output
#
# e.g. `-DCMAKE_DEBUG_OUTPUT=ON`
####
option(CMAKE_DEBUG_OUTPUT "Generate F prime's debug output while running CMake" OFF)


####
# `SKIP_TOOLS_CHECK:`
#
# For older clients, some IDEs, and other unique builds, the check that ensures tools are available can fail causing
# build instability. This option overrides the tools check enabling the system to run.
#
# **Values:**
# - ON: skip tools check
# - OFF: (default) run tools check
#
# e.g. `-DSKIP_TOOLS_CHECK=ON`
####
option(SKIP_TOOLS_CHECK "Skip the tools check for older clients." OFF)

# Set build type, when it hasn't been set
if(NOT CMAKE_BUILD_TYPE) 
    set(CMAKE_BUILD_TYPE RELEASE)
else()
    string(TOUPPER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE)
endif()

####
# `CMAKE_BUILD_TYPE:`
#
# Allows for setting the CMake build type. Release is a normal build, Testing is used for unit testing and debug
# options.
#
# **Values:**
# - Release: (default) standard flight build
# - Testing: allow for unit tests and debug enabled build
# - Debug: supplied by CMake and typlically unused for F prime
#
# e.g. `-DCMAKE_BUILD_TYPE=TESTING`
####
SET(CMAKE_CXX_FLAGS_TESTING "-g -DBUILD_UT -DPROTECTED=public -DPRIVATE=public -DSTATIC= -fprofile-arcs -ftest-coverage"
    CACHE STRING "Testing C++ flags." FORCE)
SET(CMAKE_C_FLAGS_TESTING "-g -DBUILD_UT -DPROTECTED=public -DPRIVATE=public -DSTATIC= -fprofile-arcs -ftest-coverage"
    CACHE STRING "Testing C flags." FORCE)
SET(CMAKE_EXE_LINKER_FLAGS_TESTING "" CACHE STRING "Testing linker flags." FORCE)
SET(CMAKE_SHARED_LINKER_FLAGS_TESTING "" CACHE STRING "Testing linker flags." FORCE)
MARK_AS_ADVANCED(
    CMAKE_CXX_FLAGS_TESTING
    CMAKE_C_FLAGS_TESTING
    CMAKE_EXE_LINKER_FLAGS_TESTING
    CMAKE_SHARED_LINKER_FLAGS_TESTING )
# Testing setup for UT and coverage builds
if (CMAKE_BUILD_TYPE STREQUAL "TESTING" )
    # These two lines allow for F prime style coverage. They are "unsupported" CMake features, so beware....
    set(CMAKE_C_OUTPUT_EXTENSION_REPLACE 1)
    set(CMAKE_CXX_OUTPUT_EXTENSION_REPLACE 1)
endif()

####
# `CMAKE_TOOLCHAIN_FILE:`
#
# CMake option to specify toolchain file. For F prime, toolchains are kept in the framework and library cmake/toolchain
# folder, although theoretically any CMake toolchain can be used. Default: none, which will use the native system build.
#
# e.g. `-DCMAKE_TOOLCHAIN_FILE=/path/to/cmake/toolchain`
####

####
# `PLATFORM:`
#
# Specifies the platform used when building the F prime using the CMake system. See:
# [platform.md](platform.md) for more information. Default: automatically detect platform file.
#
# e.g. `-DPLATFORM=/path/to/platform/cmake`
####


####
# Locations `FPRIME_FRAMEWORK_PATH`, `FPRIME_PROJECT_ROOT`, `FPRIME_LIBRARY_LOCATIONS`
# `FPRIME_AC_CONSTANTS_FILE`, and `FPRIME_CONFIG_DIR`:
#
# Note: these settings are supplied by `fprime-util` and need not be provided unless running CMake directly or through
# any way bypassing that utility (e.g. inside your beloved IDE).
#
# These locations specify the locations of the needed F prime paths. These are described below. Defaults are set to
# support the historical in-source deployments where F prime is merged with deployment code. Specify these settings if
# using the newer deployment structure. `fprime-util` does this for you.
#
# FPRIME_FRAMEWORK_PATH: location of F prime framework installation, always the directory above this file, however;
# since it is supplied by the project, it is validated to ensure that it points to a valid F prime framework install.
# Default: the folder above this file.
#
# e.g. `-DFPRIME_FRAMEWORK_PATH=/path/to/fprime/framework`
#
# FPRIME_PROJECT_ROOT: root path of an F prime project. This is used for relative paths for c++ includes, component
# includes, etc. Default is FPRIME_FRAMEWORK_PATH if the PROJECT_SOURCE_DIR is a child of FPRIME_FRAMEWORK_PATH
# otherwise PROJECT_SOURCE_DIR is used as the project root.
#
# e.g. `-DFPRIME_FRAMEWORK_PATH=/path/to/fprime/project`
#
# FPRIME_LIBRARY_LOCATIONS: locations of libraries included in the build. CMake list supplied in ; separated format like
# other CMake lists. Default: "", no libraries available.
#
# e.g. `-DFPRIME_LIBRARY_LOCATIONS=/path/to/fprime/library1;/path/to/fprime/library2`
#
# Additional locations FPRIME_SETTINGS_FILE and FPRIME_ENVIRONMENT_FILE:
#
# These files are used for settings in fprime-util. If supplied (typically only by fprime-util) then
# they will be added as dependencies into the build system.
####
get_filename_component(DETECTED_FRAMEWORK_PATH "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
if (DEFINED FPRIME_FRAMEWORK_PATH)
     get_filename_component(FPRIME_FRAMEWORK_PATH "${FPRIME_FRAMEWORK_PATH}" ABSOLUTE)
     # Sanity check the framework path as supplied
     if (NOT FPRIME_FRAMEWORK_PATH STREQUAL DETECTED_FRAMEWORK_PATH)
         message(FATAL_ERROR "Inconsistent FPrime location: ${FPRIME_FRAMEWORK_PATH}. Check settings.ini")
     endif()
endif()
# Force framework path to be absolute
set(FPRIME_FRAMEWORK_PATH "${DETECTED_FRAMEWORK_PATH}" CACHE PATH "F Prime framework location" FORCE)

# Setup project root
get_filename_component(FULL_PROJECT_PATH "${CMAKE_PROJECT_DIR}" ABSOLUTE)
file(RELATIVE_PATH TEMP_PATH "${FPRIME_FRAMEWORK_PATH}" "${FULL_PROJECT_PATH}")
# If defined then force it to be absolute
if (DEFINED FPRIME_PROJECT_ROOT)
    get_filename_component(FPRIME_PROJECT_ROOT_ABS "${FPRIME_PROJECT_ROOT}" ABSOLUTE)
    set(FPRIME_PROJECT_ROOT "${FPRIME_PROJECT_ROOT_ABS}" CACHE PATH "F Prime project location" FORCE)
# Forces framework path as project root, if a child
elseif( "${TEMP_PATH}" MATCHES "^[^./].*" )
    set(FPRIME_PROJECT_ROOT "${FPRIME_FRAMEWORK_PATH}" CACHE PATH "F Prime project location" FORCE)
# Force PROJECT_ROOT
else()
    set(FPRIME_PROJECT_ROOT "${FULL_PROJECT_PATH}" CACHE PATH "F Prime project location" FORCE)
endif()
# Force  FPRIME_LIBRARY_LOCATIONS to be absolute
set(FPRIME_LIBRARY_LOCATIONS_ABS)
foreach (LIBLOC  ${FPRIME_LIBRARY_LOCATIONS})
    get_filename_component(LIBLOC_ABS "${LIBLOC}" ABSOLUTE)
    list(APPEND FPRIME_LIBRARY_LOCATIONS_ABS "${LIBLOC_ABS}")
endforeach()
set(FPRIME_LIBRARY_LOCATIONS "${FPRIME_LIBRARY_LOCATIONS_ABS}" CACHE STRING "F prime library locations" FORCE)
# Add in addition file dependencies from CMake
if (DEFINED FPRIME_SETTINGS_FILE)
    get_filename_component(FPRIME_SETTINGS_FILE  "${FPRIME_SETTINGS_FILE}" ABSOLUTE)
    set(FPRIME_SETTINGS_FILE "${FPRIME_SETTINGS_FILE}" CACHE PATH "F prime settings file" FORCE)
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${FPRIME_SETTINGS_FILE}")
endif()
if (DEFINED FPRIME_ENVIRONMENT_FILE)
    get_filename_component(FPRIME_ENVIRONMENT_FILE  "${FPRIME_ENVIRONMENT_FILE}" ABSOLUTE)
    set(FPRIME_ENVIRONMENT_FILE "${FPRIME_ENVIRONMENT_FILE}" CACHE PATH "F prime environment file" FORCE)
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${FPRIME_ENVIRONMENT_FILE}")
endif()
# Override the AC constants file when specified
if (NOT DEFINED FPRIME_AC_CONSTANTS_FILE)
    set(FPRIME_AC_CONSTANTS_FILE "${FPRIME_FRAMEWORK_PATH}/config/AcConstants.ini" CACHE PATH "F prime AC constants.ini file" FORCE)
endif()
# Settings for F config directory
if (NOT DEFINED FPRIME_CONFIG_DIR)
    set(FPRIME_CONFIG_DIR "${FPRIME_FRAMEWORK_PATH}/config/" CACHE PATH "F prime configuration header directory" FORCE)
endif()

# Settings for F artifacts installation destination
if (NOT DEFINED FPRIME_INSTALL_DEST)
    set(FPRIME_INSTALL_DEST "${PROJECT_SOURCE_DIR}/build-artifacts/" CACHE PATH "F prime artifacts installation directory" FORCE)
endif()
set(IGNORE_ME ${FPRIME_INSTALL_DEST}) # Prevent warning that FPRIME_INSTALL_DEST is unused

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
# `FPRIME_USE_STUBBED_DRIVERS:`
#
# Tells fprime to use the specific stubbed set of drivers as opposed to full implementation. This applies to drivers in
# the Drv package with the exception of the serial and ipv4 drivers where a generic cross-platform solution is expected.
#
# If unspecified, it will be set in the platform file for the give architecture. If specified, may be set to ON to use
# the stubbed drivers or OFF to used full driver implementations.
###
if (DEFINED FPRIME_USE_STUBBED_DRIVERS AND NOT "${FPRIME_USE_STUBBED_DRIVERS}" STREQUAL "ON" AND NOT "${FPRIME_USE_STUBBED_DRIVERS}" STREQUAL "OFF")
    message(FATAL_ERROR "FPRIME_USE_STUBBED_DRIVERS must be set to ON, OFF, or not supplied at all")
endif()

####
# `FPRIME_USE_BAREMETAL_SCHEDULER:`
#
# Tells fprime to use the baremetal scheduler. This scheduler replaces any OS scheduler with one that loops through
# active components calling each one dispatch at a time. This is designed for use with baremetal (no-OS) system,
# however; it may be set to limit execution to a single thread and or test the baremetal scheduler on a PC.
#
# If unspecified, it will be set in the platform file for the give architecture. If specified, may be set to ON to use
# the scheduler or OFF to use the OS thread scheduler.
###
if (DEFINED FPRIME_USE_BAREMETAL_SCHEDULER AND NOT "${FPRIME_USE_BAREMETAL_SCHEDULER}" STREQUAL "ON" AND NOT "${FPRIME_USE_BAREMETAL_SCHEDULER}" STREQUAL "OFF")
    message(FATAL_ERROR "FPRIME_USE_BAREMETAL_SCHEDULER must be set to ON, OFF, or not supplied at all")
endif()


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
# `FPRIME_ENABLE_FRAMEWORK_UTS:`
#
# Allow a project to to run fprime UTs from the core framework. Default: off, do not run fprime framework UTs. This
# does not affect project specified UTs.
#
# **Values:**
# - ON: (default) adds framework UT targets to the total list of targets
# - OFF: do not add framework UTs to the target list
#
# e.g. `-DFPRIME_ENABLE_FRAMEWORK_UTS=OFF`
####
option(FPRIME_ENABLE_FRAMEWORK_UTS "Enable framework UT generation" ON)
if (NOT FPRIME_ENABLE_FRAMEWORK_UTS)
    message(WARNING "-DFPRIME_ENABLE_FRAMEWORK_UTS=OFF will be deprecated in a future release")
endif()

####
# `FPRIME_ENABLE_AUTOCODER_UTS:`
#
# When FPRIME_ENABLE_FRAMEWORK_UTS is set, this allows a projects to also enable running the autocoder UTs which do not
# represent the correctness of the C++/product software, but rather the operation of the autocoder tools.
#
# **Values:**
# - ON: (default) retains the autocoder UTs in the target list
# - OFF: removes autocoder UTs from the target list
#
# e.g. `-DFPRIME_ENABLE_AUTOCODER_UTS=OFF`
####
option(FPRIME_ENABLE_AUTOCODER_UTS "Enable autocoder UT generation" ON)
if (NOT FPRIME_ENABLE_AUTOCODER_UTS)
    message(WARNING "-DFPRIME_ENABLE_AUTOCODER_UTS=OFF will be deprecated in a future release")
endif()

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
# - Debug: supplied by CMake and typically unused for F prime
#
# e.g. `-DCMAKE_BUILD_TYPE=TESTING`
####
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

if (CMAKE_BUILD_TYPE STREQUAL "Testing" OR CMAKE_BUILD_TYPE STREQUAL "TESTING")
    set(CMAKE_CXX_STANDARD 11)
    add_compile_options("-g" "-DBUILD_UT" "-DPROTECTED=public" "-DPRIVATE=public" "-DSTATIC=" "-fprofile-arcs" "-ftest-coverage")
    link_libraries("--coverage")
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
    set(FPRIME_CONFIG_DIR "${FPRIME_FRAMEWORK_PATH}/config/")
endif()
set(FPRIME_CONFIG_DIR "${FPRIME_CONFIG_DIR}" CACHE PATH "F prime configuration header directory" FORCE)

# Settings for F artifacts installation destination
if (NOT DEFINED FPRIME_INSTALL_DEST)
    set(FPRIME_INSTALL_DEST "${PROJECT_SOURCE_DIR}/build-artifacts/")
endif()
set(FPRIME_INSTALL_DEST "${FPRIME_INSTALL_DEST}" CACHE PATH "F prime artifacts installation directory" FORCE)

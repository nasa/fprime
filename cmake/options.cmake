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
# Remap changed settings
if (DEFINED FPRIME_INSTALL_DEST)
    set(CMAKE_INSTALL_PREFIX ${FPRIME_INSTALL_DEST} CACHE PATH "Install dir" FORCE)
endif()
include("settings/ini")
ini_to_cache()


####
# `CMAKE_TOOLCHAIN_FILE:`
#
# CMake option to specify toolchain file. For F prime, toolchains are kept in the framework and library cmake/toolchain
# folder, although theoretically any CMake toolchain can be used. Default: none, which will use the native system build.
#
# e.g. `-DCMAKE_TOOLCHAIN_FILE=/path/to/cmake/toolchain`
####

####
# `CMAKE_DEBUG_OUTPUT`:
#
# Turns on the reporting of debug output of the CMake build. Can help refine the CMake system, and repair errors. For
# normal usage, this is not necessary. This only changes the verbosity of fprime CMake integration and does not effect
# CMake itself.
#
# **Values:**
# - ON: generate debugging output
# - OFF: (default) do *not* generate debugging output
#
# e.g. `-DCMAKE_DEBUG_OUTPUT=ON`
####
option(CMAKE_DEBUG_OUTPUT "Generate F prime's debug output while running CMake" OFF)

####
# `FPRIME_USE_STUBBED_DRIVERS`:
#
# Tells fprime to use the specific stubbed set of drivers as opposed to full implementation. This applies to drivers in
# the Drv package with the exception of the serial and ipv4 drivers where a generic cross-platform solution is expected.
#
# If unspecified, it will be set in the platform file for the given architecture. If specified, may be set to ON to use
# the stubbed drivers or OFF to used full driver implementations.
#
# **Values:**
# - ON: use stubbed forms of drivers
# - OFF: use full implementation of drivers, driver and OS support needed
#   Note: the chosen platform file will set the default value for this switch
#
# e.g. `-DFPRIME_USE_STUBBED_DRIVERS=ON`
###
if (DEFINED FPRIME_USE_STUBBED_DRIVERS AND NOT FPRIME_USE_STUBBED_DRIVERS STREQUAL "ON" AND NOT FPRIME_USE_STUBBED_DRIVERS STREQUAL "OFF")
    message(FATAL_ERROR "FPRIME_USE_STUBBED_DRIVERS must be set to ON, OFF, or not supplied at all.")
endif()

####
# `FPRIME_USE_BAREMETAL_SCHEDULER`:
#
# Tells fprime to use the baremetal scheduler. This scheduler replaces any OS scheduler with one that loops through
# active components calling each one dispatch at a time. This is designed for use with baremetal (no-OS) system,
# however; it may be set to limit execution to a single thread and or test the baremetal scheduler on a PC.
#
# If unspecified, it will be set in the platform file for the given architecture. If specified, may be set to ON to use
# the scheduler or OFF to use the OS thread scheduler.
#
# **Values:**
# - ON: use baremetal (single context) scheduling
# - OFF: use default (system thread library) scheduling
#   Note: the chosen platform file will set the default value for this switch
#
# e.g. `-DFPRIME_USE_BAREMETAL_SCHEDULER=ON`
###
if (DEFINED FPRIME_USE_BAREMETAL_SCHEDULER AND NOT "${FPRIME_USE_BAREMETAL_SCHEDULER}" STREQUAL "ON" AND NOT "${FPRIME_USE_BAREMETAL_SCHEDULER}" STREQUAL "OFF")
    message(FATAL_ERROR "FPRIME_USE_BAREMETAL_SCHEDULER must be set to ON, OFF, or not supplied at all")
endif()

####
# `FPRIME_ENABLE_UTIL_TARGETS`:
#
# Enables the targets required to run using `fprime-util`.  These include: check, check-leak, coverage, impl, and
# testimpl targets. This switch defaults to "ON" providing those targets, but may be set to off when running within an
# IDE where limiting the number of targets is desirable. Note: unit test targets are still only generated when running
# with -DBUILD_TESTING=ON.
#
# **Values:**
# - ON: (default) generate all targets
# - OFF: only generate executable, and library targets
#
# e.g. `-DFPRIME_ENABLE_UTIL_TARGETS=ON`
####
option(FPRIME_ENABLE_UTIL_TARGETS "Enable fprime-util targets" ON)

####
# `FPRIME_ENABLE_FRAMEWORK_UTS`:
#
# Allow a project to run fprime UTs from the core framework. Default: on,  run fprime framework UTs. This
# does not affect project specified UTs.
#
# **Values:**
# - ON: (default) adds framework UT targets to the total list of targets
# - OFF: do not add framework UTs to the target list
#
# e.g. `-DFPRIME_ENABLE_FRAMEWORK_UTS=OFF`
####
option(FPRIME_ENABLE_FRAMEWORK_UTS "Enable framework UT generation" ON)

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

####
# `FPRIME_ENABLE_UT_COVERAGE`:
#
# Enables coverage calculation within the unit test code of fprime. Disabling this may improve unit test performance when
# unit test coverage is not wanted. Note: this will also remove the coverage targets.
#
# **Values:**
# - ON: (default) calculate unit test coverage
# - OFF: do not calculate unit test coverage, remove coverage targets
#
# e.g. `-DFPRIME_ENABLE_UT_COVERAGE=OFF`
####
option(FPRIME_ENABLE_UT_COVERAGE "Calculate unit test coverage" ON)

####
# `FPRIME_ENABLE_TEXT_LOGGERS:`
#
# When FPRIME_ENABLE_TEXT_LOGGERS is set, the ActiveTextLogger and PassiveConsoleTextLogger 
# svc components are included in the build. When unset, those components are excluded, 
# allowing FpConfig.hpp:FW_ENABLE_TEXT_LOGGING to be unset as well, to save space.
# TextLoggers will fail to build if FW_ENABLE_TEXT_LOGGING=0.
#
# **Values:**
# - ON: (default) retains the text logger components in the target list
# - OFF: removes text logger components from the target list
#
# e.g. `-DFPRIME_ENABLE_TEXT_LOGGERS=OFF`
####
option(FPRIME_ENABLE_TEXT_LOGGERS "Enable text loggers in build" ON)

####
# `FPRIME_SKIP_TOOLS_VERSION_CHECK`:
#
# Skips version checking on fprime tools. This is an advanced option that should be used when the user knows the version
# of their tools is appropriate and the system would otherwise detect the tool version as an incompatible version. This
# can be used, specifically, to enable user maintained variants of standard tools.
#
# Note: no version checking will be done and as such version miss-matches will not be reported at all. Errors that
# result from version incompatibilities will be up to the user to resolve.
#
# **Values:**
# - ON:  skip the tool version check, enabling any tools found on the PATH to run
# - OFF: (default) ensure that the tools found on the path have the required version before running
#
# e.g. `-DFPRIME_SKIP_TOOLS_VERSION_CHECK=ON`
####
option(FPRIME_SKIP_TOOLS_VERSION_CHECK "Skip the version checking of tools" OFF)

####
# `FPRIME_CHECK_FRAMEWORK_VERSION`:
#
# For internal use only.  Used to check the framework version has been updated on tags.
####
option(FPRIME_CHECK_FRAMEWORK_VERSION "(Internal) Check framework version when building." OFF)

####
# `ENABLE_SANITIZER_ADDRESS:`
#
# Enables Google's AddressSanitizer. AddressSanitizer is a memory error detector for C/C++.
# More information: https://github.com/google/sanitizers/wiki/AddressSanitizer
# Practically, this adds the -fsanitizers=address flag to both the compiler and linker for the whole build.
#
# **Values:**
# - ON: enables AddressSanitizer.
# - OFF: (default) does not enable AddressSanitizer.
#
# e.g. `-DENABLE_SANITIZER_ADDRESS=ON`
####
option(ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" OFF)

####
# `ENABLE_SANITIZER_LEAK:`
#
# Enables Google's LeakSanitizer. LeakSanitizer is a memory leak detector which is integrated into AddressSanitizer.
# More information: https://github.com/google/sanitizers/wiki/AddressSanitizerLeakSanitizer
# Practically, this adds the -fsanitizers=leak flag to both the compiler and linker for the whole build.
# 
# Note: LeakSanitizer is not available on macOS. Use AddressSanitizer instead.
#
# **Values:**
# - ON: enables LeakSanitizer.
# - OFF: (default) does not enable LeakSanitizer.
#
# e.g. `-DENABLE_SANITIZER_LEAK=ON`
####
option(ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)

####
# `ENABLE_SANITIZER_UNDEFINED_BEHAVIOR:`
#
# Enables Google's UndefinedBehaviorSanitizer. UndefinedBehaviorSanitizer is an undefined behavior detector.
# More information: https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html
# Practically, this adds the -fsanitizers=undefined flag to both the compiler and linker for the whole build.
#
# **Values:**
# - ON: enables UndefinedBehaviorSanitizer.
# - OFF: (default) does not enable UndefinedBehaviorSanitizer.
#
# e.g. `-DENABLE_SANITIZER_UNDEFINED_BEHAVIOR=ON`
####
option(ENABLE_SANITIZER_UNDEFINED_BEHAVIOR "Enable undefined behavior sanitizer" OFF)

####
# `ENABLE_SANITIZER_THREAD:`
#
# Enables Google's ThreadSanitizer. ThreadSanitizer is a tool that detects data races.
# More information: https://clang.llvm.org/docs/ThreadSanitizer.html
# Practically, this adds the -fsanitizers=thread flag to both the compiler and linker for the whole build.
# 
# Note: ThreadSanitizer does not work with Address or Leak sanitizer enabled
#
# **Values:**
# - ON: enables ThreadSanitizer.
# - OFF: (default) does not enable ThreadSanitizer.
#
# e.g. `-DENABLE_SANITIZER_THREAD=ON`
####
option(ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)

# Backwards compatibility, when build type=TESTING BUILD_TESTING is on
string(TOUPPER "${CMAKE_BUILD_TYPE}" FPRIME_BUILD_TYPE)
if (FPRIME_BUILD_TYPE STREQUAL "TESTING")
else()
    option(BUILD_TESTING OFF)
endif()
include(CTest)

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

# Settings for F config directory
if (NOT DEFINED FPRIME_CONFIG_DIR)
    set(FPRIME_CONFIG_DIR "${FPRIME_FRAMEWORK_PATH}/config/")
endif()
set(FPRIME_CONFIG_DIR "${FPRIME_CONFIG_DIR}" CACHE PATH "F prime configuration header directory" FORCE)

# Override the AC constants file when specified
if (NOT DEFINED FPRIME_AC_CONSTANTS_FILE)
    set(FPRIME_AC_CONSTANTS_FILE "${FPRIME_CONFIG_DIR}/AcConstants.ini" CACHE PATH "F prime AC constants.ini file" FORCE)
endif()

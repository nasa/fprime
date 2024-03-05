####
# API.cmake:
#
# API of the fprime CMake system. These functions represent the external interface to all of the fprime CMake system.
# Users and developers should understand these functions in order to perform basic CMake setup while building as part
# of an fprime project.
#
# The standard patterns include:
# - Add a directory to the fprime system. Use this in place of add_subdirectory to get cleanly organized builds.
# - Register an fprime module/executable/ut to receive the benefits of autocoding.
# - Register an fprime build target/build stage to allow custom build steps. (Experimental)
#
####
include_guard()
include(utilities)
set(FPRIME_TARGET_LIST "" CACHE INTERNAL "FPRIME_TARGET_LIST: custom fprime targets" FORCE)
set(FPRIME_UT_TARGET_LIST "" CACHE INTERNAL "FPRIME_UT_TARGET_LIST: custom fprime targets" FORCE)
set(FPRIME_AUTOCODER_TARGET_LIST "" CACHE INTERNAL "FPRIME_AUTOCODER_TARGET_LIST: custom fprime targets" FORCE)

####
# Macro `restrict_platforms`:
#
# Restricts a CMakeLists.txt file to a given list of platforms. This prevents usage on platforms for which the module
# is incapable of being used and replaces the historical pattern of an if-tree detecting unsupported platforms.
#
# Usage:
#    restrict_platforms(Linux Darwin) # Restricts to Linux and Darwin platforms
#
# Args:
#   ARGN: list of platforms that are supported
#####
macro(restrict_platforms)
    set(__CHECKER ${ARGN})
    if (NOT FPRIME_TOOLCHAIN_NAME IN_LIST __CHECKER AND NOT FPRIME_PLATFORM IN_LIST __CHECKER)
        get_module_name("${CMAKE_CURRENT_LIST_DIR}")
        message(STATUS "Neither toolchain ${FPRIME_TOOLCHAIN_NAME} nor platform ${FPRIME_PLATFORM} supported for module ${MODULE_NAME}")
        return()
    endif()
endmacro()

####
# Macro `prevent_prescan`:
#
# Prevents a CMakeLists.txt file from being processed in the prescan phase of the project. Will generate fake targets
# for all those targets specified to ensure that dependencies may be attached to these targets in the larger system.
#
# Usage:
#    prevent_prescan(target1 target2 ...) # Generate fake targets and skip prescan
#
# Args:
#   ARGN: list of targets to synthesize
#####
macro(prevent_prescan)
    set(__CHECKER_TARGETS ${ARGN})
    if (DEFINED FPRIME_PRESCAN)
        foreach (__TARGET IN LISTS __CHECKER_TARGETS)
            # Make prevent prescan safe in the case of multiple calls
            if (NOT TARGET ${__TARGET})
                add_custom_target(${__TARGET})
            endif()
        endforeach()
        string(REPLACE ";" " " __SPACE_LIST_TARGETS "${__CHECKER_TARGETS}")
        get_module_name("${CMAKE_CURRENT_LIST_DIR}")
        message(STATUS "Skipping ${MODULE_NAME} during prescan, adding faux libraries: ${__SPACE_LIST_TARGETS}")
        return()
    endif()
endmacro()

####
# Function `add_fprime_subdirectory`:
#
# Adds a subdirectory to the build system. This allows the system to find new available modules,
# executables, and unit tests. Every module, used or not, by the deployment/root CMAKE file should
# be added as a subdirectory somewhere in the tree. CMake's dependency system will prevent superfluous building, and
# `add_fprime_subdirectory` calls construct the super-graph from which the build graph is realized. Thus
# it is inconsequential to add a subdirectory that will not be used, but all code should be found within this
# super-graph to be available to the build.
#
# Every subdirectory added should declare a `CMakeLists.txt`. These in-turn may add their own sub-
# directories. This creates a directed acyclic graph of modules, one subgraph of which will be built
# for each executable/module/library defined in the system.  The subgraph should also be a DAG.
#
# This directory is computed based off the closest path in `FPRIME_BUILD_LOCATIONS`. It must be set to
# be used. Otherwise, an error will occur. `EXCLUDE_FROM_ALL` can also be supplied.
# See: https://cmake.org/cmake/help/latest/command/add_fprime_subdirectory.html
#
# **Note:** Replaces CMake `add_subdirectory` call in order to automate the [binary_dir] argument.
#           fprime subdirectories have specific binary roots to avoid collisions, and provide for
#           the standard fprime #include paths rooted at the root of the repo.
#
# **Arguments:**
#  - **FP_SOURCE_DIR:** directory to add (same as add_directory)
#  - **EXCLUDE_FROM_ALL:** (optional) exclude any targets from 'all'. See:
#                          https://cmake.org/cmake/help/latest/command/add_fprime_subdirectory.html
####
function(add_fprime_subdirectory FP_SOURCE_DIR)
    get_module_name("${FP_SOURCE_DIR}")
    set(FPRIME_CURRENT_MODULE "${MODULE_NAME}")

    # Unset all variables that carry special meaning as it is dangerous to pass them through
    foreach (VARIABLE IN ITEMS SOURCE_FILES MOD_DEPS UT_SOURCE_FILES UT_MOD_DEPS EXECUTABLE_NAME)
        set(${VARIABLE} PARENT_SCOPE)
    endforeach()

    # Check if the binary and source directory are in agreement. If they agree, then normally add
    # the directory, as no adjustments need be made.
    get_filename_component(CBD_NAME "${CMAKE_CURRENT_BINARY_DIR}" NAME)
    get_filename_component(CSD_NAME "${CMAKE_CURRENT_SOURCE_DIR}" NAME)
    if ("${CBD_NAME}" STREQUAL "${CSD_NAME}")
        add_subdirectory(${ARGV}) # List of all args, not just extras
        return()
    endif()
    if (${ARGC} GREATER 2)
        message(FATAL_ERROR "Cannot use 'add_fprime_subdirectory' with [binary_dir] argument.")
    endif()
    get_nearest_build_root("${FP_SOURCE_DIR}")
    file(RELATIVE_PATH NEW_BIN_DIR "${FPRIME_CLOSEST_BUILD_ROOT}" "${FP_SOURCE_DIR}")
    # Add component subdirectories using normal add_subdirectory with overridden binary_dir
    add_subdirectory("${FP_SOURCE_DIR}" "${NEW_BIN_DIR}" ${ARGN})
endfunction(add_fprime_subdirectory)

####
# Function `register_fprime_module`:
#
# Registers a module using the fprime build system. This comes with dependency management and fprime
# autocoding capabilities. The caller should first set two variables before calling this function to define the
# autocoding and source inputs, and (optionally) any non-standard link dependencies.
#
# Required variables (defined in calling scope):
#
# - **SOURCE_FILES:** cmake list of input source files. Place any "*.fpp", "*.c", "*.cpp"
#   etc files here. This list will be split into autocoder inputs, and hand-coded sources based on the name/type.
#
# **i.e.:**
# ```
# set(SOURCE_FILES
#     MyComponent.fpp
#     SomeFile.cpp
#     MyComponentImpl.cpp)
# ```
# - **MOD_DEPS:** (optional) cmake list of extra link dependencies. This is optional, and only
#   needed if non-standard link dependencies are used, or if a dependency cannot be inferred from the include graph of
#   the autocoder inputs to the module. If not set or supplied, only fprime inferable dependencies will be available.
#   Link flags like "-lpthread" can be added here as well. Do NOT supply executable targets in MOD_DEPS. See:
#   `register_fprime_executable` for alternatives.
#
# **i.e.:**
# ```
# set(MOD_DEPS
#     Os
#     Module1
#     Module2
#     -lpthread)
# ```
#
# ### Standard `add_fprime_module` Example ###
#
# Standard modules don't require extra modules, and define both autocoder inputs and standard source
# files. Thus, only the SOURCE_FILE variable needs to be set and then the register call can be made.
# This is the only required lines in a module CMakeLists.txt.
#
# ```
# set(SOURCE_FILE
#     MyComponent.fpp
#     SomeFile.cpp
#     MyComponentImpl.cpp)
#
# register_fprime_module()
# ```
#
# ### Non-Autocoded and Autocode-Only Modules Example ###
#
# Modules that do not require autocoding need not specify *.xml files as source. Thus, code-only modules just define
# *.cpp. **Note:** dependency inference is only done when autocoder inputs (.fpp, .xml) are supplied.
#
# ```
# set(SOURCE_FILE
#     SomeFile1.cpp
#     Another2.cpp)
#
# register_fprime_module()
# ```
# Modules requiring only autocoding may just specify *.xml files.
#
# ```
# set(SOURCE_FILE
#     MyComponent.fpp)
#
# register_fprime_module()
# ```
#
# ### Specific Dependencies and Linking in Modules Example ###
#
# Some modules need to pick a specific set of dependencies and link flags. This can be done
# with the `MOD_DEPS` variable. This feature can be used to pick specific implementations
# for some fprime modules that implement to a generic interface like the console logger implementation.
# 
# ```
# set(SOURCE_FILE
#     MyComponent.fpp
#     SomeFile.cpp
#     MyComponentImpl.cpp)
#
# set(MOD_DEPS
#     Module1
#     -lpthread)
#
# register_fprime_module()
# ```
#
####
function(register_fprime_module)
    if(NOT DEFINED SOURCE_FILES)
        message(FATAL_ERROR "'SOURCE_FILES' not defined in '${CMAKE_CURRENT_LIST_FILE}'.")
    endif()
    get_nearest_build_root(${CMAKE_CURRENT_LIST_DIR})
    if (${ARGC} GREATER 0)
        set(MODULE_NAME ${ARGV0})
    else()
        # Check to be sure before using
        if (NOT DEFINED FPRIME_CURRENT_MODULE)
            message(FATAL_ERROR "FPRIME_CURRENT_MODULE not defined. Please supply name to: register_fprime_module()")
        endif()

        set(MODULE_NAME ${FPRIME_CURRENT_MODULE})
    endif()
    # Explicit call to module register
    generate_library("${MODULE_NAME}" "${SOURCE_FILES}" "${MOD_DEPS}")
endfunction(register_fprime_module)

####
# Function `register_fprime_executable`:
#
# Registers an executable using the fprime build system. This comes with dependency management and
# fprime autocoding capabilities. This requires three variables to define the executable name,
# autocoding and source inputs, and (optionally) any non-standard link dependencies.
#
# Note: this is not intended for deployment executables (e.g. an fprime binary) but rather for utilities,
# helper executables and tools. To register a deployment binary see `register_fprime_deployment`.
#
# Executables will automatically install itself and its dependencies into the out-of-cache build
# artifacts directory, specified by the FPRIME_INSTALL_DEST variable, when built.
#
# Required variables (defined in calling scope):
#
#
# - **EXECUTABLE_NAME:** (optional) executable name supplied. If not set, nor passed in, then
#                     FPRIME_CURRENT_MODULE from the CMake definitions is used.
#
# - **SOURCE_FILES:** cmake list of input source files. Place any "*.fpp", "*.c", "*.cpp"
#                  etc. files here. This list will be split into autocoder inputs and sources.
# **i.e.:**
# ```
# set(SOURCE_FILES
#     MyComponent.fpp
#     SomeFile.cpp
#     MyComponentImpl.cpp)
# ```
#
# - **MOD_DEPS:** (optional) cmake list of extra link dependencies. This is optional, and only
#   needed if non-standard link dependencies are used, or if a dependency cannot be inferred from the include graph of
#   the autocoder inputs to the module. If not set or supplied, only fprime
#   inferable dependencies will be available. Link flags like "-lpthread" can be here.
#
# **i.e.:**
# ```
# set(MOD_DEPS
#     Module1
#     Module2
#     -lpthread)
# ```
#
# **Note:** this operates almost identically to `register_fprime_module` with respect to the variable definitions. The
#           difference is this call will yield an optionally named linked binary executable.
#
# ### Caveats ###
#
# Executable targets should not be supplied as dependencies through MOD_DEPS  (e.g. to register_fprime_deployment).
# Doing so may cause problems with final linking of other executables due to multiple main function definitions. A
# better model would be to add a CMake only dependency without using MOD_DEPS.
#
# **Note:** these errors are definition order dependent and thus users should not supply executables through MOD_DEPS
# even if it seems to work correctly.
#
#  **i.e.:**
# ```
# set(SOURCE_FILES "tool.c")
# register_fprime_executable(TOOL)
# ...
# ...
# register_fprime_deployment(MY_DEPLOYMENT)
# add_dependencies(MY_DEPLOYMENT TOOL) # CMake only dependency
# ```
####
function(register_fprime_executable)
    if (NOT DEFINED SOURCE_FILES AND NOT DEFINED MOD_DEPS)
        message(FATAL_ERROR "SOURCE_FILES or MOD_DEPS must be defined when registering an executable")
    elseif (NOT DEFINED EXECUTABLE_NAME AND ARGC LESS 1 AND TARGET "${FPRIME_CURRENT_MODULE}")
        message(FATAL_ERROR "EXECUTABLE_NAME must be set or passed in. Use register_fprime_deployment() for deployments")
    endif()
    # MODULE_NAME is used for the executable name, unless otherwise specified.
    if(NOT DEFINED EXECUTABLE_NAME AND ARGC GREATER 0)
        set(EXECUTABLE_NAME "${ARGV0}")
    elseif(NOT DEFINED EXECUTABLE_NAME)
        # Check to be sure before using
        if (NOT DEFINED FPRIME_CURRENT_MODULE)
            message(FATAL_ERROR "FPRIME_CURRENT_MODULE not defined. Please supply name to: register_fprime_executable()")
        endif()
        set(EXECUTABLE_NAME "${FPRIME_CURRENT_MODULE}")
    endif()
    get_nearest_build_root(${CMAKE_CURRENT_LIST_DIR})
    generate_executable("${EXECUTABLE_NAME}" "${SOURCE_FILES}" "${MOD_DEPS}")
endfunction(register_fprime_executable)


####
# Function `register_fprime_deployment`:
#
# Registers an deployment using the fprime build system. This comes with dependency management and
# fprime autocoding capabilities. This requires two variables to define autocoding and source inputs, and
# (optionally) any non-standard link dependencies.
#
# An executable will be created and automatically install itself and its dependencies into the out-of-cache build
# artifacts directory, specified by the FPRIME_INSTALL_DEST variable, when built. This will automatically run all
# deployment targets such that the standard deployment will be built (e.g. the dictionary will be built).
#
# This is typically called from within the top-level CMakeLists.txt file that defines a deployment.
#
# Required variables (defined in calling scope):
#
# - **SOURCE_FILES:** cmake list of input source files. Place any "*.fpp", "*.c", "*.cpp"
#                     etc. files here. This list will be split into autocoder inputs and sources.
# **i.e.:**
# ```
# set(SOURCE_FILES
#     MyComponent.fpp
#     SomeFile.cpp
#     MyComponentImpl.cpp)
# ```
#
# - **MOD_DEPS:** cmake list of extra link dependencies. This is almost always required to supply the topology module.
#                 Other entries are only needed when they cannot be inferred from the model (e.g. linker flags). Do NOT
#                 supply executable targets in MOD_DEPS. See: `register_fprime_executable` for alternatives.
#
# **i.e.:**
# ```
# set(MOD_DEPS
#     ${FPRIME_CURRENT_MODULE}/Top
#     Module1
#     Module2
#     -lpthread)
# ```
#
# **Note:** this operates almost identically to `register_fprime_executable` and `register_fprime_module` with respect
# to the variable definitions. The difference is deployment targets will be run (e.g. dictionary generation), and the
# executable binary will be named after the module, or if project when defined directly in a project CMakeLists.txt
#
# ### Standard fprime Deployment Example ###
#
# To create a standard fprime deployment, an the user must call `register_fprime_deployment()` after defining
# SOURCE_FILES and MOD_DEPS.
#
# ```
# set(SOURCE_FILES
#   "${CMAKE_CURRENT_LIST_DIR}/Main.cpp"
# )
# # Note: supply dependencies that cannot be detected via the model here.
# set(MOD_DEPS
#   ${FPRIME_CURRENT_MODULE}/Top
# )
# register_fprime_deployment()
# ```
####
function(register_fprime_deployment)
    if (NOT DEFINED SOURCE_FILES AND NOT DEFINED MOD_DEPS)
        message(FATAL_ERROR "SOURCE_FILES or MOD_DEPS must be defined when registering an executable")
    endif()
    # Fallback to PROJECT_NAME when it is not set
    if (NOT DEFINED FPRIME_CURRENT_MODULE)
        set(FPRIME_CURRENT_MODULE "${PROJECT_NAME}")
    endif()
    get_nearest_build_root(${CMAKE_CURRENT_LIST_DIR})
    generate_deployment("${FPRIME_CURRENT_MODULE}" "${SOURCE_FILES}" "${MOD_DEPS}")
endfunction(register_fprime_deployment)


####
# Function `register_fprime_ut`:
#
# Registers an executable unit-test using the fprime build system. This comes with dependency
# management and fprime autocoding capabilities. This requires three variables defining the
# unit test name, autocoding and source inputs for the unit test, and (optionally) any
# non-standard link dependencies.
#
# **Note:** This is ONLY run when the BUILD_TESTING is enabled. Unit testing is restricted to this build type as fprime
#           sets additional flags when building for unit tests.
#
# Required variables (defined in calling scope):
#
# 
# - **UT_NAME:** (optional) executable name supplied. If not supplied, or passed in, then
#   the <MODULE_NAME>_ut_exe will be used.
#
# - **UT_SOURCE_FILES:** cmake list of UT source files. Place any "*.fpp", "*.c", "*.cpp"
#   etc. files here. This list will be split into autocoder inputs or sources. These sources only apply to the unit
#   test.
#
#  **i.e.:**
# ```
# set(UT_SOURCE_FILES
#     MyComponent.fpp
#     SomeFile.cpp
#     MyComponentImpl.cpp)
# ```
#
# - **UT_MOD_DEPS:** (optional) cmake list of extra link dependencies. This is optional, and only
#   needed if non-standard link dependencies are used. If not set or supplied, only
#   fprime detectable dependencies will be available. Link flags like "-lpthread"
#   can be supplied here.
#
# **i.e.:**
# ```
# set(UT_MOD_DEPS
#     Module1
#     Module2
#     -lpthread)
# ```
#  **Note:** this is typically called after any other register calls in the module.
#
# - **UT_AUTO_HELPERS:** (optional) When set ON, a test helper file will be generated that auto-codes the connect ports
#   and init components methods. This removes the maintenance overhead for these functions. ON additionally adds test
#   source directories to the include path for the unit test target. When set to OFF, this helper file will be created
#   when generating implementation templates allowing users to modify these files. Default: OFF
#
# ### Unit-Test Example ###
#
# A standard unit test defines only UT_SOURCES. These sources have the test cpp files and the model
# .fpp of the module being tested. This is used to generate the GTest harness.
#
# ```
# set(UT_SOURCE_FILES
#   "${FPRIME_FRAMEWORK_PATH}/Svc/CmdDispatcher/CommandDispatcher.fpp"
#   "${CMAKE_CURRENT_LIST_DIR}/test/ut/CommandDispatcherTester.cpp"
#   "${CMAKE_CURRENT_LIST_DIR}/test/ut/CommandDispatcherImplTester.cpp"
# )
# register_fprime_ut()
# ```
####
function(register_fprime_ut)
    #### CHECK UT BUILD ####
    if (NOT BUILD_TESTING OR __FPRIME_NO_UT_GEN__)
        return()
    elseif(NOT DEFINED UT_SOURCE_FILES)
        message(FATAL_ERROR "UT_SOURCE_FILES not defined. Cannot register unittest without sources")
    elseif(${ARGC} GREATER 1)
        message(FATAL_ERROR "register_fprime_ut accepts only one optional argument: test name")
    endif()
    get_module_name(${CMAKE_CURRENT_LIST_DIR})
    # UT name is passed in or is the module name with _ut_exe added
    if (${ARGC} GREATER 0)
        set(UT_NAME "${ARGV0}")
    elseif (NOT DEFINED UT_NAME)
        set(UT_NAME "${MODULE_NAME}_ut_exe")
    endif()
    set(MD_IFS ${MODULE_NAME} ${UT_MOD_DEPS})
    get_nearest_build_root(${CMAKE_CURRENT_LIST_DIR})
    # Turn allow turning GTest on/off
    set(INCLUDE_GTEST ON)
    if (DEFINED UT_INCLUDE_GTEST)
        set(INCLUDE_GTEST ${UT_INCLUDE_GTEST})
    endif()
    # Check no multiple UTs
    if (TARGET UT_NAME)
        message(FATAL_ERROR "${UT_NAME} already used. Please supply a unique name using 'register_fprime_ut(NAME)'")
    endif()

    # Explicit call to module register
    generate_ut("${UT_NAME}" "${UT_SOURCE_FILES}" "${MD_IFS}")
endfunction(register_fprime_ut)

####
# Macro `register_fprime_target`:
#
# This function allows users to register custom build targets into the build system.  These targets are defined in a
# CMake file and consist of three functions that operate on different parts of the build: global, per-module, and
# per-deployment. See: [Targets](targets.md).
#
# This function takes in either a file path to a CMake file defining targets, or an short include path that accomplishes
# the same thing. Note: make sure the directory is on the CMake include path to use the second form. The supplied file
# should define three functions: `add_global_target`, `add_module_target`, and `add_deployment_target`.
#
# **TARGET_FILE_PATH:** include path or file path file defining above functions
###
macro(register_fprime_target TARGET_FILE_PATH)
    if (CMAKE_DEBUG_OUTPUT)
        message(STATUS "[target] Registering custom target: ${TARGET_FILE_PATH}")
    endif()
    register_fprime_list_helper("${TARGET_FILE_PATH}" FPRIME_TARGET_LIST)
endmacro(register_fprime_target)

####
# Macro `register_fprime_ut_target`:
#
# Identical to the above `register_fprime_target` function except that these targets are only created when the system
# is building unit tests. e.g. BUILD_TESTING=ON.
#
# **TARGET_FILE_PATH:** include path or file path files
###
macro(register_fprime_ut_target TARGET_FILE_PATH)
    # UT targets only allowed when testing
    if (BUILD_TESTING)
        if (CMAKE_DEBUG_OUTPUT)
            message(STATUS "[target] Registering custom target: ${TARGET_FILE_PATH}")
        endif()
        register_fprime_list_helper("${TARGET_FILE_PATH}" FPRIME_UT_TARGET_LIST)
    endif()
endmacro(register_fprime_ut_target)

####
# Macro `register_fprime_list_helper`:
#
# Helper function to do the actual registration. Also used to side-load prescan to bypass the not-on-prescan check.
####
macro(register_fprime_list_helper TARGET_FILE_PATH TARGET_LIST)
    if (NOT DEFINED FPRIME_SUB_BUILD_TARGETS OR "${TARGET_FILE_PATH}" IN_LIST FPRIME_SUB_BUILD_TARGETS)
        include("${TARGET_FILE_PATH}")
        # Prevent out-of-order setups
        get_property(MODULE_DETECTION_STARTED GLOBAL PROPERTY MODULE_DETECTION SET)
        if (MODULE_DETECTION_STARTED)
            message(FATAL_ERROR "Cannot register fprime target after including subdirectories or FPrime-Code.cmake'")
        endif()
        get_property(TARGETS GLOBAL PROPERTY "${TARGET_LIST}")
        if (NOT TARGET_FILE_PATH IN_LIST TARGETS)
            set_property(GLOBAL APPEND PROPERTY "${TARGET_LIST}" "${TARGET_FILE_PATH}")
        endif()
    endif()
endmacro(register_fprime_list_helper)


####
# Macro `register_fprime_build_autocoder`:
# 
# This function allows users to register custom autocoders into the build system. These autocoders will execute during
# the build process. An autocoder is defined in a CMake file and must do three things:
# 1. Call one of `autocoder_setup_for_individual_sources()` or `autocoder_setup_for_multiple_sources()` from file scope
# 2. Implement `<autocoder name>_is_supported(AC_POSSIBLE_INPUT_FILE)` returning true the autocoder processes given source 
# 3. Implement `<autocoder name>_setup_autocode AC_INPUT_FILE)` to run the autocoder on files filter by item 2. 
# See: [Autocoders](dev/autocoder_integration.md).
#
# This function takes in either a file path to a CMake file defining an autocoder target, or an short include path that accomplishes
# the same thing. Note: make sure the directory is on the CMake include path to use the second form.
#
# **TARGET_FILE_PATH:** include path or file path file defining above functions
####
macro(register_fprime_build_autocoder TARGET_FILE_PATH)
    # Normal registered targets don't run in prescan
    if (CMAKE_DEBUG_OUTPUT)
        message(STATUS "[autocoder] Registering custom build target autocoder: ${TARGET_FILE_PATH}")
    endif()
    register_fprime_list_helper("${TARGET_FILE_PATH}" FPRIME_AUTOCODER_TARGET_LIST)
endmacro(register_fprime_build_autocoder)

####
# Function `create_implementation_interface`:
#
# Helper function to create implementation interface library once and only once to ensure it exists.
#
# **IMPLEMENTATION**: implementation library name (resolved)
####
function (create_implementation_interface IMPLEMENTATION)
    if (TARGET "${IMPLEMENTATION}")
        return()
    endif()
    add_library("${IMPLEMENTATION}" INTERFACE)
endfunction()


####
# Function `require_fprime_implementation`:
#
# Designates that the current module requires a separate implementation in order for it to function properly. As an
# example, Os requires an implementation of `Os_Task`. These implementations must be set via
# `choose_fprime_implementation` in the platform and may be overridden in in the executable/deployment.
#
# **IMPLEMENTATION:** implementation module name that must be covered
# **REQUESTER:** (optional) the requester of the implementation. Default: ${FPRIME_CURRENT_MODULE}
####
function(require_fprime_implementation IMPLEMENTATION)
    if (ARGC EQUAL 2)
        set(REQUESTER "${ARGV1}")
    elseif (FPRIME_CURRENT_MODULE)
        set(REQUESTER "${FPRIME_CURRENT_MODULE}")
    else ()
        message(FATAL_ERROR "Cannot determine current module, please supply as second argument")
    endif()
    resolve_dependencies(IMPLEMENTATION "${IMPLEMENTATION}")
    resolve_dependencies(REQUESTER "${REQUESTER}")

    create_implementation_interface("${IMPLEMENTATION}")
    append_list_property("${IMPLEMENTATION}" GLOBAL PROPERTY "REQUIRED_IMPLEMENTATIONS")
    append_list_property("${FPRIME_CURRENT_MODULE}" TARGET "${IMPLEMENTATION}" PROPERTY "REQUESTERS")
    add_dependencies("${REQUESTER}" "${IMPLEMENTATION}")
endfunction()

####
# Function `register_fprime_implementation`:
#
# Designates that the given implementor implements the required implementation. As an example Os_Task_Posix implements
# Os_Task. These implementations must be set via
## `choose_fprime_implementation` in the platform and may be overridden in in the executable/deployment.
#
# **IMPLEMENTATION:** implementation module name that is implemented by IMPLEMENTOR
# **IMPLEMENTOR:** implementor of IMPLEMENTATION
####
function(register_fprime_implementation IMPLEMENTATION IMPLEMENTOR)
    resolve_dependencies(IMPLEMENTATION "${IMPLEMENTATION}")
    resolve_dependencies(IMPLEMENTOR "${IMPLEMENTOR}")
    create_implementation_interface("${IMPLEMENTATION}")
    append_list_property("${IMPLEMENTOR}" TARGET "${IMPLEMENTATION}" PROPERTY "IMPLEMENTORS")
    append_list_property("${ARGN}" TARGET "${IMPLEMENTOR}" PROPERTY "REQUIRED_SOURCE_FILES")
    add_dependencies("${IMPLEMENTATION}" "${IMPLEMENTOR}")
endfunction()

####
# Function `choose_fprime_implementation`:
#
# Designates that the given implementor is the selected implementor for the needed implementation. Platforms must call
# this function once for each defined IMPLEMENTATION. An executable/deployment/unit-test may call this function to set
# a specific implementor for any needed implementation
#
# **IMPLEMENTATION:** implementation module name that is implemented by IMPLEMENTOR
# **IMPLEMENTOR:** implementor of IMPLEMENTATION
####
function(choose_fprime_implementation IMPLEMENTATION IMPLEMENTOR)
    resolve_dependencies(IMPLEMENTATION "${IMPLEMENTATION}")
    resolve_dependencies(IMPLEMENTOR "${IMPLEMENTOR}")
    # Check for passed in module name
    if (ARGC EQUAL 3)
        set(ACTIVE_MODULE "${ARGV2}")
    elseif (FPRIME_CURRENT_MODULE)
        set(ACTIVE_MODULE "${FPRIME_CURRENT_MODULE}")
    elseif(FPRIME_PLATFORM)
        set(ACTIVE_MODULE "${FPRIME_PLATFORM}")
    else()
        message(FATAL_ERROR "Cannot call 'choose_fprime_implementation' outside an fprime module or platform CMake file")
    endif()
    create_implementation_interface("${IMPLEMENTATION}")
    # Add this implementation in the case it has not been added
    append_list_property("${IMPLEMENTATION}" GLOBAL PROPERTY "REQUIRED_IMPLEMENTATIONS")
    set_property(GLOBAL PROPERTY "${ACTIVE_MODULE}_${IMPLEMENTATION}" "${IMPLEMENTOR}")
endfunction()

#### Documentation links
# Next Topics:
#  - Setting Options: [Options](Options.md) are used to vary a CMake build.
#  - Adding Deployments: [Deployments](deployment.md) create fprime builds.
#  - Adding Modules: [Modules](module.md) register fprime Ports, Components, etc.
#  - Creating Toolchains: [Toolchains](toolchain.md) setup standard CMake Cross-Compiling.
#  - Adding Platforms: [Platforms](platform.md) help fprime set Cross-Compiling specific items.
#  - Adding Targets: [Targets](targets.md) for help defining custom build targets
#  - Implementation Packages Design: [Implementation Packages](/Design/package-implementor.md)
####

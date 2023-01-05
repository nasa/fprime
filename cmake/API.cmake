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
set(FPRIME_TARGET_LIST "" CACHE INTERNAL "FPRIME_TARGET_LIST: custom fprime targets" FORCE)
set(FPRIME_UT_TARGET_LIST "" CACHE INTERNAL "FPRIME_UT_TARGET_LIST: custom fprime targets" FORCE)
set(FPRIME_AUTOCODER_TARGET_LIST "" CACHE INTERNAL "FPRIME_AUTOCODER_TARGET_LIST: custom fprime targets" FORCE)
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
# - **SOURCE_FILES:** cmake list of input source files. Place any "*.fpp", "*Ai.xml", "*.c", "*.cpp"
#   etc files here. This list will be split into autocoder inputs, and hand-coded sources based on the name/type.
#
# **i.e.:**
# ```
# set(SOURCE_FILES
#     MyComponentAi.xml
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
#     MyComponentAi.xml
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
#     MyComponentAi.xml)
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
#     MyComponentAi.xml
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
        get_module_name("${CMAKE_CURRENT_LIST_DIR}")
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
#                     PROJECT_NAME from the CMake definitions is used.
#
# - **SOURCE_FILES:** cmake list of input source files. Place any "*Ai.xml", "*.c", "*.cpp"
#                  etc. files here. This list will be split into autocoder inputs and sources.
# **i.e.:**
# ```
# set(SOURCE_FILES
#     MyComponentAi.xml
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
    get_module_name("${CMAKE_CURRENT_LIST_DIR}")
    if (NOT DEFINED SOURCE_FILES AND NOT DEFINED MOD_DEPS)
        message(FATAL_ERROR "SOURCE_FILES or MOD_DEPS must be defined when registering an executable")
    elseif (NOT DEFINED EXECUTABLE_NAME AND ARGC LESS 1 AND TARGET "${MODULE_NAME}")
        message(FATAL_ERROR "EXECUTABLE_NAME must be set or passed in. Use register_fprime_deployment() for deployments")
    endif()
    # MODULE_NAME is used for the executable name, unless otherwise specified.
    if(NOT DEFINED EXECUTABLE_NAME AND ARGC GREATER 0)
        set(EXECUTABLE_NAME "${ARGV0}")
    elseif(NOT DEFINED EXECUTABLE_NAME)
        set(EXECUTABLE_NAME "${MODULE_NAME}")
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
# - **SOURCE_FILES:** cmake list of input source files. Place any "*Ai.xml", "*.c", "*.cpp"
#                     etc. files here. This list will be split into autocoder inputs and sources.
# **i.e.:**
# ```
# set(SOURCE_FILES
#     MyComponentAi.xml
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
#     ${PROJECT_NAME}/Top
#     Module1
#     Module2
#     -lpthread)
# ```
#
# **Note:** this operates almost identically to `register_fprime_executable` and `register_fprime_module` with respect
# to the variable definitions. The difference is deployment targets will be run (e.g. dictionary generation), and the
# executable binary will be named for ${PROJECT_NAME}.
#
# ### Standard fprime Deployment Example ###
#
# To create a standard fprime deployment, an executable needs to be created. This executable
# uses the CMake PROJECT_NAME as the executable name. Thus, it can be created with the following
# source lists. In most fprime deployments, some modules must be specified as they don't tie
# directly to an Ai.xml.
#
# ```
# set(SOURCE_FILES
#   "${CMAKE_CURRENT_LIST_DIR}/Main.cpp"
# )
# # Note: supply non-explicit dependencies here. These are implementations to an XML that is
# # defined in a different module.
# set(MOD_DEPS
#   ${PROJECT_NAME}/Top
# )
# register_fprime_deployment()
# ```
####
function(register_fprime_deployment)
    get_module_name("${CMAKE_CURRENT_LIST_DIR}")
    if (NOT DEFINED SOURCE_FILES AND NOT DEFINED MOD_DEPS)
        message(FATAL_ERROR "SOURCE_FILES or MOD_DEPS must be defined when registering an executable")
    elseif(NOT MODULE_NAME STREQUAL PROJECT_NAME)
        message(WARNING "Project name ${PROJECT_NAME} does not match expected name ${MODULE_NAME}")
    endif()
    get_nearest_build_root(${CMAKE_CURRENT_LIST_DIR})
    generate_deployment("${PROJECT_NAME}" "${SOURCE_FILES}" "${MOD_DEPS}")
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
# - **UT_SOURCE_FILES:** cmake list of UT source files. Place any "*Ai.xml", "*.c", "*.cpp"
#   etc. files here. This list will be split into autocoder inputs or sources. These sources only apply to the unit
#   test.
#
#  **i.e.:**
# ```
# set(UT_SOURCE_FILES
#     MyComponentAi.xml
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
# A standard unit test defines only UT_SOURCES. These sources have the test cpp files and the module
# Ai.xml of the module being tested. This is used to generate the GTest and TesterBase files from this
# Ai.xml. The other UT source files define the implementation of the test.
#
# ```
# set(UT_SOURCE_FILES
#   "${FPRIME_FRAMEWORK_PATH}/Svc/CmdDispatcher/CommandDispatcherComponentAi.xml"
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
    # Normal registered targets don't run in prescan
    if (NOT DEFINED FPRIME_PRESCAN)
        register_fprime_list_helper("${TARGET_FILE_PATH}" FPRIME_TARGET_LIST)
        setup_global_target("${TARGET_FILE_PATH}")
    endif()
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
    if (BUILD_TESTING AND NOT DEFINED FPRIME_PRESCAN)
        register_fprime_list_helper("${TARGET_FILE_PATH}" FPRIME_UT_TARGET_LIST)
        setup_global_target("${TARGET_FILE_PATH}")
    endif()
endmacro(register_fprime_ut_target)

####
# Macro `register_fprime_list_helper`:
#
# Helper function to do the actual registration. Also used to side-load prescan to bypass the not-on-prescan check.
####
macro(register_fprime_list_helper TARGET_FILE_PATH TARGET_LIST)
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
###
macro(register_fprime_build_autocoder TARGET_FILE_PATH)
    # Normal registered targets don't run in prescan
    message(STATUS "Registering custom autocoder: ${TARGET_FILE_PATH}")
    if (NOT DEFINED FPRIME_PRESCAN)
        register_fprime_list_helper("${TARGET_FILE_PATH}" FPRIME_AUTOCODER_TARGET_LIST)
    endif()
endmacro(register_fprime_build_autocoder)

#### Documentation links
# Next Topics:
#  - Setting Options: [Options](Options.md) are used to vary a CMake build.
#  - Adding Deployment: [Deployments](deployment.md) create fprime builds.
#  - Adding Module: [Modules](module.md) register fprime Ports, Components, etc.
#  - Creating Toolchains: [Toolchains](toolchain.md) setup standard CMake Cross-Compiling.
#  - Adding Platforms: [Platforms](platform.md) help fprime set Cross-Compiling specific items.
#  - Adding Targets: [Targets](targets.md) for help defining custom build targets
####

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
# be used. Otherwise, an error will occur.
#
# A user can specify an optional argument to set the build-space, creating a sub-directory under
# the `CMAKE_BINARY_DIR` to place the outputs of the builds of this directory. This is typically
# **not needed**. `EXCLUDE_FROM_ALL` can also be supplied.
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
        add_subdirectory(${ARGV})
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
# - **SOURCE_FILES:** cmake list of input source files. Place any "*Ai.xml", "*.c", "*.cpp"
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
#   the autocoder inputs to the module. If not set or supplied, only fprime
#   inferable dependencies will be available. Link flags like "-lpthread" can be here.
#
# **i.e.:**
# ```
# set(LINK_DEPS
#     Os
#     Module1
#     Module2
#     -lpthread)
# ```
#
# **Note:** if desired, these fields may be supplied in-order as arguments to the function. Passing
#           these as positional arguments overrides any specified in the parent scope.  This is typically not done.
#
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
# *.cpp. **Note:** no dependency inference is done without autocoder inputs.
#
# ```
# set(SOURCE_FILE
#     SomeFile1.cpp
#     Another2.cpp)
#
# register_fprime_module()
# ```
# Modules requiring only autocoding can just specify *.xml files.
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
    # SOURCE_FILES is supplied as the first positional -OR- as the list 'SOURCE_FILES'
    if (${ARGC} GREATER 0)
        set(SC_IFS "${ARGV0}")
    elseif(DEFINED SOURCE_FILES)
    	set(SC_IFS "${SOURCE_FILES}")
    else()
        message(FATAL_ERROR "'SOURCE_FILES' not defined in '${CMAKE_CURRENT_LIST_FILE}'.")
    endif()
    # MOD_DEPS is supplied as an optional second positional -OR- or as  the list 'MOD_DEPS'
    if (${ARGC} GREATER 1)
        set(MD_IFS "${ARGV1}")
    elseif(DEFINED MOD_DEPS)
    	set(MD_IFS "${MOD_DEPS}")
    elseif(${CMAKE_DEBUG_OUTPUT})
        message(STATUS "No extra 'MOD_DEPS' found in '${CMAKE_CURRENT_LIST_FILE}'.")
    endif()
    if (${ARGC} GREATER 2)
        set(MODULE_NAME "${ARGV2}")
    else()
        # Sets MODULE_NAME to unique name based on path, and then adds the library of
        get_module_name(${CMAKE_CURRENT_LIST_DIR})
    endif()
    get_nearest_build_root(${CMAKE_CURRENT_LIST_DIR})
    # Explicit call to module register
    generate_library("${MODULE_NAME}" "${SC_IFS}" "${MD_IFS}")

    # Globally expose source and ac files to be used as necessary within unit test logic.
    set(SOURCE_FILE "${SC_IFS}" PARENT_SCOPE)
    set(AC_OUTPUTS "${AC_OUTPUTS}" PARENT_SCOPE)
endfunction(register_fprime_module)

####
# Function `register_fprime_executable`:
#
# Registers an executable using the fprime build system. This comes with dependency management and
# fprime autocoding capabilities. This requires three variables to define the executable name,
# autocoding and source inputs, and (optionally) any non-standard link dependencies.
#
# Executables will automatically install itself and its dependencies into the out-of-cache build
# artifacts directory, specified by the FPRIME_INSTALL_DEST variable, when built. To skip this
# installation step, set the SKIP_INSTALL variable before registering an executable.
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
# set(LINK_DEPS
#     Module1
#     Module2
#     -lpthread)
# ```
#
# **Note:** if desired, these fields may be supplied in-order as arguments to the function. Passing
#           these as positional arguments overrides any specified in the parent scope.
#
# **Note:** this operates almost identically to `register_fprime_module` with respect to the variable definitions. The
#           difference is this call will yield an optionally named linked binary file.
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
#   "${CMAKE_CURRENT_LIST_DIR}/RefTopologyAppAi.xml"
#   "${CMAKE_CURRENT_LIST_DIR}/Topology.cpp"  
#   "${CMAKE_CURRENT_LIST_DIR}/Main.cpp"  
# )
# # Note: supply non-explicit dependencies here. These are implementations to an XML that is
# # defined in a different module.
# set(MOD_DEPS
#   Svc/PassiveConsoleTextLogger
#   Svc/SocketGndIf
#   Svc/LinuxTime
# )
# register_fprime_executable()
# ```
# ### fprime Executable With Autocoding/Dependencies ###
#
# Developers can make executables or other utilities that take advantage of fprime autocoding
# and fprime dependencies. These can be registered using the same executable registrar function
# but should specify a specific executable name.
#
# ```
# set(EXECUTABLE_NAME "MyUtility")
#
# set(SOURCE_FILES
#   "${CMAKE_CURRENT_LIST_DIR)/ModuleAi.xml"
#   "${CMAKE_CURRENT_LIST_DIR}/Main.cpp"  
# )
# set(MOD_DEPS
#   Svc/LinuxTime
#   -lm
#   -lpthread
# )
# register_fprime_executable()
# ```
#
####
function(register_fprime_executable)
    # PROJECT_NAME is used for the executable name, unless otherwise specified.
    if (${ARGC} GREATER 0)
        set(EX_NAME "${ARGV0}")
    elseif(DEFINED EXECUTABLE_NAME)
        set(EX_NAME ${EXECUTABLE_NAME})
    elseif(DEFINED PROJECT_NAME)
    	set(EX_NAME "${PROJECT_NAME}")
    else()
        message(FATAL_ERROR "'EXECUTABLE_NAME' not defined in '${CMAKE_CURRENT_LIST_FILE}'.")
    endif()
    # SOURCE_FILES is supplied as the first positional -OR- as the list 'SOURCE_FILES'
    if (${ARGC} GREATER 1)
        set(SC_IFS "${ARGV1}")
    elseif(DEFINED SOURCE_FILES)
    	set(SC_IFS "${SOURCE_FILES}")
    else()
        message(FATAL_ERROR "'SOURCE_FILES' not defined in '${CMAKE_CURRENT_LIST_FILE}'.")
    endif()
    # MOD_DEPS is supplied as an optional second positional -OR- or as  the list 'MOD_DEPS'
    if (${ARGC} GREATER 2)
        set(MD_IFS "${ARGV2}")
    elseif(DEFINED MOD_DEPS)
    	set(MD_IFS "${MOD_DEPS}")
    elseif(${CMAKE_DEBUG_OUTPUT})
        message(STATUS "No extra 'MOD_DEPS' found in '${CMAKE_CURRENT_LIST_FILE}'.")
    endif()
    get_nearest_build_root(${CMAKE_CURRENT_LIST_DIR})
    # Register executable and module with name '<exe name>_exe', then create an empty target with
    # name '<exe name>' that depends on the executable. This enables additional post-processing
    # targets that depend on the built executable.
    generate_executable("${EX_NAME}_exe" "${SC_IFS}" "${MD_IFS}")
    set_target_properties("${EX_NAME}_exe" PROPERTIES OUTPUT_NAME "${EX_NAME}")
    add_custom_target(${EX_NAME} ALL)
    add_dependencies("${EX_NAME}" "${EX_NAME}_exe")

    # Only install into artifacts directory in release builds when SKIP_INSTALL is not set.
    if (NOT DEFINED SKIP_INSTALL AND CMAKE_BUILD_TYPE STREQUAL "RELEASE")
        add_dependencies("${EX_NAME}" "package_gen")
    endif()
endfunction(register_fprime_executable)

####
# Function `register_fprime_ut`:
#
# Registers an executable unit-test using the fprime build system. This comes with dependency
# management and fprime autocoding capabilities. This requires three variables defining the
# unit test name, autocoding and source inputs for the unit test, and (optionally) any
# non-standard link dependencies.
#
# **Note:** This is ONLY run when the build type is TESTING. Unit testing is restricted to this build type as fprime
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
#
#   **Note:** if desired, these fields may be supplied in-order as arguments to the function. Passing
#             these as positional arguments overrides any specified in the parent scope.
#
#   **Note:** UTs automatically depend on the module. In order to prevent this, explicitly pass in args
#             to this module, excluding the module.
#
#         e.g. register_fprime_ut("MY_SPECIAL_UT" "${SOME_SOURCE_FILE_LIST}" "") #No dependencies.
#
#  **Note:** this is typically called after any other register calls in the module.
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
#
# ### Unit-Test Without GTest/TesterBase Example ###
#
# Some unit tests run without the need for the autocoding the GTest and TesterBase files. This can be
# done without specifying the Ai.xml file. Most of the time, this style requires specifying some module
# dependencies.
#
# ```
# set(UT_SOURCE_FILES
#   "${FPRIME_FRAMEWORK_PATH}/Svc/CmdDispatcher/CommandDispatcherComponentAi.xml"
#   "${CMAKE_CURRENT_LIST_DIR}/test/ut/CommandDispatcherTester.cpp"
#   "${CMAKE_CURRENT_LIST_DIR}/test/ut/CommandDispatcherImplTester.cpp"
# )
# set(UT_MOD_DEPS
#   Os
# )
# register_fprime_ut()
# ```
#
####
function(register_fprime_ut)
    #### CHECK UT BUILD ####
    if (NOT CMAKE_BUILD_TYPE STREQUAL "TESTING" OR __FPRIME_NO_UT_GEN__)
        return()
    endif()
    get_module_name(${CMAKE_CURRENT_LIST_DIR})
    # PROJECT_NAME is used for the executable name, unless otherwise specified.
    if (${ARGC} GREATER 0)
        set(UT_NAME "${ARGV0}")
    elseif(DEFINED UT_NAME)
        set(UT_NAME ${UT_NAME})
    else()
        set(UT_NAME "${MODULE_NAME}_ut_exe")
    endif()
    # SOURCE_FILES is supplied as the first positional -OR- as the list 'SOURCE_FILES'
    if (${ARGC} GREATER 1)
        set(SC_IFS "${ARGV1}")
    elseif(DEFINED UT_SOURCE_FILES)
    	set(SC_IFS "${UT_SOURCE_FILES}")
    else()
        message(FATAL_ERROR "'UT_SOURCE_FILES' not defined in '${CMAKE_CURRENT_LIST_FILE}'.")
    endif()
    # MOD_DEPS is supplied as an optional second positional -OR- or as  the list 'MOD_DEPS'
    set(MODULE_NAME_NO_SUFFIX "${MODULE_NAME}")
    if (${ARGC} GREATER 2)
        set(MD_IFS "${ARGV2}")
    elseif(DEFINED UT_MOD_DEPS)
    	set(MD_IFS "${MODULE_NAME_NO_SUFFIX};${UT_MOD_DEPS}")
    else()
        set(MD_IFS "${MODULE_NAME_NO_SUFFIX}")
        if (CMAKE_DEBUG_OUTPUT)
            message(STATUS "No extra 'MOD_DEPS' found in '${CMAKE_CURRENT_LIST_FILE}'.")
        endif()
    endif()
    get_nearest_build_root(${CMAKE_CURRENT_LIST_DIR})
    # Explicit call to module register
    generate_ut("${UT_NAME}" "${SC_IFS}" "${MD_IFS}")
    setup_all_module_targets(FPRIME_UT_TARGET_LIST ${MODULE_NAME} "" "${SOURCE_FILES}" "${AC_OUTPUTS}" "${MD_IFS}")
endfunction(register_fprime_ut)

####
# Function `register_fprime_target`:
#
# Some custom targets require a multi-phase build process that is run for each module, and for the
# deployment/executable that is being built. These must therefore register module-specific and
# deployment specific instructions.
#
# **Examples:**
# - dict: build sub dictionaries for each module, and roll-up into a global deployment dictionary
# - sloc: lines of code are counted per-module
# - docs: documentation is also per-module
#
# This function allows the user to register a file containing two functions `add_module_target`
# and `add_global_target`. `add_global_target` adds a top-level target like `make dict` which will
# then depend on every one of the targets created in `add_module_target`.
#
# **TARGET_FILE_PATH:** path to file defining above functions 
###
function(register_fprime_target TARGET_FILE_PATH)
    register_fprime_target_generic(FPRIME_TARGET_LIST ${TARGET_FILE_PATH})
endfunction(register_fprime_target)

function(register_fprime_ut_target TARGET_FILE_PATH)
    register_fprime_target_generic(FPRIME_UT_TARGET_LIST ${TARGET_FILE_PATH})
endfunction(register_fprime_ut_target)

function(register_fprime_target_generic TARGET_LIST TARGET_FILE_PATH)
    # Check for some problems moving forward
    if (NOT EXISTS ${TARGET_FILE_PATH})
        message(FATAL_ERROR "${TARGET_FILE_PATH} does not exist.")
        return()
    endif()
    # Update the global list of target files
    set(TMP "${${TARGET_LIST}}")
    list(APPEND TMP "${TARGET_FILE_PATH}")
    list(REMOVE_DUPLICATES TMP)
    SET(${TARGET_LIST} "${TMP}" CACHE INTERNAL "${TARGET_LIST}: custom fprime targets" FORCE)

    #Setup global target. Note: module targets found during module processing
    setup_global_target("${TARGET_FILE_PATH}")
endfunction(register_fprime_target_generic)

#### Documentation links
# Next Topics:
#  - Setting Options: [Options](Options.md) are used to vary a CMake build.
#  - Adding Deployment: [Deployments](deployment.md) create fprime builds.
#  - Adding Module: [Modules](module.md) register fprime Ports, Components, etc.
#  - Creating Toolchains: [Toolchains](toolchain.md) setup standard CMake Cross-Compiling.
#  - Adding Platforms: [Platforms](platform.md) help fprime set Cross-Compiling specific items.
####

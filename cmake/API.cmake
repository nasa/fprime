####
# API.cmake:
#
# API of the F prime CMake system. These functions represent the external interface to all of
# CMake system. Users and developers should understand these functions in order to perform the
# following actions in CMake:
#
#  - Add an F prime subdirectory to the system.
#  - Register an F prime module (library).
#  - Register an F prime executable (deployment, other executable).
#  - Register an F prime unit-test (executable with special dependencies).
#
# @author mstarch
####

####
# Function `add_fprime_subdirectory`:
#
# Adds a subdirectory to the build system. This allows the system to find new available modules,
# executables, and unit tests. Every module, used or not, by the deployment/root CMAKE file should
# be added as a subdirectory. CMake's dependency system will prevent superfluous building, and thus
# it is inconsequential to add a subdirectory that will not be used.
#
# Every subdirectory added should declare a `CMakeLists.txt`. These in-turn may add their own sub-
# directories. This creates a directed acyclic graph of modules, one subtree of which will be built
# for each executable in the system.
#
# This directory is computed based off the `FPRIME_CURRENT_BUILD_ROOT` variable. It must be set to
# be used. Otherwise, an error will occure.
#
# A user can specify an optional argument to set the build-space, creating a sub-directory under
# the `CMAKE_BINARY_DIR` to place the outputs of the builds of this directory. This is typically
# **not needed**. `EXCLUDE-FROM-ALL` can also be supplied.
# See: https://cmake.org/cmake/help/latest/command/add_fprime_subdirectory.html
#
# **Note:** Replaces CMake `add_subdirectory` call in order to automate the [binary_dir] argument.
#           F prime subdirectories have specific binary roots to avoid collisions, and provide for
#           the standard F prime #include paths rooted at the root of the repo.
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
    # Cannot run without `FPRIME_CURRENT_BUILD_ROOT`
    if (NOT DEFINED FPRIME_CURRENT_BUILD_ROOT)
        message(FATAL_ERROR "FPRIME_CURRENT_BUILD_ROOT not defined. Please include 'FPrime.cmake'")
    elseif (${ARGC} GREATER 2)
        message(FATAL_ERROR "Cannot use 'add_fprime_subdirectory' with [binary_dir] argument.")
    endif()
	file(RELATIVE_PATH NEW_BIN_DIR "${FPRIME_CURRENT_BUILD_ROOT}" "${FP_SOURCE_DIR}")
    # Add component subdirectories using normal add_subdirectory with overriden binary_dir
    add_subdirectory("${FP_SOURCE_DIR}" "${NEW_BIN_DIR}" ${ARGN})
endfunction(add_fprime_subdirectory)

####
# Function `register_fprime_module`:
#
# Registers a module using the F prime build system. This comes with dependency management and F
# Prime autocoding capabilities. This requires two variables to define the autocoding and source
# inputs, and (optionally) any non-standard link dependencies.
#
# Required variables (defined in calling scope):
#
#
# - **SOURCE_FILES:** cmake list of input source files. Place any "*Ai.xml", "*.txt, "*.c", "*.cpp"
#   etc files here. This list will be split into autocoder inputs or sources.
#
# **i.e.:**
# ```
# set(SOURCE_FILES
#     MyComponentAi.xml
#     SomeFile.cpp
#     MyComponentImpl.cpp)
# ```
# - **MOD_DEPS:** (optional) cmake list of extra link dependencies. This is optional, and only
#   needed if non-standard link dependencies are used. If not set or supplied, only F prime
#   detectable dependencies will be available. Link flags like "-lpthread" can be here.
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
#
# ### Standard Module Example ###
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
# Modules that do not require autocoding need not specify *.xml and *.txt files as source. Thus,
# code-only modules just define *.cpp.
#
# ```
# set(SOURCE_FILE
#     SomeFile1.cpp
#     Another2.cpp)
#
# register_fprime_module()
# ```
# Modules requiring only autocoding can just specify *.xml and *.txt files.
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
# Some modules need to pick a specific set of dependencies and ling flags. This can be done
# with the `MOD_DEPS` variable. This feature can be used to pick specific implementations
# for some F prime modules.
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
        message(STATUS "No exra 'MOD_DEPS' found in '${CMAKE_CURRENT_LIST_FILE}'.")
    endif()
    # Explicit call to module register
    generate_library("${SC_IFS}" "${MD_IFS}")
endfunction(register_fprime_module)

####
# Function `register_fprime_executable`:
#
# Registers an executable using the F prime build system. This comes with dependency management and
# F Prime autocoding capabilities. This requires three variables to define the executable name,
# autocoding and source inputs, and (optionally) any non-standard link dependencies.
#
# Required variables (defined in calling scope):
#
#
# - **EXECUTABLE_NAME:** (optional) executable name supplied. If not supplied, or passed in, then
#                     PROJECT_NAME from the CMake definitions is used.
#
# - **SOURCE_FILES:** cmake list of input source files. Place any "*Ai.xml", "*.txt, "*.c", "*.cpp"
#                  etc. files here. This list will be split into autocoder inputs or sources.
# **i.e.:**
# ```
# set(SOURCE_FILES
#     MyComponentAi.xml
#     SomeFile.cpp
#     MyComponentImpl.cpp)
# ```
#
# - **MOD_DEPS:** (optional) cmake list of extra link dependencies. This is optional, and only needed
#   if non-standard link dependencies are used. If not set or supplied, only F prime
#   detectable dependencies will be available. Link flags like "-lpthread" can be here.
#
# **i.e.:**
# ```
# set(LINK_DEPS
#     Module1
#     Module2
#     -lpthread)
# ```
#
# Note: if desired, these fields may be supplied in-order as arguments to the function. Passing
#       these as positional arguments overrides any specified in the parent scope.
#
# ### Standard F Prime Deployment Example ###
#
# To create a standard F prime deployment, an executable needs to be created. This executable
# uses the CMake PROJECT_NAME as the executable name. Thus, it can be created with the following
# source lists. In most F prime deployments, some modules must be specified as they don't tie
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
# ### F Prime Executable With Autocoding/Dependencies ###
#
# Developers can make executables or other utilites that take advantage of F prime autocoding
# and F prime dependencies. These can be registered using the same executable registrar function
# but should specify a specific executable name.
#
# ```
# set(EXECUTABLE_NAME "MyUtitlity")
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
    # Explicit call to module register
    generate_executable("${EX_NAME}" "${SC_IFS}" "${MD_IFS}")
endfunction(register_fprime_executable)

####
# Function `register_fprime_ut`:
#
# Registers an executable unit-test using the F prime build system. This comes with dependency
# management and F Prime autocoding capabilities. This requires three variables defining the
# unit test name, autocoding and source inputs for the unit test, and (optionally) any
# non-standard link dependencies.
#
# **Note:** This is ONLY run when the build type is  TESTING
#
# Required variables (defined in calling scope):
#
# 
# - **UT_NAME:** (optional) executable name supplied. If not supplied, or passed in, then
#   the module name _exe will be used.
#
# - **UT_SOURCE_FILES:** cmake list of UT source files. Place any "*Ai.xml", "*.txt, "*.c", "*.cpp"
#   etc. files here. This list will be split into autocoder inputs or sources.
#   It is only used when building this specific unit test target.
#
#  **i.e.:**
# ```
# set(SOURCE_FILES
#     MyComponentAi.xml
#     SomeFile.cpp
#     MyComponentImpl.cpp)
# ```
#
# - **UT_MOD_DEPS:** (optional) cmake list of extra link dependencies. This is optional, and only
#   needed if non-standard link dependencies are used. If not set or supplied, only
#   F prime detectable dependencies will be available. Link flags like "-lpthread"
#   can be here.
#
# **i.e.:**
# ```
# set(LINK_DEPS
#     Module1
#     Module2
#     -lpthread)
# ```
#
#   **Note:** if desired, these fields may be supplied in-order as arguments to the function. Passing
#             these as positional arguments overrides any specified in the parent scope.
#
#   **Note:** UTs automaitcally depend on the module. In order to prevent this, explicitly pass in args
#             to this module, excluding the module.
#
#         e.g. register_fprime_ut("MY_SPECIAL_UT" "${SOME_SOURCE_FILE_LIST}" "") #No dependenices.
#
# ### Unit-Test Example ###
#
# A standard unit test defines only UT_SOURCES. These sources have the test cpp files and the module
# Ai.xml of the module being tested. This is used to generate the GTest and TesterBase files from this
# Ai.xml. The other UT source files define the implementation of the test.
#
# ```
# set(UT_SOURCE_FILES
#   "${FPRIME_CORE_DIR}/Svc/CmdDispatcher/CommandDispatcherComponentAi.xml"
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
#   "${FPRIME_CORE_DIR}/Svc/CmdDispatcher/CommandDispatcherComponentAi.xml"
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
    if (NOT CMAKE_BUILD_TYPE STREQUAL "TESTING")
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
            message(STATUS "No exra 'MOD_DEPS' found in '${CMAKE_CURRENT_LIST_FILE}'.")
        endif()
    endif()
    # Explicit call to module register
    generate_ut("${UT_NAME}" "${SC_IFS}" "${MD_IFS}")
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
    # Check for some problems moving forward
    if (NOT EXISTS ${TARGET_FILE_PATH})
        message(FATAL_ERROR "${TARGET_FILE_PATH} does not exist.")
        return()
    endif()
    # Update the global list of target files
    set(TMP "${FPRIME_TARGET_LIST}")
    list(APPEND TMP "${TARGET_FILE_PATH}")
    list(REMOVE_DUPLICATES TMP)
    SET(FPRIME_TARGET_LIST "${TMP}" CACHE INTERNAL "FPRIME_TARGET_LIST: custom F prime targtes" FORCE)
	#Setup global target. Note: module targets found during module processing
	setup_global_target("${TARGET_FILE_PATH}")
endfunction(register_fprime_target)

#### Documentation links
# Next Topics:
#  - Setting Options: [Options](Options.md) are used to vary a CMake build.
#  - Adding Deployment: [Deployments](deployment.md) create F prime builds.
#  - Adding Module: [Modules](module.md) register F prime Ports, Compontents, etc.
#  - Creating Toolchains: [Toolchains](toolchain.md) setup standard CMake Cross-Compiling.
#  - Adding Platforms: [Platforms](platform.md) help F prime set Cross-Compiling specific items.
####

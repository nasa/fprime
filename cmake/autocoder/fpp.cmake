####
# autocoder/fpp.cmake:
#
# CMake implementation of an fprime autocoder. Includes the necessary function definitions to implement the fprime
# autocoder API and wraps calls to the FPP tools.
####
include(utilities)

# Does not handle source files one-by-one, but as a complete set
set_property(GLOBAL PROPERTY FPP_HANDLES_INDIVIDUAL_SOURCES FALSE)

####
# Function `is_supported`:
#
# Given a single input file, determines if that input file is processed by this autocoder. Sets the variable named
# IS_SUPPORTED in parent scope to be TRUE if FPP can process the given file or FALSE otherwise.
#
# AC_INPUT_FILE: filepath for consideration
####
function(fpp_is_supported AC_INPUT_FILE)
    set(IS_SUPPORTED FALSE PARENT_SCOPE)
    if (AC_INPUT_FILE MATCHES ".*.fpp")
        set(IS_SUPPORTED TRUE PARENT_SCOPE)
    endif()
endfunction(fpp_is_supported)

####
# Function `fpp_get_framework_dependency_helper`:
#
# Helps detect framework dependencies. Either, it calculates specific dependencies *or* if the Fw roll-up target exists,
# it will depend on that.  Note: targets within Fw always calculate the internal Fw targets as depending on Fw would
# cause a circular dependency.
#
# MODULE_NAME: current module being processed
# FRAMEWORK: list of framework dependencies. **NOTE:** will be overridden in PARENT_SCOPE with updated list
####
function(fpp_get_framework_dependency_helper MODULE_NAME FRAMEWORK)
    # Subset the framework dependencies, or where possible use the Fw interface target
    if (NOT DEFINED FPRIME_FRAMEWORK_MODULES)
        message(FATAL_ERROR "Fw/CMakeLists.txt not included in deployment")
    elseif (NOT TARGET Fw OR MODULE_NAME IN_LIST FPRIME_FRAMEWORK_MODULES)
        list(APPEND FRAMEWORK ${FPRIME_FRAMEWORK_MODULES})
        list(FIND FRAMEWORK "${MODULE_NAME}" START_INDEX)
        math(EXPR START_INDEX "${START_INDEX} + 1")
        list(SUBLIST FRAMEWORK ${START_INDEX} -1 FRAMEWORK)
    else()
        list(APPEND FRAMEWORK Fw)
    endif()
    set(FRAMEWORK "${FRAMEWORK}" PARENT_SCOPE)
endfunction(fpp_get_framework_dependency_helper)

####
# Function `fpp_get_generated_files`:
#
# Given a set of supported autocoder input files, this will produce a list of files that will be generated. It sets the
# following variables in parent scope:
#
# - GENERATED_FILES: a list of files generated for the given input sources
# - MODULE_DEPENDENCIES: inter-module dependencies determined from the given input sources
# - FILE_DEPENDENCIES: specific file dependencies of the given input sources
# - EXTRAS: used to publish the 'imported' file dependencies of the given input files
#
# Note: although this function is only required to set `GENERATED_FILES`, the remaining information is also set as
# setting this information now will prevent a duplicated call to the tooling.
#
# AC_INPUT_FILES: list of supported autocoder input files
####
function(fpp_get_generated_files AC_INPUT_FILES)
    find_program(FPP_DEPEND fpp-depend)
    if (DEFINED FPP_TO_DEPEND-NOTFOUND)
        message(FATAL_ERROR "fpp tools not found, please install them onto your system path")
    endif()
    set(DIRECT_DEPENDENCIES_FILE "${CMAKE_CURRENT_BINARY_DIR}/fpp-cache/direct.txt")
    set(INCLUDED_FILE "${CMAKE_CURRENT_BINARY_DIR}/fpp-cache/include.txt")
    set(MISSING_FILE "${CMAKE_CURRENT_BINARY_DIR}/fpp-cache/missing.txt")
    set(GENERATED_FILE "${CMAKE_CURRENT_BINARY_DIR}/fpp-cache/generated.txt")
    set(FRAMEWORK_FILE "${CMAKE_CURRENT_BINARY_DIR}/fpp-cache/framework.txt")
    set(STDOUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/fpp-cache/stdout.txt")

    # Read files and convert to lists of dependencies. e.g. read INCLUDED_FILE file into INCLUDED variable, then process
    foreach(NAME INCLUDED MISSING GENERATED DIRECT_DEPENDENCIES FRAMEWORK STDOUT)
        if (NOT EXISTS "${${NAME}_FILE}")
            message(FATAL_ERROR "fpp-depend cache did not generate '${${NAME}_FILE}'")
        endif()
        file(READ "${${NAME}_FILE}" "${NAME}")
        string(STRIP "${${NAME}}" "${NAME}")
        string(REPLACE "\n" ";" "${NAME}" "${${NAME}}")
    endforeach()

    fpp_get_framework_dependency_helper("${MODULE_NAME}" "${FRAMEWORK}")

    # First assemble the generated files list
    set(GENERATED_FILES)
    foreach(LINE IN LISTS GENERATED)
        list(APPEND GENERATED_FILES "${CMAKE_CURRENT_BINARY_DIR}/${LINE}")
    endforeach()

    # If we have missing dependencies, print and fail
    if (MISSING)
        message(WARNING "[autocode/fpp] Missing dependencies ${AC_INPUT_FILES}")
        foreach (MISS IN LISTS MISSING)
            message(WARNING "[autocode/fpp]  ${MISS}")
        endforeach()
        message(FATAL_ERROR)
    endif()

    # Module dependencies are: detected "direct" + framework dependencies
    fpp_to_modules("${DIRECT_DEPENDENCIES}" "${AC_INPUT_FILES}" MODULE_DEPENDENCIES)
    list(APPEND MODULE_DEPENDENCIES ${FRAMEWORK})
    list(REMOVE_DUPLICATES MODULE_DEPENDENCIES)
    # File dependencies are any files that this depends on
    set(FILE_DEPENDENCIES ${AC_INPUT_FILES} ${STDOUT})

    # Should have been inherited from previous call to `get_generated_files`
    set(GENERATED_FILES "${GENERATED_FILES}" PARENT_SCOPE)
    set(MODULE_DEPENDENCIES "${MODULE_DEPENDENCIES}" PARENT_SCOPE)
    set(FILE_DEPENDENCIES "${FILE_DEPENDENCIES}" PARENT_SCOPE)
    set(EXTRAS "${IMPORTED}" PARENT_SCOPE)
endfunction(fpp_get_generated_files)

####
# Function `fpp_get_dependencies`:
#
# Given a set of supported autocoder input files, this will produce a set of dependencies. Since this should have
# already been done in `get_generated_files` the implementation just checks the variables are still set.
#
# - MODULE_DEPENDENCIES: inter-module dependencies determined from the given input sources
# - FILE_DEPENDENCIES: specific file dependencies of the given input sources
#
# AC_INPUT_FILES: list of supported autocoder input files
####
function(fpp_get_dependencies AC_INPUT_FILES)
    # Should have been inherited from previous call to `get_generated_files`
    if (NOT DEFINED MODULE_DEPENDENCIES OR NOT DEFINED FILE_DEPENDENCIES)
        message(FATAL_ERROR "The CMake system is inconsistent. Expected pre-calculated MODULE_DEPENDENCIES.")
    endif()
endfunction(fpp_get_dependencies)

####
# Function `fpp_setup_autocode`:
#
# Sets up the steps to run the autocoder and produce the files during the build. This is passed the lists generated
# in calls to `get_generated_files` and `get_dependencies`.
#
# AC_INPUT_FILES: list of supported autocoder input files
# GENERATED_FILES: a list of files generated for the given input sources
# MODULE_DEPENDENCIES: inter-module dependencies determined from the given input sources
# FILE_DEPENDENCIES: specific file dependencies of the given input sources
# EXTRAS: used to publish the 'imported' file dependencies of the given input files
####
function(fpp_setup_autocode AC_INPUT_FILES GENERATED_FILES MODULE_DEPENDENCIES FILE_DEPENDENCIES EXTRAS)
    if (DEFINED FPP_TO_XML-NOTFOUND OR DEFINED FPP_TO_CPP-NOTFOUND)
        message(FATAL_ERROR "fpp tools not found, please install them onto your system path")
    endif()
    string(REGEX REPLACE ";" ","  FPRIME_BUILD_LOCATIONS_SEP_FPP "${FPRIME_BUILD_LOCATIONS}")
    string(REGEX REPLACE ";" ","  FPP_IMPORTED_SEP "${EXTRAS}")
    set(INCLUDES)
    if (FPP_IMPORTED_SEP)
        set(INCLUDES "-i" "${FPP_IMPORTED_SEP}")
    endif()
    # Separate the source files into the CPP and XML steps
    set(GENERATED_AI)
    set(GENERATED_CPP)
    foreach(GENERATED IN LISTS GENERATED_FILES)
        if (GENERATED MATCHES ".*\\.xml")
            list(APPEND GENERATED_AI "${GENERATED}")
        else()
            list(APPEND GENERATED_CPP "${GENERATED}")
        endif()
    endforeach()

    # Add in steps for Ai.xml generation
    if (GENERATED_AI)
        add_custom_command(
                OUTPUT  ${GENERATED_AI}
                COMMAND ${FPP_TO_XML} "-d" "${CMAKE_CURRENT_BINARY_DIR}" ${FILE_DEPENDENCIES}
                    "-p" "${FPRIME_BUILD_LOCATIONS_SEP_FPP}"
                DEPENDS ${EXTRAS} ${FILE_DEPENDENCIES} ${MODULE_DEPENDENCIES}
        )
    endif()
    # Add in steps for CPP generation
    if (GENERATED_CPP)
        add_custom_command(
                OUTPUT  ${GENERATED_CPP}
                COMMAND ${REMOVAL_FILE} ${FPP_TO_CPP} "-d" "${CMAKE_CURRENT_BINARY_DIR}" ${FILE_DEPENDENCIES}
                "-p" "${FPRIME_BUILD_LOCATIONS_SEP_FPP},${CMAKE_BINARY_DIR}"
                DEPENDS ${EXTRAS} ${FILE_DEPENDENCIES} ${MODULE_DEPENDENCIES}
        )
    endif()
endfunction(fpp_setup_autocode)

####
# `fpp_to_modules`:
#
# Helper function. Converts a list of files and a list of autocoder inputs into a list of module names.
#
# FILE_LIST: list of files
# AC_INPUT_FILES: list of autocoder input files
# OUTPUT_VAR: output variable to set with result
####
function(fpp_to_modules FILE_LIST AC_INPUT_FILES OUTPUT_VAR)
    init_variables(OUTPUT_DATA)
    get_module_name("${CMAKE_CURRENT_SOURCE_DIR}")
    set(CURRENT_MODULE "${MODULE_NAME}")
    foreach(INCLUDE IN LISTS AC_INPUT_FILES FILE_LIST)
        get_module_name(${INCLUDE})
        # Here we are adding a module to the modules list if all three of the following are true:
        #  1. Not present already (deduplication)
        #  2. Not the current module directory as learned by the path to the autocoder inputs
        #  3. Not a child of the fprime configuration directory
        # NOTE: item 3 is build on the assumption that configuration .fpp files do not require autocode, but maintain
        # only definitions useful to other modules. This assumption holds as of v3.0.0, but should this assumption break
        # remove the check here, return a known module name (e.g. 'config') for this directory, and place a
        # CMakeLists.txt in that directory that sets up the aforementioned known module and associated target.
        if ("${MODULE_NAME}" IN_LIST OUTPUT_DATA OR CURRENT_MODULE STREQUAL MODULE_NAME OR INCLUDE MATCHES "${FPRIME_CONFIG_DIR}/.*")
            continue() # Skip adding to module list
        endif()
        list(APPEND OUTPUT_DATA "${MODULE_NAME}")
    endforeach()
    set(${OUTPUT_VAR} "${OUTPUT_DATA}" PARENT_SCOPE)
endfunction(fpp_to_modules)

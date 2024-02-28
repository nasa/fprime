####
# implementation.cmake:
#
# Functions used to handle different registered implementations of various cmake packages within F´. The process breaks
# down as follows:
#   1. A module will "require" an implementation X
#   2. A number of modules will "register" as an implementation of X
#   3. A platform package will "choose" a single implementation for X
#   4. Linked modules (executable, deployment, unit-test) may override and "choose" an alternate implementation for X
#
# For example: Os requires an implementation of Os_File. Os_File_Posix implements Os_File. The Linux platform chooses
# Os_File_Posix as the implementation of Os_File. This applies to all linked modules in the system. Os_Baremetal_ut may
# then override this choose using Os_File_MicroFs instead.
#
# > All required implementations apply system-wide. Platforms specifically disallowing use of a given package should
# > "choose" the special implementation "None" to bypass the check.
#
# Internally this uses several properties. Each of these properties are GLOBAL such that these may be set before the
# various targets have been explicitly created. These pro
#
#
####
include_guard()

####
# Function `remap_implementation_choices`:
#
# When a target is declared with a different name than the module name, we need to pull the implementation choices into
# a property for the declared target instead of the inferred target. This is done by reading each implementation choice
# and setting the new property.
#
# Args:
#  INFERRED: inferred target name (FPRIME_CURRENT_MODULE)
#  DECLARED: real target name
####
function(remap_implementation_choices INFERRED DECLARED)
    # Check and setup implementors
    get_property(GLOBAL_REQUIRED GLOBAL PROPERTY "REQUIRED_IMPLEMENTATIONS")
    if (GLOBAL_REQUIRED)
        foreach (IMPLEMENTATION IN LISTS GLOBAL_REQUIRED)
            get_property(IMPLEMENTOR GLOBAL PROPERTY "${INFERRED}_${IMPLEMENTATION}")
            if (IMPLEMENTOR)
                set_property(GLOBAL PROPERTY "${DECLARED}_${IMPLEMENTATION}" "${IMPLEMENTOR}")
            endif()
        endforeach()
    endif ()
endfunction()

####
# Function `setup_executable_implementations`:
#
# Sets up all the needed implementations for the supplied module. This looks up the global set of required
# implementations and delegates each to `setup_executable_implementation` for final setup.
#
# Args:
#    MODULE: module to setup implementation choices for
#####
function(setup_executable_implementations MODULE)
    # Check and setup implementors
    get_property(LOCAL_REQUIRED GLOBAL PROPERTY "REQUIRED_IMPLEMENTATIONS")
    if (LOCAL_REQUIRED)
        init_variables(IMPLEMENTATION_SOURCES IMPLEMENTATION_DEPENDENCIES)
        foreach (IMPLEMENTATION IN LISTS LOCAL_REQUIRED)
            setup_executable_implementation("${IMPLEMENTATION}" "${MODULE}")
            list(APPEND IMPLEMENTATION_SOURCES ${NEW_IMPLEMENTATION_SOURCES})
            list(APPEND IMPLEMENTATION_DEPENDENCIES ${NEW_IMPLEMENTATION_DEPENDENCIES})
        endforeach ()
    endif ()
    set(IMPLEMENTATION_SOURCES "${IMPLEMENTATION_SOURCES}" PARENT_SCOPE)
    set(IMPLEMENTATION_DEPENDENCIES "${IMPLEMENTATION_DEPENDENCIES}" PARENT_SCOPE)
endfunction()

####
# Function `setup_executable_implementation`:
#
# Sets up the given implementation for the given module. Ensures that choices have been made and linking is setup
# without causing global link dependencies.
#
# Args:
#    MODULE: module to setup implementation choices for
#####
function(setup_executable_implementation IMPLEMENTATION MODULE)
    # Get the chosen implementor and fallback to the platform choice
    get_property(IMPLEMENTOR GLOBAL PROPERTY "${MODULE}_${IMPLEMENTATION}")
    if (NOT IMPLEMENTOR)
        get_property(IMPLEMENTOR GLOBAL PROPERTY "${FPRIME_PLATFORM}_${IMPLEMENTATION}")
    endif()
    # Handle a failure to choose anything
    if (NOT IMPLEMENTOR)
        get_property(LOCAL_IMPLEMENTATIONS GLOBAL PROPERTY "${IMPLEMENTATION}_IMPLEMENTORS")
        if (NOT LOCAL_IMPLEMENTATIONS)
            set(LOCAL_IMPLEMENTATIONS "")
        endif ()
        string(REPLACE ";" ", " POSSIBLE "${LOCAL_IMPLEMENTATIONS}")
        message(FATAL_ERROR "An implementation of ${IMPLEMENTATION} is required.  Choose from: ${POSSIBLE}")
    elseif (IMPLEMENTOR STREQUAL "${IMPLEMENTATION}_None")
        if (CMAKE_DEBUG_OUTPUT)
            message(STATUS "[${MODULE}] Declining Implementation of ${IMPLEMENTATION}")
        endif ()
        return()
    endif ()
    if (CMAKE_DEBUG_OUTPUT)
        message(STATUS "[${MODULE}] Using Implementation: ${IMPLEMENTOR} for ${IMPLEMENTATION}")
    endif()
    target_sources("${MODULE}" PRIVATE "$<TARGET_PROPERTY:${IMPLEMENTOR},REQUIRED_SOURCE_FILES>")
    target_link_libraries("${MODULE}" PRIVATE ${IMPLEMENTOR})
    add_dependencies("${MODULE}" ${IMPLEMENTOR})
endfunction()

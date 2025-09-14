####
# implementation.cmake:
#
# This file handles enables deployments (and other executables) to choose implementations that resolve
# at link time. This enables projects to choose different underlying implementations for specific concepts
# (e.g. OS, Memory, etc.) at link time.
####
include_guard()
include(config_assembler)

####
# Function `fprime_target_implementations`:
#
# This function will calculate and add the implementations required for a target. It will use the default
# implementations if no OVERRIDES are provided. Default implementations are the final set of chosen
# implementations as specified in `register_fprime_config` using the CHOOSES_IMPLEMENTATIONS call.
#
# Choosing implementations ensures that there is some implementation of all globally required
# implementations specified by `register_fprime_*` via the REQUIRED_IMPLEMENTATIONS argument.
#
# > [!WARNING]
# > This function may only be called on targets that already exist
#
# > [!NOTE]
# > Target will be updated with FPRIME_CHOSEN_IMPLEMENTATIONS property and FPRIME_CHOSEN_<IMPLEMENTATION>
# > properties for each implementation.  `target_link_libraries` will be updated.
#
# Args:
#  **BUILD_SYSTEM_TARGET**: the target in the build system to add implementations dependencies to
#  **ARGN**: a list of implementations to override the default implementations with
####
function(fprime_target_implementations BUILD_SYSTEM_TARGET)
    append_list_property("${ARGN}" TARGET "${BUILD_SYSTEM_TARGET}" PROPERTY FPRIME_CHOSEN_IMPLEMENTATIONS)
    fprime__internal_choose_implementations("${BUILD_SYSTEM_TARGET}" INTERNAL_ALL_IMPLEMENTATIONS)
    fprime_target_dependencies("${BUILD_SYSTEM_TARGET}" PRIVATE "${INTERNAL_ALL_IMPLEMENTATIONS}")
endfunction()

####
# Function `fprime__internal_implementation_detect_implementations`:
#
# This function will scan the list of global default implementations (detected via the config modules and set in
# the sub-build) and the implementations chosen by the current module. It will then choose one implementation for
# each "IMPLEMENTS" type.
#
# If IMPLEMENTS cannot be determined it is added to the list of unknown implementations.
#
# Args:
#  CURRENT_MODULE: the module to check for implementations
####
function(fprime__internal_implementation_detect_implementations CURRENT_MODULE)
    get_target_property(MODULE_CHOSEN "${CURRENT_MODULE}" FPRIME_CHOSEN_IMPLEMENTATIONS)
    get_property(CONFIG_CHOSEN GLOBAL PROPERTY FPRIME_BASE_CHOSEN_IMPLEMENTATIONS)
    fprime_cmake_debug_message("[implementation] ${CURRENT_MODULE} has chosen implementations: ${MODULE_CHOSEN}")   
    fprime_cmake_debug_message("[implementation]     and base implementations: ${CONFIG_CHOSEN}")   

    foreach (IMPLEMENTATION IN LISTS CONFIG_CHOSEN MODULE_CHOSEN)
        # Break our from empty property
        if (NOT IMPLEMENTATION)
            continue()
        endif()
        # If we know about this implementation, add it as a chosen property
        get_property(IMPLEMENTS GLOBAL PROPERTY "FPRIME_${IMPLEMENTATION}_IMPLEMENTS")
        if (IMPLEMENTS)
            set_target_properties("${CURRENT_MODULE}" PROPERTIES "FPRIME_CHOSEN_${IMPLEMENTS}" "${IMPLEMENTATION}")
        # # None implementation is a special case, the caller is responsible for 
        # elseif(IMPLEMENTATION MATCHES ".*None")
        # Otherwise add it as an unknown implementation
        elseif(NOT FPRIME_IS_SUB_BUILD)
            message(WARNING "[implementation] ${CURRENT_MODULE} has chosen unknown implementation: ${IMPLEMENTATION}")
            append_list_property("${IMPLEMENTATION}" TARGET "${CURRENT_MODULE}" PROPERTY FPRIME_UNKNOWN_IMPLEMENTATIONS)
        endif()
    endforeach()
endfunction()

####
# Function `fprime__internal_choose_implementations`:
#
# This function will scan the list of global default implementations module choices via
# `fprime__internal_implementation_detect_implementations`. It will then ensure that each REQUIRED implementation
# has a chosen implementation.
#
# It is an error to not have a chosen implementation for required unless there is a set of unknown implementations
# and in that case it is a warning.
#
# > [!WARNING]
# > If UNKNOWN implementations are set, it is assumed that one of them fills any missing REQUIRED implementations
# > but users should update those modules to explicitly set IMPLEMENTS flag to avoid this warning.
# >
# > If the UNKNOWN implementations do not provided the required implementations a linker error will occur.
#
# Args:
#  CURRENT_MODULE: the module to check for implementations
#  OUTPUT_VARIABLE: the variable to set with the list of chosen implementations
####
function(fprime__internal_choose_implementations CURRENT_MODULE OUTPUT_VARIABLE)
    set(IMPLEMENTATION_DEPENDENCIES)
    fprime__internal_implementation_detect_implementations("${CURRENT_MODULE}")
    
    # Need to know the modules that were chosen but not known yet
    get_target_property(UNKNOWNS "${CURRENT_MODULE}" FPRIME_UNKNOWN_IMPLEMENTATIONS)
    if (UNKNOWNS)
        list(APPEND IMPLEMENTATION_DEPENDENCIES "${UNKNOWNS}")
    endif()

    # Get all required implementations
    get_property(REQUIRED_IMPLEMENTATIONS GLOBAL PROPERTY "FPRIME_REQUIRED_IMPLEMENTATIONS")
    foreach(REQUIRED IN LISTS REQUIRED_IMPLEMENTATIONS)
        get_target_property(IMPLEMENTOR "${CURRENT_MODULE}" "FPRIME_CHOSEN_${REQUIRED}")
        if (IMPLEMENTOR)
            fprime_cmake_debug_message("[implementation] ${CURRENT_MODULE} has chosen implementation: ${IMPLEMENTOR} for ${REQUIRED}")
            list(APPEND IMPLEMENTATION_DEPENDENCIES "${IMPLEMENTOR}")
        elseif(UNKNOWNS AND NOT FPRIME_IS_SUB_BUILD)
            fprime_cmake_warning("[implementation] ${CURRENT_MODULE} requires implementation of ${REQUIRED} assuming one of ${UNKNOWNS} is correct")
        elseif(NOT FPRIME_IS_SUB_BUILD)
            fprime_cmake_fatal_error("[implementation] ${CURRENT_MODULE} requires implementation of ${REQUIRED} but none was chosen")
        endif()
    endforeach()

    append_list_property("${IMPLEMENTATION_DEPENDENCIES}" TARGET "${CURRENT_MODULE}" PROPERTY FPRIME_CHOSEN_IMPLEMENTATIONS)
    set("${OUTPUT_VARIABLE}" "${IMPLEMENTATION_DEPENDENCIES}" PARENT_SCOPE)
endfunction()

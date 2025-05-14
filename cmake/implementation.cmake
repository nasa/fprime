
include_guard()
include(config_assembler)


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


# ####
# # implementation.cmake:
# #
# # Functions used to handle different registered implementations of various cmake packages within F´. The process breaks
# # down as follows:
# #   1. A module will "require" an implementation X
# #   2. A number of modules will "register" as an implementation of X
# #   3. A platform package will "choose" a single implementation for X
# #   4. Linked modules (executable, deployment, unit-test) may override and "choose" an alternate implementation for X
# #
# # For example: Os requires an implementation of Os_File. Os_File_Posix implements Os_File. The Linux platform chooses
# # Os_File_Posix as the implementation of Os_File. This applies to all linked modules in the system. Os_Baremetal_ut may
# # then override this choose using Os_File_MicroFs instead.
# #
# # > All required implementations apply system-wide. Platforms specifically disallowing use of a given package should
# # > "choose" the special implementation "None" to bypass the check.
# #
# # Internally this uses several properties. Each of these properties are GLOBAL such that these may be set before the
# # various targets have been explicitly created. These pro
# #
# #
# ####
# include_guard()

# ####
# # Function `remap_implementation_choices`:
# #
# # When a target is declared with a different name than the module name, we need to pull the implementation choices into
# # a property for the declared target instead of the inferred target. This is done by reading each implementation choice
# # and setting the new property.
# #
# # Args:
# #  INFERRED: inferred target name (FPRIME_CURRENT_MODULE)
# #  DECLARED: real target name
# ####
# function(remap_implementation_choices INFERRED DECLARED)
#     # Check and setup implementors
#     get_property(GLOBAL_REQUIRED GLOBAL PROPERTY "REQUIRED_IMPLEMENTATIONS")
#     if (GLOBAL_REQUIRED)
#         foreach (IMPLEMENTATION IN LISTS GLOBAL_REQUIRED)
#             get_property(IMPLEMENTOR GLOBAL PROPERTY "${INFERRED}_${IMPLEMENTATION}")
#             if (IMPLEMENTOR)
#                 set_property(GLOBAL PROPERTY "${DECLARED}_${IMPLEMENTATION}" "${IMPLEMENTOR}")
#             endif()
#         endforeach()
#     endif ()
# endfunction()

# ####
# # Function `setup_executable_implementations`:
# #
# # Sets up all the needed implementations for the supplied module. This looks up the global set of required
# # implementations and delegates each to `setup_executable_implementation` for final setup.
# #
# # Args:
# #    MODULE: module to setup implementation choices for
# #####
# function(setup_executable_implementations MODULE)
#     # Check and setup implementors
#     get_property(LOCAL_REQUIRED GLOBAL PROPERTY "REQUIRED_IMPLEMENTATIONS")
#     if (LOCAL_REQUIRED)
#         init_variables(IMPLEMENTATION_SOURCES IMPLEMENTATION_DEPENDENCIES)
#         foreach (IMPLEMENTATION IN LISTS LOCAL_REQUIRED)
#             setup_executable_implementation("${IMPLEMENTATION}" "${MODULE}")
#             list(APPEND IMPLEMENTATION_SOURCES ${NEW_IMPLEMENTATION_SOURCES})
#             list(APPEND IMPLEMENTATION_DEPENDENCIES ${NEW_IMPLEMENTATION_DEPENDENCIES})
#         endforeach ()
#     endif ()
#     set(IMPLEMENTATION_SOURCES "${IMPLEMENTATION_SOURCES}" PARENT_SCOPE)
#     set(IMPLEMENTATION_DEPENDENCIES "${IMPLEMENTATION_DEPENDENCIES}" PARENT_SCOPE)
# endfunction()

# ####
# # Function `setup_executable_implementation`:
# #
# # Sets up the given implementation for the given module. Ensures that choices have been made and linking is setup
# # without causing global link dependencies.
# #
# # Args:
# #    MODULE: module to setup implementation choices for
# #####
# function(setup_executable_implementation IMPLEMENTATION MODULE)
#     # Get the chosen implementor for the module
#     get_property(IMPLEMENTOR GLOBAL PROPERTY "${MODULE}_${IMPLEMENTATION}")
#     # Fallback to the platform in the case that the module does not specify
#     if (NOT IMPLEMENTOR)
#         get_property(IMPLEMENTOR GLOBAL PROPERTY "${FPRIME_PLATFORM}_${IMPLEMENTATION}")
#     endif()
#     # Fallback to the FRAMEWORK_DEFAULT in the case that the platform does not specify
#     if (NOT IMPLEMENTOR)
#         get_property(IMPLEMENTOR GLOBAL PROPERTY "FRAMEWORK_DEFAULT_${IMPLEMENTATION}")
#     endif()
   
#     # Handle a failure to choose anything
#     if (NOT IMPLEMENTOR)
#         get_property(LOCAL_IMPLEMENTATIONS GLOBAL PROPERTY "${IMPLEMENTATION}_IMPLEMENTORS")
#         if (NOT LOCAL_IMPLEMENTATIONS)
#             set(LOCAL_IMPLEMENTATIONS "")
#         endif ()
#         string(REPLACE ";" ", " POSSIBLE "${LOCAL_IMPLEMENTATIONS}")
#         message(FATAL_ERROR "An implementation of ${IMPLEMENTATION} is required.  Choose from: ${POSSIBLE}")
#     elseif (IMPLEMENTOR STREQUAL "${IMPLEMENTATION}_None")
#         if (CMAKE_DEBUG_OUTPUT)
#             message(STATUS "[${MODULE}] Declining Implementation of ${IMPLEMENTATION}")
#         endif ()
#         return()
#     endif ()
#     if (CMAKE_DEBUG_OUTPUT)
#         message(STATUS "[${MODULE}] Using Implementation: ${IMPLEMENTOR} for ${IMPLEMENTATION}")
#     endif()
#     target_sources("${MODULE}" PRIVATE "$<TARGET_PROPERTY:${IMPLEMENTOR},REQUIRED_SOURCE_FILES>")
#     target_link_libraries("${MODULE}" PRIVATE ${IMPLEMENTOR})
#     append_list_property("${IMPLEMENTOR}" TARGET "${MODULE}" PROPERTY FPRIME_DEPENDENCIES)
#     add_dependencies("${MODULE}" ${IMPLEMENTOR})
# endfunction()

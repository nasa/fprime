####
# Module.cmake:
#
# This cmake file contains the functions needed to compile a module for F prime. This
# includes code for generating Enums, Serializables, Ports, Components, and Topologies.
#
# These are used as the building blocks of F prime items. This includes deployments,
# tools, and individual components.
####
include_guard()
include(target/target)
include(implementation)
include(utilities)
set(FPRIME__INTERNAL_EMPTY_CPP "${FPRIME_FRAMEWORK_PATH}/cmake/empty.cpp")
set(VALID_EMPTY "${FPRIME_FRAMEWORK_PATH}/cmake/valid-empty.cpp")

# Name of the F Prime implicit dependency of all modules (our project config)
set(FPRIME_IMPLICIT_DEPENDENCY config)


function(fprime__add_build_system_target TARGET_NAME TYPE SOURCES HEADERS DEPENDENCIES EXTRA_CMAKE_DIRECTIVES)
    # Remap F Prime target type to CMake targe type
    if (TYPE STREQUAL "Executable" OR TYPE STREQUAL "Deployment" OR TYPE STREQUAL "Unit Test")
        set(MODULE_CMAKE_TYPE executable)
    elseif(TYPE STREQUAL "Library")
        set(MODULE_CMAKE_TYPE library)
    else()
        fprime_fatal_cmake_error("Cannot register compilation target of type ${TYPE}")
    endif()
    # Add implicit dependency and filter out self-dependencies 
    list(APPEND DEPENDENCIES config)
    list(REMOVE_ITEM DEPENDENCIES "${TARGET_NAME}")

    # Historical status message for posterity...and to prevent panic amongst users
    message(STATUS "Adding ${TYPE}: ${TARGET_NAME}")
    
    # Add cmake target and attach basic properties
    cmake_language(CALL "add_${MODULE_CMAKE_TYPE}" "${TARGET_NAME}" ${EXTRA_CMAKE_DIRECTIVES} "${SOURCES}")
    # TODO: this is needed because sub-builds still attempt register targets, but without the build target to add back in the
    #       autocoding output. Thus empty must be substituted. Would it be possible to force the library to be an INTERFACE
    #       instead?  Or only add empty on sub-builds? 
    target_sources("${TARGET_NAME}" PRIVATE "${FPRIME__INTERNAL_EMPTY_CPP}")
    target_link_libraries("${TARGET_NAME}" PUBLIC ${DEPENDENCIES})
    set_target_properties("${TARGET_NAME}"
        PROPERTIES
            HEADERS "${HEADERS}"
            FP_TYPE "${TYPE}"
            DEPENDENCIES "${DEPENDENCIES}"
    )

    # System-wide properties
    set_property(GLOBAL PROPERTY MODULE_DETECTION TRUE)
    set_property(GLOBAL APPEND PROPERTY FPRIME_MODULES "${TARGET_NAME}")
endfunction(fprime__add_build_system_target)


####
# module_info.cmake:
#
# module_info is a special target used to precalculate model information for the outer build. It calculates information
# as modules are processed and then saves this information to the outer build cache as a cmake file that sets
# properties.
####
include_guard()
include(config_assembler)
set(FPRIME__INTERNAL_PROPERTY_WRITER "${PYTHON}" "${CMAKE_CURRENT_LIST_DIR}/../tools/property_writer.py")
set(FPRIME__INTERNAL_CAT "${PYTHON}" "${CMAKE_CURRENT_LIST_DIR}/../tools/cat.py")



####
# Function `module_info_add_global_target`:
#
# Reads properties set by module processing and writes them to the outer build cache via a cmake file.
# 
# - **CUSTOM_TARGET_NAME:** name of the target to setup (module_info)
####
function(module_info_add_global_target CUSTOM_TARGET_NAME)
    fprime_cmake_ASSERT("Cannot run module_info outside of a sub-build" FPRIME_IS_SUB_BUILD)
    set(MODULE_INFO_FILE "${CMAKE_CURRENT_BINARY_DIR}/fprime_module_info.cmake")
    add_custom_target("module_info"
        COMMAND "${FPRIME__INTERNAL_PROPERTY_WRITER}" "--file" "${MODULE_INFO_FILE}.part"
            SET GLOBAL PROPERTY FPRIME_BASE_CHOSEN_IMPLEMENTATIONS
            "$<TARGET_PROPERTY:${FPRIME__INTERNAL_CONFIG_TARGET_NAME},FPRIME_CHOSEN_IMPLEMENTATIONS>" 
        COMMAND_EXPAND_LISTS
        COMMAND "${FPRIME__INTERNAL_CAT}"
           $<TARGET_PROPERTY:${CUSTOM_TARGET_NAME},FPRIME_CONCATENATED_FILES>
           "--output" "${MODULE_INFO_FILE}"
        COMMAND_EXPAND_LISTS
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "${MODULE_INFO_FILE}"
            "${FPRIME_BINARY_DIR}/fprime_module_info.cmake"
    )
    append_list_property("${MODULE_INFO_FILE}.part" TARGET "${CUSTOM_TARGET_NAME}" PROPERTY FPRIME_CONCATENATED_FILES)
endfunction(module_info_add_global_target)

####
# Function `module_info_add_deployment_target`:
#
# Just calls `module_info_add_module_target`.
####
function(module_info_add_deployment_target BUILD_MODULE_NAME CUSTOM_TARGET_NAME SOURCES DEPENDENCIES FULL_DEPENDENCIES)
    module_info_add_module_target("${BUILD_MODULE_NAME}" "${CUSTOM_TARGET_NAME}" "${SOURCES}" "${DEPENDENCIES}")
endfunction(module_info_add_deployment_target)

####
# Function `module_info_add_module_target`:
#
# Generates a CMake file that sets a bunch of properties for the outer build. All the module file commands will be
# concatenated together and set in the above scope.
#
# - **MODULE:** module name, unused
# - **TARGET:** name of the target to setup (module_info)
# - **SOURCES:** list of sources filtered to .fpp
# - **DEPENDENCIES:** module dependencies, unused.
####
function(module_info_add_module_target BUILD_MODULE_NAME CUSTOM_TARGET_NAME SOURCES DEPENDENCIES)
    get_target_property(IMPLEMENTS "${BUILD_MODULE_NAME}" FPRIME_IMPLEMENTS)
    get_target_property(REQUIRES "${BUILD_MODULE_NAME}" FPRIME_REQUIRES_IMPLEMENTATIONS)
    set(FILE_LINES "")

    # If this module implements something, set a property
    if (IMPLEMENTS)
        list(APPEND "FILE_LINES"
             "set_property(GLOBAL PROPERTY FPRIME_${BUILD_MODULE_NAME}_IMPLEMENTS ${IMPLEMENTS})\n"
        )
    endif()

    # If this module requires something append it to the list property
    if (REQUIRES)
        string(REPLACE ";" "\;" REQUIRES_ESCAPED "${REQUIRES}")
        list(APPEND "FILE_LINES"
            "include(utilities)\n"
            "append_list_property(\"${REQUIRES_ESCAPED}\" GLOBAL PROPERTY FPRIME_REQUIRED_IMPLEMENTATIONS)\n"
        )
    endif()
    get_target_property(AUTOCODER_INPUTS "${BUILD_MODULE_NAME}" AUTOCODER_INPUTS)
    foreach(AUTOCODER_INPUT IN LISTS AUTOCODER_INPUTS)
        list(APPEND "FILE_LINES"
            "set_property(GLOBAL PROPERTY \"FPRIME_${AUTOCODER_INPUT}_MODULE\"\n"
            "    \"${BUILD_MODULE_NAME}\")\n"
        )
    endforeach()
    set(OUTPUT_FILE_NAME "${CMAKE_CURRENT_BINARY_DIR}/implements-snippet-${BUILD_MODULE_NAME}.cmake")
    file(WRITE "${OUTPUT_FILE_NAME}" ${FILE_LINES})
    append_list_property("${OUTPUT_FILE_NAME}" TARGET "${CUSTOM_TARGET_NAME}" PROPERTY FPRIME_CONCATENATED_FILES)
endfunction(module_info_add_module_target)

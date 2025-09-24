####
# fpp_to_json.cmake:
#
# fpp_to_json is a special target used to generate the JSON representation of the FPP model. This allows tools built on
# the JSON model representation to integrate with the build system.
####
include_guard()
set(FPP__INTERNAL_ARGS_FROM_FILE "${PYTHON}" "${CMAKE_CURRENT_LIST_DIR}/../tools/arguments-from-file.py")
####
# Function `fpp_to_json_add_global_target`:
#
# Sets up the `fpp_to_json` target used to generate depend output across the whole build.
# - **TARGET:** name of the target to setup (fpp_to_json)
####
function(fpp_to_json_add_global_target TARGET)
    add_custom_target("${TARGET}")
endfunction(fpp_to_json_add_global_target)

####
# Function `fpp_to_json_add_deployment_target`:
#
# Pass-through to fpp_to_json_add_module_target. FULL_DEPENDENCIES is unused.
####
function(fpp_to_json_add_deployment_target MODULE TARGET SOURCES DEPENDENCIES FULL_DEPENDENCIES)
    fpp_to_json_add_module_target("${MODULE}" "${TARGET}" "${SOURCES}" "${DEPENDENCIES}")
endfunction(fpp_to_json_add_deployment_target)

####
# Function `fpp_to_json_add_module_target`:
#
# Generates the cached fpp-to-json output fore each module and registers the target to the global fpp_to_json target.
# - **MODULE:** module name, unused
# - **TARGET:** name of the target to setup (fpp_depend)
# - **SOURCES:** list of sources filtered to .fpp
# - **DEPENDENCIES:** module dependencies, unused.
####
function(fpp_to_json_add_module_target MODULE TARGET SOURCES_UNUSED DEPENDENCIES)
    get_target_property(AUTOCODER_INPUTS "${MODULE}" AUTOCODER_INPUTS)
    set(FPP_SOURCES "")
    # Check each source for FPP support
    foreach(SOURCE IN LISTS AUTOCODER_INPUTS)
        fpp_is_supported("${SOURCE}")
        if (IS_SUPPORTED)
            list(APPEND FPP_SOURCES "${SOURCE}")
        endif()
    endforeach()
    file(RELATIVE_PATH OFFSET "${CMAKE_BINARY_DIR}" "${CMAKE_CURRENT_BINARY_DIR}")
    set(LOCAL_DIR "${CMAKE_CURRENT_BINARY_DIR}")
    set(DELIVERY_DIR "${FPRIME_BINARY_DIR}/${OFFSET}")
    file(MAKE_DIRECTORY "${LOCAL_DIR}")
    file(MAKE_DIRECTORY "${DELIVERY_DIR}")
    if (FPP_SOURCES)
        set(OUTPUT_FILES
            "${LOCAL_DIR}/fpp-ast.json"
            "${LOCAL_DIR}/fpp-analysis.json"
            "${LOCAL_DIR}/fpp-loc-map.json"
        )
        add_custom_command(
            OUTPUT ${OUTPUT_FILES}
            COMMAND ${FPP__INTERNAL_ARGS_FROM_FILE}
                "${LOCAL_DIR}/fpp-cache/stdout.txt"
                "fpp-to-json"
                ${FPP_SOURCES}
            DEPENDS
                fpp_depend
                ${FPP_SOURCES}
                "${LOCAL_DIR}/fpp-cache/stdout.txt"
        )
        add_custom_target("${TARGET}_${MODULE}" DEPENDS ${OUTPUT_FILES}
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different ${OUTPUT_FILES} "${DELIVERY_DIR}"
        )
    else()
        add_custom_target("${TARGET}_${MODULE}")
    endif()
    add_dependencies("${TARGET}" "${TARGET}_${MODULE}")
endfunction(fpp_to_json_add_module_target)

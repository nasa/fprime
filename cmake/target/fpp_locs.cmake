####
# fpp_locs.cmake:
#
# fpp_locs is a special target used to build fpp_locs file output. It is run as part of the sub-build that generates
# cached-information about the build itself. This file defines the following target functions:
#
# fpp_locs_add_global_target: global registration target setting up the fpp-locs target run
# fpp_locs_add_deployment_target: unused, required for the API
# fpp_locs_add_module_target: used to identify all source files to pass to location global target
####
include_guard()
set(FPP_LOCATE_DEFS_HELPER "${PYTHON}" "${CMAKE_CURRENT_LIST_DIR}/tools/redirector.py")

####
# Function `fpp_locs_add_global_target`:
#
# Sets up the `fpp_locs` target used to generate the FPP locs file. This is build and then updated in the outer build
# cache.
# - **TARGET:** name of the target to setup (fpp_locs)
####
function(fpp_locs_add_global_target TARGET)
    set(FPP_CONFIGS "${FPRIME_FRAMEWORK_PATH}/Fpp/ToCpp.fpp")
    add_custom_command(
        OUTPUT "${FPRIME_BINARY_DIR}/locs.fpp"
        COMMAND 
            "${FPP_LOCATE_DEFS_HELPER}"
            "${CMAKE_BINARY_DIR}/locs.fpp"
            "${FPP_LOCATE_DEFS}"
            -d "${FPRIME_BINARY_DIR}"
            $<TARGET_PROPERTY:${TARGET},GLOBAL_FPP_FILES>
        COMMAND_EXPAND_LISTS
        COMMAND
            "${CMAKE_COMMAND}"
            -E copy_if_different
            "${CMAKE_BINARY_DIR}/locs.fpp"
            "${FPRIME_BINARY_DIR}/locs.fpp"
    )
    add_custom_target("${TARGET}" DEPENDS "${FPRIME_BINARY_DIR}/locs.fpp")
    set_property(TARGET "${TARGET}" PROPERTY GLOBAL_FPP_FILES ${FPP_CONFIGS})
endfunction(fpp_locs_add_global_target)

####
# Function `fpp_locs_add_module_target`:
#
# Pass-through to fpp_locs_add_module_target. FULL_DEPENDENCIES is unused.
####
function(fpp_locs_add_deployment_target MODULE TARGET SOURCES DEPENDENCIES FULL_DEPENDENCIES)
    fpp_locs_add_module_target("${MODULE}" "${TARGET}" "${SOURCES}" "${DEPENDENCIES}")
endfunction(fpp_locs_add_deployment_target)

####
# Function `fpp_locs_add_module_target`:
#
# Sets up the list of FPP files used in locations generation.  Each FPP source file for each module is added to the
# global target's GLOBAL_FPP_FILES property that is referenced to pass in targets.
# - **MODULE:** module name, unused
# - **TARGET:** name of the target to setup (fpp_locs)
# - **SOURCES:** list of sources filtered to .fpp
# - **DEPENDENCIES:** module dependencies, unused.
####
function(fpp_locs_add_module_target MODULE TARGET SOURCES DEPENDENCIES)
    # Check each source for FPP support
    foreach(SOURCE IN LISTS SOURCES)
        fpp_is_supported("${SOURCE}")
        if (IS_SUPPORTED)
            append_list_property("${SOURCE}" TARGET "${TARGET_NAME}" PROPERTY GLOBAL_FPP_FILES)
        endif()
    endforeach()
endfunction(fpp_locs_add_module_target)

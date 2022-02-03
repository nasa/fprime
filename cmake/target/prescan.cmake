####
# `add_global_target`:
#
# The default implementation defines the target using `add_custom_target` and nothing more.
####
include(autocoder/fpp)

# Prescan output files
set(DEPEND_INPUT_FILE "${FPRIME_PRESCAN}/fpp-depend-input" )
set(FPP_LIST_FILE "${FPRIME_PRESCAN}/prescan-fpp-list")
set(DEP_LIST_FILE "${FPRIME_PRESCAN}/prescan-dep-list")

####
# `add_global_target`:
#
# Used to clear the cache files before the prescan process runs module-to-module.
####
function(prescan_add_global_target TARGET)
    if (NOT DEFINED FPRIME_PRESCAN)
        return()
    endif()
    # Ensure the prescan file is clear
    file(WRITE "${FPP_LIST_FILE}" "")
    file(WRITE "${DEPEND_INPUT_FILE}" "")
    file(WRITE "${DEP_LIST_FILE}" "")
endfunction(prescan_add_global_target)

####
# `add_deployment_target`:
#
# Defers to add_module_target for the prescan target. Nothing else special is done.
####
function(prescan_add_deployment_target MODULE TARGET SOURCES DIRECT_DEPENDENCIES FULL_DEPENDENCY_LIST)
    if (NOT DEFINED FPRIME_PRESCAN)
        return()
    endif()
    prescan_add_module_target("${MODULE}" "${TARGET}" "${SOURCES}" "")
endfunction(prescan_add_deployment_target)

####
# `add_module_target`:
#
# Used to detect scanned modules and inputs during the prescan.  These detected outputs are placed in the cache files
# that the prescan builds.
#
# - **MODULE:** name of the module being processed
# - **TARGET:** unused
# - **SOURCE:** list of source file inputs straight from the CMakeList.txt setup
# - **DEPENDENCIES_DEFUNCT:** not really functioning in prescan. DO NOT USE.
####
function(prescan_add_module_target MODULE TARGET SOURCES DEPENDENCIES_DEFUNCT)
    if (NOT DEFINED FPRIME_PRESCAN)
        return()
    endif()
    file(APPEND "${DEP_LIST_FILE}" ";${MODULE}")
    set(TARGET_FPPS)
    foreach(SOURCE IN LISTS SOURCES)
        fpp_is_supported("${SOURCE}")
        if (IS_SUPPORTED)
            get_filename_component(ABSOLUTE_SOURCE "${SOURCE}" ABSOLUTE)
            file(APPEND "${FPP_LIST_FILE}" ";${ABSOLUTE_SOURCE}")
            list(APPEND TARGET_FPPS "${ABSOLUTE_SOURCE}")
        endif()
    endforeach()
    if (TARGET_FPPS)
        file(RELATIVE_PATH RELATIVE_PATH "${CMAKE_BINARY_DIR}" "${CMAKE_CURRENT_BINARY_DIR}")
        file(APPEND "${DEPEND_INPUT_FILE}" "${FPRIME_PRESCAN}/${RELATIVE_PATH};${TARGET_FPPS}\n")
    endif()
endfunction(prescan_add_module_target)

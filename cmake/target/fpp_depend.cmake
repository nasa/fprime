####
# fpp_depend.cmake:
#
# fpp_depend is a special target used to build cached information for fpp-depend output. It is run as part of the
# sub-build that generates cached-information about the build itself. This file defines the following target
# functions:
#
# fpp_locs_add_global_target: global registration target setting up the fpp-locs target run
####
include_guard()
set(FPP_LOCATE_DEFS_HELPER "${PYTHON}" "${CMAKE_CURRENT_LIST_DIR}/tools/redirector.py")
####
# Function `fpp_depend_add_global_target`:
#
# Sets up the `fpp_depend` target used to generate depend output across the whole build.
# - **TARGET:** name of the target to setup (fpp_depend)
####
function(fpp_depend_add_global_target TARGET)
    add_custom_target("${TARGET}")
endfunction(fpp_depend_add_global_target)

####
# Function `fpp_depend_add_deployment_target`:
#
# Pass-through to fpp_depend_add_module_target. FULL_DEPENDENCIES is unused.
####
function(fpp_depend_add_deployment_target MODULE TARGET SOURCES DEPENDENCIES FULL_DEPENDENCIES)
    fpp_depend_add_module_target("${MODULE}" "${TARGET}" "${SOURCES}" "${DEPENDENCIES}")
endfunction(fpp_depend_add_deployment_target)

####
# Function `fpp_depend_add_module_target`:
#
# Generates the cached fpp-depend output fore each module and registers the target to the global fpp_depend target.
# - **MODULE:** module name, unused
# - **TARGET:** name of the target to setup (fpp_depend)
# - **SOURCES:** list of sources filtered to .fpp
# - **DEPENDENCIES:** module dependencies, unused.
####
function(fpp_depend_add_module_target MODULE TARGET SOURCES DEPENDENCIES)
    set(FPP_SOURCES "")
    # Check each source for FPP support
    foreach(SOURCE IN LISTS SOURCES)
        fpp_is_supported("${SOURCE}")
        if (IS_SUPPORTED)
            list(APPEND FPP_SOURCES "${SOURCE}")
        endif()
    endforeach()
    file(RELATIVE_PATH OFFSET "${CMAKE_BINARY_DIR}" "${CMAKE_CURRENT_BINARY_DIR}")
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/fpp_cache")
    file(MAKE_DIRECTORY "${FPRIME_BINARY_DIR}/${OFFSET}/fpp-cache")
    if (FPP_SOURCES)
        add_custom_command(
            OUTPUT
                "${CMAKE_CURRENT_BINARY_DIR}/fpp_cache/stdout.txt"
                "${CMAKE_CURRENT_BINARY_DIR}/fpp_cache/direct.txt"
                "${CMAKE_CURRENT_BINARY_DIR}/fpp_cache/missing.txt"
                "${CMAKE_CURRENT_BINARY_DIR}/fpp_cache/framework.txt"
                "${CMAKE_CURRENT_BINARY_DIR}/fpp_cache/generated.txt"
                "${CMAKE_CURRENT_BINARY_DIR}/fpp_cache/include.txt"
                "${CMAKE_CURRENT_BINARY_DIR}/fpp_cache/unittest.txt"
            COMMAND
                ${FPP_LOCATE_DEFS_HELPER}
                "${CMAKE_CURRENT_BINARY_DIR}/fpp_cache/stdout.txt"
                "${FPP_DEPEND}"
                "${FPRIME_BINARY_DIR}/locs.fpp"
                "-d" "${CMAKE_CURRENT_BINARY_DIR}/fpp_cache/direct.txt"
                "-m" "${CMAKE_CURRENT_BINARY_DIR}/fpp_cache/missing.txt"
                "-f" "${CMAKE_CURRENT_BINARY_DIR}/fpp_cache/framework.txt"
                "-g" "${CMAKE_CURRENT_BINARY_DIR}/fpp_cache/generated.txt"
                "-i" "${CMAKE_CURRENT_BINARY_DIR}/fpp_cache/include.txt"
                "-u" "${CMAKE_CURRENT_BINARY_DIR}/fpp_cache/unittest.txt"
                "-a"
                ${FPP_SOURCES}
            DEPENDS
                "${FPRIME_BINARY_DIR}/locs.fpp"
                ${FPP_SOURCES}
            COMMAND_EXPAND_LISTS
        )
    else()
        add_custom_command(
            OUTPUT
                "${CMAKE_CURRENT_BINARY_DIR}/fpp_cache/stdout.txt"
                "${CMAKE_CURRENT_BINARY_DIR}/fpp_cache/direct.txt"
                "${CMAKE_CURRENT_BINARY_DIR}/fpp_cache/missing.txt"
                "${CMAKE_CURRENT_BINARY_DIR}/fpp_cache/framework.txt"
                "${CMAKE_CURRENT_BINARY_DIR}/fpp_cache/generated.txt"
                "${CMAKE_CURRENT_BINARY_DIR}/fpp_cache/include.txt"
                "${CMAKE_CURRENT_BINARY_DIR}/fpp_cache/unittest.txt"
            COMMAND
                "${CMAKE_COMMAND}" -E touch
                "${CMAKE_CURRENT_BINARY_DIR}/fpp_cache/stdout.txt"
                "${CMAKE_CURRENT_BINARY_DIR}/fpp_cache/direct.txt"
                "${CMAKE_CURRENT_BINARY_DIR}/fpp_cache/missing.txt"
                "${CMAKE_CURRENT_BINARY_DIR}/fpp_cache/framework.txt"
                "${CMAKE_CURRENT_BINARY_DIR}/fpp_cache/generated.txt"
                "${CMAKE_CURRENT_BINARY_DIR}/fpp_cache/include.txt"
                "${CMAKE_CURRENT_BINARY_DIR}/fpp_cache/unittest.txt"
            COMMAND_EXPAND_LISTS
        )
    endif()

    add_custom_command(
        OUTPUT
            "${FPRIME_BINARY_DIR}/${OFFSET}/fpp_cache/stdout.txt"
            "${FPRIME_BINARY_DIR}/${OFFSET}/fpp-cache/direct.txt"
            "${FPRIME_BINARY_DIR}/${OFFSET}/fpp-cache/missing.txt"
            "${FPRIME_BINARY_DIR}/${OFFSET}/fpp-cache/framework.txt"
            "${FPRIME_BINARY_DIR}/${OFFSET}/fpp-cache/generated.txt"
            "${FPRIME_BINARY_DIR}/${OFFSET}/fpp-cache/include.txt"
            "${FPRIME_BINARY_DIR}/${OFFSET}/fpp-cache/unittest.txt"
        DEPENDS
            "${CMAKE_CURRENT_BINARY_DIR}/fpp_cache/stdout.txt"
            "${CMAKE_CURRENT_BINARY_DIR}/fpp_cache/direct.txt"
            "${CMAKE_CURRENT_BINARY_DIR}/fpp_cache/missing.txt"
            "${CMAKE_CURRENT_BINARY_DIR}/fpp_cache/framework.txt"
            "${CMAKE_CURRENT_BINARY_DIR}/fpp_cache/generated.txt"
            "${CMAKE_CURRENT_BINARY_DIR}/fpp_cache/include.txt"
            "${CMAKE_CURRENT_BINARY_DIR}/fpp_cache/unittest.txt"
        COMMAND
            "${CMAKE_COMMAND}" -E copy_if_different
            "${CMAKE_CURRENT_BINARY_DIR}/fpp_cache/stdout.txt"
            "${CMAKE_CURRENT_BINARY_DIR}/fpp_cache/direct.txt"
            "${CMAKE_CURRENT_BINARY_DIR}/fpp_cache/missing.txt"
            "${CMAKE_CURRENT_BINARY_DIR}/fpp_cache/framework.txt"
            "${CMAKE_CURRENT_BINARY_DIR}/fpp_cache/generated.txt"
            "${CMAKE_CURRENT_BINARY_DIR}/fpp_cache/include.txt"
            "${CMAKE_CURRENT_BINARY_DIR}/fpp_cache/unittest.txt"
            "${FPRIME_BINARY_DIR}/${OFFSET}/fpp-cache"
    )
    add_custom_target("${TARGET}_${MODULE}" DEPENDS 
        "${FPRIME_BINARY_DIR}/${OFFSET}/fpp-cache/direct.txt"
        "${FPRIME_BINARY_DIR}/${OFFSET}/fpp-cache/missing.txt"
        "${FPRIME_BINARY_DIR}/${OFFSET}/fpp-cache/framework.txt"
        "${FPRIME_BINARY_DIR}/${OFFSET}/fpp-cache/generated.txt"
        "${FPRIME_BINARY_DIR}/${OFFSET}/fpp-cache/include.txt"
        "${FPRIME_BINARY_DIR}/${OFFSET}/fpp-cache/unittest.txt"
    )
    add_dependencies("${TARGET}" "${TARGET}_${MODULE}")
endfunction(fpp_depend_add_module_target)

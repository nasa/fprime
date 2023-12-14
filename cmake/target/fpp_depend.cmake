####
# fpp_depend.cmake:
#
# fpp_depend is a special target used to build cached information for fpp-depend output. It is run as part of the
# sub-build that generates cached-information about the build itself.
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
    set(LOCAL_CACHE "${CMAKE_CURRENT_BINARY_DIR}/fpp-cache")
    set(DELIVERY_CACHE "${FPRIME_BINARY_DIR}/${OFFSET}/fpp-cache")
    file(MAKE_DIRECTORY "${LOCAL_CACHE}")
    file(MAKE_DIRECTORY "${DELIVERY_CACHE}")
    if (FPP_SOURCES)
        set(OUTPUT_FILES
            "${LOCAL_CACHE}/stdout.txt"
            "${LOCAL_CACHE}/direct.txt"
            "${LOCAL_CACHE}/missing.txt"
            "${LOCAL_CACHE}/framework.txt"
            "${LOCAL_CACHE}/generated.txt"
            "${LOCAL_CACHE}/include.txt"
            "${LOCAL_CACHE}/unittest.txt"
        )
        add_custom_command(
            OUTPUT ${OUTPUT_FILES}
            COMMAND ${FPP_LOCATE_DEFS_HELPER}
                "${LOCAL_CACHE}/stdout.txt"
                "${FPP_DEPEND}"
                "${FPRIME_BINARY_DIR}/locs.fpp"
                "-d" "${LOCAL_CACHE}/direct.txt"
                "-m" "${LOCAL_CACHE}/missing.txt"
                "-f" "${LOCAL_CACHE}/framework.txt"
                "-g" "${LOCAL_CACHE}/generated.txt"
                "-i" "${LOCAL_CACHE}/include.txt"
                "-u" "${LOCAL_CACHE}/unittest.txt"
                "-a"
                ${FPP_SOURCES}
            DEPENDS
                fpp_locs
                ${FPP_SOURCES}
        )
        add_custom_target("${TARGET}_${MODULE}" DEPENDS ${OUTPUT_FILES}
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different ${OUTPUT_FILES} "${DELIVERY_CACHE}"
        )
    else()
        add_custom_target("${TARGET}_${MODULE}")
    endif()
    add_dependencies("${TARGET}" "${TARGET}_${MODULE}")
endfunction(fpp_depend_add_module_target)

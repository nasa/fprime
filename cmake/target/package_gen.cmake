####
# package_gen.cmake:
#
# Target for packaging and installing artifacts from modules. Defined as a standard target pattern.
# This means that the following functions are defined:
#
# - `add_global_target`: adds a global target 'package_gen'
# - `add_module_target`: adds sub-targets for '<MODULE_NAME>_package_gen'
####


####
# Package function `add_global_target`:
#
# Add target for the `package` custom target. Register a global 'package' target and a 'prepackage' 
# target that creates needed packaging directories.
#
# - **TARGET_NAME:** target name to be generated
####
function(add_global_target TARGET_NAME)
    if (NOT CMAKE_BUILD_TYPE STREQUAL "RELEASE")
        return()
    endif()

    add_custom_target(${TARGET_NAME} ALL)
endfunction(add_global_target)

####
# Package function `add_module_target`:
#
# Adds a module level packaging target that packages module artifacts.
#
# - **MODULE_NAME:** name of the module
# - **TARGET_NAME:** name of target to produce
# - **GLOBAL_TARGET_NAME:** name of produced global target
# - **AC_INPUTS:** list of autocoder inputs
# - **SOURCE_FILES:** list of source file inputs
# - **AC_OUTPUTS:** list of autocoder outputs
# - **MOD_DEPS:** module dependencies of the target
####
function(add_module_target MODULE_NAME TARGET_NAME GLOBAL_TARGET_NAME AC_INPUTS SOURCE_FILES AC_OUTPUTS MOD_DEPS)
    if (NOT CMAKE_BUILD_TYPE STREQUAL "RELEASE")
        return()
    endif()

    set(PKG_DEPS "")
    foreach (DEP ${MODULE_DEPENDENCIES};${MOD_DEPS})
        list(APPEND PKG_DEPS "${DEP}_${GLOBAL_TARGET_NAME}")
    endforeach()

    if ("${FPRIME_OBJECT_TYPE}" STREQUAL "Library")
        add_custom_target(
            ${TARGET_NAME}
            COMMAND ${CMAKE_COMMAND} -E make_directory "${FPRIME_INSTALL_DEST}/${TOOLCHAIN_NAME}/lib/static/"
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${MODULE_NAME}> "${FPRIME_INSTALL_DEST}/${TOOLCHAIN_NAME}/lib/static/"
            DEPENDS ${PKG_DEPS}
        )
    elseif("${FPRIME_OBJECT_TYPE}" STREQUAL "Executable")
        add_custom_target(
            ${TARGET_NAME}
            COMMAND ${CMAKE_COMMAND} -E make_directory "${FPRIME_INSTALL_DEST}/${TOOLCHAIN_NAME}/bin/"
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${MODULE_NAME}> "${FPRIME_INSTALL_DEST}/${TOOLCHAIN_NAME}/bin/"
            DEPENDS ${PKG_DEPS}
        )
        add_dependencies("${GLOBAL_TARGET_NAME}" "${TARGET_NAME}")
    else()
        add_custom_target(${TARGET_NAME} DEPENDS ${PKG_DEPS})
    endif()
endfunction(add_module_target)

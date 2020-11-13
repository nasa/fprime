####
# package_gen.cmake:
#
# Target for packaging and installing artifacts from modules. Defined as a standard target pattern.
# This means that the following functions are defined:
#
# - `add_module_target`: adds sub-targets for '<MODULE_NAME>_package_gen'
####

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

    set(ALL_FILES "")

    if (NOT AC_INPUTS STREQUAL "")
        list(APPEND ALL_FILES "${AC_INPUTS}")
    endif()

    if (NOT SOURCE_FILES STREQUAL "")
        list(APPEND ALL_FILES "${SOURCE_FILES}")
    endif()

    if (NOT AC_OUTPUTS STREQUAL "")
        list(APPEND ALL_FILES "${AC_OUTPUTS}")
    endif()

    string(REPLACE ";" "\n" ALL_FILES "${ALL_FILES}")

    set(LISTING_FILE "${CMAKE_BINARY_DIR}/module_file_listings/${MODULE_NAME}.txt")
    file(WRITE "${LISTING_FILE}" "${ALL_FILES}")

    set(ALL_DEPS "")

    if (NOT MODULE_DEPENDENCIES STREQUAL "")
        list(APPEND ALL_DEPS "${MODULE_DEPENDENCIES}")
    endif()

    if (NOT MOD_DEPS STREQUAL "")
        list(APPEND ALL_DEPS "${MOD_DEPS}")
    endif()

    string(REPLACE ";" "\n" ALL_DEPS "${ALL_DEPS}")

    set(DEPS_FILE "${CMAKE_BINARY_DIR}/module_dependencies/${MODULE_NAME}.txt")
    file(WRITE "${DEPS_FILE}" "${ALL_DEPS}")
endfunction(add_module_target)

####
# target/install.cmake:
#
# Installs fprime into the build-artifacts folder. This is done using CMake's install command. Requires CMake 3.13+.
####
include(utilities)

set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY TRUE CACHE BOOL "Install all dependency" FORCE)

####
# Function `_install_real_helper`:
#
# Ensures targets are real before installing them. Real targets are executables, libraries, and other compile artifacts.
# - **OUTPUT**: output variable set with list of real dependencies
# - **FULL_DEPENDENCIES**: full list of (recursive) dependencies
####
function(_install_real_helper OUTPUT FULL_DEPENDENCIES)
    set(OUTPUT_LIST)
    foreach(DEPENDENCY IN LISTS FULL_DEPENDENCIES)
        is_target_real(IS_REAL "${DEPENDENCY}")
        if (IS_REAL)
            list(APPEND OUTPUT_LIST "${DEPENDENCY}")
        endif()
    endforeach()
    set("${OUTPUT}" "${OUTPUT_LIST}" PARENT_SCOPE)
endfunction()

# Dictionaries are per-deployment, a global variant does not make sense
function(install_add_global_target)
endfunction()

# Function `add_deployment_target`:
#
# Creates a target for UTs per-deployment.
#
# - **MODULE:** name of the module
# - **TARGET:** name of target to produce
# - **SOURCES:** list of source file inputs
# - **DEPENDENCIES:** MOD_DEPS input from CMakeLists.txt
# - **FULL_DEPENDENCIES:** MOD_DEPS input from CMakeLists.txt
####
function(install_add_deployment_target MODULE TARGET SOURCES DEPENDENCIES FULL_DEPENDENCIES)
    set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY TRUE)
    _install_real_helper(INSTALL_DEPENDENCIES "${FULL_DEPENDENCIES}")
    install(TARGETS ${MODULE} ${INSTALL_DEPENDENCIES}
            RUNTIME DESTINATION ${TOOLCHAIN_NAME}/bin
            LIBRARY DESTINATION ${TOOLCHAIN_NAME}/lib
            ARCHIVE DESTINATION ${TOOLCHAIN_NAME}/lib/static)
    get_property(DICTIONARY GLOBAL PROPERTY "${PROJECT_NAME}_FPRIME_DICTIONARY_FILE")
    install(FILES ${DICTIONARY} DESTINATION ${TOOLCHAIN_NAME}/dict)
    add_custom_command(TARGET "${MODULE}" POST_BUILD COMMAND "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}" --target install)
endfunction()

# Install is per-deployment, a module-by-module variant does not make sense
function(install_add_module_target MODULE_NAME TARGET_NAME SOURCE_FILES DEPENDENCIES)
endfunction(install_add_module_target)

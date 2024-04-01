**Note:** auto-generated from comments in: ./target/install.cmake

## target/install.cmake:

Installs fprime into the build-artifacts folder. This is done using CMake's install command. Requires CMake 3.13+.


## Function `_install_real_helper`:

Ensures targets are real before installing them. Real targets are executables, libraries, and other compile artifacts.
- **OUTPUT**: output variable set with list of real dependencies
- **FULL_DEPENDENCIES**: full list of (recursive) dependencies


## function(install_add_deployment_target MODULE TARGET SOURCES DEPENDENCIES FULL_DEPENDENCIES)
    set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY TRUE)
    _install_real_helper(INSTALL_DEPENDENCIES "${FULL_DEPENDENCIES}")
    install(TARGETS ${MODULE} ${INSTALL_DEPENDENCIES}
            RUNTIME DESTINATION ${TOOLCHAIN_NAME}/${MODULE}/bin
            COMPONENT ${MODULE}
            LIBRARY DESTINATION ${TOOLCHAIN_NAME}/${MODULE}/lib
            COMPONENT ${MODULE}
            ARCHIVE DESTINATION ${TOOLCHAIN_NAME}/${MODULE}/lib/static
            COMPONENT ${MODULE}
    )
    install(FILES ${FPRIME_CURRENT_DICTIONARY_FILE} DESTINATION ${TOOLCHAIN_NAME}/${MODULE}/dict COMPONENT ${MODULE})
    add_custom_command(TARGET "${MODULE}" POST_BUILD COMMAND "${CMAKE_COMMAND}"
            -DCMAKE_INSTALL_COMPONENT=${MODULE} -P ${CMAKE_BINARY_DIR}/cmake_install.cmake)
endfunction()

Install is per-deployment, a module-by-module variant does not make sense
function(install_add_module_target MODULE_NAME TARGET_NAME SOURCE_FILES DEPENDENCIES)
endfunction(install_add_module_target)

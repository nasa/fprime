set(LOCATOR_FILES
    "${FPRIME_FRAMEWORK_PATH}/config/locs.fpp"
    "${FPRIME_FRAMEWORK_PATH}/Drv/locs.fpp"
    "${FPRIME_FRAMEWORK_PATH}/Fw/locs.fpp"
    "${FPRIME_FRAMEWORK_PATH}/Ref/locs.fpp"
    "${FPRIME_FRAMEWORK_PATH}/Svc/locs.fpp"
)
string(REGEX REPLACE ";" " " LOCATOR_FILES "${LOCATOR_FILES}")

function(is_supported AC_INPUT_FILE)
    if (AC_INPUT_FILE MATCHES ".*.fpp")
        set(IS_SUPPORTED TRUE PARENT_SCOPE)
    else()
        set(IS_SUPPORTED FALSE PARENT_SCOPE)
    endif()
endfunction(is_supported)

function(get_generated_files AC_INPUT_FILE)
    execute_process(COMMAND fpp-filenames "${AC_INPUT_FILE}" RESULT_VARIABLE ERR_RETURN OUTPUT_VARIABLE STDOUT OUTPUT_STRIP_TRAILING_WHITESPACE)
    if (ERR_RETURN)
        return()
    endif()
    STRING(REGEX REPLACE "\n" ";" STDOUT "${STDOUT}")

    set(GENERATED_FILES)
    foreach(LINE IN LISTS STDOUT)
        list(APPEND GENERATED_FILES "${CMAKE_CURRENT_BINARY_DIR}/${LINE}")
    endforeach()
    set(GENERATED_FILES "${GENERATED_FILES}" PARENT_SCOPE)

endfunction(get_generated_files)

function(get_dependencies AC_INPUT_FILE)
    set(INCLUDED_FILE "${CMAKE_CURRENT_BINARY_DIR}/included.txt")
    set(MISSING_FILE "${CMAKE_CURRENT_BINARY_DIR}/missing.txt")

    execute_process(COMMAND fpp-depend ${LOCATOR_FILES} "${AC_INPUT_FILE}"
        -i "${INCLUDED_FILE}"
        -m "${MISSING_FILE}"
        RESULT_VARIABLE ERR_RETURN
        OUTPUT_VARIABLE STDOUT OUTPUT_STRIP_TRAILING_WHITESPACE)
    # Report failure.  If we are generating files, this must work.
    if (ERR_RETURN)
        message(FATAL_ERRO "Failed to run 'fpp-depend ${LOCATOR_FILES} ${AC_INPUT_FILE} -i ${INCLUDED_FILE} -m ${MISSING_FILE}'")
        return()
    endif()

    # Read file and convert to lists of dependencies
    file(READ "${INCLUDED_FILE}" INCLUDED)
    file(READ "${MISSING_FILE}" MISSING)
    string(REGEX REPLACE "\n" ";" INCLUDED "${INCLUDED}")
    string(REGEX REPLACE "\n" ";" MISSING "${MISSING}")
    string(REGEX REPLACE "\n" ";" IMPORTED "${STDOUT}")

    # If we have missing dependencies, print and fail
    if (MISSING)
        message(WARNING "[Autocode/fpp] Missing dependencies ${AC_INPUT_FILE}")
        foreach (MISS IN LISTS MISSING)
            message(WARNING "[Autocode/fpp]  ${MISS}")
        endforeach()
        message(FATAL_ERROR)
    endif()
    fpp_to_modules("${IMPORTED}" MODULE_DEPENDENCIES)
    set(FILE_DEPENDENCIES)
    list(APPEND FILE_DEPENDENCIES ${INCLUDED} ${IMPORTED})
    # Should have been inherited from previous call to `get_generated_files`
    set(MODULE_DEPENDENCIES "${MODULE_DEPENDENCIES}" PARENT_SCOPE)
    set(FILE_DEPENDENCIES "${FILE_DEPENDENCIES}" PARENT_SCOPE)
    set(FPP_IMPORTED "${IMPORTED}" PARENT_SCOPE)
endfunction(get_dependencies)


function(setup_autocode AC_INPUT_FILE GENERATED_FILES MODULE_DEPENDENCIES FILE_DEPENDENCIES)
    string(REGEX REPLACE ";" ","  FPRIME_BUILD_LOCATIONS_SEP_FPP "${FPRIME_BUILD_LOCATIONS}")
    string(REGEX REPLACE ";" ","  FPP_IMPORTED_SEP "${FPP_IMPORTED}")
    set(INCLUDES)
    if (FPP_IMPORTED_SEP)
        set(INCLUDES "-i" "${FPP_IMPORTED_SEP}")
    endif()
    add_custom_command(
            OUTPUT  ${GENERATED_FILES}
            COMMAND fpp-to-xml "${AC_INPUT_FILE}" "-d" "${CMAKE_CURRENT_BINARY_DIR}" ${INCLUDES}
                "-p" "${FPRIME_BUILD_LOCATIONS_SEP_FPP}"
            DEPENDS ${AC_INPUT_FILE} ${FILE_DEPENDENCIES} ${MODULE_DEPENDENCIES}
    )
endfunction(setup_autocode)



function(fpp_to_modules FILE_LIST OUTPUT_VAR)
    set(OUTPUT)
    foreach(INCLUDE IN LISTS FILE_LIST)
        get_filename_component(MODULE_DIR "${INCLUDE}" DIRECTORY)
        if (NOT "${MODULE_DIR}" IN_LIST OUTPUT)
            list(APPEND OUTPUT "${MODULE_DIR}")
        endif()
    endforeach()
    set(${OUTPUT_VAR} "${OUTPUT}" PARENT_SCOPE)
endfunction(fpp_to_modules)
set(LOCATOR_FILES
    "${FPRIME_FRAMEWORK_PATH}/Fpp/locs.fpp"
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
    find_program(FPP_DEPEND fpp-depend)
    if (DEFINED FPP_TO_DEPEND-NOTFOUND)
        message(FATAL_ERROR "fpp tools not found, please install them onto your system path")
    endif()
    set(DIRECT_FILE "${CMAKE_CURRENT_BINARY_DIR}/direct.txt")
    set(INCLUDED_FILE "${CMAKE_CURRENT_BINARY_DIR}/included.txt")
    set(MISSING_FILE "${CMAKE_CURRENT_BINARY_DIR}/missing.txt")
    set(GENERATED_FILE "${CMAKE_CURRENT_BINARY_DIR}/generated.txt")
    execute_process(COMMAND ${FPP_DEPEND} ${LOCATOR_FILES} "${AC_INPUT_FILE}"
        -d "${DIRECT_FILE}"
        -i "${INCLUDED_FILE}"
        -m "${MISSING_FILE}"
        -g "${GENERATED_FILE}"
        RESULT_VARIABLE ERR_RETURN
        OUTPUT_VARIABLE STDOUT OUTPUT_STRIP_TRAILING_WHITESPACE)
    # Report failure.  If we are generating files, this must work.
    if (ERR_RETURN)
        message(FATAL_ERROR "Failed to run 'fpp-depend ${LOCATOR_FILES} ${AC_INPUT_FILE} -d ${DIRECT_FILE} -i ${INCLUDED_FILE} -m ${MISSING_FILE} -g ${GENERATED_FILE}' and RC ${ERR_RETURN}")
        return()
    endif()

    # Read file and convert to lists of dependencies
    file(READ "${INCLUDED_FILE}" INCLUDED)
    file(READ "${MISSING_FILE}" MISSING)
    file(READ "${GENERATED_FILE}" GENERATED)
    file(READ "${DIRECT_FILE}" DIRECT_DEPENDENCIES)

    string(STRIP "${INCLUDED}" INCLUDED)
    string(STRIP "${MISSING}" MISSING)
    string(STRIP "${DIRECT_DEPENDENCIES}" DIRECT_DEPENDENCIES)
    string(STRIP "${GENERATED}" GENERATED)

    string(REGEX REPLACE "\n" ";" IMPORTED "${STDOUT}")
    string(REGEX REPLACE "\n" ";" INCLUDED "${INCLUDED}")
    string(REGEX REPLACE "\n" ";" MISSING "${MISSING}")
    string(REGEX REPLACE "\n" ";" DIRECT_DEPENDENCIES "${DIRECT_DEPENDENCIES}")
    string(REGEX REPLACE "\n" ";" GENERATED "${GENERATED}")

    # First assemble the generated files list
    set(GENERATED_FILES)
    foreach(LINE IN LISTS GENERATED)
        list(APPEND GENERATED_FILES "${CMAKE_CURRENT_BINARY_DIR}/${LINE}")
    endforeach()
    set(GENERATED_FILES "${GENERATED_FILES}" PARENT_SCOPE)

    # If we have missing dependencies, print and fail
    if (MISSING)
        message(WARNING "[Autocode/fpp] Missing dependencies ${AC_INPUT_FILE}")
        foreach (MISS IN LISTS MISSING)
            message(WARNING "[Autocode/fpp]  ${MISS}")
        endforeach()
        message(FATAL_ERROR)
    endif()
    fpp_to_modules("${DIRECT_DEPENDENCIES}" "${AC_INPUT_FILE}" MODULE_DEPENDENCIES)
    set(FILE_DEPENDENCIES)
    list(APPEND FILE_DEPENDENCIES ${AC_INPUT_FILE} ${INCLUDED})
    # TODO: fix-me
    if (NOT "${MODULE_NAME}" STREQUAL "Os" AND NOT "${MODULE_NAME}" MATCHES "^Fw_")
        foreach(KNOWN IN ITEMS "Fw_Cfg" "Fw_Types" "Fw_Time" "Fw_Com" "Os" "Fw_Tlm" "Fw_Cmd" "Fw_Log" "Fw_Prm" "Fw_Comp" "Fw_CompQueued")
            list(APPEND MODULE_DEPENDENCIES "${KNOWN}")
        endforeach()
    endif()
    # Should have been inherited from previous call to `get_generated_files`
    set(MODULE_DEPENDENCIES "${MODULE_DEPENDENCIES}" PARENT_SCOPE)
    set(FILE_DEPENDENCIES "${FILE_DEPENDENCIES}" PARENT_SCOPE)
    set(EXTRAS "${IMPORTED}" PARENT_SCOPE)
endfunction(get_generated_files)

function(get_dependencies AC_INPUT_FILE)
    # Should have been inherited from previous call to `get_generated_files`
    if (NOT DEFINED MODULE_DEPENDENCIES OR NOT DEFINED FILE_DEPENDENCIES)
        message(FATAL "The CMake system is inconsistent. Please contact a developer.")
    endif()
endfunction(get_dependencies)

function(setup_autocode AC_INPUT_FILE GENERATED_FILES MODULE_DEPENDENCIES FILE_DEPENDENCIES EXTRAS)
    find_program(FPP_TO_XML fpp-to-xml)
    find_program(FPP_TO_CPP fpp-to-cpp)
    if (DEFINED FPP_TO_XML-NOTFOUND OR DEFINED FPP_TO_CPP-NOTFOUND)
        message(FATAL_ERROR "fpp tools not found, please install them onto your system path")
    endif()
    string(REGEX REPLACE ";" ","  FPRIME_BUILD_LOCATIONS_SEP_FPP "${FPRIME_BUILD_LOCATIONS}")
    string(REGEX REPLACE ";" ","  FPP_IMPORTED_SEP "${EXTRAS}")
    set(INCLUDES)
    if (FPP_IMPORTED_SEP)
        set(INCLUDES "-i" "${FPP_IMPORTED_SEP}")
    endif()
    # Separate the source files into the CPP and XML steps
    set(GENERATED_AI)
    set(GENERATED_CPP)
    foreach(GENERATED IN LISTS GENERATED_FILES)
        if (GENERATED MATCHES ".*\\.xml")
            list(APPEND GENERATED_AI "${GENERATED}")
        else()
            list(APPEND GENERATED_CPP "${GENERATED}")
        endif()
    endforeach()

    # Add in steps for Ai.xml generation
    if (GENERATED_AI)
        add_custom_command(
                OUTPUT  ${GENERATED_AI}
                COMMAND ${FPP_TO_XML} "${AC_INPUT_FILE}" "-d" "${CMAKE_CURRENT_BINARY_DIR}" ${INCLUDES}
                    "-p" "${FPRIME_BUILD_LOCATIONS_SEP_FPP}"
                DEPENDS ${AC_INPUT_FILE} ${FILE_DEPENDENCIES} ${MODULE_DEPENDENCIES}
        )
    endif()
    # Add in steps for CPP generation
    if (GENERATED_CPP)
        add_custom_command(
                OUTPUT  ${GENERATED_CPP}
                COMMAND ${FPP_TO_CPP} "${AC_INPUT_FILE}" "-d" "${CMAKE_CURRENT_BINARY_DIR}" ${INCLUDES}
                "-p" "${FPRIME_BUILD_LOCATIONS_SEP_FPP},${CMAKE_BINARY_DIR}"
                DEPENDS ${AC_INPUT_FILE} ${FILE_DEPENDENCIES} ${MODULE_DEPENDENCIES}
        )
    endif()
endfunction(setup_autocode)



function(fpp_to_modules FILE_LIST AC_INPUT_FILE OUTPUT_VAR)
    get_filename_component(AI_DIR "${AC_INPUT_FILE}" DIRECTORY)
    set(OUTPUT)
    foreach(INCLUDE IN LISTS FILE_LIST)
        get_filename_component(MODULE_DIR "${INCLUDE}" DIRECTORY)
        if (NOT "${MODULE_DIR}" IN_LIST OUTPUT AND NOT AI_DIR STREQUAL MODULE_DIR)
            list(APPEND OUTPUT "${MODULE_DIR}")
        endif()
    endforeach()
    set(${OUTPUT_VAR} "${OUTPUT}" PARENT_SCOPE)
endfunction(fpp_to_modules)

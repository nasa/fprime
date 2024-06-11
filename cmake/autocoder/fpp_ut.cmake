####
# autocoder/fpp.cmake:
#
# CMake implementation of an fprime autocoder. Includes the necessary function definitions to implement the fprime
# autocoder API and wraps calls to the FPP tools.
####
include(utilities)
include(autocoder/helpers)
include(autocoder/fpp)

autocoder_setup_for_multiple_sources()

####
# Function `fpp_ut_is_supported`:
#
# Runs on any .fpp file.
####
function(fpp_ut_is_supported AC_INPUT_FILE)
    autocoder_support_by_suffix(".fpp" "${AC_INPUT_FILE}" TRUE)
endfunction(fpp_ut_is_supported)

####
# Function `fpp_ut_setup_autocode`:
#
# Sets up the steps to run the autocoder and produce the files during the build. This is passed the lists generated
# in calls to `get_generated_files` and `get_dependencies`.
#
# AC_INPUT_FILES: list of supported autocoder input files
####
function(fpp_ut_setup_autocode AC_INPUT_FILES)
    if (DEFINED FPP_TO_XML-NOTFOUND OR DEFINED FPP_TO_CPP-NOTFOUND)
        message(FATAL_ERROR "fpp tools not found, please install them onto your system path")
    endif()
    fpp_info("${AC_INPUT_FILES}")
    string(REGEX REPLACE ";" ","  FPRIME_BUILD_LOCATIONS_COMMA_SEP "${FPRIME_BUILD_LOCATIONS}")
    string(REGEX REPLACE ";" ","  FPP_IMPORTS_COMMA_SEP "${FPP_IMPORTS}")
    set(IMPORTS)
    if (FPP_IMPORTS_COMMA_SEP)
        set(IMPORTS "-i" "${FPP_IMPORTS_COMMA_SEP}")
    endif()
    # Separate the source files into the CPP and XML steps
    set(GENERATED_AI)
    set(GENERATED_CPP)
    foreach(GENERATED IN LISTS UNITTEST_FILES)
        if (GENERATED MATCHES ".*\\.xml")
            list(APPEND GENERATED_AI "${GENERATED}")
        else()
            list(APPEND GENERATED_CPP "${GENERATED}")
        endif()
    endforeach()
    set(CLI_ARGS ${FPP_TO_CPP} "-u" "-d" "${CMAKE_CURRENT_BINARY_DIR}" ${IMPORTS} ${AC_INPUT_FILES} "-p" "${FPRIME_BUILD_LOCATIONS_COMMA_SEP},${CMAKE_BINARY_DIR}")
    if (UT_AUTO_HELPERS)
        list(APPEND CLI_ARGS "-a")
    else()
        set(NEW_GENERATED_CPP)
        foreach(GC_FILE IN LISTS GENERATED_CPP)
            ends_with(IS_MATCHING "${GC_FILE}" "TesterHelpers.cpp")
            if (NOT IS_MATCHING)
                list(APPEND NEW_GENERATED_CPP "${GC_FILE}")
            endif()
        endforeach()
        set(GENERATED_CPP "${NEW_GENERATED_CPP}")
    endif()

    # Add in steps for CPP generation
    if (GENERATED_CPP)
        add_custom_command(
                OUTPUT ${GENERATED_CPP}
                COMMAND ${CLI_ARGS}
                DEPENDS ${FILE_DEPENDENCIES} ${MODULE_DEPENDENCIES}
        )
    endif()
    set(AUTOCODER_GENERATED ${GENERATED_AI} ${GENERATED_CPP})
    set(AUTOCODER_GENERATED "${AUTOCODER_GENERATED}" PARENT_SCOPE)
    set(AUTOCODER_DEPENDENCIES "${MODULE_DEPENDENCIES}" PARENT_SCOPE)
    set(AUTOCODER_INCLUDES "${FILE_DEPENDENCIES}" PARENT_SCOPE)
endfunction(fpp_ut_setup_autocode)


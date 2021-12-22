####
# fpp.cmake:
#
# A setup to install fpp tool suite automatically as part of the CMake run. If the user wants to avoid this, the user
# should install FPP on the system path and that will be used.
####

set(FPP_VERSION ee355fc99eb8040157c62e69f58ac6a8435cd981)
set(FPP_TOOLS_PATH "${CMAKE_BINARY_DIR}/fpp-tools-install" CACHE PATH "Installation path for fpp tools")

####
# locate_fpp_tools:
#
# Locates the fpp tool suite and sets FPP_FOUND if the right version of the tools is found. It will look first to the
# above install location and then to the system path as a fallback.
####
function(locate_fpp_tools NO_DEFAULTS)
    set(FPP_RE_MATCH "(v[0-9].[0-9].[0-9]) commit ([a-f0-9]+)")
    # Loop through each tool, looking if it was found and check the version
    foreach(TOOL FPP_DEPEND FPP_TO_XML FPP_TO_CPP FPP_LOCATE_DEFS)
        string(TOLOWER ${TOOL} PROGRAM)
        string(REPLACE "_" "-" PROGRAM "${PROGRAM}")

        # Clear any previous version of this find and search in this order: install dir, system path
        unset(${TOOL} CACHE)
        find_program(${TOOL} ${PROGRAM} PATHS ${FPP_TOOLS_PATH} NO_DEFAULT_PATH)
        find_program(${TOOL} ${PROGRAM} PATHS ${FPP_TOOLS_PATH})

        # If the tool exists, check the version
        if (${TOOL})
            execute_process(COMMAND ${${TOOL}} --help OUTPUT_VARIABLE OUTPUT_TEXT)
            if (OUTPUT_TEXT MATCHES "${FPP_RE_MATCH}")
                if ("${CMAKE_MATCH_1}" STREQUAL "${FPP_VERSION}" OR "${CMAKE_MATCH_2}" STREQUAL "${FPP_VERSION}")
                    continue()
                endif()
                message(STATUS "[fpp-tools] ${${TOOL}} version ${CMAKE_MATCH_0} not expected version ${FPP_VERSION}")
            endif()
        endif()
        set(FPP_FOUND False PARENT_SCOPE)
        return()
    endforeach()
    set(FPP_FOUND True PARENT_SCOPE)
    message(STATUS "[fpp-tools] fpp-depend found at: ${FPP_DEPEND}")
    message(STATUS "[fpp-tools] fpp-to-xml found at: ${FPP_TO_XML}")
    message(STATUS "[fpp-tools] fpp-to-cpp found at: ${FPP_TO_CPP}")
    message(STATUS "[fpp-tools] fpp-locate-defs found at: ${FPP_LOCATE_DEFS}")
endfunction(locate_fpp_tools)


message(STATUS "[fpp-tools] Searching for fpp-tools")
locate_fpp_tools(False)
# Download and unpack googletest at configure time if it doesn't exit already
if (NOT FPP_FOUND)
    make_directory(${FPP_TOOLS_PATH})
    include("${CMAKE_CURRENT_LIST_DIR}/sbt.cmake")
    message(STATUS "[fpp-tools] Installing new tools to ${FPP_TOOLS_PATH}, this will take a few minutes")
    configure_file("${CMAKE_CURRENT_LIST_DIR}/CMakeLists.fpp.txt.in" fpp-download/CMakeLists.txt)
    execute_process(
        COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/fpp-download
        OUTPUT_QUIET
        ERROR_VARIABLE ERROR_OUT
    )
    if(result)
      message(FATAL_ERROR "Download step for fpp failed: ${result}\n${ERROR_OUT}")
    endif()
    # Cached JAVA and sbt items
    find_program(JAVA java)
    find_program(SBT sbt)
    get_filename_component(JAVA_PATH "${JAVA}" DIRECTORY)
    get_filename_component(SBT_PATH "${SBT}" DIRECTORY)
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E env PATH="${JAVA_PATH}:${SBT_PATH}:$ENV{PATH}" ${CMAKE_COMMAND} --build .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/fpp-download
        OUTPUT_QUIET
        ERROR_VARIABLE ERROR_OUT
    )
    if(result)
        message(FATAL_ERROR "Build step for fpp failed: ${result}\n${ERROR_OUT}")
    endif()
    locate_fpp_tools(True)
    if (NOT FPP_FOUND)
        message(FATAL_ERROR "[fpp-tools] Failed to install fpp-tools version ${FPP_VERSION}")
    endif()
endif()

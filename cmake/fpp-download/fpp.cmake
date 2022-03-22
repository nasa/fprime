####
# fpp.cmake:
#
# A setup to install fpp tool suite automatically as part of the CMake run. If the user wants to avoid this, the user
# should install FPP on the system path and that will be used.
####

set(FPP_VERSION v1.0.1)
set(FPP_TOOLS_PATH "${CMAKE_BINARY_DIR}/fpp-tools-install" CACHE PATH "Installation path for fpp tools")

####
# locate_fpp_tools:
#
# Locates the fpp tool suite and sets FPP_FOUND if the right version of the tools is found. It will look first to the
# above install location and then to the system path as a fallback.
####
function(locate_fpp_tools)
    # Loop through each tool, looking if it was found and check the version
    foreach(TOOL FPP_DEPEND FPP_TO_XML FPP_TO_CPP FPP_LOCATE_DEFS)
        string(TOLOWER ${TOOL} PROGRAM)
        string(REPLACE "_" "-" PROGRAM "${PROGRAM}")

        # Clear any previous version of this find and search in this order: install dir, system path
        unset(${TOOL} CACHE)
        find_program(${TOOL} ${PROGRAM} PATHS ${FPP_TOOLS_PATH} NO_DEFAULT_PATH)
        find_program(${TOOL} ${PROGRAM} PATHS ${FPP_TOOLS_PATH})
        # If the tool exists, check the version
        if (TOOL AND FPRIME_SKIP_TOOLS_VERSION_CHECK)
            continue()
        elseif(TOOL)
            set(FPP_RE_MATCH "(v[0-9]+\.[0-9]+\.[0-9]+[a-g0-9-]*)")
            execute_process(COMMAND ${${TOOL}} --help OUTPUT_VARIABLE OUTPUT_TEXT)
            if (OUTPUT_TEXT MATCHES "${FPP_RE_MATCH}")
                if (CMAKE_MATCH_1 STREQUAL "${FPP_VERSION}")
                    continue()
                endif()
                message(STATUS "[fpp-tools] ${${TOOL}} version ${CMAKE_MATCH_1} not expected version ${FPP_VERSION}")
            endif()
        endif()
        set(FPP_FOUND FALSE PARENT_SCOPE)
        return()
    endforeach()
    set(FPP_FOUND TRUE PARENT_SCOPE)
endfunction(locate_fpp_tools)


message(STATUS "[fpp-tools] Searching for fpp-tools")
locate_fpp_tools()
# Download and unpack googletest at configure time if it doesn't exit already
if (NOT FPP_FOUND)
    message(STATUS "[fpp-tools] Installing new tools to ${FPP_TOOLS_PATH}, this will take a few minutes")
    make_directory(${FPP_TOOLS_PATH})
    include("${CMAKE_CURRENT_LIST_DIR}/sbt.cmake")
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
    locate_fpp_tools()
    if (NOT FPP_FOUND)
        message(FATAL_ERROR "[fpp-tools] Failed to install fpp-tools version ${FPP_VERSION}")
    endif()
endif()

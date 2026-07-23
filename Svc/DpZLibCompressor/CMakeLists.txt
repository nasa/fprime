####
# F Prime CMakeLists.txt:
#
# SOURCES: list of source files (to be compiled)
# AUTOCODER_INPUTS: list of files to be passed to the autocoders
# DEPENDS: list of libraries that this module depends on
#
# More information in the F´ CMake API documentation:
# https://fprime.jpl.nasa.gov/latest/docs/reference/api/cmake/API/
#
####

# Module names are derived from the path from the nearest project/library/framework
# root when not specifically overridden by the developer, i.e. the module defined by
# `MyProj/Some/Path/CMakeLists.txt` will be named `MyProj_Some_Path`.

# For cross-compilation, look in the cross-compiler's sysroot
if (CMAKE_CROSSCOMPILING AND CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
    find_library(ZLIB_LIBRARY z PATHS /usr/aarch64-linux-gnu/lib NO_DEFAULT_PATH)
    if (ZLIB_LIBRARY STREQUAL "ZLIB_LIBRARY-NOTFOUND")
        message(STATUS "Unable to add Svc_DpZLibCompressor. libz not found")
        set(ZLIB_FOUND FALSE)
    else()
        set(ZLIB_FOUND TRUE)
        set(ZLIB_LIBRARIES ${ZLIB_LIBRARY})
        set(ZLIB_INCLUDE_DIRS /usr/aarch64-linux-gnu/include)
    endif()
else()
    find_package(ZLIB)

    if (NOT ZLIB_FOUND)
        find_library(ZLIB_LIBRARY z)
        if (ZLIB_LIBRARY STREQUAL "ZLIB_LIBRARY-NOTFOUND")
            message(STATUS "Unable to add Svc_DpZLibCompressor. libz not found")
            set(ZLIB_FOUND FALSE)
        else()
            set(ZLIB_FOUND TRUE)
            set(ZLIB_LIBRARIES ${ZLIB_LIBRARY})
        endif()
    endif()
endif()

if (ZLIB_FOUND)

    register_fprime_library(
        AUTOCODER_INPUTS
            "${CMAKE_CURRENT_LIST_DIR}/DpZLibCompressor.fpp"
        SOURCES
            "${CMAKE_CURRENT_LIST_DIR}/DpZLibCompressor.cpp"
       DEPENDS
            ${ZLIB_LIBRARIES}
    )

    # Add include directories if set (needed for cross-compilation)
    if (ZLIB_INCLUDE_DIRS)
        target_include_directories(Svc_DpZLibCompressor PUBLIC ${ZLIB_INCLUDE_DIRS})
    endif()

    ### Unit Tests ###
    register_fprime_ut(
        AUTOCODER_INPUTS
            "${CMAKE_CURRENT_LIST_DIR}/DpZLibCompressor.fpp"
        SOURCES
            "${CMAKE_CURRENT_LIST_DIR}/test/ut/DpZLibCompressorTestMain.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/test/ut/DpZLibCompressorTester.cpp"
        DEPENDS
            STest # For rules-based testing
            ${ZLIB_LIBRARIES}
        UT_AUTO_HELPERS
    )
endif()

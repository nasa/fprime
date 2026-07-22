find_library(ZLIB_LIBRARY z)

if (NOT ZLIB_LIBRARY STREQUAL "ZLIB_LIBRARY-NOTFOUND")
    add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/DpCompressionConfig/")

    register_fprime_module(
        EXCLUDE_FROM_ALL
        AUTOCODER_INPUTS
            "${CMAKE_CURRENT_LIST_DIR}/DpCompression.fpp"
        HEADERS
            "${CMAKE_CURRENT_LIST_DIR}/SubtopologyTopologyDefs.hpp"
        DEPENDS
            Svc_Subtopologies_DpCompression_DpCompressionConfig
            Svc/DpCompressProc
            Svc/DpZLibCompressor
            Svc/BufferManager
        INTERFACE
    )
else()
    message(STATUS "Skipping Svc/Subtopologies/DpCompression - zlib library not found")
endif()


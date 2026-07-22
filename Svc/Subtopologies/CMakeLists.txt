add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/CdhCore/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/ComCcsds/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/ComFprime/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/FileHandling/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/DataProducts/")

# DpCompression requires zlib - only add if available
find_package(ZLIB)
if (NOT ZLIB_FOUND)
    find_library(ZLIB_LIBRARY z)
    if (NOT ZLIB_LIBRARY STREQUAL "ZLIB_LIBRARY-NOTFOUND")
        set(ZLIB_FOUND TRUE)
    endif()
endif()

if (ZLIB_FOUND)
    add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/DpCompression/")
else()
    message(STATUS "Skipping DpCompression subtopology - zlib not found")
endif()

add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/ComLoggerTee/")

# Build list of subtopology dependencies
set(SUBTOPOLOGY_DEPS
    Svc_Subtopologies_CdhCore
    Svc_Subtopologies_CdhCore_CdhCoreConfig
    Svc_Subtopologies_ComCcsds
    Svc_Subtopologies_ComCcsds_ComCcsdsConfig
    Svc_Subtopologies_ComFprime
    Svc_Subtopologies_ComFprime_ComFprimeConfig
    Svc_Subtopologies_FileHandling
    Svc_Subtopologies_FileHandling_FileHandlingConfig
    Svc_Subtopologies_DataProducts
    Svc_Subtopologies_DataProducts_DataProductsConfig
    Svc_Subtopologies_ComLoggerTee
)

# Add DpCompression only if zlib was found
if (TARGET Svc_Subtopologies_DpCompression)
    list(APPEND SUBTOPOLOGY_DEPS
        Svc_Subtopologies_DpCompression
        Svc_Subtopologies_DpCompression_DpCompressionConfig
    )
endif()

add_custom_target(
    Svc_Subtopologies
    DEPENDS ${SUBTOPOLOGY_DEPS}
)

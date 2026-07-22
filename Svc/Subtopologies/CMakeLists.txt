add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/CdhCore/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/ComCcsds/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/ComFprime/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/FileHandling/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/DataProducts/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/DpCompression/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/ComLoggerTee/")

# Build list of dependencies conditionally
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

# Add DpCompression dependencies only if the target was created (zlib available)
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

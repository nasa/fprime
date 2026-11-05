add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/DpCompressionConfig/")

register_fprime_module(
    EXCLUDE_FROM_ALL
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/DpCompression.fpp"
    HEADERS
        "${CMAKE_CURRENT_LIST_DIR}/SubtopologyTopologyDefs.hpp"
    DEPENDS
        Svc_Subtopologies_DpCompression_DpCompressionConfig
    INTERFACE
)


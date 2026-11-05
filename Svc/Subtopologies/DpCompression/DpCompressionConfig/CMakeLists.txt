register_fprime_config(
    EXCLUDE_FROM_ALL
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/DpCompressionSubtopologyConfig.cpp"
    HEADERS
        "${CMAKE_CURRENT_LIST_DIR}/DpCompressionSubtopologyConfig.hpp"
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/DpCompressionConfig.fpp"
    DEPENDS
        Fw_Types
)


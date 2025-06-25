add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/DataProductsConfig/")

register_fprime_module(
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/DataProducts.fpp"
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/SubtopologyTopologyDefs.cpp"
    HEADERS
        "${CMAKE_CURRENT_LIST_DIR}/SubtopologyTopologyDefs.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/PingEntries.hpp"
    DEPENDS
        Svc_Subtopologies_DataProducts_DataProductsConfig
)

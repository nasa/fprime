add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/DataProductsConfig/")

register_fprime_module(
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/DataProducts.fpp"
    HEADERS
        "${CMAKE_CURRENT_LIST_DIR}/DataProductsTopologyDefs.hpp"
    INTERFACE
)

# Conditionally select driver config based on socket availability
if(FPRIME_HAS_SOCKETS)
    # Use TCP-based driver when sockets are available
    set(DRIVER_CONFIG_FILE "${CMAKE_CURRENT_LIST_DIR}/ComFprimeTcpDriverConfig.fpp")
else()
    # Use UART-based driver when sockets are not available
    set(DRIVER_CONFIG_FILE "${CMAKE_CURRENT_LIST_DIR}/ComFprimeUartDriverConfig.fpp")
endif()

register_fprime_config(
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/ComFprimeSubtopologyConfig.cpp"
    HEADERS
        "${CMAKE_CURRENT_LIST_DIR}/ComFprimeSubtopologyConfig.hpp"
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/ComFprimeConfig.fpp"
        #Swap this line to use a different comDriver config
        "${DRIVER_CONFIG_FILE}"
    DEPENDS
        Fw_Types
)

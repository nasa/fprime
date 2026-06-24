add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/ComFprimeConfig")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/ComCcsdsConfig")

register_fprime_config(
    CONFIGURATION_OVERRIDES
        "${CMAKE_CURRENT_LIST_DIR}/AcConstants.fpp"
        "${CMAKE_CURRENT_LIST_DIR}/BufferManagerComponentImplCfg.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/CommandDispatcherImplCfg.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/FpConfig.h"
        "${CMAKE_CURRENT_LIST_DIR}/StaticMemoryConfig.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/TlmChanImplCfg.hpp"
    INTERFACE
)

####
# CMakeLists.txt:
#
# Sets up the fprime module build within CMake.
####
register_fprime_module(
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/TlmPacketizerPorts.fpp"
    DEPENDS
        Fw_Types
        Svc_Types_TlmPacketizerTypes
        Svc_TlmPacketizer_config_TlmPacketizerConfig
)

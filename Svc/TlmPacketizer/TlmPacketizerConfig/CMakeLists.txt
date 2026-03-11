####
# Svc/TlmPacketizer/TlmPacketizerConfig/CMakeLists.txt:
#
# Sets a list of source files for cmake to process as part of autocoding.
####
register_fprime_config(
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/TlmPacketizerCfg.fpp"
    HEADERS
        "${CMAKE_CURRENT_LIST_DIR}/TlmPacketizerCfg.hpp"
    DEPENDS
        Svc_Types_TlmPacketizerTypes
)

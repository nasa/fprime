####
# Svc/Ccsds/SdlsSaRouter/config/SdlsSaRouterConfig/CMakeLists.txt:
#
# Compile-time configuration for the SdlsSaRouter component.
####
register_fprime_config(
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/SdlsSaRouterCfg.fpp"
    DEPENDS
        Svc_Ccsds_Types
)

register_fprime_config(
    EXCLUDE_FROM_ALL
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/ComCcsdsSdlsConfig.fpp"
    DEPENDS
        Fw_Types
        Svc_Ccsds_ClearTextDecryptor
)

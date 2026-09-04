####
# Svc/Ccsds/AesGcmEncryptor/config/AesGcmEncryptorConfig/CMakeLists.txt:
#
# Compile-time configuration for the AesGcmEncryptor component.
####
register_fprime_config(
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/AesGcmEncryptorCfg.fpp"
    DEPENDS
        Fw_Types
)

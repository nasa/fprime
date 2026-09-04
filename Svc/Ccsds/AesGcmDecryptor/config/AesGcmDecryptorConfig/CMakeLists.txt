####
# Svc/Ccsds/AesGcmDecryptor/config/AesGcmDecryptorConfig/CMakeLists.txt:
#
# Compile-time configuration for the AesGcmDecryptor component.
####
register_fprime_config(
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/AesGcmDecryptorCfg.fpp"
    DEPENDS
        Fw_Types
)

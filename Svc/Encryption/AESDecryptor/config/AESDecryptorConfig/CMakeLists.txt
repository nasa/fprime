####
# Svc/Encryption/AESDecryptor/config/AESDecryptorConfig/CMakeLists.txt:
#
# Compile-time configuration for the AESDecryptor component.
####
register_fprime_config(
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/AESDecryptorCfg.fpp"
    DEPENDS
        Fw_Types
)

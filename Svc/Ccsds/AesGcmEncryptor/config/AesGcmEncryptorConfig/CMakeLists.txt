####
# Svc/Encryption/AESEncryptor/config/AESEncryptorConfig/CMakeLists.txt:
#
# Compile-time configuration for the AESEncryptor component.
####
register_fprime_config(
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/AESEncryptorCfg.fpp"
    DEPENDS
        Fw_Types
)

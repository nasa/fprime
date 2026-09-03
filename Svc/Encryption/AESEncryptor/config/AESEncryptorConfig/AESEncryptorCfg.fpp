# ======================================================================
# AESEncryptorCfg.fpp
# Compile-time configuration for the AESEncryptor component
# ======================================================================

module SdlsCfg {

    @ Bytes AES-256-GCM adds to the plaintext: a 12-byte IV ahead of the ciphertext and a
    @ 16-byte MAC after it. Fixed by the component, and not a project choice.
    @ Overriding this will fail the build.
    constant AesFrameOverhead = 28

    @ Largest output AESEncryptor can produce: the largest plaintext it accepts plus the
    @ 12-byte IV and 16-byte MAC
    constant AesMaxOutputSize = 1024


}

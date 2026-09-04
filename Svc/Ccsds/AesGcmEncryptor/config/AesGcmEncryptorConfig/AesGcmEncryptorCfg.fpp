# ======================================================================
# AesGcmEncryptorCfg.fpp
# Compile-time configuration for the AesGcmEncryptor component
# ======================================================================

module SdlsCfg {

    @ Largest output AesGcmEncryptor can produce: the largest plaintext it accepts plus the
    @ 12-byte IV and 16-byte MAC
    constant AesMaxOutputSize = ComCfg.TmFrameFixedSize


}

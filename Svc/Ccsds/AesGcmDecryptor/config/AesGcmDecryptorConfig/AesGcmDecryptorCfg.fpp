# ======================================================================
# AesGcmDecryptorCfg.fpp
# Compile-time configuration for the AesGcmDecryptor component
# ======================================================================

module SdlsCfg {

    @ Largest SDLS-protected frame body AesGcmDecryptor will accept: the 12-byte IV, the
    @ ciphertext, and the 16-byte MAC, as they arrive once Svc.Ccsds.CcsdsSdlsDeframer has
    @ stripped the SPI. Anything larger is rejected with DECRYPTION_FAILURE.
    constant AesMaxInputSize = 1024

}

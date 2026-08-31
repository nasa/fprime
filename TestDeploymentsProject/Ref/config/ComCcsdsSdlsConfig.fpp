# Ref override of ComCcsdsSdlsConfig, selecting AES-256-GCM instead of clear-mode.
# Registered via CONFIGURATION_OVERRIDES in Ref/CMakeLists.txt.

module ComCcsdsSdlsConfig {
    # Base ID for the ComCcsdsSdls Subtopology; the SDLS decryption instances are offsets
    # from this base ID. The packet and transfer frame layers are reused from the ComCcsds
    # subtopology and are configured through ComCcsdsConfig.
    constant BASE_ID = 0x06000000
}

module ComCcsdsSdls {
    @ AES-256-GCM decryptor handling the base security association (SdlsSaRouter port 0).
    @ Reads its key from keyManager and must be given the uplink VC id via configure().
    instance decryptor: Svc.Ccsds.AESDecryptor base id ComCcsdsSdlsConfig.BASE_ID + 0x02000

    @ AES-256-GCM encryptor used by the SDLS framer on the downlink path. Uses a fresh
    @ random 12-byte IV per frame and must be given the downlink VC id via configure().
    instance encryptor: Svc.Ccsds.AESEncryptor base id ComCcsdsSdlsConfig.BASE_ID + 0x04000

    @ Key source for the AES components, supplying the AES-256 key from a file.
    instance keyManager: Svc.Ccsds.SdlsFileKeyManager base id ComCcsdsSdlsConfig.BASE_ID + 0x06000
}

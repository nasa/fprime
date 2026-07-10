module ComCcsdsSdlsConfig {
    # Base ID for the ComCcsdsSdls Subtopology; the SDLS decryption instances are offsets
    # from this base ID. The packet and transfer frame layers are reused from the ComCcsds
    # subtopology and are configured through ComCcsdsConfig.
    constant BASE_ID = 0x06000000
}

module ComCcsdsSdls {
    @ Default decryptor handling the base security association (SdlsSaRouter port 0).
    @ Defined in the configuration module so projects may override the configuration
    @ to select a different decryptor implementation.
    @
    @ WARNING: the default Svc.Ccsds.ClearTextDecryptor provides NO security: no
    @ confidentiality, no integrity, and no authentication.
    instance decryptor: Svc.Ccsds.ClearTextDecryptor base id ComCcsdsSdlsConfig.BASE_ID + 0x02000

    @ Default encryptor used by the SDLS framer on the downlink path. Defined in the
    @ configuration module so projects may override the configuration to select a
    @ different encryptor implementation.
    @
    @ WARNING: the default Svc.Ccsds.ClearTextEncryptor provides NO security: no
    @ confidentiality, no integrity, and no authentication.
    instance encryptor: Svc.Ccsds.ClearTextEncryptor base id ComCcsdsSdlsConfig.BASE_ID + 0x04000
}

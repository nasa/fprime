# ======================================================================
# SdlsSaRouterCfg.fpp
# Compile-time configuration for the SdlsSaRouter component
# ======================================================================

module SdlsCfg {

    @ Number of downstream decryptor ports on the SdlsSaRouter
    constant SaRouterPortCount = 4

    @ Number of entries in the SA-to-port routing map
    constant SaRouterMapEntryCount = 4

    @ Maximum number of decrypted data buffers outstanding (sent downstream, not yet returned)
    constant SaRouterMaxOutstandingBuffers = 4

    @ Compile-time map from security association index to downstream port index. Projects
    @ may define sparse or non-linear SA ranges that map down to a compact, linear port
    @ array. Port indices must be in [0, SaRouterPortCount).
    @
    @ This default configuration maps SAs 0 through 3 directly to ports 0 through 3.
    array SaMap = [SaRouterMapEntryCount] Svc.Ccsds.SaMapEntry default [
        { securityAssociationIndex = 0, portIndex = 0 },
        { securityAssociationIndex = 1, portIndex = 1 },
        { securityAssociationIndex = 2, portIndex = 2 },
        { securityAssociationIndex = 3, portIndex = 3 }
    ]

}

# ======================================================================
# SdlsSaRouterCfg.fpp
# Compile-time configuration for the SdlsSaRouter component
# ======================================================================

module SdlsCfg {

    @ Number of downstream crypto component ports on the SdlsSaRouter
    constant SaRouterPortCount = 2

    @ Number of entries in the SA-to-port routing map
    constant SaRouterMapEntryCount = 2

    @ Maximum number of processed data buffers outstanding (sent downstream, not yet returned)
    constant SaRouterMaxOutstandingBuffers = 4

    @ Enumeration of the downstream crypto component ports on the SdlsSaRouter
    enum SaRouterPorts : FwIndexType {
        PLAINTEXT = 0
        UNCONNECTED = 1
    }

    @ Compile-time map from security association index to downstream port index. Projects
    @ may define sparse or non-linear SA ranges that map down to a compact, linear port
    @ array. Port indices must be in [0, SaRouterPortCount).
    @
    @ This default configuration maps SA 0 to the plain-text (no security) port; the second
    @ entry maps SA 1 to a port that standard topologies may leave unconnected.
    array SaMap = [SaRouterMapEntryCount] Svc.Ccsds.SaMapEntry default [
        { securityAssociationIndex = 0, portIndex = SaRouterPorts.PLAINTEXT },
        { securityAssociationIndex = 1, portIndex = SaRouterPorts.UNCONNECTED }
    ]

}

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
        UNCONNECTED = 0
        PLAINTEXT = 1
    }

    @ Compile-time default map from security association index to downstream port index. SA
    @ ranges may be sparse or non-linear; port indices must be in [0, SaRouterPortCount).
    @
    @ CCSDS 355.0-B-2 reserves SPI 0 and 65535 for Extended Procedures; an SA is simplex
    @ (sect. 2.3.1.1), so each direction needs its own SPI and routing table.
    @
    @ Maps the uplink SPI to the plain-text port and Extended Procedures to a port standard
    @ topologies leave unconnected. A deployment with one router per direction -- as ComCcsdsSdls
    @ has -- must give each its own table via SdlsSaRouter::configure(); sharing one lets each
    @ direction accept the other's SPI, defeating SA verification (sect. 4.2.4.3).
    array SaMap = [SaRouterMapEntryCount] Svc.Ccsds.SaMapEntry default [
        { securityAssociationIndex = 1, portIndex = SaRouterPorts.PLAINTEXT },
        { securityAssociationIndex = 0, portIndex = SaRouterPorts.UNCONNECTED }
    ]

}

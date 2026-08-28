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

    # SPI assignment. CCSDS 355.0-B-2 reserves SPI 0 and 65535 for Extended Procedures
    # An SA is simplex (sect. 2.3.1.1), so each direction needs its own SPI and routing table

    @ SPI on SDLS-protected TC (uplink) frames. Must match encryptionSpi on the YAMCS link.
    constant SpiTcUplink = 1

    @ SPI on SDLS-protected TM (downlink) frames. Must match the CcsdsSdlsFramer SA_INDEX default.
    constant SpiTmDownlink = 2

    @ SPI reserved for Extended Procedures PDUs. Mapped to UNCONNECTED until key management
    @ exists, so routing it reports UNKNOWN_PORT rather than UNKNOWN_SA.
    constant SpiExtendedProcedures = 0

    @ Compile-time default map from security association index to downstream port index. SA
    @ ranges may be sparse or non-linear; port indices must be in [0, SaRouterPortCount).
    @
    @ Maps the uplink SPI to the plain-text port and Extended Procedures to a port standard
    @ topologies leave unconnected. A deployment with one router per direction -- as ComCcsdsSdls
    @ has -- must give each its own table via SdlsSaRouter::configure(); sharing one lets each
    @ direction accept the other's SPI, defeating SA verification (sect. 4.2.4.3).
    array SaMap = [SaRouterMapEntryCount] Svc.Ccsds.SaMapEntry default [
        { securityAssociationIndex = SpiTcUplink, portIndex = SaRouterPorts.PLAINTEXT },
        { securityAssociationIndex = SpiExtendedProcedures, portIndex = SaRouterPorts.UNCONNECTED }
    ]

}

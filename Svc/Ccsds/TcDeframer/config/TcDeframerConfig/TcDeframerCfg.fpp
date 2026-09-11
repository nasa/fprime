# ======================================================================
# TcDeframerCfg.fpp
# Compile-time configuration for the TcDeframer component
# ======================================================================

module TcDeframerCfg {

    @ Maximum number of reassembled spanning packets simultaneously owned by downstream components
    @ (sent on dataOut, not yet returned on dataReturnIn). A completed packet is dropped when this
    @ many are outstanding. Only used when packet spanning is enabled.
    constant MaxSpanningPacketsInFlight = 8

}

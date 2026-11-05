# ======================================================================
# FPP file for Svc.Ccsds.AsmFramer configuration
# ======================================================================

module Svc {

  module AsmFramerCfg {

    @ Largest Attached Sync Marker, in bytes, that the AsmFramer accepts (128 bits covers
    @ every CCSDS 131.0-B-5 pattern except the 192-bit rate-1/6 Turbo marker)
    constant AsmMaxSize = 16

    @ Largest transfer frame, in bytes, that the AsmFramer accepts on dataIn; defaults to the
    @ AOS bound so the component can follow either framer. Projects may lower it, but not below
    @ ComCfg.TmFrameFixedSize: the AsmFramer enforces that floor at compile time so a TM frame
    @ always fits
    constant MaxFrameSize = ComCfg.AosMaxFrameFixedSize

  }

}

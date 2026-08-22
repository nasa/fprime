# ======================================================================
# FPP file for FprimeRouter configuration
# ======================================================================

module Svc {

  module FprimeRouterCfg {

    @ Capacity of FprimeRouter's buffer->FrameContext table, bounding how many
    @ buffers handed off on fileOut and unknownDataOut can await return at once.
    @ Size it to the buffer pool that feeds dataIn, the hard upper bound on
    @ outstanding buffers.
    constant BufferContextTableSize = 50

  }

}

# ======================================================================
# FPP file for configuration of the communications stack
#
# The only reason to modify these definitions is if you are writing your own
# Framer/Deframer implementations and need more contextual data than what is
# defined
# ======================================================================

module ComCfg {

    @ Type used to pass context info between components during framing/deframing
    struct FrameContext {
        comQueueIndex: FwIndexType  @< Queue Index used by the ComQueue, other components shall not modify
    } default {
        comQueueIndex = 0
    }

}

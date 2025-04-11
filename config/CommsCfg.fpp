module CommsCfg {

    # Note:
    # The only reason to modify this type is if you are writing your own 
    # Framer/Deframer implementations and need more contextual data than what is
    # defined

    @ Type used to pass context info between components during framing/deframing
    struct FrameContext {
        apid: U32    @< Application Process Identifier
    } default {
        apid = 0
    }

}

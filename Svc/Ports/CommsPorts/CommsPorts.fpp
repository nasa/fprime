#####
# Communications Ports:
#
# A port for passing framing / deframing data
#####

module Svc {

    @ Port for sending data buffer along with context buffer
    @ This is useful for passing data that needs context to be interpreted
    # Review note: I thought using a ref for context could be neat, maybe we don't want that
    # For example, a downstream framer could theoretically "send context back" by modifying the reference...
    # TODO: no, we want context to be const sine we're using it for deallocation
    port DataWithContext(ref data: Fw.Buffer, ref context: CommsCfg.FrameContext)

}

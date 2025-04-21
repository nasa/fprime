#####
# Communications Ports:
#
# A port for passing framing / deframing data
#####

module Svc {

    @ Port for sending data buffer along with context information
    @ This is useful for passing data that needs context to be interpreted
    port DataWithContext(ref data: Fw.Buffer, context: CommsCfg.FrameContext)

}

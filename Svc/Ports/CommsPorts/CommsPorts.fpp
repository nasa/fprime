#####
# Communications Ports:
#
# A port for passing framing / deframing data
# This is used by the Communications components which need context to interpret framed data
#####

module Svc {

    @ Port for sending communications data (frames) buffer along with context information
    port ComDataWithContext(ref data: Fw.Buffer, context: ComCfg.FrameContext)

}

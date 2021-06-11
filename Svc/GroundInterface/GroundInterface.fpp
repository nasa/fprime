module Svc {

  passive component GroundInterface {

    @ FPP from XML: original path was Svc/GroundInterface/Events.xml
    include "Events.fppi"

    guarded input port downlinkPort: [1] Fw.Com

    guarded input port fileDownlinkBufferSendIn: [1] Fw.BufferSend

    guarded input port readCallback: [1] Fw.BufferSend

    guarded input port schedIn: [1] Svc.Sched

    output port fileUplinkBufferSendOut: [1] Fw.BufferSend

    output port uplinkPort: [1] Fw.Com

    output port fileDownlinkBufferSendOut: [1] Fw.BufferSend

    output port fileUplinkBufferGet: [1] Fw.BufferGet

    output port write: [1] Fw.BufferSend

    output port readPoll: [1] Fw.BufferSend

    event port Log

    text event port LogText

    time get port Time

  }

}

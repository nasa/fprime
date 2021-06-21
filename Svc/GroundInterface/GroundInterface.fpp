module Svc {

  passive component GroundInterface {

    # ----------------------------------------------------------------------
    # Events
    # ----------------------------------------------------------------------

    include "Events.fppi"

    # ----------------------------------------------------------------------
    # General Ports
    # ----------------------------------------------------------------------

    @ Mutexed Downlink input port
    guarded input port downlinkPort: [1] Fw.Com

    @ Mutexed File downlink buffer send input port
    guarded input port fileDownlinkBufferSendIn: [1] Fw.BufferSend

    @ Mutexed Read cal back input port
    guarded input port readCallback: [1] Fw.BufferSend

    @ Mutexed Schedule input port
    guarded input port schedIn: [1] Svc.Sched

    @ File Uplink buffer send output port
    output port fileUplinkBufferSendOut: [1] Fw.BufferSend

    @ Uplink port
    output port uplinkPort: [1] Fw.Com

    @ File Downlink buffer send output port
    output port fileDownlinkBufferSendOut: [1] Fw.BufferSend

    @ File Uplink buffer get output port
    output port fileUplinkBufferGet: [1] Fw.BufferGet

    @ Write output port
    output port write: [1] Fw.BufferSend

    @ Read poll output port
    output port readPoll: [1] Fw.BufferSend

    # ----------------------------------------------------------------------
    # Special Ports
    # ----------------------------------------------------------------------

    @ Event port
    event port Log

    @ Text event port
    text event port LogText

    @ Time get port
    time get port Time

  }

}

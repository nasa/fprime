module Svc {

  @ A component representing the FSW ground interface
  passive component GroundInterface {

    # ----------------------------------------------------------------------
    # General Ports
    # ----------------------------------------------------------------------

    @ Mutexed Downlink input port
    guarded input port downlinkPort: Fw.Com

    @ Mutexed File downlink buffer send input port
    guarded input port fileDownlinkBufferSendIn: Fw.BufferSend

    @ Mutexed Read cal back input port
    guarded input port readCallback: Fw.BufferSend

    @ Mutexed Schedule input port
    guarded input port schedIn: Svc.Sched

    @ File Uplink buffer send output port
    output port fileUplinkBufferSendOut: Fw.BufferSend

    @ Uplink port
    output port uplinkPort: Fw.Com

    @ File Downlink buffer send output port
    output port fileDownlinkBufferSendOut: Fw.BufferSend

    @ File Uplink buffer get output port
    output port fileUplinkBufferGet: Fw.BufferGet

    @ Write output port
    output port write: Fw.BufferSend

    @ Read poll output port
    output port readPoll: Fw.BufferSend

    # ----------------------------------------------------------------------
    # Special Ports
    # ----------------------------------------------------------------------

    @ Event port
    event port Log

    @ Text event port
    text event port LogText

    @ Time get port
    time get port Time

    # ----------------------------------------------------------------------
    # Events
    # ----------------------------------------------------------------------

    include "Events.fppi"

  }

}

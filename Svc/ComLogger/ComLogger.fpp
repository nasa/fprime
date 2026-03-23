module Svc {

  @ A component for logging Com buffers
  active component ComLogger {

    # ----------------------------------------------------------------------
    # General ports
    # ----------------------------------------------------------------------

    @ Buffer input port
    async input port bufferSendIn: Fw.BufferSend

    @ Buffer output port
    output port bufferSendOut: Fw.BufferSend

    @ Ping input port
    async input port pingIn: Svc.Ping

    @ Ping output port
    output port pingOut: Svc.Ping

    # ----------------------------------------------------------------------
    # Special ports
    # ----------------------------------------------------------------------

    @ Command registration port
    command reg port cmdRegOut

    @ Command received port
    command recv port cmdIn

    @ Command response port
    command resp port cmdResponseOut

    @ Event port
    event port logOut

    @ Text event port
    text event port LogText

    @ Time get port
    time get port timeCaller

    # ----------------------------------------------------------------------
    # Commands
    # ----------------------------------------------------------------------

    include "Commands.fppi"

    # ----------------------------------------------------------------------
    # Events
    # ----------------------------------------------------------------------

    include "Events.fppi"

  }

}

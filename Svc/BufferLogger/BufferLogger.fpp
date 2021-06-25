module Svc {

  active component BufferLogger {

    # ----------------------------------------------------------------------
    # General ports
    # ----------------------------------------------------------------------

    @ Buffer input port
    async input port bufferSendIn: Fw.BufferSend

    @ Buffer output port
    output port bufferSendOut: Fw.BufferSend

    @ Packet input port
    async input port comIn: Fw.Com

    @ Ping input port
    async input port pingIn: Svc.Ping

    @ Ping output port
    output port pingOut: Svc.Ping

    async input port schedIn: Svc.Sched

    # ----------------------------------------------------------------------
    # Special ports
    # ----------------------------------------------------------------------

    @ Port for receiving commands
    command recv port cmdIn

    @ Port for sending command registration requests
    command reg port cmdRegOut

    @ Port for sending command response
    command resp port cmdResponseOut

    @ Port for emitting events
    event port eventOut

    @ Port for emitting text events
    text event port eventOutText

    @ Port for getting the time
    time get port timeCaller

    @ Port for emitting telemetry
    telemetry port tlmOut

    # ----------------------------------------------------------------------
    # Commands
    # ----------------------------------------------------------------------

    include "Commands.fppi"

    # ----------------------------------------------------------------------
    # Events
    # ----------------------------------------------------------------------

    include "Events.fppi"

    # ----------------------------------------------------------------------
    # Telemetry
    # ----------------------------------------------------------------------

    include "Telemetry.fppi"

  }

}

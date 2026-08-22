module Svc {

  active component BufferAccumulator {

    include "Commands.fppi"

    include "Events.fppi"

    include "Telemetry.fppi"

    @ Receive a Buffer from an upstream component to enqueue
    async input port bufferSendInFill: [1] Fw.BufferSend

    @ Receive a Buffer back from a downstream component
    async input port bufferSendInReturn: [1] Fw.BufferSend

    @ Pass a Buffer onwards to a downstream component
    output port bufferSendOutDrain: [1] Fw.BufferSend

    @ Return a Buffer to the original upstream component
    output port bufferSendOutReturn: [1] Fw.BufferSend

    @ Port for receiving commands
    command recv port cmdIn

    @ Port for sending command registration requests
    command reg port cmdRegOut

    @ Port for sending command response
    command resp port cmdResponseOut

    @ Event port for emitting events
    event port eventOut

    @ Event port for emitting text events
    text event port eventOutText

    @ Ping input port for health
    async input port pingIn: [1] Svc.Ping

    @ Ping output port for health
    output port pingOut: [1] Svc.Ping

    @ A port for getting the time
    time get port timeCaller

    @ A port for emitting telemetry
    telemetry port tlmOut

  }

}

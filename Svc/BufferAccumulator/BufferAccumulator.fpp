module Svc {

  active component BufferAccumulator {

    @ FPP from XML: original path was Svc/BufferAccumulator/Commands.xml
    include "Commands.fppi"

    @ FPP from XML: original path was Svc/BufferAccumulator/Events.xml
    include "Events.fppi"

    @ FPP from XML: original path was Svc/BufferAccumulator/Telemetry.xml
    include "Telemetry.fppi"

    async input port bufferSendInFill: [1] Fw.BufferSend

    async input port bufferSendInReturn: [1] Fw.BufferSend

    output port bufferSendOutDrain: [1] Fw.BufferSend

    output port bufferSendOutReturn: [1] Fw.BufferSend

    command recv port cmdIn

    command reg port cmdRegOut

    command resp port cmdResponseOut

    event port eventOut

    text event port eventOutText

    async input port pingIn: [1] Svc.Ping

    output port pingOut: [1] Svc.Ping

    time get port timeCaller

    telemetry port tlmOut

  }

}

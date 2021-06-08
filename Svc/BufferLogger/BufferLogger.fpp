module Svc {

  active component BufferLogger {

    @ FPP from XML: original path was Svc/BufferLogger/Commands.xml
    include "Commands.fppi"

    @ FPP from XML: original path was Svc/BufferLogger/Events.xml
    include "Events.fppi"

    @ FPP from XML: original path was Svc/BufferLogger/Telemetry.xml
    include "Telemetry.fppi"

    async input port bufferSendIn: [1] Fw.BufferSend

    output port bufferSendOut: [1] Fw.BufferSend

    command recv port cmdIn

    command reg port cmdRegOut

    command resp port cmdResponseOut

    async input port comIn: [1] Fw.Com

    event port eventOut

    text event port eventOutText

    async input port pingIn: [1] Svc.Ping

    output port pingOut: [1] Svc.Ping

    time get port timeCaller

    async input port schedIn: [1] Svc.Sched

    telemetry port tlmOut

  }

}

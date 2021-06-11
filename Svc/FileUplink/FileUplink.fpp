module Svc {

  active component FileUplink {

    @ FPP from XML: original path was Svc/FileUplink/Telemetry.xml
    include "Telemetry.fppi"

    @ FPP from XML: original path was Svc/FileUplink/Events.xml
    include "Events.fppi"

    async input port bufferSendIn: [1] Fw.BufferSend

    time get port timeCaller

    output port bufferSendOut: [1] Fw.BufferSend

    telemetry port tlmOut

    event port eventOut

    text event port LogText

    async input port pingIn: [1] Svc.Ping

    output port pingOut: [1] Svc.Ping

    

  }

}

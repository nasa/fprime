module Svc {

  passive component BufferManager {

    @ FPP from XML: original path was Svc/BufferManager/Telemetry.xml
    include "Telemetry.fppi"

    @ FPP from XML: original path was Svc/BufferManager/Events.xml
    include "Events.fppi"

    time get port timeCaller

    event port eventOut

    text event port textEventOut

    guarded input port bufferSendIn: [1] Fw.BufferSend

    guarded input port bufferGetCallee: [1] Fw.BufferGet

    telemetry port tlmOut

    sync input port schedIn: Svc.Sched

  }

}

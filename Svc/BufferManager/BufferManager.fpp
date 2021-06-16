module Svc {

  passive component BufferManager {

    include "Telemetry.fppi"

    include "Events.fppi"

    time get port timeCaller

    event port eventOut

    text event port textEventOut

    guarded input port bufferSendIn: Fw.BufferSend

    guarded input port bufferGetCallee: Fw.BufferGet

    telemetry port tlmOut

    sync input port schedIn: Svc.Sched

  }

}

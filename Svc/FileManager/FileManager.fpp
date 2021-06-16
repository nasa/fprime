module Svc {

  active component FileManager {

    @ FPP from XML: original path was Svc/FileManager/Commands.xml
    include "Commands.fppi"

    @ FPP from XML: original path was Svc/FileManager/Telemetry.xml
    include "Telemetry.fppi"

    @ FPP from XML: original path was Svc/FileManager/Events.xml
    include "Events.fppi"

    command recv port cmdIn

    command reg port cmdRegOut

    command resp port cmdResponseOut

    event port eventOut

    time get port timeCaller

    telemetry port tlmOut

    async input port pingIn: [1] Svc.Ping

    output port pingOut: [1] Svc.Ping

    text event port LogText

  }

}

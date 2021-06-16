module Svc {

  active component ComLogger {

    @ FPP from XML: original path was Svc/ComLogger/Commands.xml
    include "Commands.fppi"

    @ FPP from XML: original path was Svc/ComLogger/Events.xml
    include "Events.fppi"

    async input port comIn: [1] Fw.Com

    time get port timeCaller

    command reg port cmdRegOut

    command recv port cmdIn

    event port logOut

    command resp port cmdResponseOut

    async input port pingIn: [1] Svc.Ping

    output port pingOut: [1] Svc.Ping

    text event port LogText

  }

}

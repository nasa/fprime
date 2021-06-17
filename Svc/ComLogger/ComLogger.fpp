module Svc {

  active component ComLogger {

    include "Commands.fppi"

    include "Events.fppi"

    # ----------------------------------------------------------------------
    # General ports 
    # ----------------------------------------------------------------------

    @ Com input port
    async input port comIn: [1] Fw.Com

    @ Ping input port
    async input port pingIn: [1] Svc.Ping

    @ Ping output port
    output port pingOut: [1] Svc.Ping

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

  }

}

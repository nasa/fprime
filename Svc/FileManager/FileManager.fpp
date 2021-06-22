module Svc {

  active component FileManager {

    # ----------------------------------------------------------------------
    # Commands
    # ----------------------------------------------------------------------

    include "Commands.fppi"

    # ----------------------------------------------------------------------
    # Telemetry
    # ----------------------------------------------------------------------

    include "Telemetry.fppi"

    # ----------------------------------------------------------------------
    # Events
    # ----------------------------------------------------------------------

    include "Events.fppi"

    # ----------------------------------------------------------------------
    # General Ports
    # ----------------------------------------------------------------------

    @ Ping input port
    async input port pingIn: [1] Svc.Ping

    @ Ping output port
    output port pingOut: [1] Svc.Ping

    # ----------------------------------------------------------------------
    # Special ports 
    # ----------------------------------------------------------------------
    
    @ Command receive port
    command recv port cmdIn

    @ Command Registration port
    command reg port cmdRegOut

    @ Command response port
    command resp port cmdResponseOut

    @ Log event port
    event port eventOut

    @ Log text event port
    text event port LogText

    @ Time get port
    time get port timeCaller
    
    @ Telemtry port
    telemetry port tlmOut

    

  }

}

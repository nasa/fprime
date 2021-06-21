module Svc {

  active component FileUplink {

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

    @ 
    async input port bufferSendIn: [1] Fw.BufferSend

    @ 
    output port bufferSendOut: [1] Fw.BufferSend
    
    @ 
    async input port pingIn: [1] Svc.Ping

    @ 
    output port pingOut: [1] Svc.Ping

    # ----------------------------------------------------------------------
    # Special Ports
    # ----------------------------------------------------------------------
    
    @ 
    time get port timeCaller
    
    @ 
    telemetry port tlmOut

    @ 
    event port eventOut
    @ 
    text event port LogText

    

    

  }

}

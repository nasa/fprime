module Svc {

  @ A component for deframing input received from the ground
  @ via a FrameAccumulator
  passive component FprimeDeframer {

    # ----------------------------------------------------------------------
    # Deframer interface
    # ----------------------------------------------------------------------

    include "../Interfaces/DeframerInterface.fppi"

    @ Port for deallocating dropped frames
    output port bufferDeallocate: Fw.BufferSend

    @ An invalid frame was received
    event InvalidBufferReceived \
      severity warning high \
      format "The received buffer is not long enough to contain a valid frame (header + trailer)"

    ###############################################################################
    # Standard AC Ports for Events 
    ###############################################################################
    @ Port for requesting the current time
    time get port timeCaller

    @ Port for sending textual representation of events
    text event port logTextOut

    @ Port for sending events to downlink
    event port logOut
  }

}

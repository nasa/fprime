module Svc {

  @ A component for deframing input received from the ground
  @ via a FrameAccumulator
  passive component FprimeDeframer {

    # ----------------------------------------------------------------------
    # Deframer interface
    # ----------------------------------------------------------------------

    import Deframer

    @ An invalid frame was received (too short to be a frame)
    event InvalidBufferReceived \
      severity warning high \
      format "Frame dropped: The received buffer is not long enough to contain a valid frame (header + trailer)"

    @ An invalid frame was received (start word is wrong)
    event InvalidStartWord \
      severity warning high \
      format "Frame dropped: The received buffer does not start with the F Prime start word"

    @ An invalid frame was received (length is wrong)
    event InvalidLengthReceived \
      severity warning high \
      format "Frame dropped: The received buffer size cannot hold a frame of specified payload length"

    @ An invalid frame was received (checksum mismatch)
    event InvalidChecksum \
      severity warning high \
      format "Frame dropped: The transmitted frame checksum does not match that computed by the receiver"

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

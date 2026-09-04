module Svc {
module Ccsds {

    @ Pads a buffer to a fixed size with a CCSDS idle space packet
    @ Sits ahead of SDLS encryption so the fill lands inside the authenticated data
    passive component SpacePacketIdleFiller {

        import Svc.Framer

        ###############################################################################
        # Standard AC Ports: Required for Events                                      #
        ###############################################################################

        import Fw.Event

        @ Port for requesting the current time
        time get port timeCaller

        @ A buffer larger than the configured fill target arrived; the deployment has sized its
        @ aggregation buffer above what the frame can carry.
        event InputTooLarge(bufferSize: FwSizeType, targetSize: FwSizeType) \
            severity warning high \
            format "Buffer of {} bytes exceeds fill target of {}; dropped"

        @ A buffer arrived leaving too little room for a well-formed idle packet
        event GapTooSmall(gap: FwSizeType) \
            severity warning high \
            format "Cannot fill a {} byte gap; a space packet needs at least 7"
    }
}
}

module Svc {
    @ Framer implementation for the F Prime protocol
    passive component FprimeFramer {

        import Framer

        # ----------------------------------------------------------------------
        # Allocation of buffers
        # ----------------------------------------------------------------------

        @ Port for allocating buffers to hold framed data
        output port bufferAllocate: Fw.BufferGet

        @ Port for deallocating buffers allocated for framed data
        output port bufferDeallocate: Fw.BufferSend

        # ----------------------------------------------------------------------
        # Standard AC Ports
        # ----------------------------------------------------------------------
        @ Port for requesting the current time
        time get port timeCaller

    }
}

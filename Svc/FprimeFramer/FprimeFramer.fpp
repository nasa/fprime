module Svc {
    @ Framer implementation for the F Prime protocol
    passive component FprimeFramer {

        include "../Interfaces/FramerInterface.fppi"

        # ----------------------------------------------------------------------
        # Allocation of buffers
        # ----------------------------------------------------------------------

        @ Port for allocating buffers to hold framed data
        output port bufferAllocate: Fw.BufferGet

        # ----------------------------------------------------------------------
        # Handling of of ready signals
        # ----------------------------------------------------------------------

        @ Port receiving the general status from the downstream component
        @ indicating it is ready or not-ready for more input
        sync input port comStatusIn: Fw.SuccessCondition

        @ Port receiving indicating the status of framer for receiving more data
        output port comStatusOut: Fw.SuccessCondition

        # ----------------------------------------------------------------------
        # Standard AC Ports
        # ----------------------------------------------------------------------
        @ Port for requesting the current time
        time get port timeCaller

    }
}
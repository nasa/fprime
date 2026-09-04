module Svc {

  @ A component for logging Com buffers as data products
  active component ComLoggerDp {

    # ----------------------------------------------------------------------
    # General ports
    # ----------------------------------------------------------------------

    @ Com input port
    async input port comIn: Fw.Com

    @ Ping input port
    async input port pingIn: Svc.Ping

    @ Ping output port
    output port pingOut: Svc.Ping

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

    @ Data product get port
    product get port productGetOut

    @ Data product send port
    product send port productSendOut

    # ----------------------------------------------------------------------
    # Commands
    # ----------------------------------------------------------------------

    @ Starts recording ComBuffers at the specified priority
    async command StartComDp (
        packetsPerContainer: U32
        $priority: U32
    ) \
    opcode 0x00

    @ Updates currently generating data products. If off, no effect
    async command UpdatePriority (
        $priority: U32
    ) \
    opcode 0x01

    @ Stops recording buffers
    async command StopComDp \
    opcode 0x02


    # ----------------------------------------------------------------------
    # Events
    # ----------------------------------------------------------------------

    event DpBufferError(
                        $size: U32 @< The requested size
                    ) \
    severity warning high \
    id 0x00 \
    format "Error getting ComDp buffer of size {}"


    # ----------------------------------------------------------------------
    # Products
    # ----------------------------------------------------------------------


    @ Array to hold ComBuffers
    array ComBufferArray = [FW_COM_BUFFER_MAX_SIZE] U8

    @ Record for ComBuffers
    product record ComBufferRecord: ComBufferArray id 0
    @ Container for ComBuffers
    product container ComBuffContainer id 0 default priority 5    

  }

}

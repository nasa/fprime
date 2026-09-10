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

    @ Sched input port for writing telemetry
    async input port schedIn: Svc.Sched

    @ Port to start recording
    async input port startRecordingIn: Svc.ComLoggerStart

    @ Port to stop recording
    async input port stopRecordingIn: Svc.ComLoggerStop

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

    @ Telemetry port
    telemetry port tlmOut

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
        $priority: FwDpPriorityType
    ) \
    opcode 0x00

    @ Updates currently generating data products. If off, no effect
    async command UpdatePriority (
        $priority: FwDpPriorityType
    ) \
    opcode 0x01

    @ Stops recording buffers
    async command StopComDp \
    opcode 0x02

    @ Clears NumBuffersLogged counter and DpBufferError event throttle
    async command CLEAR_COUNTERS \
    opcode 0x03


    # ----------------------------------------------------------------------
    # Events
    # ----------------------------------------------------------------------

    event DpBufferError(
                        $size: U32 @< The requested size
                    ) \
    severity warning high \
    id 0x00 \
    format "Error getting ComDp buffer of size {}" \
    throttle DpBufferErrorThrottle

    @ Started recording Com buffers
    event ComDpStarted($packetsPerContainer: U32) \
    severity activity high \
    id 0x01 \
    format "Started Com DP logging: {} packets per container"

    @ Stopped recording Com buffers
    event ComDpStopped($numSent: U32) \
    severity activity high \
    id 0x02 \
    format "Stopped Com DP logging: sent {} partial container"

    @ Updated data product priority
    event PriorityUpdated($priority: U32) \
    severity activity low \
    id 0x03 \
    format "Updated Com DP priority to {}"

    @ Counters cleared
    event CountersCleared \
    severity activity low \
    id 0x04 \
    format "Cleared NumBuffersLogged counter and DpBufferError throttle"

    @ Failed to start recording due to invalid configuration
    event StartRecordingFailed(
        $packetsPerContainer: U32 @< The invalid packets per container value
    ) \
    severity warning low \
    id 0x05 \
    format "Failed to start Com DP recording: packetsPerContainer {} is invalid (must be > 0)"

    # ----------------------------------------------------------------------
    # Telemetry
    # ----------------------------------------------------------------------

    @ Whether data product logging is currently active
    telemetry LoggingEnabled: bool id 0x00

    @ Total number of Com buffers logged since initialization
    telemetry NumBuffersLogged: U32 id 0x01

    @ Number of Com buffers dropped due to allocation failure
    telemetry NumBuffersDropped: U32 id 0x02

    # ----------------------------------------------------------------------
    # Products
    # ----------------------------------------------------------------------

    @ Record for ComBuffers
    product record ComBufferRecord: U8 array id 0
    @ Container for ComBuffers
    product container ComBuffContainer id 0 default priority 5    

  }

}

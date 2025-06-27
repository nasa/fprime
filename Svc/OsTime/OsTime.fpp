module Svc {

    @ A time component using OSAL RawTime abstractions
    passive component OsTime {
        import Time

        sync input port setEpoch: OsTimeEpoch

        @ Port for receiving commands
        command recv port CmdDisp

        @ Port for sending command registration requests
        command reg port CmdReg

        @ Port for sending command responses
        command resp port CmdStatus

        @ Event port
        event port EventOut

        @ Text event port
        text event port LogText

        @ Time get port
        time get port timeCaller

        sync command SetCurrentTime(seconds_now: U32) opcode 0x00

        @ An error occurred while attempting to set the current time
        event SetCurrentTimeError(
                            status: U32 @< The error status
                            ) \
        severity warning high \
        id 0x00 \
        format "Could not set current time due to RawTime error status {}"

    }
}

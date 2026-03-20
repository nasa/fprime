module FppTest {
    module FrameworkPortData {
        @ An extension of Fw.Buffer but with `==` overridden to not compare
        @ address but rather just compare the memory.
        type DataBuffer

        struct CmdReg {
            portNum: FwIndexType
            opCode: FwOpcodeType @< Command Op Code
        }

        struct CmdResponse {
            portNum: FwIndexType
            opCode: FwOpcodeType @< Command Op Code
            cmdSeq: U32 @< Command Sequence
            response: Fw.CmdResponse @< The command response argument
        }

        struct Log {
            portNum: FwIndexType
            $id: FwEventIdType @< Log ID
            timeTag: Fw.Time @< Time Tag
            $severity: Fw.LogSeverity @< The severity argument
            args: DataBuffer @< Buffer containing serialized log entry
        }

        struct LogText {
            portNum: FwIndexType
            $id: FwEventIdType @< Log ID
            timeTag: Fw.Time @< Time Tag
            $severity: Fw.LogSeverity @< The severity argument
            $text: Fw.TextLogString @< Text of log message
        }

        struct Tlm {
            portNum: FwIndexType
            $id: FwChanIdType @< Telemetry Channel ID
            timeTag: Fw.Time @< Time Tag
            val: DataBuffer @< Buffer containing serialized telemetry value
        }

        struct PrmGet {
            portNum: FwIndexType
            $id: FwPrmIdType @< Parameter ID
            @ Buffer containing serialized parameter value.
            @ Unmodified if param not found.
            val: DataBuffer
        }

        struct PrmSet {
            portNum: FwIndexType
            $id: FwPrmIdType @< Parameter ID
            val: DataBuffer @< Buffer containing serialized parameter value
        }

        struct DpGet {
            portNum: FwIndexType
            @ The container ID (input)
            $id: FwDpIdType
            @ The data size of the requested buffer (input)
            dataSize: FwSizeType
            @ The buffer (output)
            buffer: DataBuffer
        }

        struct DpRequest {
            portNum: FwIndexType
            @ The container ID
            $id: FwDpIdType
            @ The data size of the requested buffer
            dataSize: FwSizeType
        }

        struct DpResponse {
            portNum: FwIndexType
            @ The container ID
            $id: FwDpIdType
            @ The buffer
            buffer: DataBuffer
            @ The status
            status: Fw.Success
        }

        struct DpSend {
            portNum: FwIndexType
            @ The container ID
            $id: FwDpIdType
            @ The buffer
            buffer: DataBuffer
        }

        struct Ping {
            portNum: FwIndexType
            key: U32 @< Value to return to pinger
        }
    }
}

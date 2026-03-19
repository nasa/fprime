module FppTest {

    module FrameworkPortData {
        struct CmdReg {
            opCode: FwOpcodeType @< Command Op Code
        }

        struct CmdResponse {
            opCode: FwOpcodeType @< Command Op Code
            cmdSeq: U32 @< Command Sequence
            response: Fw.CmdResponse @< The command response argument
        }

        struct Log {
            $id: FwEventIdType @< Log ID
            timeTag: Fw.Time @< Time Tag
            $severity: Fw.LogSeverity @< The severity argument
            args: Fw.Buffer @< Buffer containing serialized log entry
        }

        struct LogText {
            $id: FwEventIdType @< Log ID
            timeTag: Fw.Time @< Time Tag
            $severity: Fw.LogSeverity @< The severity argument
            $text: Fw.TextLogString @< Text of log message
        }

        struct Tlm {
            $id: FwChanIdType @< Telemetry Channel ID
            timeTag: Fw.Time @< Time Tag
            val: Fw.Buffer @< Buffer containing serialized telemetry value
        }

        struct PrmGet {
            $id: FwPrmIdType @< Parameter ID
            @ Buffer containing serialized parameter value.
            @ Unmodified if param not found.
            val: Fw.Buffer
        }

        struct PrmSet {
            $id: FwPrmIdType @< Parameter ID
            val: Fw.Buffer @< Buffer containing serialized parameter value
        }

        struct Time {
            $time: Fw.Time @< Reference to Time object
        }

        struct DpGet {
            @ The container ID (input)
            $id: FwDpIdType
            @ The data size of the requested buffer (input)
            dataSize: FwSizeType
            @ The buffer (output)
            buffer: Fw.Buffer
        }

        struct DpRequest {
            @ The container ID
            $id: FwDpIdType
            @ The data size of the requested buffer
            dataSize: FwSizeType
        }

        struct DpResponse {
            @ The container ID
            $id: FwDpIdType
            @ The buffer
            buffer: Fw.Buffer
            @ The status
            status: Fw.Success
        }

        struct Ping {
            key: U32 @< Value to return to pinger
        }
    }

    active component Framework {

        @ Command dispatch port
        output port compCmdSend: Fw.Cmd

        @ Command Registration Port. max_number should match dispatch port.
        guarded input port compCmdReg: Fw.CmdReg

        @ Input Command Status Port
        sync input port compCmdStat: Fw.CmdResponse

        @ Event input port
        sync input port LogRecv: Fw.Log

        @ Text Event input port
        sync input port TextLogRecv: Fw.LogText

        @ Guarded port for receiving telemetry values
        guarded input port TlmRecv: Fw.Tlm

        @ Port that replies with parameter value
        sync input port ParamGetIn: Fw.PrmGet

        @ Port that sets a parameter
        sync input port ParamSetIn: Fw.PrmSet

        @ Port for getting current time
        sync input port timeGetIn: Fw.Time

        @ Ports for responding to a data product get from a client component
        sync input port productGetIn: Fw.DpGet

        @ Ports for receiving data product buffer requests from a client component
        async input port productRequestIn: Fw.DpRequest

        @ Ports for sending requested data product buffers to a client component
        output port productResponseOut: Fw.DpResponse

        @ Ping output port
        output port PingSend: Svc.Ping

        @ Ping return port
        async input port PingReturn: Svc.Ping

        @ Signal from component that this test has finished
        async input port Finish: Svc.Sched

        # ----------------------------------------------------------------------
        # Port matching specifiers
        # ----------------------------------------------------------------------

        match compCmdSend with compCmdReg

    }

}
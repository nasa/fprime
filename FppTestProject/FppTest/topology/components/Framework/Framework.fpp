module FppTest {
    active component Framework {
        @ Command dispatch port
        output port compCmdSend: [2] Fw.Cmd

        @ Command Registration Port. max_number should match dispatch port.
        guarded input port compCmdReg: [2] Fw.CmdReg

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
        sync input port productGetIn: [2] Fw.DpGet

        @ Ports for receiving data product buffer requests from a client component
        async input port productRequestIn: [2] Fw.DpRequest

        @ Ports for sending requested data product buffers to a client component
        output port productResponseOut: [2] Fw.DpResponse

        @ Ports for getting buffers from a Buffer Manager
        output port bufferGetOut: [2] Fw.BufferGet

        # ----------------------------------------------------------------------
        # Ports for forwarding filled data products
        # ----------------------------------------------------------------------

        @ Ports for receiving filled data product buffers from a client component
        async input port productSendIn: [2] Fw.DpSend

        @ Ports for sending filled data product buffers to a downstream component
        output port productSendOut: [2] Fw.BufferSend

        @ Ping output port
        output port PingSend: [2] Svc.Ping

        @ Ping return port
        async input port PingReturn: [2] Svc.Ping

        output port syncOut: [2] Svc.Sched
        async input port syncIn: [2] Svc.Sched

        # ----------------------------------------------------------------------
        # Port matching specifiers
        # ----------------------------------------------------------------------

        match compCmdSend with compCmdReg
        match syncOut with syncIn
    }
}

module Svc {

  @ A generic hub component
  passive component GenericHub {

    # ----------------------------------------------------------------------
    # Mimic Ports
    # ----------------------------------------------------------------------

    @ Telemetry input port for mimicking an event processor
    sync input port TlmRecv: Fw.Tlm

    @ Event input port for mimicking an event processor
    sync input port LogRecv: Fw.Log

    @ Cross-hub event output port
    output port LogSend:  Fw.Log

    @ Cross-hub telemetry output port
    output port TlmSend: Fw.Tlm

    # ----------------------------------------------------------------------
    # General Ports
    # ----------------------------------------------------------------------

    @ Input array of generic ports to shuttle to the other side of the hub connection
    sync input port portIn: [GenericHubInputPorts] serial

    @ Output array of generic ports shuttled from the other side of the hub connection
    output port portOut: [GenericHubOutputPorts] serial

    @ Input array of generic ports shuttling in copy-free buffers form external sources
    sync input port buffersIn: [GenericHubInputBuffers] Fw.BufferSend

    @ Output array of generic ports shuttling in copy-free buffers form external sources
    output port buffersOut: [GenericHubOutputBuffers] Fw.BufferSend

    @ Buffer return from incoming bufferIn calls
    output port bufferDeallocate: Fw.BufferSend

    @ Data output to remote hub
    output port dataOut: Fw.BufferSend

    @ Data input from remote hub
    sync input port dataIn: Fw.BufferSend

    @ Deallocation of buffer passed into data in
    output port dataInDeallocate: Fw.BufferSend

    @ Allocation of buffer passed to passed out dataOut
    output port dataOutAllocate: Fw.BufferGet

  }

}

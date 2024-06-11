module Svc {

  @ A component for repeating Fw.BufferSend calls to multiple consumers
  passive component BufferRepeater {
    @ Port to duplicate across the repeater
    sync input port portIn: Fw.BufferSend

    @ Duplicated output ports
    output port portOut: [BufferRepeaterOutputPorts] Fw.BufferSend

    @ Port to allocate new memory for each buffer output
    output port allocate: Fw.BufferGet

    @ Port to deallocate original buffer output
    output port deallocate: Fw.BufferSend

    @ Event port
    event port Log

    @ Text event port
    text event port LogText

    @ Time get port
    time get port Time

    @ Soft failure in allocation
    event AllocationSoftFailure(
                            $port: I32 @< The port index that needed an allocation
                            $size: U32 @< The requested allocation size
                          ) \
        severity warning high \
        id 0 \
        format "Failed to allocate {} byte buffer for port {}"


    @ Hard failure in allocation
    event AllocationHardFailure(
                            $port: I32 @< The port index that needed an allocation
                            $size: U32 @< The requested allocation size
                          ) \
        severity fatal \
        id 1 \
        format "Failed to allocate {} byte buffer for port {}"
  }
}

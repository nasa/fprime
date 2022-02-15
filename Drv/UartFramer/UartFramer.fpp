module Drv {

  passive component UartFramer {

    # ----------------------------------------------------------------------
    # Ports for UART
    # ----------------------------------------------------------------------

    @ port to send a buffer for reads to the UART driver
    output port readBufferSend: Fw.BufferSend

    @ send a UART buffer
    output port serialSend: Drv.SerialWrite

    @ receive a buffer from the UART - one of the above
    sync input port serialRecv: Drv.SerialRead

    # ----------------------------------------------------------------------
    # Ports for Framer/Deframer
    # ----------------------------------------------------------------------

    @ Buffer from Framer
    sync input port Framer: Drv.ByteStreamSend

    @ Buffer to Deframer
    output port Deframer: Drv.ByteStreamRecv
    
    # ----------------------------------------------------------------------
    # Ports to get buffers
    # ----------------------------------------------------------------------

    @ Allocation output port - will be sent to Deframer for deallocation
    output port DeframerAllocate: Fw.BufferGet

    @ Deallocation output port - deallocation of Framer buffers
    output port FramerDeallocate: Fw.BufferSend


    # ----------------------------------------------------------------------
    # Special F Prime ports
    # ----------------------------------------------------------------------

    event port Log

    text event port LogText

    time get port Time

    # ----------------------------------------------------------------------
    # Events
    # ----------------------------------------------------------------------

    @ UART open error
    event BuffErr() \
      severity warning high \
      id 0 \
      format "Couldn't get UART buffer" \
      throttle 5

  }

}

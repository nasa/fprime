module Drv {

  @ The ready and receive interfaces for a byte stream driver client
  interface PassiveByteStreamDriverClientReadyRecv {

    # ----------------------------------------------------------------------
    # Ready interface
    # ----------------------------------------------------------------------
    @ Port for receiving ready signals from the driver
    @ Sample connection: byteStreamDriver.ready -> byteStreamDriverClient.byteStreamReady
    sync input port driverReady: Drv.ByteStreamReady

    # ----------------------------------------------------------------------
    # Receive interface
    # ----------------------------------------------------------------------
    @ Port for receiving data from the driver
    @ Sample connection: byteStreamDriver.$recv -> byteStreamDriverClient.fromDriver
    sync input port fromDriver: Drv.ByteStreamData

    @ Port for returning ownership of buffers received on fromDriver
    @ Sample connection: byteStreamDriverClient.byteStreamReturn -> byteStreamDriver.recvReturnIn
    output port fromDriverReturn: Fw.BufferSend

  }

}

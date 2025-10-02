module Drv {

  @ The send interface of passive client of an asynchronous byte stream driver
  interface PassiveByteStreamDriverClientSendAsync {

    @ Port for sending data to the driver
    @ Sample connection: client.bufferOut -> driver.$send
    output port bufferOut: Fw.BufferSend

    @ Port for receiving buffers sent on bufferOut and then returned
    @ Sample connection: driver.sendReturnOut -> client.bufferOutReturn
    sync input port bufferOutReturn: Drv.ByteStreamData

  }

  @ A passive client of an asynchronous byte stream driver
  interface PassiveAsyncByteStreamDriverClient {

    import PassiveByteStreamDriverClientReadyRecv

    import PassiveByteStreamDriverClientSendAsync

  }

}

module Drv {

  @ The send interface of passive client of an asynchronous byte stream driver
  interface PassiveByteStreamDriverClientSendAsync {

    @ Port for sending data to the driver
    @ Sample connection: client.toByteStreamDriver -> driver.$send
    output port toByteStreamDriver: Fw.BufferSend

    @ Port for receiving buffers sent on toByteStreamDriver and then returned
    @ Sample connection: driver.sendReturnOut -> client.toByteStreamDriverReturn
    sync input port toByteStreamDriverReturn: Drv.ByteStreamData

  }

  @ A passive client of an asynchronous byte stream driver
  interface PassiveAsyncByteStreamDriverClient {

    import PassiveByteStreamDriverClientReadyRecv

    import PassiveByteStreamDriverClientSendAsync

  }

}

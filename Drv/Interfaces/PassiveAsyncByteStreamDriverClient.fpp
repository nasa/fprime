module Drv {

  @ The send interface of passive client of an asynchronous byte stream driver
  interface PassiveByteStreamDriverClientSendAsync {

    @ Port for sending data to the driver
    @ Sample connection: client.toDriver -> driver.$send
    output port toDriver: Fw.BufferSend

    @ Port for receiving buffers sent on toDriver and then returned
    @ Sample connection: driver.sendReturnOut -> client.toDriverReturn
    sync input port toDriverReturn: Drv.ByteStreamData

  }

  @ A passive client of an asynchronous byte stream driver
  interface PassiveAsyncByteStreamDriverClient {

    import PassiveByteStreamDriverClientReadyRecv

    import PassiveByteStreamDriverClientSendAsync

  }

}

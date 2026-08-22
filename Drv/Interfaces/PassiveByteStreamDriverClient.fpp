module Drv {

  @ The send interface of passive client of a synchronous byte stream driver
  interface PassiveByteStreamDriverClientSendSync {

    @ Port for sending data to the driver
    @ Sample connection: client.toByteStreamDriver -> driver.$send
    output port toByteStreamDriver: Drv.ByteStreamSend

  }

  @ A passive client of a synchronous byte stream driver
  interface PassiveByteStreamDriverClient {

    @ The ready and receive interfaces
    import PassiveByteStreamDriverClientReadyRecv

    @ The send interface
    import PassiveByteStreamDriverClientSendSync

  }
}

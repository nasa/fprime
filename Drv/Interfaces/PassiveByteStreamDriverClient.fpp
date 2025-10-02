module Drv {

  @ The send interface of passive client of a synchronous byte stream driver
  interface PassiveByteStreamDriverClientSendSync {

    @ Port for sending data to the driver
    @ Sample connection: client.byteStreamOut -> driver.$send
    output port byteStreamOut: Drv.ByteStreamSend

  }

  @ A passive client of a synchronous byte stream driver
  interface PassiveByteStreamDriverClient {

    import PassiveByteStreamDriverClientReadyRecv

    import PassiveByteStreamDriverClientSendSync

  }
}

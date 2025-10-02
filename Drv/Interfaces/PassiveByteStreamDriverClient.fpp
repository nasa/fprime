module Drv {

  @ A passive client of a synchronous byte stream driver
  interface PassiveByteStreamDriverClient {

    import PassiveByteStreamDriverClientReadyRecv

    import PassiveByteStreamDriverClientSendSync

  }

  @ The send interface of passive client of a synchronous byte stream driver
  interface PassiveByteStreamDriverClientSendSync {

    @ Port for sending data to the driver
    @ Sample connection: byteStreamDriverCient.byteStreamOut -> byteStreamDriver.$send
    output port byteStreamOut: Drv.ByteStreamSend

  }

}

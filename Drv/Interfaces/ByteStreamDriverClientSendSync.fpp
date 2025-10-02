module Drv {

  @ The send interface of a synchronous byte stream driver
  interface ByteStreamDriverClientSendSync {

    @ Port for sending data to the driver
    @ Sample connection: byteStreamDriverCient.byteStreamOut -> byteStreamDriver.$send
    output port byteStreamOut: Drv.ByteStreamSend

  }

}

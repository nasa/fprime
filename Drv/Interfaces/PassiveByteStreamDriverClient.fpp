module Drv {

  @ A passive client of a synchronous byte stream driver
  interface PassiveByteStreamDriverClient {

    import PassiveByteStreamDriverClientReadyRecv

    import PassiveByteStreamDriverClientSendSync

  }

}

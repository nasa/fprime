module Drv {

  passive component ByteStreamBufferAdapter {

    @ The ByteStreamBufferAdapter is a passive client of the ByteStreamDriver
    @ interface
    import PassiveByteStreamDriverClient

    @ The ByteStreamBufferAdapter is a PassiveBufferDriver
    import PassiveBufferDriver

  }

}

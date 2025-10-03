module Drv {

  @ A passive component for mediating between the ByteStreamDriver
  @ interface and the PassiveBufferDriver interface
  passive component ByteStreamBufferAdapter {

    @ The ByteStreamBufferAdapter is a passive client of the ByteStreamDriver
    @ interface
    import PassiveByteStreamDriverClient

    @ The ByteStreamBufferAdapter is a PassiveBufferDriver
    import PassiveBufferDriver

  }

}

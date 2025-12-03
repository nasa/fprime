module Drv {

  @ A passive component for mediating between the ByteStreamDriver
  @ interface and the PassiveBufferDriver interface
  @
  @ Sample topology:
  @
  @ -------------------------------------------------
  @ |                                               |
  @ | ByteStreamDriver <--> ByteStreamBufferAdapter | <--> PassiveBufferDriverClient
  @ |                                               |
  @ -------------------------------------------------
  @
  @ The two components in the box function together as a PassiveBufferDriver:
  @
  @ -------------------------------------------------
  @ |                                               |
  @ |             PassiveBufferDriver               | <--> PassiveBufferDriverClient
  @ |                                               |
  @ -------------------------------------------------
  @
  passive component ByteStreamBufferAdapter {

    @ ByteStreamBufferAdapter is a passive client of the ByteStreamDriver
    @ interface
    import PassiveByteStreamDriverClient

    @ ByteStreamBufferAdapter is a PassiveBufferDriver
    import PassiveBufferDriver

  }

}

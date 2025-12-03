module Drv {

  @ A passive component for mediating between the AsyncByteStreamDriver
  @ interface and the PassiveBufferDriver interface
  @
  @ Sample topology:
  @
  @ -----------------------------------------------------------
  @ |                                                         |
  @ | AsyncByteStreamDriver <--> AsyncByteStreamBufferAdapter | <--> PassiveBufferDriverClient
  @ |                                                         |
  @ -----------------------------------------------------------
  @
  @ The two components in the box function together as a PassiveBufferDriver:
  @
  @ -------------------------------------------------
  @ |                                               |
  @ |             PassiveBufferDriver               | <--> PassiveBufferDriverClient
  @ |                                               |
  @ -------------------------------------------------
  @
  passive component AsyncByteStreamBufferAdapter {

    @ AsyncByteStreamBufferAdapter is a passive client of the
    @ AsyncByteStreamDriver interface
    import PassiveAsyncByteStreamDriverClient

    @ AsyncByteStreamBufferAdapter is a PassiveBufferDriver
    import PassiveBufferDriver

  }

}

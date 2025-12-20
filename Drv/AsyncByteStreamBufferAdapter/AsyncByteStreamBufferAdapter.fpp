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

    @ Event port
    event port Log

    @ Text event port
    text event port LogText

    @ Time get port
    time get port Time

    @ Driver Ready
    event DriverNotReady \
      severity warning low \
      id 0 \
      format "Buffer Driver not ready"

    @Data Send
    event DataSendError(
                          stat: I32 
                        ) \
      severity warning low \
      id 1 \
      format "Data send error status: {}"

    @Data Receive
    event DataReceiveError(
                          stat: I32 
                        ) \
      severity warning low \
      id 2 \
      format "Data reception error status: {}"
  }

}

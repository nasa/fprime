module Drv {

  @ A passive buffer driver
  interface PassiveBufferDriver {

    @ The interface for sending data to the driver
    @ Sample connections:
    @
    @ client.toBufferDriver -> driver.bufferIn
    @ client.bufferInReturn -> driver.toBufferDriverReturn
    import Fw.PassiveBufferIn

    @ The interface for receiving data from the driver
    @ Sample connections:
    @
    @ driver.bufferOut -> client.fromBufferDriver
    @ client.fromBufferDriverReturn -> driver.bufferOutReturn
    import Fw.PassiveBufferOut

    @ The interface for allocating and deallocating buffers
    @ Sample connections:
    @
    @ driver.allocate -> bufferManager.bufferGetCallee
    @ driver.deallocate -> bufferManager.bufferSendIn
    import Fw.BufferAllocation

  }

}

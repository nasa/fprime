module Drv {

  @ The send interface of passive client of a buffer driver
  interface PassiveBufferDriverClientSend {

    @ Port for sending data to the driver
    @ Sample connection: client.toBufferDriver -> driver.bufferIn
    output port toBufferDriver: Fw.BufferSend

    @ Port for receiving buffers sent on toBufferDriver and then returned
    @ Sample connection: driver.bufferInReturn -> client.toBufferDriverReturn
    sync input port toBufferDriverReturn: Fw.BufferSend

  }

  @ The receive interface of passive client of a buffer driver
  interface PassiveBufferDriverClientRecv {

    @ Port for receiving data from the driver
    @ Sample connection: driver.bufferOut -> client.fromBufferDriver
    sync input port fromBufferDriver: Fw.BufferSend

    @ Port for returning buffers received on fromBufferDriver
    @ Sample connection: client.fromBufferDriverReturn -> driver.bufferOutReturn
    output port fromBufferDriverReturn: Fw.BufferSend

  }

  @ A passive client of a buffer driver
  interface PassiveBufferDriverClient {

    @ The interface for sending data to the driver
    import PassiveBufferDriverClientSend

    @ The interface for receiving data from the driver
    import PassiveBufferDriverClientRecv

  }

}

module Drv {

  @ A passive buffer driver
  interface PassiveBufferDriver {

    import PassiveBufferDriverSend

    import PassiveBufferDriverRecv

  }

  @ The send interface for a passive buffer driver
  interface PassiveBufferDriverSend {

    @ Port for sending data to the network
    @ Sample connection: passiveBufferDriverClient.toBufferDriver -> passiveBufferDriver.bufferIn
    sync input port bufferIn: Fw.BufferSend

    @ Port for returning buffers received on bufferIn
    @ Sample connection: passiveBufferDriver.bufferInReturn -> passiveBufferDriverClient.toBufferDriverReturn
    output port bufferInReturn: Fw.BufferSend

  }

  @ The receive interface for a passive buffer driver
  interface PassiveBufferDriverRecv {

    @ Port for receiving data from the network
    @ Sample connection: passiveBufferDriver.bufferOut -> passiveBufferDriverClient.fromBufferDriver
    output port bufferOut: Fw.BufferSend

    @ Port for receiving buffers sent on bufferOut and returned
    @ Sample connection: passiveBufferDriverClient.fromBufferDriverReturn -> passiveBufferDriver.bufferOutReturn
    sync input port bufferOutReturn: Fw.BufferSend

  }

}

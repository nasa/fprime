module Drv {

  @ The send interface of passive client of an asynchronous byte stream driver
  interface PassiveByteStreamDriverClientSendAsync {

    @ Port for sending data to the driver
    @ Sample connection: asyncByteStreamDriverCient.bufferOut -> asyncByteStreamDriver.$send
    output port bufferOut: Fw.BufferSend

    @ Port for receiving buffers sent on bufferOut and then returned
    @ Sample connection: asyncByteStreamDriver.sendReturnOut -> asyncByteStreamDriverClient.bufferOutReturn
    sync input port bufferOutReturn: Drv.ByteStreamData

  }

}

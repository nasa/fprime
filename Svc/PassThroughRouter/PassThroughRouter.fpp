module Svc {

  @ A direct pass through router for single custom route use cases
  passive component PassThroughRouter {

    @ Receiving data (Fw::Buffer) to be routed with optional context to help with routing
    sync input port dataIn: Svc.ComDataWithContext

    @ Port for returning ownership of data (includes Fw.Buffer) received on dataIn
    output port dataReturnOut: Svc.ComDataWithContext

    @ Pass-through port for sending all packets received on dataIn
    output port allPacketsOut: Fw.BufferSend

    @ Port for receiving ownership back of buffers sent on allPacketsOut
    sync input port allPacketsReturnIn: Fw.BufferSend

    @ Port for requesting the current time
    time get port timeCaller

    @ Enables event handling
    import Fw.Event

  }

}

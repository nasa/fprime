module Svc {

  @ A component for splitting a Com buffer stream
  passive component ComSplitter {

    @ Com input port
    sync input port comIn: Svc.ComBufferSend

    @ Com output port
    output port comOut: [5] Svc.ComBufferSend

  }

}

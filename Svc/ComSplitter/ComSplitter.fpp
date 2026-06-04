module Svc {

  @ A component for splitting a Com buffer stream
  passive component ComSplitter {

    @ Com input port
    sync input port comIn: Fw.ComBufferSend

    @ Com output port
    output port comOut: [5] Fw.ComBufferSend

  }

}

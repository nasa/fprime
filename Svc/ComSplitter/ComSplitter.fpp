module Svc {

  @ A component for splitting a Com buffer stream
  passive component ComSplitter {

    @ Com input port
    sync input port comIn: Fw.Com

    @ Com output port
    output port comOut: [5] Fw.Com

  }

}

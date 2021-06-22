module Svc {

  passive component ComSplitter {

    @ Com input port
    sync input port comIn: [1] Fw.Com

    @ Com output port
    output port comOut: [5] Fw.Com

  }

}

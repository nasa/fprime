module Svc {

  passive component ComSplitter {

    sync input port comIn: [1] Fw.Com

    output port comOut: [5] Fw.Com

  }

}

module Svc {

  passive component Framer {

    guarded input port comIn: [1] Fw.Com

    guarded input port bufferIn: [1] Fw.BufferSend

    output port bufferDeallocate: [1] Fw.BufferSend

    output port framedAllocate: [1] Fw.BufferGet

    output port framedOut: [1] Drv.ByteStreamSend

    time get port timeGet

  }

}

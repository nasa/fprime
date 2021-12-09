module Drv {

  passive component LinuxSerialDriver {

    include "Events.fppi"

    include "Telemetry.fppi"

    telemetry port Tlm

    event port Log

    sync input port readBufferSend: [1] Fw.BufferSend

    text event port LogText

    output port serialRecv: [1] Drv.SerialRead

    time get port Time

    sync input port serialSend: [1] Drv.SerialWrite

  }

}

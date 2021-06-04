module Svc {

  passive component GenericRepeater {

    @ Port to duplicate across the repeater
    sync input port portIn: [1] serial

    @ Duplicated output port
    output port portOut: [$GenericRepeaterOutputPorts] serial

  }

}

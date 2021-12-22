module Svc {

  @ A generic component for repeating input
  passive component GenericRepeater {

    @ Port to duplicate across the repeater
    sync input port portIn: serial

    @ Duplicated output port
    output port portOut: [GenericRepeaterOutputPorts] serial

  }

}

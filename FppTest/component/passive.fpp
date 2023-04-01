@ A passive component
passive component PassiveTest {

  include "include/typed_ports.fppi"
  include "include/special_ports.fppi"

  include "include/commands.fppi"
  include "include/events.fppi"
  include "include/telemetry.fppi"
  include "include/params.fppi"

}

@ A passive component with serial ports
passive component PassiveSerial {

  include "include/typed_ports.fppi"
  include "include/serial_ports.fppi"
  include "include/special_ports.fppi"

  include "include/commands.fppi"
  include "include/events.fppi"
  include "include/telemetry.fppi"
  include "include/params.fppi"

}

@ An passive component with commands
passive component PassiveCommands {

  include "include/typed_ports.fppi"
  include "include/special_ports.fppi"

  include "include/commands.fppi"

}

@ An passive component with events
passive component PassiveEvents {

  include "include/typed_ports.fppi"
  include "include/special_ports.fppi"

  include "include/events.fppi"

}

@ An passive component with telemetry
passive component PassiveTelemetry {

  include "include/typed_ports.fppi"
  include "include/special_ports.fppi"

  include "include/telemetry.fppi"

}

@ An passive component with params
passive component PassiveParams {

  include "include/typed_ports.fppi"
  include "include/special_ports.fppi"

  include "include/params.fppi"

}

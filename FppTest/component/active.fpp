@ An active component
active component ActiveTest {

  include "include/typed_ports.fppi"
  include "include/typed_ports_async.fppi"
  include "include/special_ports.fppi"
  include "include/internal_ports.fppi"

  include "include/commands.fppi"
  include "include/commands_async.fppi"
  include "include/events.fppi"
  include "include/telemetry.fppi"
  include "include/params.fppi"

}

@ An active component with serial ports
active component ActiveSerial {

  include "include/typed_ports.fppi"
  include "include/typed_ports_async.fppi"
  include "include/serial_ports.fppi"
  include "include/serial_ports_async.fppi"
  include "include/special_ports.fppi"
  include "include/internal_ports.fppi"

  include "include/commands.fppi"
  include "include/commands_async.fppi"
  include "include/events.fppi"
  include "include/telemetry.fppi"
  include "include/params.fppi"

}

@ An active component with commands
active component ActiveCommands {

  include "include/typed_ports.fppi"
  include "include/typed_ports_async.fppi"
  include "include/special_ports.fppi"

  include "include/commands.fppi"
  include "include/commands_async.fppi"

}

@ An active component with events
active component ActiveEvents {

  include "include/typed_ports.fppi"
  include "include/typed_ports_async.fppi"
  include "include/special_ports.fppi"

  include "include/events.fppi"

}

@ An active component with telemetry
active component ActiveTelemetry {

  include "include/typed_ports.fppi"
  include "include/typed_ports_async.fppi"
  include "include/special_ports.fppi"

  include "include/telemetry.fppi"

}

@ An active component with params
active component ActiveParams {

  include "include/typed_ports.fppi"
  include "include/typed_ports_async.fppi"
  include "include/special_ports.fppi"

  include "include/params.fppi"

}

@ A queued component
queued component QueuedTest {

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

@ A queued component with serial ports
queued component QueuedSerial {

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

@ An queued component with commands
queued component QueuedCommands {

  include "include/typed_ports.fppi"
  include "include/typed_ports_async.fppi"
  include "include/special_ports.fppi"

  include "include/commands.fppi"
  include "include/commands_async.fppi"

}

@ An queued component with events
queued component QueuedEvents {

  include "include/typed_ports.fppi"
  include "include/typed_ports_async.fppi"
  include "include/special_ports.fppi"

  include "include/events.fppi"

}

@ An queued component with telemetry
queued component QueuedTelemetry {

  include "include/typed_ports.fppi"
  include "include/typed_ports_async.fppi"
  include "include/special_ports.fppi"

  include "include/telemetry.fppi"

}

@ An queued component with params
queued component QueuedParams {

  include "include/typed_ports.fppi"
  include "include/typed_ports_async.fppi"
  include "include/special_ports.fppi"

  include "include/params.fppi"

}

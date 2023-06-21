include "../include/fpp_types.fppi"
include "../include/port_types.fppi"
include "../include/port_index_enums.fppi"

@ An active component
active component ActiveTest {

  include "../include/typed_ports.fppi"
  include "../include/typed_ports_async.fppi"
  include "../include/serial_ports.fppi"
  include "../include/serial_ports_async.fppi"
  include "../include/special_ports.fppi"
  include "../include/internal_ports.fppi"

  include "../include/commands.fppi"
  include "../include/commands_async.fppi"
  include "../include/events.fppi"
  include "../include/telemetry.fppi"
  include "../include/params.fppi"

}

@ An active component
active component ActiveTest {

  include "../include/internal_ports.fppi"
  import FppTest.SerialPorts
  import FppTest.SerialPortsAsync
  import FppTest.SpecialPorts
  import FppTest.TypedPortsSyncInputs
  import FppTest.TypedPortsGuardedInputs
  import FppTest.TypedPortsOutputs
  import FppTest.TypedPortsHelpers
  import FppTest.TypedPortsAsync
  import FppTest.OutputPorts

  include "../include/commands.fppi"
  include "../include/commands_async.fppi"
  include "../include/events.fppi"
  include "../include/external_params.fppi"
  include "../include/params.fppi"
  include "../include/telemetry.fppi"

}

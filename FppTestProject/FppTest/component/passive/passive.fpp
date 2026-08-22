@ A passive component
passive component PassiveTest {

  import FppTest.SerialPorts
  import FppTest.SpecialPorts
  import FppTest.TypedPortsSyncInputs
  import FppTest.TypedPortsGuardedInputs
  import FppTest.TypedPortsOutputs
  import FppTest.TypedPortsHelpers

  include "../include/commands.fppi"
  include "../include/events.fppi"
  include "../include/telemetry.fppi"
  include "../include/params.fppi"
  include "../include/external_params.fppi"

}

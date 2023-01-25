# ======================================================================
# AcConstants.fpp
# F Prime configuration constants
# ======================================================================

@ Number of rate group member output ports for ActiveRateGroup
constant ActiveRateGroupOutputPorts = 10

@ Used to drive rate groups
constant RateGroupDriverRateGroupPorts = 3

@ Used for command and registration ports
constant CmdDispatcherComponentCommandPorts = 30

@ Used for uplink/sequencer buffer/response ports
constant CmdDispatcherSequencePorts = 5

@ Number of static memory allocations
constant StaticMemoryAllocations = 4

@ Used to ping active components
constant HealthPingPorts = 25

@ Used for broadcasting completed file downlinks
constant FileDownCompletePorts = 1

@ Used for number of Fw::Com type ports supported by Svc::ComQueue
constant ComQueueComPorts = 2

@ Used for number of Fw::Buffer type ports supported by Svc::ComQueue
constant ComQueueBufferPorts = 1

@ Used for maximum number of connected buffer repeater consumers
constant BufferRepeaterOutputPorts = 10

# ----------------------------------------------------------------------
# Hub connections. Connections on all deployments should mirror these settings.
# ----------------------------------------------------------------------

constant GenericHubInputPorts = 10
constant GenericHubOutputPorts = 10
constant GenericHubInputBuffers = 10
constant GenericHubOutputBuffers = 10

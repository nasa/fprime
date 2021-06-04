type FwEventIdType
type FwChanIdType
type FwOpcodeType
type FwPrmIdType
type NATIVE_INT_TYPE
type NATIVE_UINT_TYPE
type POINTER_CAST

#  Number of rate group member output ports for ActiveRateGroup
constant ActiveRateGroupOutputPorts          =       10  
# Used to drive rate groups
constant RateGroupDriverRateGroupPorts       =       3   

# Hub connections. Connections on all deployments should mirror these settings.
constant GenericHubInputPorts = 10
constant GenericHubOutputPorts = 10
constant GenericHubInputBuffers = 10
constant GenericHubOutputBuffers = 10

# Outputs from the generic repeater.
constant GenericRepeaterOutputPorts = 2
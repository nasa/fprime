@ A port for receiving commands
command recv port cmdIn

@ A port for sending command registration requests
command reg port cmdRegOut

@ A port for sending command responses
command resp port cmdResponseOut

@ A port for emitting events
event port eventOut

@ A port for emitting text events
text event port textEventOut

@ A port for emitting telemetry
telemetry port tlmOut

@ A port for getting parameter values
param get port prmGetOut

@ A port for setting parameter values
param set port prmSetOut

@ A port for getting the time
time get port timeGetOut

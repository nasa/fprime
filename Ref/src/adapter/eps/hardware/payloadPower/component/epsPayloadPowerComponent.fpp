module Ref {

  @ Component for managing payload power 
  passive component epsPayloadPowerComponent {

    # ----------------------------------------------------------------------
    # General ports
    # ----------------------------------------------------------------------

    @ Input port for receiving payload power on/off signal
    sync input port receivePowerCommandIn: receive_power_command_input

    @ Output port for sending success/failure in carrying out power operation
    output port sendPowerCommandStatusOut: send_power_command_status_output

    @ Output port for requesting current payload power status
    output port requestPowerStatusOut: request_power_status_output 

    @ Input port for receiving the on/off status 
    sync input port receivePowerStatusIn: receive_power_status_input

    @ Output port for sending a power on/off signal to EPS hardware
    output port sendPowerCommandOut: send_power_command_output


    # ----------------------------------------------------------------------
    # Special ports
    # ----------------------------------------------------------------------

    @ Command receive port
    command recv port cmdIn

    @ Command registration port
    command reg port cmdRegOut

    @ Command response port
    command resp port cmdResponseOut

    @ Event port
    event port eventOut

    @ Telemetry port
    telemetry port tlmOut

    @ Text event port
    text event port textEventOut

    @ Time get port
    time get port timeGetOut

    # ----------------------------------------------------------------------
    # Commands
    # ----------------------------------------------------------------------

    @ Command that receives a power signal for payload from the ground station
    sync command PAYLOAD_POWER_CMD(
                                    power: bool @< boolean val, true -> ON, false -> OFF
                                  )

    # ----------------------------------------------------------------------
    # Events
    # ----------------------------------------------------------------------

    @ Power command received
    event COMMAND_RECV(
                        power: bool @< boolean val, true -> ON, false -> OFF
                      ) \
      severity activity low \
      format "Payload power command received: {}"

    @ Received payload power status
    event COMMAND_STATUS(
                  status: bool @< boolean val, true -> SUCCESS, false -> FAILURE
                ) \
      severity activity high \
      format "Current payload power status is {}"

    # ----------------------------------------------------------------------
    # Telemetry
    # ----------------------------------------------------------------------

    @ Power command sent
    telemetry power: bool

    @ Current payload power status
    telemetry status: bool

    @ Payload power status request
    telemetry req_status: bool

  }

}

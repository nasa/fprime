module Ref {

  @ Input port for receiving payload power on/off signal
  port receive_power_command_input(
                                    power: bool @< boolean val, true -> ON, false -> OFF
                                  ) -> cmdRecv

  @ Output port for sending success/failure in carrying out power operation
  port send_power_command_status_output(
                                        status: bool @< boolean val, true -> SUCCESS, false -> FAILURE
                                        )
                          
  @ Output port for requesting current payload power status
  port request_power_status_output(
                                    req_status: bool @< boolean val, true -> get status
                                  )

  @ Input port for receiving the on/off status 
  port receive_power_status_input(
                                  status: bool @< boolean val, true -> ON, false -> OFF
                                  ) -> cmdRecv

  @ Output port for sending a power on/off signal to EPS hardware
  port send_power_command_output(
                                  power: bool @< boolean val, true -> ON, false -> OFF
                                )

}

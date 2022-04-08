module Ref {
  @ need 2 ports: 2 going in and out from controller 

  @ receiving battery level from controller
  port state_controller_in(battery_level: F32)

  @ sending battery level to controller
  port state_controller_out(battery_level: F32)

}
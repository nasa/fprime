
module FppTest {

  state machine DeviceSm 
  state machine HackSm 

  @ A component for testing data product code gen
  active component SmTest {

    # ----------------------------------------------------------------------
    # Types
    # ----------------------------------------------------------------------


    # ----------------------------------------------------------------------
    # General ports
    # ----------------------------------------------------------------------

    @ A schedIn port to run the data product generation
    async input port schedIn: Svc.Sched

    state machine instance device1: DeviceSm priority 1 block
    state machine instance device2: DeviceSm priority 2 assert
    state machine instance device3: HackSm priority 3 drop
    state machine instance device4: HackSm priority 4 hook
    state machine instance device5: HackSm
  
  }

}

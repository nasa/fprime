
module FppTest {

  state machine DeviceSm 

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

    state machine instance device1: DeviceSm
    state machine instance device2: DeviceSm
  
  }

}

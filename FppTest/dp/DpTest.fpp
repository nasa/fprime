module FppTest {

  @ A component for testing  data product code gen
  active component DpTest {

    # ----------------------------------------------------------------------
    # Types
    # ----------------------------------------------------------------------

    @ Data for a DataRecord
    struct Data {
      @ A U16 field
      u16Field: U16
    }

    # ----------------------------------------------------------------------
    # Special ports
    # ----------------------------------------------------------------------

    @ Data product get port
    product get port productGetOut

    @ Data product request port
    product request port productRequestOut

    @ Data product receive port
    async product recv port productRecvIn

    @ Data product send port
    product send port productSendOut

    @ Time get port
    time get port timeGetOut

    # ----------------------------------------------------------------------
    # General ports
    # ----------------------------------------------------------------------

    @ A schedIn port to run the data product generation
    async input port schedIn: Svc.Sched

    # ----------------------------------------------------------------------
    # Records
    # ----------------------------------------------------------------------

    @ Record 1
    product record U32Record: U32 id 100

    @ Record 2
    product record DataRecord: Data id 200

    @ Record 3
    product record U8ArrayRecord: U8 array id 300

    @ Record 4
    product record U32ArrayRecord: U32 array id 400

    @ Record 5
    product record DataArrayRecord: Data array id 500

    # ----------------------------------------------------------------------
    # Containers
    # ----------------------------------------------------------------------

    @ Container 1
    product container Container1 id 100 default priority 10

    @ Container 2
    product container Container2 id 200 default priority 20

    @ Container 3
    product container Container3 id 300 default priority 30

    @ Container 4
    product container Container4 id 400 default priority 40

    @ Container 5
    product container Container5 id 500 default priority 50

  }

}

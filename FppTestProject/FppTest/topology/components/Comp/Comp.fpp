module FppTest {

  @ A struct with a fixed-size member array
  struct FixedSizeData {
    a: U32
    b: [3] F32
    c: string size 10
  }

  port EmitTelemetry(a: U32)
  port GetParameter() -> U32

  active component Comp {

    import SpecialPorts

    @ Signal from the framework to synchronize with the testing context
    async input port syncIn: Svc.Sched
    output port syncOut: Svc.Sched

    async input port PingIn: Svc.Ping
    output port PingOut: Svc.Ping

    async command Start(nRecords: U32)
    async command Data(
        a: U32,
        b: F32,
        c: string size 10
    )
    async command End()

    event Event(
        a: U32,
        b: F32,
        c: string size 10
    ) severity activity high format "a: {}, b: {}, c: {}"

    telemetry Telemetry: U32

    param Param: U32 default 0

    struct ParamUpdateEvent {
        $id: FwPrmIdType
        value: U32
    }

    telemetry ParamUpdated: ParamUpdateEvent

    @ A record containing fixed-size data
    product record FixedSizeDataRecord: FixedSizeData id 0x00
    @ A record containing a variable-size array
    product record F32ArrayRecord: F32 array id 0x01

    product container Product

  }

}

module Svc {

    constant MAX_STRING_BYTES = 120

    port FileRead(
        path: string size MAX_STRING_BYTES
        ref buffer: Fw.Buffer
    )

    port FileWrite(
        path: string size MAX_STRING_BYTES
        ref buffer: Fw.Buffer
        offsetBytes: U32
    )

    port SignalDone(
        status: U32
        arg: U32
    )

    port CancelStatus(
    ) -> I8

    port VerifyStatus(
        path: string size MAX_STRING_BYTES
        crc: U32
    )

}

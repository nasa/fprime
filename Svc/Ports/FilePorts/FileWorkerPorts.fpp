module Svc {

    port FileRead(
        path: string size FileNameStringSize
        ref buffer: Fw.Buffer
    )

    port FileWrite(
        path: string size FileNameStringSize
        ref buffer: Fw.Buffer
        offsetBytes: FwSizeType
        append: bool
    )

    port SignalDone(
        status: U32
        sizeBytes: FwSizeType
    )

    port CancelStatus(
    )

    port VerifyStatus(
        path: string size FileNameStringSize
        crc: U32
    )

}

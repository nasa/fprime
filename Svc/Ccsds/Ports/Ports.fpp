module Svc {
module Ccsds {

    @ Enum representing a framing error in CCSDS protocols
    enum ErrorType: U8 {
        SP_INVALID_LENGTH = 0
        TC_INVALID_SCID = 1
        TC_INVALID_LENGTH = 2
        TC_INVALID_VCID = 3
        TC_INVALID_CRC = 4
    }


    @ Port for requesting a sequence count for a given APID
    port ApidSequenceCount (apid: ComCfg.Apid, sequenceCount: U16) -> U16

    @ Port for notifying of an error in framing/deframing
    port ErrorNotify (errorCode: Svc.Ccsds.ErrorType)

}
}

module Svc {
module Ccsds {

    @ Port for requesting a sequence count for a given APID
    port ApidSequenceCount (apid: ComCfg.Apid, sequenceCount: U16) -> U16

    @ Port for notifying of an error in framing/deframing
    port ErrorNotify (errorCode: Ccsds.FrameError)

    @ Port for requesting SDLS (Space Data Link Security) encryption/decryption
    @ on a buffer containing the initialization vector, data, and security trailer
    port CcsdsSdlsEncryption (securityAssociationIndex: U16, ref data: Fw.Buffer, context: ComCfg.FrameContext)

    @ Port for passing forward the result of an SDLS encryption/decryption:
    @ the operation status alongside the output data buffer and frame context
    port CcsdsSdlsData (status: SdlsStatus, ref data: Fw.Buffer, context: ComCfg.FrameContext)

}
}

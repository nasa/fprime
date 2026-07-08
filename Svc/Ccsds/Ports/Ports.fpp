module Svc {
module Ccsds {

    @ Port for requesting a sequence count for a given APID
    port ApidSequenceCount (apid: ComCfg.Apid, sequenceCount: U16) -> U16

    @ Port for notifying of an error in framing/deframing
    port ErrorNotify (errorCode: Ccsds.FrameError)

    @ Port for performing SDLS (Space Data Link Security) encryption/decryption
    @ on a buffer containing the initialization vector, data, and security trailer
    port CcsdsSdlsEncryption (securityAssociationIndex: U16, ref data: Fw.Buffer) -> SdlsStatus

}
}

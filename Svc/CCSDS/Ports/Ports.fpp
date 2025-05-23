module Svc {
module CCSDS {

    @ Port for requesting a sequence count for a given APID
    port ApidSequenceCount (apid: ComCfg.APID, sequenceCount: U16) -> Fw.Success

    # port ApidSequenceCount (rawApidValue: U16, ref apid: ComCfg.APID, ref sequenceCount: U16) -> Fw.Success


}
}

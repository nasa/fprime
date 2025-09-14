module Svc {
module Ccsds {

    @ Port for requesting a sequence count for a given APID
    port ApidSequenceCount (apid: ComCfg.Apid, sequenceCount: U16) -> U16

}
}

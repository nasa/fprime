module Svc {
module CCSDS {

    @ Port for requesting a sequence count for a given APID
    port ApidSequenceCount (apid: ComCfg.APID, sequenceCount: U16) -> U16

}
}

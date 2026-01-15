# ======================================================================
# CfdpCfg.fpp
# F Prime CFDP configuration constants
# ======================================================================

module Svc {
    module Ccsds {
        @ Number of buffer ports used to send PDUs
        @ This must match the CF_NUM_CHANNELS macro defined in CfdpCfg.hpp
        @ BPC TODO: Remove CF_NUM_CHANNELS in favor of this type
        constant CfdpManagerNumChannels = 2

        @ File path size used for CFDP file system operations
        constant CfdpManagerMaxFileSize = 200

        @ @brief Entity id size
        @
        @ @par Description:
        @      The maximum size of the entity id as expected for all CFDP packets.
        @      CF supports the spec's variable size of EID, where the actual size is
        @      selected at runtime, and therefore the size in CFDP PDUs may be smaller
        @      than the size specified here.  This type only establishes the maximum
        @      size (and therefore maximum value) that an EID may be.
        @
        @ @note This type is used in several CF commands, and so changing the size
        @       of this type will affect the following structs:
        @        CF_ConfigTable_t, configuration table - will change size of file
        @        CF_ConfigPacket_t, set config params command
        @        CF_TxFileCmd_t, transmit file command
        @        CF_PlaybackDirCmd_t, equivalent to above
        @        CF_Transaction_Payload_t, any command that selects a transaction based on EID
        @
        @ @par Limits
        @         Must be one of U8, U16, U32, U64.
        type CfdpEntityId = U32

        @ @brief transaction sequence number size
        @
        @ @par Description:
        @      The max size of the transaction sequence number as expected for all CFDP packets.
        @      CF supports the spec's variable size of TSN, where the actual size is
        @      selected at runtime, and therefore the size in CFDP PDUs may be smaller
        @      than the size specified here.  This type only establishes the maximum
        @      size (and therefore maximum value) that a TSN may be.
        @
        @ @note This type is used in several CF commands, and so changing the size
        @       of this type will affect the following structure:
        @        CF_Transaction_Payload_t, any command that selects a transaction based on TSN
        @
        @ @par Limits
        @         Must be one of U8, U16, U32, U64.
        @
        @ BPC TODO: Refactor use of CfdpTransactionSeq to use this type
        type CfdpTransactionSeq = U32

        @ @brief Maximum PDU size in bytes
        @
        @ @par Description:
        @      Limits the maximum possible Tx PDU size. This value must match
        @      CF_MAX_PDU_SIZE in CfdpCfg.hpp. The resulting CCSDS packet also
        @      includes a CCSDS header and additional bytes.
        @
        @ @par Limits:
        @      Must respect any CCSDS packet size limits on the system.
        constant CfdpMaxPduSize = 512

        @ @brief Maximum file data payload size in a File Data PDU
        @
        @ @par Description:
        @      This is the maximum data bytes that can be carried in a File Data PDU
        @      after accounting for CFDP headers (PDU header + File Data header).
        @      This value should be CfdpMaxPduSize minus typical header overhead.
        @
        @ @par Limits:
        @      Must be less than CfdpMaxPduSize.
        constant CfdpMaxFileDataSize = 450
    }
}

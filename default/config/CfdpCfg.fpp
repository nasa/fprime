# ======================================================================
# CfdpCfg.fpp
# F Prime CFDP configuration constants
# ======================================================================

module Svc {
    module Ccsds {
        module Cfdp {
            @ Number of CFDP channels
            constant NumChannels = 2

            @ File path size used for CFDP file system operations
            constant MaxFilePathSize = 200
            
            @ @brief Entity id size
            @
            @ @par Description:
            @      The maximum size of the entity id as expected for all CFDP packets.
            @      CF supports the spec's variable size of EID, where the actual size is
            @      selected at runtime, and therefore the size in CFDP PDUs may be smaller
            @      than the size specified here.  This type only establishes the maximum
            @      size (and therefore maximum value) that an EID may be.
            @
            @ @note This type is used in several commands, and so changing the size
            @       of this type will affect various command structures.
            @
            @ @par Limits
            @         Must be one of U8, U16, U32, U64.
            type EntityId = U32

            @ @brief transaction sequence number size
            @
            @ @par Description:
            @      The max size of the transaction sequence number as expected for all CFDP packets.
            @      CF supports the spec's variable size of TSN, where the actual size is
            @      selected at runtime, and therefore the size in CFDP PDUs may be smaller
            @      than the size specified here.  This type only establishes the maximum
            @      size (and therefore maximum value) that a TSN may be.
            @
            @ @note This type is used in several commands, and so changing the size
            @       of this type will affect various command structures.
            @
            @ @par Limits
            @         Must be one of U8, U16, U32, U64.
            type TransactionSeq = U32

            @ @brief File size and offset type
            @
            @ @par Description:
            @      The type used for file sizes and offsets in CFDP operations.
            @      The CFDP protocol permits use of 64-bit values for file size/offsets,
            @      although the current implementation supports 32-bit values.
            @
            @ @par Limits
            @         Must be one of U8, U16, U32, U64.
            type FileSize = U32

            @ @brief Maximum PDU size in bytes
            @
            @ @par Description:
            @      Limits the maximum possible Tx PDU size.
            @
            @ @par Limits:
            @      Must respect any CCSDS packet size limits on the system.
            constant MaxPduSize = 512
        }
    }
}

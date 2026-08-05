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
            @      The CFDP protocol permits use of 64-bit values for file size/offsets.
            @      Changes have been made to support U64, but it remains untested.
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
            constant MaxPduSize = 1024

            @ @brief Max NAK segments supported in a NAK PDU
            @
            @ @par Description:
            @      When a NAK PDU is sent or received, this is the max number of
            @      segment requests supported. This number should match the ground
            @      CFDP engine configuration as well.
            constant NakMaxSegments = 58

            @ @brief Maximum TLVs (Type-Length-Value) per PDU
            @
            @ @par Description:
            @      Maximum number of TLV tuples that can be included in a single CFDP PDU.
            @      TLVs are optional metadata fields used in EOF and FIN PDUs. The limit
            @      of 4 is sufficient for typical operations (1 Entity ID TLV plus 3
            @      additional for filestore requests/responses or messages).
            @
            @ @par Limits:
            @      Must be > 0.
            @
            @ @reference
            @      CCSDS 727.0-B-5, section 5.4, table 5-3
            constant MaxTlv = 4

            @ @brief R2 CRC calc chunk size
            @
            @ @par Description:
            @      R2 performs CRC calculation upon file completion in chunks. This is the
            @      size of the buffer. A larger size uses more stack but computes faster.
            @      The overall bytes calculated per wakeup is set in the configuration table.
            constant R2CrcChunkSize = 1024

            @ @brief Max number of simultaneous file receives
            @
            @ @par Description:
            @      Each channel can support this number of active/concurrent file receive
            @      transactions. This contributes to the total transaction pool size.
            constant MaxSimultaneousRx = 5

            @ @brief Number of max commanded playback files per channel
            @
            @ @par Description:
            @      The max number of outstanding ground commanded file transmits per channel.
            constant MaxCommandedPlaybackFilesPerChan = 10

            @ @brief Max number of commanded playback directories per channel
            @
            @ @par Description:
            @      Each channel can support this number of ground commanded directory playbacks.
            constant MaxCommandedPlaybackDirectoriesPerChan = 2

            @ @brief Max number of polling directories per channel
            @
            @ @par Description:
            @      This affects the configuration table. There must be an entry (can be
            @      empty) for each of these polling directories per channel.
            constant MaxPollingDirPerChan = 5

            @ @brief Number of transactions per playback directory
            @
            @ @par Description:
            @      Each playback/polling directory operation can have this many active
            @      transfers at a time pending or active.
            constant NumTransactionsPerPlayback = 5

            @ @brief Number of histories per channel
            @
            @ @par Description:
            @      Each channel maintains a circular buffer of completed transaction records
            @      (history entries) for debugging and reference.
            @
            @ @par Limits:
            @      65536 is the current max.
            constant NumHistoriesPerChannel = 256
        }
    }
}

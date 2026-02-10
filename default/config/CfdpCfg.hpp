// ======================================================================
// \title  CfdpCfg.hpp
// \author Brian Campuzano
// \brief  F Prime CFDP configuration constants
// ======================================================================

namespace Svc {
namespace Ccsds {
namespace Cfdp {

// ==================================================================
// Protocol Configuration
// ==================================================================

/**
 *  @brief Max NAK segments supported in a NAK PDU
 *
 *  @par Description:
 *       When a NAK PDU is sent or received, this is the max number of
 *       segment requests supported. This number should match the ground
 *       CFDP engine configuration as well.
 *
 *  @par Limits:
 *
 */
#define CFDP_NAK_MAX_SEGMENTS (58)

/**
 *  @brief Maximum TLVs (Type-Length-Value) per PDU
 *
 *  @par Description:
 *       Maximum number of TLV (Type-Length-Value) tuples that can be
 *       included in a single CFDP PDU. TLVs are optional metadata fields
 *       used in EOF and FIN PDUs to convey diagnostic information.
 *
 *       Per CCSDS 727.0-B-5 section 5.4, TLVs are variable-length fields
 *       that encode information such as entity IDs, fault handler overrides,
 *       or messages to the user. The most common use is the Entity ID TLV
 *       (type 6), automatically added to EOF and FIN PDUs on error conditions
 *       to aid in debugging.
 *
 *       This value sets an upper bound on TLV storage per PDU to prevent
 *       unbounded memory growth. The limit of 4 is sufficient for typical
 *       CFDP operations:
 *       - 1 for Entity ID TLV
 *       - 3 additional for filestore requests/responses or messages
 *
 *  @par Limits:
 *       Must be > 0.
 *       Larger values consume more memory per PDU but allow more metadata.
 *
 *  @reference
 *       CCSDS 727.0-B-5, section 5.4, table 5-3
 */
#define CFDP_MAX_TLV (4)

/**
 *  @brief R2 CRC calc chunk size
 *
 *  @par Description
 *       R2 performs CRC calculation upon file completion in chunks. This is the size
 *       of the buffer. The larger the size the more stack will be used, but
 *       the faster it can go. The overall number of bytes calculated per wakeup
 *       is set in the configuration table.
 *
 *  @par Limits:
 *
 */
#define CFDP_R2_CRC_CHUNK_SIZE (1024)

/**
 *  @brief RX chunks per transaction (per channel)
 *
 *  @par Description:
 *       Number of chunks per transaction per channel (RX).
 *
 * RX CHUNKS -
 * For Class 2 CFDP receive transactions, the receiver must track which file segments
 * have been successfully received. A chunk represents a contiguous range (offset, size)
 * of received file data. By tracking received chunks, the receiver can identify gaps
 * in the file data and generate NAK PDUs to request retransmission of missing segments.
 *
 * (array size must be NumChannels)
 * CFDP_CHANNEL_NUM_RX_CHUNKS_PER_TRANSACTION is an array for each channel indicating
 * the number of chunks per transaction to track received file segments. This enables
 * gap detection and NAK generation for reliable Class 2 transfers.
 *
 *  @par Limits:
 *
 */
#define CFDP_CHANNEL_NUM_RX_CHUNKS_PER_TRANSACTION \
    {                                            \
        CFDP_NAK_MAX_SEGMENTS, CFDP_NAK_MAX_SEGMENTS \
    }

/**
 *  @brief TX chunks per transaction (per channel)
 *
 *  @par Description:
 *       Number of chunks per transaction per channel (TX).
 *
 * TX CHUNKS -
 * For Class 2 CFDP transmit transactions, the sender must track which file segments
 * the receiver has requested via NAK PDUs. Each chunk represents a gap (offset, size)
 * that needs to be retransmitted.
 *
 * (array size must be NumChannels)
 * CFDP_CHANNEL_NUM_TX_CHUNKS_PER_TRANSACTION is an array for each channel indicating
 * the number of chunks to track NAK segment requests from the receiver per transaction.
 * This allows the sender to queue and retransmit the requested missing file data.
 *
 *  @par Limits:
 *
 */
#define CFDP_CHANNEL_NUM_TX_CHUNKS_PER_TRANSACTION \
    {                                            \
        CFDP_NAK_MAX_SEGMENTS, CFDP_NAK_MAX_SEGMENTS \
    }

// ==================================================================
// Resource Pool Configuration
// ==================================================================

/**
 *  @brief Max number of simultaneous file receives.
 *
 *  @par Description:
 *       Each channel can support this number of active/concurrent file receive
 *       transactions. This contributes to the total transaction pool size and
 *       limits how many incoming files can be received simultaneously.
 *
 *  @par Limits:
 *
 */
#define CFDP_MAX_SIMULTANEOUS_RX (5)

/**
 *  @brief Number of max commanded playback files per chan.
 *
 *  @par Description:
 *       This is the max number of outstanding ground commanded file transmits per channel.
 *
 *  @par Limits:
 *
 */
#define CFDP_MAX_COMMANDED_PLAYBACK_FILES_PER_CHAN (10)

/**
 *  @brief Max number of commanded playback directories per channel.
 *
 *  @par Description:
 *       Each channel can support this number of ground commanded directory playbacks.
 *
 *  @par Limits:
 *
 */
#define CFDP_MAX_COMMANDED_PLAYBACK_DIRECTORIES_PER_CHAN (2)

/**
 *  @brief Max number of polling directories per channel.
 *
 *  @par Description:
 *       This affects the configuration table. There must be an entry (can
 *       be empty) for each of these polling directories per channel.
 *
 *  @par Limits:
 *
 */
#define CFDP_MAX_POLLING_DIR_PER_CHAN (5)

/**
 *  @brief Number of transactions per playback directory.
 *
 *  @par Description:
 *       Each playback/polling directory operation will be able to have this
 *       many active transfers at a time pending or active.
 *
 *  @par Limits:
 *
 */
#define CFDP_NUM_TRANSACTIONS_PER_PLAYBACK (5)

/**
 *  @brief Number of histories per channel
 *
 *  @par Description:
 *       Each channel maintains a circular buffer of completed transaction records
 *       (history entries) for debugging and reference. This defines the maximum
 *       number of completed transactions to keep in the history buffer.
 *
 *  @par Limits:
 *       65536 is the current max.
 */
#define CFDP_NUM_HISTORIES_PER_CHANNEL (256)

// ==================================================================
// Miscellaneous
// ==================================================================

/**
 * @brief Macro type for Entity id that is used in printf style formatting
 *
 * @note This must match the size of CfdpEntityId as defined in CfdpCfg.fpp
 */
#define CFDP_PRI_ENTITY_ID PRIu32

/**
 * @brief Macro type for transaction sequences that is used in printf style formatting
 *
 * @note This must match the size of CfdpTransactionSeq as defined in CfdpCfg.fpp
 */
#define CFDP_PRI_TRANSACTION_SEQ PRIu32

}  // namespace Cfdp
}  // namespace Ccsds
}  // namespace Svc
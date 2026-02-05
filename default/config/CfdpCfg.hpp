// ======================================================================
// \title  CfdpCfg.hpp
// \author campuzan
// \brief  F Prime CFDP configuration constants
// ======================================================================

#include <Fw/FPrimeBasicTypes.hpp>

namespace Svc {
namespace Ccsds {

/**
 *  @brief Number of channels
 *
 *  @par Description:
 *       The number of channels in the engine. Changing this
 *       value changes the configuration table for the application.
 *       This must match CfdpManagerNumChannels defined in CfdpCfg.fpp
 *
 *  @par Limits:
 *       Must be less <= 200. Obviously it will be smaller than that.
 * 
 * BPC TODO: replace with CfdpManagerNumChannels
 */
#define CFDP_NUM_CHANNELS (2)

/**
 *  @brief Type for logical file size / file offset values used by CFDP
 *
 *  @par Limits:
 *       Must be a U32 or U64.
 *
 *       Per CCSDS 727.0-B-5 (CFDP Blue Book), all File Size Sensitive (FSS)
 *       fields, including file size and file offset, are encoded as either
 *       32-bit or 64-bit unsigned integers depending on the value of the
 *       CFDP Large File flag.
 *
 *       When the Large File flag is 0, FSS fields are 32 bits.
 *       When the Large File flag is 1, FSS fields are 64 bits.
 *
 *  @reference
 *       CCSDS 727.0-B-5, CCSDS File Delivery Protocol (CFDP),
 *       https://public.ccsds.org/Pubs/727x0b5e1.pdf
 */
typedef U32 CfdpFileSize;

/**
 *  @brief RX chunks per transaction (per channel)
 *
 *  @par Description:
 *       Number of chunks per transaction per channel (RX).
 *
 * CHUNKS -
 * A chunk is a representation of a range (offset, size) of data received by a receiver.
 *
 * Class 2 CFDP deals with NAK, so received data must be tracked for receivers in order to generate
 * the NAK. The sender must also keep track of NAK requests and send new file data PDUs as a result.
 * (array size must be CFDP_NUM_CHANNELS)
 * CFDP_CHANNEL_NUM_RX_CHUNKS_PER_TRANSACTION is an array for each channel indicating the number of chunks per transaction
 * CFDP_CHANNEL_NUM_TX_CHUNKS_PER_TRANSACTION is an array for each channel indicating the number of chunks to keep track
 * of NAK requests from the receiver per transaction
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
 *  @par Limits:
 *
 */
#define CFDP_CHANNEL_NUM_TX_CHUNKS_PER_TRANSACTION \
    {                                            \
        CFDP_NAK_MAX_SEGMENTS, CFDP_NAK_MAX_SEGMENTS \
    }

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
 *  @brief Max number of simultaneous file receives.
 *
 *  @par Description:
 *       Each channel can support this number of file receive transactions at a time.
 *
 *  @par Limits:
 *
 */
#define CFDP_MAX_SIMULTANEOUS_RX (5)

/* definitions that affect execution */

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
 *  @brief Number of histories per channel
 *
 *  @par Description:
 *       Each channel can support this number of file receive transactions at a time.
 *
 *  @par Limits:
 *       65536 is the current max.
 */
#define CFDP_NUM_HISTORIES_PER_CHANNEL (256)

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
 *  @brief Total number of chunks (tx, rx, all channels)
 *
 *  @par Description:
 *       Must be equal to the sum of all values input in CFDP_CHANNEL_NUM_RX_CHUNKS_PER_TRANSACTION
 *       and CFDP_CHANNEL_NUM_TX_CHUNKS_PER_TRANSACTION.
 *
 *  @par Limits:
 *
 */
/* CFDP_TOTAL_CHUNKS must be equal to the total number of chunks per rx/tx transactions per channel */
/* (in other words, the summation of all elements in CFDP_CHANNEL_NUM_R/TX_CHUNKS_PER_TRANSACTION */
#define CFDP_TOTAL_CHUNKS (CFDP_NAK_MAX_SEGMENTS * 4)

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
 *       unbounded memory growth. The limit of 4 is based on NASA's cFS CF
 *       implementation and is sufficient for typical CFDP operations:
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
 *  @brief Max PDU size.
 *
 *  @par Description:
 *       Limits the maximum possible Tx PDU size. Note the resulting CCSDS packet
 *       also includes a CCSDS header and CF_PDU_ENCAPSULATION_EXTRA_TRAILING_BYTES.
 *       The outgoing file data chunk size is also limited from the table configuration
 *       or by set parameter command, which is checked against this value
 *       (+ smallest possible PDU header).
 *
 *  @par Note:
 *       This does NOT limit Rx PDUs, since the file data is written from
 *       the transport packet to the file.
 *
 *  @par Limits:
 *       Since PDUs are wrapped in CCSDS packets, need to respect any
 *       CCSDS packet size limits on the system.
 *
 */
#define CFDP_MAX_PDU_SIZE (512)

/**
 *  @brief Maximum file name length.
 *
 *  @par Limits:
 *
 */
#define CFDP_FILENAME_MAX_NAME FileNameStringSize

/**
 *  @brief Max filename and path length.
 *
 *  @par Limits:
 *
 */
#define CFDP_FILENAME_MAX_LEN FileNameStringSize

/**
 * @brief Macro type for Entity id that is used in printf style formatting
 * 
 * @note This must match the size of CfdpEntityId as defined in CfdpCfg.fpp
 */
#define CFDP_PRI_ENTITY_ID PRIu32

/**
 * @brief Macro type for transaction seqeunces that is used in printf style formatting
 * 
 * @note This must match the size of CfdpTransactionSeq as defined in CfdpCfg.fpp
 */
#define CFDP_PRI_TRANSACTION_SEQ PRIu32

}  // namespace Svc
}  // namespace Ccsds
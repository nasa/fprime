// ======================================================================
// \title  Types.hpp
// \author campuzan
// \brief  hpp file for shared CFDP protocol type definitions
// ======================================================================

#ifndef Svc_Ccsds_Cfdp_Types_HPP
#define Svc_Ccsds_Cfdp_Types_HPP

#include <functional>

#include <Fw/FPrimeBasicTypes.hpp>
#include <Fw/Types/String.hpp>
#include <Fw/Types/EnabledEnumAc.hpp>
#include <Os/File.hpp>
#include <Os/Directory.hpp>
#include <config/CfdpCfg.hpp>
#include <config/EntityIdAliasAc.hpp>
#include <config/TransactionSeqAliasAc.hpp>
#include <config/FileSizeAliasAc.hpp>

#include <CFDP/Checksum/Checksum.hpp>
#include <Svc/Ccsds/CfdpManager/Types/StatusEnumAc.hpp>
#include <Svc/Ccsds/CfdpManager/Types/FlowEnumAc.hpp>
#include <Svc/Ccsds/CfdpManager/Types/ClassEnumAc.hpp>
#include <Svc/Ccsds/CfdpManager/Types/KeepEnumAc.hpp>
#include <Svc/Ccsds/CfdpManager/Types/QueueIdEnumAc.hpp>
#include <Svc/Ccsds/CfdpManager/Clist.hpp>
#include <Svc/Ccsds/CfdpManager/Chunk.hpp>
#include <Svc/Ccsds/CfdpManager/Timer.hpp>

namespace Svc {
namespace Ccsds {
namespace Cfdp {

// Forward declarations for class types used in structs below
class Transaction;

/**
 * @brief Maximum possible number of transactions that may exist on a single CFDP channel
 */
#define CFDP_NUM_TRANSACTIONS_PER_CHANNEL                                                \
    (CFDP_MAX_COMMANDED_PLAYBACK_FILES_PER_CHAN + CFDP_MAX_SIMULTANEOUS_RX +               \
     ((CFDP_MAX_POLLING_DIR_PER_CHAN + CFDP_MAX_COMMANDED_PLAYBACK_DIRECTORIES_PER_CHAN) * \
      CFDP_NUM_TRANSACTIONS_PER_PLAYBACK))

// CFDP File Directive Codes
// Blue Book section 5.2, table 5-4
enum FileDirective : U8 {
    FILE_DIRECTIVE_INVALID_MIN = 0,  // Minimum used to limit range
    FILE_DIRECTIVE_END_OF_FILE = 4,  // End of File
    FILE_DIRECTIVE_FIN = 5,          // Finished
    FILE_DIRECTIVE_ACK = 6,          // Acknowledge
    FILE_DIRECTIVE_METADATA = 7,     // Metadata
    FILE_DIRECTIVE_NAK = 8,          // Negative Acknowledge
    FILE_DIRECTIVE_PROMPT = 9,       // Prompt
    FILE_DIRECTIVE_KEEP_ALIVE = 12,  // Keep Alive
    FILE_DIRECTIVE_INVALID_MAX = 13  // Maximum used to limit range
};

// CFDP Condition Codes
// Blue Book section 5.2.2, table 5-5
enum ConditionCode : U8 {
    CONDITION_CODE_NO_ERROR = 0,
    CONDITION_CODE_POS_ACK_LIMIT_REACHED = 1,
    CONDITION_CODE_KEEP_ALIVE_LIMIT_REACHED = 2,
    CONDITION_CODE_INVALID_TRANSMISSION_MODE = 3,
    CONDITION_CODE_FILESTORE_REJECTION = 4,
    CONDITION_CODE_FILE_CHECKSUM_FAILURE = 5,
    CONDITION_CODE_FILE_SIZE_ERROR = 6,
    CONDITION_CODE_NAK_LIMIT_REACHED = 7,
    CONDITION_CODE_INACTIVITY_DETECTED = 8,
    CONDITION_CODE_INVALID_FILE_STRUCTURE = 9,
    CONDITION_CODE_CHECK_LIMIT_REACHED = 10,
    CONDITION_CODE_UNSUPPORTED_CHECKSUM_TYPE = 11,
    CONDITION_CODE_SUSPEND_REQUEST_RECEIVED = 14,
    CONDITION_CODE_CANCEL_REQUEST_RECEIVED = 15
};

// CFDP ACK Transaction Status
// Blue Book section 5.2.4, table 5-8
enum AckTxnStatus : U8 {
    ACK_TXN_STATUS_UNDEFINED = 0,
    ACK_TXN_STATUS_ACTIVE = 1,
    ACK_TXN_STATUS_TERMINATED = 2,
    ACK_TXN_STATUS_UNRECOGNIZED = 3,
    ACK_TXN_STATUS_INVALID = 4
};

// CFDP FIN Delivery Code
// Blue Book section 5.2.3, table 5-7
enum FinDeliveryCode : U8 {
    FIN_DELIVERY_CODE_COMPLETE = 0,    // Data complete
    FIN_DELIVERY_CODE_INCOMPLETE = 1   // Data incomplete
};

// CFDP FIN File Status
// Blue Book section 5.2.3, table 5-7
enum FinFileStatus : U8 {
    FIN_FILE_STATUS_DISCARDED = 0,            // File discarded deliberately
    FIN_FILE_STATUS_DISCARDED_FILESTORE = 1,  // File discarded due to filestore rejection
    FIN_FILE_STATUS_RETAINED = 2,             // File retained successfully
    FIN_FILE_STATUS_UNREPORTED = 3            // File status unreported
};

// CFDP Checksum Type
// Blue Book section 5.2.5, table 5-9
enum ChecksumType : U8 {
    CHECKSUM_TYPE_MODULAR = 0,        // Modular checksum
    CHECKSUM_TYPE_CRC_32 = 1,         // CRC-32 (not currently supported)
    CHECKSUM_TYPE_NULL_CHECKSUM = 15  // Null checksum
};

/**
 * @brief High-level state of a transaction
 */
enum TxnState : U8
{
    TXN_STATE_UNDEF   = 0, /**< \brief State assigned to an unused object on the free list */
    TXN_STATE_INIT    = 1, /**< \brief State assigned to a newly allocated transaction object */
    TXN_STATE_R1      = 2, /**< \brief Receive file as class 1 */
    TXN_STATE_S1      = 3, /**< \brief Send file as class 1 */
    TXN_STATE_R2      = 4, /**< \brief Receive file as class 2 */
    TXN_STATE_S2      = 5, /**< \brief Send file as class 2 */
    TXN_STATE_DROP    = 6, /**< \brief State where all PDUs are dropped */
    TXN_STATE_HOLD    = 7, /**< \brief State assigned to a transaction after freeing it */
    TXN_STATE_INVALID = 8  /**< \brief Marker value for the highest possible state number */
};

/**
 * @brief Sub-state of a send file transaction
 */
enum TxSubState : U8
{
    TX_SUB_STATE_METADATA      = 0, /**< sending the initial MD directive */
    TX_SUB_STATE_FILEDATA      = 1, /**< sending file data PDUs */
    TX_SUB_STATE_EOF           = 2, /**< sending the EOF directive */
    TX_SUB_STATE_CLOSEOUT_SYNC = 3, /**< pending final acks from remote */
    TX_SUB_STATE_NUM_STATES    = 4
};

/**
 * @brief Sub-state of a receive file transaction
 */
enum RxSubState : U8
{
    RX_SUB_STATE_FILEDATA      = 0, /**< receive file data PDUs */
    RX_SUB_STATE_EOF           = 1, /**< got EOF directive */
    RX_SUB_STATE_CLOSEOUT_SYNC = 2, /**< pending final acks from remote */
    RX_SUB_STATE_NUM_STATES    = 3
};

/**
 * @brief Direction identifier
 *
 * Differentiates between send and receive history entries
 */
enum Direction : U8
{
    DIRECTION_RX  = 0,
    DIRECTION_TX  = 1,
    DIRECTION_NUM = 2,
};

/**
 * @brief Transaction initiation method
 *
 * Differentiates between command-initiated and port-initiated transactions
 */
enum TransactionInitType : U8
{
    INIT_BY_COMMAND = 0,  //!< Transaction initiated via command interface
    INIT_BY_PORT    = 1   //!< Transaction initiated via port interface
};

/**
 * @brief Identifies the type of timer tick being processed
 */
enum CfdpTickType : U8
{
    CFDP_TICK_TYPE_RX,
    CFDP_TICK_TYPE_TXW_NORM,
    CFDP_TICK_TYPE_TXW_NAK,
    CFDP_TICK_TYPE_NUM_TYPES
};

/**
 * @brief Values for Transaction Status code
 *
 * This enum defines the possible values representing the
 * result of a transaction.  This is a superset of the condition codes
 * defined in CCSDS book 727 (condition codes) but with additional
 * values for local conditions that the blue book does not have,
 * such as protocol/state machine or decoding errors.
 *
 * The values here are designed to not overlap with the condition
 * codes defined in the blue book, but can be translated to one
 * of those codes for the purposes of FIN/ACK/EOF PDUs.
 */
enum TxnStatus : I32
{
    /**
     * The undefined status is a placeholder for new transactions before a value is set.
     */
    TXN_STATUS_UNDEFINED = -1,

    /* Status codes 0-15 share the same values/meanings as the CFDP condition code (CC) */
    TXN_STATUS_NO_ERROR                  = CONDITION_CODE_NO_ERROR,
    TXN_STATUS_POS_ACK_LIMIT_REACHED     = CONDITION_CODE_POS_ACK_LIMIT_REACHED,
    TXN_STATUS_KEEP_ALIVE_LIMIT_REACHED  = CONDITION_CODE_KEEP_ALIVE_LIMIT_REACHED,
    TXN_STATUS_INVALID_TRANSMISSION_MODE = CONDITION_CODE_INVALID_TRANSMISSION_MODE,
    TXN_STATUS_FILESTORE_REJECTION       = CONDITION_CODE_FILESTORE_REJECTION,
    TXN_STATUS_FILE_CHECKSUM_FAILURE     = CONDITION_CODE_FILE_CHECKSUM_FAILURE,
    TXN_STATUS_FILE_SIZE_ERROR           = CONDITION_CODE_FILE_SIZE_ERROR,
    TXN_STATUS_NAK_LIMIT_REACHED         = CONDITION_CODE_NAK_LIMIT_REACHED,
    TXN_STATUS_INACTIVITY_DETECTED       = CONDITION_CODE_INACTIVITY_DETECTED,
    TXN_STATUS_INVALID_FILE_STRUCTURE    = CONDITION_CODE_INVALID_FILE_STRUCTURE,
    TXN_STATUS_CHECK_LIMIT_REACHED       = CONDITION_CODE_CHECK_LIMIT_REACHED,
    TXN_STATUS_UNSUPPORTED_CHECKSUM_TYPE = CONDITION_CODE_UNSUPPORTED_CHECKSUM_TYPE,
    TXN_STATUS_SUSPEND_REQUEST_RECEIVED  = CONDITION_CODE_SUSPEND_REQUEST_RECEIVED,
    TXN_STATUS_CANCEL_REQUEST_RECEIVED   = CONDITION_CODE_CANCEL_REQUEST_RECEIVED,

    /* Additional status codes for items not representable in a CFDP CC, these can be set in
     * transactions that did not make it to the point of sending FIN/EOF. */
    TXN_STATUS_PROTOCOL_ERROR     = 16,
    TXN_STATUS_ACK_LIMIT_NO_FIN   = 17,
    TXN_STATUS_ACK_LIMIT_NO_EOF   = 18,
    TXN_STATUS_NAK_RESPONSE_ERROR = 19,
    TXN_STATUS_SEND_EOF_FAILURE   = 20,
    TXN_STATUS_EARLY_FIN          = 21,

    /* keep last */
    TXN_STATUS_MAX = 22
};

/**
 * @brief Cache of source and destination filename
 *
 * This pairs a source and destination file name together
 * to be retained for future reference in the transaction/history
 */
struct CfdpTxnFilenames
{
    Fw::String src_filename;
    Fw::String dst_filename;
};

/**
 * @brief CFDP History entry
 *
 * Records CFDP operations for future reference
 */
struct History
{
    CfdpTxnFilenames fnames;   /**< \brief file names associated with this history entry */
    CListNode cl_node;  /**< \brief for connection to a CList */
    Direction dir;      /**< \brief direction of this history entry */
    TxnStatus txn_stat; /**< \brief final status of operation */
    EntityId src_eid;  /**< \brief the source eid of the transaction */
    EntityId peer_eid; /**< \brief peer_eid is always the "other guy", same src_eid for RX */
    TransactionSeq seq_num;  /**< \brief transaction identifier, stays constant for entire transfer */
};

/**
 * @brief Wrapper around a CfdpChunkList object
 *
 * This allows a CfdpChunkList to be stored within a CList data storage structure.
 * The wrapper is pooled by Channel for reuse across transactions.
 */
struct CfdpChunkWrapper
{
    CfdpChunkList chunks;   //!< Chunk list for gap tracking
    CListNode cl_node; //!< Circular list node for pooling

    /**
     * @brief Constructor for initializing the chunk list
     *
     * @param maxChunks Maximum number of chunks this list can hold
     * @param chunkMem Pointer to pre-allocated chunk memory
     */
    CfdpChunkWrapper(ChunkIdx maxChunks, Chunk* chunkMem)
        : chunks(maxChunks, chunkMem), cl_node{} {}
};

/**
 * @brief CFDP Playback entry
 *
 * Keeps the state of CFDP playback requests
 */
struct Playback
{
    Os::Directory dir;
    Class::T cfdp_class;
    CfdpTxnFilenames fnames;
    U16 num_ts; /**< \brief number of transactions */
    U8 priority;
    EntityId dest_id;
    char pending_file[CfdpManagerMaxFileSize];

    bool busy;
    bool diropen;
    Keep::T keep;
    bool counted;
};

/**
 * \brief Directory poll entry
 *
 * Keeps the state of CFDP directory polling
 */
struct CfdpPollDir
{
    Playback pb; /**< \brief State of the currrent playback requests */
    Timer intervalTimer; /**< \brief Timer object used to poll the directory */

    U32 intervalSec; /**< \brief number of seconds to wait before trying a new directory */

    U8 priority;   /**< \brief priority to use when placing transactions on the pending queue */
    Class::T cfdpClass; /**< \brief the CFDP class to send */
    EntityId destEid;   /**< \brief destination entity id */

    Fw::String srcDir; /**< \brief path to source dir */
    Fw::String dstDir; /**< \brief path to destination dir */

    Fw::Enabled enabled; /**< \brief Enabled flag */
};

/**
 * @brief Data specific to a class 2 send file transaction
 */
struct CfdpTxS2Data
{
    U8 fin_cc; /**< \brief remember the cc in the received FIN PDU to echo in eof-fin */
    U8 acknak_count;
};

/**
 * @brief Data specific to a send file transaction
 */
struct CfdpTxStateData
{
    TxSubState sub_state;
    FileSize cached_pos;

    CfdpTxS2Data s2;
};

/**
 * @brief Data specific to a class 2 receive file transaction
 */
struct CfdpRxS2Data
{
    U32                    eof_crc;
    FileSize           eof_size;
    FileSize           rx_crc_calc_bytes;
    FinDeliveryCode dc;
    FinFileStatus   fs;
    U8                     eof_cc; /**< \brief remember the cc in the received EOF PDU to echo in eof-ack */
    U8                     acknak_count;
};

/**
 * @brief Data specific to a receive file transaction
 */
struct CfdpRxStateData
{
    RxSubState sub_state;
    FileSize cached_pos;

    CfdpRxS2Data r2;
};

/**
 * @brief Data that applies to all types of transactions
 */
struct CfdpFlagsCommon
{
    U8 q_index; /**< \brief Q index this is in */
    bool  ack_timer_armed;
    bool  suspended;
    bool  canceled;
    bool  crc_calc;
    bool  inactivity_fired; /**< \brief set whenever the inactivity timeout expires */
    bool  keep_history;     /**< \brief whether history should be preserved during recycle */
};

/**
 * @brief Flags that apply to receive transactions
 */
struct CfdpFlagsRx
{
    CfdpFlagsCommon com;

    bool md_recv; /**< \brief md received for r state */
    bool eof_recv;
    bool send_nak;
    bool send_fin;
    bool send_eof_ack;
    bool complete;    /**< \brief r2 */
    bool fd_nak_sent; /**< \brief latches that at least one NAK has been sent for file data */
};

/**
 * @brief Flags that apply to send transactions
 */
struct CfdpFlagsTx
{
    CfdpFlagsCommon com;

    bool md_need_send;
    bool send_eof;
    bool eof_ack_recv;
    bool fin_recv;
    bool send_fin_ack;
    bool cmd_tx; /**< \brief indicates transaction is commanded (ground) tx */
};

/**
 * @brief Summary of all possible transaction flags (tx and rx)
 */
union CfdpStateFlags
{
    CfdpFlagsCommon com; /**< \brief applies to all transactions */
    CfdpFlagsRx     rx;  /**< \brief applies to only receive file transactions */
    CfdpFlagsTx     tx;  /**< \brief applies to only send file transactions */
};

/**
 * @brief Summary of all possible transaction state information (tx and rx)
 */
union CfdpStateData
{
    CfdpTxStateData send;    /**< \brief applies to only send file transactions */
    CfdpRxStateData receive; /**< \brief applies to only receive file transactions */
};

/**
 * @brief Callback function type for use with Channel::traverseAllTransactions()
 *
 * @param txn Pointer to current transaction being traversed
 * @param context Opaque object passed from initial call
 */
using CfdpTraverseAllTransactionsFunc = std::function<void(Transaction *txn, void *context)>;

}  // namespace Cfdp
}  // namespace Ccsds
}  // namespace Svc

#endif  // Svc_Ccsds_Cfdp_Types_HPP

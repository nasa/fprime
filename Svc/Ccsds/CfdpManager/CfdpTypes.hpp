// ======================================================================
// \title  CfdpTypes.hpp
// \brief  Macros and data types used by CFDP
//
// This file is a port of the cf_cfdp_types.hpp file from the 
// NASA Core Flight System (cFS) CFDP (CF) Application,
// version 3.0.0, adapted for use within the F-Prime (F') framework.
//
// Functions should not be declared in this file. This should
// be limited to shared macros and data types only. 
//
// ======================================================================
//
// NASA Docket No. GSC-18,447-1
//
// Copyright (c) 2019 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may
// not use this file except in compliance with the License. You may obtain
// a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// ======================================================================

#ifndef CFDP_TYPES_HPP
#define CFDP_TYPES_HPP

#include <config/CfdpCfg.hpp>
#include <Os/File.hpp>
#include <Os/Directory.hpp>

#include <CFDP/Checksum/Checksum.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpPdu.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpClist.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpChunk.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpCodec.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpTimer.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpManager.hpp>
#include <Svc/Ccsds/Types/CfdpFlowEnumAc.hpp>
#include <Svc/Ccsds/Types/CfdpClassEnumAc.hpp>
#include <Svc/Ccsds/Types/CfdpQueueIdEnumAc.hpp>

namespace Svc {
namespace Ccsds {

/**
 * @brief Maximum possible number of transactions that may exist on a single CF channel
 */
#define CF_NUM_TRANSACTIONS_PER_CHANNEL                                                \
    (CF_MAX_COMMANDED_PLAYBACK_FILES_PER_CHAN + CF_MAX_SIMULTANEOUS_RX +               \
     ((CF_MAX_POLLING_DIR_PER_CHAN + CF_MAX_COMMANDED_PLAYBACK_DIRECTORIES_PER_CHAN) * \
      CF_NUM_TRANSACTIONS_PER_PLAYBACK))

/**
 * @brief Maximum possible number of transactions that may exist in the CF application
 */
#define CF_NUM_TRANSACTIONS (CF_NUM_CHANNELS * CF_NUM_TRANSACTIONS_PER_CHANNEL)

/**
 * @brief Maximum possible number of history entries that may exist in the CF application
 */
#define CF_NUM_HISTORIES (CF_NUM_CHANNELS * CF_NUM_HISTORIES_PER_CHANNEL)

/**
 * @brief Maximum possible number of chunk entries that may exist in the CF application
 */
#define CF_NUM_CHUNKS_ALL_CHANNELS (CF_TOTAL_CHUNKS * CF_NUM_TRANSACTIONS_PER_CHANNEL)

/**
 * @brief High-level state of a transaction
 */
typedef enum
{
    CF_TxnState_UNDEF   = 0, /**< \brief State assigned to an unused object on the free list */
    CF_TxnState_INIT    = 1, /**< \brief State assigned to a newly allocated transaction object */
    CF_TxnState_R1      = 2, /**< \brief Receive file as class 1 */
    CF_TxnState_S1      = 3, /**< \brief Send file as class 1 */
    CF_TxnState_R2      = 4, /**< \brief Receive file as class 2 */
    CF_TxnState_S2      = 5, /**< \brief Send file as class 2 */
    CF_TxnState_DROP    = 6, /**< \brief State where all PDUs are dropped */
    CF_TxnState_HOLD    = 7, /**< \brief State assigned to a transaction after freeing it */
    CF_TxnState_INVALID = 8  /**< \brief Marker value for the highest possible state number */
} CF_TxnState_t;

/**
 * @brief Sub-state of a send file transaction
 */
typedef enum
{
    CF_TxSubState_METADATA      = 0, /**< sending the initial MD directive */
    CF_TxSubState_FILEDATA      = 1, /**< sending file data PDUs */
    CF_TxSubState_EOF           = 2, /**< sending the EOF directive */
    CF_TxSubState_CLOSEOUT_SYNC = 3, /**< pending final acks from remote */
    CF_TxSubState_NUM_STATES    = 4
} CF_TxSubState_t;

/**
 * @brief Sub-state of a receive file transaction
 */
typedef enum
{
    CF_RxSubState_FILEDATA      = 0, /**< receive file data PDUs */
    CF_RxSubState_EOF           = 1, /**< got EOF directive */
    CF_RxSubState_CLOSEOUT_SYNC = 2, /**< pending final acks from remote */
    CF_RxSubState_NUM_STATES    = 3
} CF_RxSubState_t;

/**
 * @brief Direction identifier
 *
 * Differentiates between send and receive history entries
 */
typedef enum
{
    CF_Direction_RX  = 0,
    CF_Direction_TX  = 1,
    CF_Direction_NUM = 2,
} CF_Direction_t;

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
typedef enum
{
    /**
     * The undefined status is a placeholder for new transactions before a value is set.
     */
    CF_TxnStatus_UNDEFINED = -1,

    /* Status codes 0-15 share the same values/meanings as the CFDP condition code (CC) */
    CF_TxnStatus_NO_ERROR                  = CF_CFDP_ConditionCode_NO_ERROR,
    CF_TxnStatus_POS_ACK_LIMIT_REACHED     = CF_CFDP_ConditionCode_POS_ACK_LIMIT_REACHED,
    CF_TxnStatus_KEEP_ALIVE_LIMIT_REACHED  = CF_CFDP_ConditionCode_KEEP_ALIVE_LIMIT_REACHED,
    CF_TxnStatus_INVALID_TRANSMISSION_MODE = CF_CFDP_ConditionCode_INVALID_TRANSMISSION_MODE,
    CF_TxnStatus_FILESTORE_REJECTION       = CF_CFDP_ConditionCode_FILESTORE_REJECTION,
    CF_TxnStatus_FILE_CHECKSUM_FAILURE     = CF_CFDP_ConditionCode_FILE_CHECKSUM_FAILURE,
    CF_TxnStatus_FILE_SIZE_ERROR           = CF_CFDP_ConditionCode_FILE_SIZE_ERROR,
    CF_TxnStatus_NAK_LIMIT_REACHED         = CF_CFDP_ConditionCode_NAK_LIMIT_REACHED,
    CF_TxnStatus_INACTIVITY_DETECTED       = CF_CFDP_ConditionCode_INACTIVITY_DETECTED,
    CF_TxnStatus_INVALID_FILE_STRUCTURE    = CF_CFDP_ConditionCode_INVALID_FILE_STRUCTURE,
    CF_TxnStatus_CHECK_LIMIT_REACHED       = CF_CFDP_ConditionCode_CHECK_LIMIT_REACHED,
    CF_TxnStatus_UNSUPPORTED_CHECKSUM_TYPE = CF_CFDP_ConditionCode_UNSUPPORTED_CHECKSUM_TYPE,
    CF_TxnStatus_SUSPEND_REQUEST_RECEIVED  = CF_CFDP_ConditionCode_SUSPEND_REQUEST_RECEIVED,
    CF_TxnStatus_CANCEL_REQUEST_RECEIVED   = CF_CFDP_ConditionCode_CANCEL_REQUEST_RECEIVED,

    /* Additional status codes for items not representable in a CFDP CC, these can be set in
     * transactions that did not make it to the point of sending FIN/EOF. */
    CF_TxnStatus_PROTOCOL_ERROR     = 16,
    CF_TxnStatus_ACK_LIMIT_NO_FIN   = 17,
    CF_TxnStatus_ACK_LIMIT_NO_EOF   = 18,
    CF_TxnStatus_NAK_RESPONSE_ERROR = 19,
    CF_TxnStatus_SEND_EOF_FAILURE   = 20,
    CF_TxnStatus_EARLY_FIN          = 21,

    /* keep last */
    CF_TxnStatus_MAX = 22
} CF_TxnStatus_t;

/**
 * @brief Cache of source and destination filename
 *
 * This pairs a source and destination file name together
 * to be retained for future reference in the transaction/history
 */
typedef struct CF_TxnFilenames
{
    Fw::String src_filename;
    Fw::String dst_filename;
} CfdpTxnFilenames;

/**
 * @brief CF History entry
 *
 * Records CF app operations for future reference
 */
typedef struct CF_History
{
    CfdpTxnFilenames fnames;   /**< \brief file names associated with this history entry */
    CF_CListNode_t cl_node;  /**< \brief for connection to a CList */
    CF_Direction_t dir;      /**< \brief direction of this history entry */
    CF_TxnStatus_t txn_stat; /**< \brief final status of operation */
    CfdpEntityId src_eid;  /**< \brief the source eid of the transaction */
    CfdpEntityId peer_eid; /**< \brief peer_eid is always the "other guy", same src_eid for RX */
    CfdpTransactionSeq seq_num;  /**< \brief transaction identifier, stays constant for entire transfer */
} CF_History_t;

/**
 * @brief Wrapper around a CF_ChunkList_t object
 *
 * This allows a CF_ChunkList_t to be stored within a CList data storage structure
 */
typedef struct CF_ChunkWrapper
{
    CF_ChunkList_t chunks;
    CF_CListNode_t cl_node;
} CF_ChunkWrapper_t;

/**
 * @brief CF Playback entry
 *
 * Keeps the state of CF playback requests
 */
typedef struct CF_Playback
{
    Os::Directory dir;
    CfdpClass::T cfdp_class;
    CfdpTxnFilenames fnames;
    U16 num_ts; /**< \brief number of transactions */
    U8 priority;
    CfdpEntityId dest_id;
    char pending_file[FppConstant_CfdpManagerMaxFileSize::CfdpManagerMaxFileSize];

    bool busy;
    bool diropen;
    CfdpKeep::T keep;
    bool counted;
} CF_Playback_t;

/**
 * \brief Directory poll entry
 * 
 * Keeps the state of CF directory polling
 */
typedef struct CF_PollDir
{
    CF_Playback_t pb; /**< \brief State of the currrent playback requests */
    CfdpTimer intervalTimer; /**< \brief Timer object used to poll the directory */

    U32 intervalSec; /**< \brief number of seconds to wait before trying a new directory */

    U8 priority;   /**< \brief priority to use when placing transactions on the pending queue */
    CfdpClass::T cfdpClass; /**< \brief the CFDP class to send */
    CfdpEntityId destEid;   /**< \brief destination entity id */

    Fw::String srcDir; /**< \brief path to source dir */
    Fw::String dstDir; /**< \brief path to destination dir */

    Fw::Enabled enabled; /**< \brief Enabled flag */
} CF_PollDir_t;

/**
 * @brief Data specific to a class 2 send file transaction
 */
typedef struct CF_TxS2_Data
{
    U8 fin_cc; /**< \brief remember the cc in the received FIN PDU to echo in eof-fin */
    U8 acknak_count;
} CF_TxS2_Data_t;

/**
 * @brief Data specific to a send file transaction
 */
typedef struct CF_TxState_Data
{
    CF_TxSubState_t sub_state;
    U32          cached_pos;

    CF_TxS2_Data_t s2;
} CF_TxState_Data_t;

/**
 * @brief Data specific to a class 2 receive file transaction
 */
typedef struct CF_RxS2_Data
{
    U32                    eof_crc;
    U32                    eof_size;
    U32                    rx_crc_calc_bytes;
    CF_CFDP_FinDeliveryCode_t dc;
    CF_CFDP_FinFileStatus_t   fs;
    U8                     eof_cc; /**< \brief remember the cc in the received EOF PDU to echo in eof-ack */
    U8                     acknak_count;
} CF_RxS2_Data_t;

/**
 * @brief Data specific to a receive file transaction
 */
typedef struct CF_RxState_Data
{
    CF_RxSubState_t sub_state;
    U32          cached_pos;

    CF_RxS2_Data_t r2;
} CF_RxState_Data_t;

/**
 * @brief Data that applies to all types of transactions
 */
typedef struct CF_Flags_Common
{
    U8 q_index; /**< \brief Q index this is in */
    bool  ack_timer_armed;
    bool  suspended;
    bool  canceled;
    bool  crc_calc;
    bool  inactivity_fired; /**< \brief set whenever the inactivity timeout expires */
    bool  keep_history;     /**< \brief whether history should be preserved during recycle */
} CF_Flags_Common_t;

/**
 * @brief Flags that apply to receive transactions
 */
typedef struct CF_Flags_Rx
{
    CF_Flags_Common_t com;

    bool md_recv; /**< \brief md received for r state */
    bool eof_recv;
    bool send_nak;
    bool send_fin;
    bool send_eof_ack;
    bool complete;    /**< \brief r2 */
    bool fd_nak_sent; /**< \brief latches that at least one NAK has been sent for file data */
} CF_Flags_Rx_t;

/**
 * @brief Flags that apply to send transactions
 */
typedef struct CF_Flags_Tx
{
    CF_Flags_Common_t com;

    bool md_need_send;
    bool send_eof;
    bool eof_ack_recv;
    bool fin_recv;
    bool send_fin_ack;
    bool cmd_tx; /**< \brief indicates transaction is commanded (ground) tx */
} CF_Flags_Tx_t;

/**
 * @brief Summary of all possible transaction flags (tx and rx)
 */
typedef union CF_StateFlags
{
    CF_Flags_Common_t com; /**< \brief applies to all transactions */
    CF_Flags_Rx_t     rx;  /**< \brief applies to only receive file transactions */
    CF_Flags_Tx_t     tx;  /**< \brief applies to only send file transactions */
} CF_StateFlags_t;

/**
 * @brief Summary of all possible transaction state information (tx and rx)
 */
typedef union CF_StateData
{
    CF_TxState_Data_t send;    /**< \brief applies to only send file transactions */
    CF_RxState_Data_t receive; /**< \brief applies to only receive file transactions */
} CF_StateData_t;

/**
 * @brief Transaction state object
 *
 * This keeps the state of CF file transactions
 */
typedef struct CF_Transaction
{
    CF_TxnState_t state; /**< \brief each engine is commanded to do something, which is the overall state */

    CF_History_t * history; /**< \brief weird, holds active filenames and possibly other info */
    CF_ChunkWrapper_t *chunks; /**< \brief for gap tracking, only used on class 2 */
    CfdpTimer inactivity_timer; /**< \brief set to the overall inactivity timer of a remote */
    CfdpTimer ack_timer; /**< \brief called ack_timer, but is also nak_timer */

    CfdpFileSize fsize; /**< \brief File size */
    CfdpFileSize foffs; /**< \brief offset into file for next read */
    Os::File fd;

    CFDP::Checksum crc;

    CfdpKeep::T keep;
    U8 chan_num; /**< \brief if ever more than one engine, this may need to change to pointer */
    U8 priority;

    CF_CListNode_t cl_node;

    CF_Playback_t *pb; /**< \brief NULL if transaction does not belong to a playback */

    CF_StateData_t state_data;

    /**
     * @brief State flags
     *
     * \note The flags here look a little strange, because there are different flags for TX and RX.
     * Both types share the same type of flag, though. Since RX flags plus the global flags is
     * over one byte, storing them this way allows 2 bytes to cover all possible flags.
     * Please ignore the duplicate declarations of the "all" flags.
     */
    CF_StateFlags_t flags;

    /**< \brief Reference to the wrapper F' component in order to send PDUs */
    CfdpManager* cfdpManager;

} CF_Transaction_t;

/**
 * @brief Identifies the type of timer tick being processed
 */
typedef enum
{
    CF_TickType_RX,
    CF_TickType_TXW_NORM,
    CF_TickType_TXW_NAK,
    CF_TickType_NUM_TYPES
} CF_TickType_t;

/**
 * @brief Channel state object
 *
 * This keeps the state of CF channels
 *
 * Each CF channel has a separate transaction list, PDU throttle, playback,
 * and poll state, as well as separate addresses on the underlying message
 * transport (e.g. SB).
 */
typedef struct CF_Channel
{
    CF_CListNode_t *qs[CfdpQueueId::NUM];
    CF_CListNode_t *cs[CF_Direction_NUM];

    // TODO remove all pipe references
    // CFE_SB_PipeId_t pipe;

    U32 num_cmd_tx;

    CF_Playback_t playback[CF_MAX_COMMANDED_PLAYBACK_DIRECTORIES_PER_CHAN];

    /* Polling directory state */
    CF_PollDir_t polldir[CF_MAX_POLLING_DIR_PER_CHAN];

    // TODO remove all semaphore references
    // osal_id_t sem_id; /**< \brief semaphore id for output pipe */

    const CF_Transaction_t *cur; /**< \brief current transaction during channel cycle */

    /**< \brief Reference to the wrapper F' component in order to reference parameters */
    CfdpManager* cfdpManager;
    
    U8 tick_type;
    
    /**< \brief ID used to index into the engine channel array */
    U8 channel_id;
    
    /**< \brief State of the channel */
    CfdpFlow::T flowState;

} CF_Channel_t;

/**
 * @brief An engine represents a pairing to a local EID
 *
 * Each engine can have at most CF_MAX_SIMULTANEOUS_TRANSACTIONS
 * 
 * @note This struct8ure was ported from CF_Engine_t
 */
typedef struct CfdpEngineDataT
{
    CfdpEngineDataT()
        : seq_num(0),
        outgoing_counter(0),
        enabled(false)
    {

    }
    CfdpTransactionSeq seq_num; /* \brief keep track of the next sequence number to use for sends */

    // CF_Output_t out;
    // CF_Input_t  in;

    /* NOTE: could have separate array of transactions as part of channel? */
    CF_Transaction_t transactions[CF_NUM_TRANSACTIONS];
    CF_History_t     histories[CF_NUM_HISTORIES];
    CF_Channel_t     channels[CF_NUM_CHANNELS];

    CF_ChunkWrapper_t chunks[CF_NUM_TRANSACTIONS * CF_Direction_NUM];
    CF_Chunk_t        chunk_mem[CF_NUM_CHUNKS_ALL_CHANNELS];

    U32 outgoing_counter;
    bool   enabled;
} CfdpEngineData;

}  // namespace Ccsds
}  // namespace Svc

#endif /* !CFDP_TYPES_HPP */

// ======================================================================
// \title  CfdpTypes.hpp
// \brief  Macros and data types used by CFDP
//
// This file is a port of CFDP type definitions from the following files
// from the NASA Core Flight System (cFS) CFDP (CF) Application, version 3.0.0,
// adapted for use within the F-Prime (F') framework:
// - cf_cfdp_types.h (CFDP macros and data type definitions)
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

#include <functional>

#include <Fw/Types/BasicTypes.hpp>
#include <config/CfdpCfg.hpp>
#include <config/CfdpEntityIdAliasAc.hpp>
#include <config/CfdpTransactionSeqAliasAc.hpp>
#include <config/CfdpFileSizeAliasAc.hpp>
#include <Fw/Types/EnabledEnumAc.hpp>
#include <Os/File.hpp>
#include <Os/Directory.hpp>

#include <CFDP/Checksum/Checksum.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpPdu.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpClist.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpChunk.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpTimer.hpp>
#include <Svc/Ccsds/CfdpManager/Types/FlowEnumAc.hpp>
#include <Svc/Ccsds/CfdpManager/Types/ClassEnumAc.hpp>
#include <Svc/Ccsds/CfdpManager/Types/QueueIdEnumAc.hpp>
#include <Svc/Ccsds/CfdpManager/Types/KeepEnumAc.hpp>

namespace Svc {
namespace Ccsds {

// Forward declarations
class CfdpChannel;
class CfdpManager;
class CfdpEngine;
class CfdpTransaction;

/**
 * @brief Maximum possible number of transactions that may exist on a single CFDP channel
 */
#define CFDP_NUM_TRANSACTIONS_PER_CHANNEL                                                \
    (CFDP_MAX_COMMANDED_PLAYBACK_FILES_PER_CHAN + CFDP_MAX_SIMULTANEOUS_RX +               \
     ((CFDP_MAX_POLLING_DIR_PER_CHAN + CFDP_MAX_COMMANDED_PLAYBACK_DIRECTORIES_PER_CHAN) * \
      CFDP_NUM_TRANSACTIONS_PER_PLAYBACK))

/**
 * @brief Maximum possible number of transactions that may exist in the CFDP implementation
 */
#define CFDP_NUM_TRANSACTIONS (CFDP_NUM_CHANNELS * CFDP_NUM_TRANSACTIONS_PER_CHANNEL)

/**
 * @brief Maximum possible number of history entries that may exist in the CFDP implementation
 */
#define CFDP_NUM_HISTORIES (CFDP_NUM_CHANNELS * CFDP_NUM_HISTORIES_PER_CHANNEL)

/**
 * @brief Maximum possible number of chunk entries that may exist in the CFDP implementation
 */
#define CFDP_NUM_CHUNKS_ALL_CHANNELS (CFDP_TOTAL_CHUNKS * CFDP_NUM_TRANSACTIONS_PER_CHANNEL)

/**
 * @brief High-level state of a transaction
 */
enum CfdpTxnState : U8
{
    CFDP_TXN_STATE_UNDEF   = 0, /**< \brief State assigned to an unused object on the free list */
    CFDP_TXN_STATE_INIT    = 1, /**< \brief State assigned to a newly allocated transaction object */
    CFDP_TXN_STATE_R1      = 2, /**< \brief Receive file as class 1 */
    CFDP_TXN_STATE_S1      = 3, /**< \brief Send file as class 1 */
    CFDP_TXN_STATE_R2      = 4, /**< \brief Receive file as class 2 */
    CFDP_TXN_STATE_S2      = 5, /**< \brief Send file as class 2 */
    CFDP_TXN_STATE_DROP    = 6, /**< \brief State where all PDUs are dropped */
    CFDP_TXN_STATE_HOLD    = 7, /**< \brief State assigned to a transaction after freeing it */
    CFDP_TXN_STATE_INVALID = 8  /**< \brief Marker value for the highest possible state number */
};

/**
 * @brief Sub-state of a send file transaction
 */
enum CfdpTxSubState : U8
{
    CFDP_TX_SUB_STATE_METADATA      = 0, /**< sending the initial MD directive */
    CFDP_TX_SUB_STATE_FILEDATA      = 1, /**< sending file data PDUs */
    CFDP_TX_SUB_STATE_EOF           = 2, /**< sending the EOF directive */
    CFDP_TX_SUB_STATE_CLOSEOUT_SYNC = 3, /**< pending final acks from remote */
    CFDP_TX_SUB_STATE_NUM_STATES    = 4
};

/**
 * @brief Sub-state of a receive file transaction
 */
enum CfdpRxSubState : U8
{
    CFDP_RX_SUB_STATE_FILEDATA      = 0, /**< receive file data PDUs */
    CFDP_RX_SUB_STATE_EOF           = 1, /**< got EOF directive */
    CFDP_RX_SUB_STATE_CLOSEOUT_SYNC = 2, /**< pending final acks from remote */
    CFDP_RX_SUB_STATE_NUM_STATES    = 3
};

/**
 * @brief Direction identifier
 *
 * Differentiates between send and receive history entries
 */
enum CfdpDirection : U8
{
    CFDP_DIRECTION_RX  = 0,
    CFDP_DIRECTION_TX  = 1,
    CFDP_DIRECTION_NUM = 2,
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
enum CfdpTxnStatus : I32
{
    /**
     * The undefined status is a placeholder for new transactions before a value is set.
     */
    CFDP_TXN_STATUS_UNDEFINED = -1,

    /* Status codes 0-15 share the same values/meanings as the CFDP condition code (CC) */
    CFDP_TXN_STATUS_NO_ERROR                  = CFDP_CONDITION_CODE_NO_ERROR,
    CFDP_TXN_STATUS_POS_ACK_LIMIT_REACHED     = CFDP_CONDITION_CODE_POS_ACK_LIMIT_REACHED,
    CFDP_TXN_STATUS_KEEP_ALIVE_LIMIT_REACHED  = CFDP_CONDITION_CODE_KEEP_ALIVE_LIMIT_REACHED,
    CFDP_TXN_STATUS_INVALID_TRANSMISSION_MODE = CFDP_CONDITION_CODE_INVALID_TRANSMISSION_MODE,
    CFDP_TXN_STATUS_FILESTORE_REJECTION       = CFDP_CONDITION_CODE_FILESTORE_REJECTION,
    CFDP_TXN_STATUS_FILE_CHECKSUM_FAILURE     = CFDP_CONDITION_CODE_FILE_CHECKSUM_FAILURE,
    CFDP_TXN_STATUS_FILE_SIZE_ERROR           = CFDP_CONDITION_CODE_FILE_SIZE_ERROR,
    CFDP_TXN_STATUS_NAK_LIMIT_REACHED         = CFDP_CONDITION_CODE_NAK_LIMIT_REACHED,
    CFDP_TXN_STATUS_INACTIVITY_DETECTED       = CFDP_CONDITION_CODE_INACTIVITY_DETECTED,
    CFDP_TXN_STATUS_INVALID_FILE_STRUCTURE    = CFDP_CONDITION_CODE_INVALID_FILE_STRUCTURE,
    CFDP_TXN_STATUS_CHECK_LIMIT_REACHED       = CFDP_CONDITION_CODE_CHECK_LIMIT_REACHED,
    CFDP_TXN_STATUS_UNSUPPORTED_CHECKSUM_TYPE = CFDP_CONDITION_CODE_UNSUPPORTED_CHECKSUM_TYPE,
    CFDP_TXN_STATUS_SUSPEND_REQUEST_RECEIVED  = CFDP_CONDITION_CODE_SUSPEND_REQUEST_RECEIVED,
    CFDP_TXN_STATUS_CANCEL_REQUEST_RECEIVED   = CFDP_CONDITION_CODE_CANCEL_REQUEST_RECEIVED,

    /* Additional status codes for items not representable in a CFDP CC, these can be set in
     * transactions that did not make it to the point of sending FIN/EOF. */
    CFDP_TXN_STATUS_PROTOCOL_ERROR     = 16,
    CFDP_TXN_STATUS_ACK_LIMIT_NO_FIN   = 17,
    CFDP_TXN_STATUS_ACK_LIMIT_NO_EOF   = 18,
    CFDP_TXN_STATUS_NAK_RESPONSE_ERROR = 19,
    CFDP_TXN_STATUS_SEND_EOF_FAILURE   = 20,
    CFDP_TXN_STATUS_EARLY_FIN          = 21,

    /* keep last */
    CFDP_TXN_STATUS_MAX = 22
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
struct CfdpHistory
{
    CfdpTxnFilenames fnames;   /**< \brief file names associated with this history entry */
    CfdpCListNode cl_node;  /**< \brief for connection to a CList */
    CfdpDirection dir;      /**< \brief direction of this history entry */
    CfdpTxnStatus txn_stat; /**< \brief final status of operation */
    CfdpEntityId src_eid;  /**< \brief the source eid of the transaction */
    CfdpEntityId peer_eid; /**< \brief peer_eid is always the "other guy", same src_eid for RX */
    CfdpTransactionSeq seq_num;  /**< \brief transaction identifier, stays constant for entire transfer */
};

/**
 * @brief Wrapper around a CfdpChunkList object
 *
 * This allows a CfdpChunkList to be stored within a CList data storage structure.
 * The wrapper is pooled by CfdpChannel for reuse across transactions.
 */
struct CfdpChunkWrapper
{
    CfdpChunkList chunks;   //!< Chunk list for gap tracking
    CfdpCListNode cl_node; //!< Circular list node for pooling

    /**
     * @brief Constructor for initializing the chunk list
     *
     * @param maxChunks Maximum number of chunks this list can hold
     * @param chunkMem Pointer to pre-allocated chunk memory
     */
    CfdpChunkWrapper(CfdpChunkIdx maxChunks, CfdpChunk* chunkMem)
        : chunks(maxChunks, chunkMem), cl_node{} {}
};

/**
 * @brief CFDP Playback entry
 *
 * Keeps the state of CFDP playback requests
 */
struct CfdpPlayback
{
    Os::Directory dir;
    Cfdp::Class::T cfdp_class;
    CfdpTxnFilenames fnames;
    U16 num_ts; /**< \brief number of transactions */
    U8 priority;
    CfdpEntityId dest_id;
    char pending_file[FppConstant_CfdpManagerMaxFileSize::CfdpManagerMaxFileSize];

    bool busy;
    bool diropen;
    Cfdp::Keep::T keep;
    bool counted;
};

/**
 * \brief Directory poll entry
 *
 * Keeps the state of CFDP directory polling
 */
struct CfdpPollDir
{
    CfdpPlayback pb; /**< \brief State of the currrent playback requests */
    CfdpTimer intervalTimer; /**< \brief Timer object used to poll the directory */

    U32 intervalSec; /**< \brief number of seconds to wait before trying a new directory */

    U8 priority;   /**< \brief priority to use when placing transactions on the pending queue */
    Cfdp::Class::T cfdpClass; /**< \brief the CFDP class to send */
    CfdpEntityId destEid;   /**< \brief destination entity id */

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
    CfdpTxSubState sub_state;
    CfdpFileSize cached_pos;

    CfdpTxS2Data s2;
};

/**
 * @brief Data specific to a class 2 receive file transaction
 */
struct CfdpRxS2Data
{
    U32                    eof_crc;
    CfdpFileSize           eof_size;
    CfdpFileSize           rx_crc_calc_bytes;
    CfdpFinDeliveryCode dc;
    CfdpFinFileStatus   fs;
    U8                     eof_cc; /**< \brief remember the cc in the received EOF PDU to echo in eof-ack */
    U8                     acknak_count;
};

/**
 * @brief Data specific to a receive file transaction
 */
struct CfdpRxStateData
{
    CfdpRxSubState sub_state;
    CfdpFileSize cached_pos;

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
 * @brief Callback function type for use with CfdpChannel::traverseAllTransactions()
 *
 * @param txn Pointer to current transaction being traversed
 * @param context Opaque object passed from initial call
 */
using CfdpTraverseAllTransactionsFunc = std::function<void(CfdpTransaction *txn, void *context)>;

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

}  // namespace Ccsds
}  // namespace Svc

#endif /* !CFDP_TYPES_HPP */

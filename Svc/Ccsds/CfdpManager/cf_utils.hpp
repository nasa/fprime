/************************************************************************
 * NASA Docket No. GSC-18,447-1, and identified as “CFS CFDP (CF)
 * Application version 3.0.0”
 *
 * Copyright (c) 2019 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ************************************************************************/

/**
 * @file
 *
 *  The CF Application utils header file
 */

#ifndef CF_UTILS_HPP
#define CF_UTILS_HPP

#include "cf_cfdp.hpp"

#include <Fw/Types/Assert.hpp>

namespace Svc {
namespace Ccsds {

/**
 * @brief Argument structure for use with CList_Traverse()
 *
 * This identifies a specific transaction sequence number and entity ID
 * The transaction pointer is set by the implementation
 */
typedef struct CF_Traverse_TransSeqArg
{
    CF_TransactionSeq_t transaction_sequence_number;
    CF_EntityId_t       src_eid;
    CF_Transaction_t *  txn; /**< \brief output transaction pointer */
} CF_Traverse_TransSeqArg_t;

/**
 * @brief Callback function type for use with CF_TraverseAllTransactions()
 *
 * @param txn Pointer to current transaction being traversed
 * @param context Opaque object passed from initial call
 */
typedef void (*CF_TraverseAllTransactions_fn_t)(CF_Transaction_t *txn, void *context);

/**
 * @brief Argument structure for use with CF_TraverseAllTransactions()
 *
 * This basically allows for running a CF_Traverse on several lists at once
 */
typedef struct CF_TraverseAll_Arg
{
    CF_TraverseAllTransactions_fn_t fn;      /**< \brief internal callback to use for each CList_Traverse */
    void *                          context; /**< \brief opaque object to pass to internal callback */
    I32                           counter; /**< \brief Running tally of all nodes traversed from all lists */
} CF_TraverseAll_Arg_t;

/**
 * @brief Argument structure for use with CF_CList_Traverse_R()
 *
 * This is for searching for transactions of a specific priority
 */
typedef struct CF_Traverse_PriorityArg
{
    CF_Transaction_t *txn; /**< \brief OUT: holds value of transaction with which to call CF_CList_InsertAfter on */
    U8             priority; /**< \brief seeking this priority */
} CF_Traverse_PriorityArg_t;

/* free a transaction from the queue it's on.
 * NOTE: this leaves the transaction in a bad state,
 * so it must be followed by placing the transaction on
 * another queue. Need this function because the path of
 * freeing a transaction (returning to default state)
 * means that it must be removed from the current queue
 * otherwise if the structure is zero'd out the queue
 * will become corrupted due to other nodes on the queue
 * pointing to an invalid node */
static inline void CF_DequeueTransaction(CF_Transaction_t *txn)
{
    FW_ASSERT(txn && (txn->chan_num < CF_NUM_CHANNELS));
    CF_CList_Remove(&cfdpEngine.channels[txn->chan_num].qs[txn->flags.com.q_index], &txn->cl_node);
    // FW_ASSERT(CF_AppData.hk.Payload.channel_hk[txn->chan_num].q_size[txn->flags.com.q_index]); /* sanity check */
    // --CF_AppData.hk.Payload.channel_hk[txn->chan_num].q_size[txn->flags.com.q_index];
}

static inline void CF_MoveTransaction(CF_Transaction_t *txn, CF_QueueIdx_t queue)
{
    FW_ASSERT(txn && (txn->chan_num < CF_NUM_CHANNELS));
    CF_CList_Remove(&cfdpEngine.channels[txn->chan_num].qs[txn->flags.com.q_index], &txn->cl_node);
    // FW_ASSERT(CF_AppData.hk.Payload.channel_hk[txn->chan_num].q_size[txn->flags.com.q_index]); /* sanity check */
    // --CF_AppData.hk.Payload.channel_hk[txn->chan_num].q_size[txn->flags.com.q_index];
    CF_CList_InsertBack(&cfdpEngine.channels[txn->chan_num].qs[queue], &txn->cl_node);
    txn->flags.com.q_index = queue;
    // ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].q_size[txn->flags.com.q_index];
}

static inline void CF_CList_Remove_Ex(CF_Channel_t *chan, CF_QueueIdx_t queueidx, CF_CListNode_t *node)
{
    CF_CList_Remove(&chan->qs[queueidx], node);
    // FW_ASSERT(CF_AppData.hk.Payload.channel_hk[chan - cfdpEngine.channels].q_size[queueidx]); /* sanity check */
    // --CF_AppData.hk.Payload.channel_hk[chan - cfdpEngine.channels].q_size[queueidx];
}

static inline void CF_CList_InsertAfter_Ex(CF_Channel_t *chan, CF_QueueIdx_t queueidx, CF_CListNode_t *start,
                                           CF_CListNode_t *after)
{
    CF_CList_InsertAfter(&chan->qs[queueidx], start, after);
    // ++CF_AppData.hk.Payload.channel_hk[chan - cfdpEngine.channels].q_size[queueidx];
}

static inline void CF_CList_InsertBack_Ex(CF_Channel_t *chan, CF_QueueIdx_t queueidx, CF_CListNode_t *node)
{
    CF_CList_InsertBack(&chan->qs[queueidx], node);
    // ++CF_AppData.hk.Payload.channel_hk[chan - cfdpEngine.channels].q_size[queueidx];
}

/************************************************************************/
/** @brief Find an unused transaction on a channel.
 *
 * @par Assumptions, External Events, and Notes:
 *       chan must not be NULL.
 *
 * @param chan Pointer to the CF channel
 * @param direction Intended direction of data flow (TX or RX)
 *
 * @returns Pointer to a free transaction
 * @retval  NULL if no free transactions available.
 */
CF_Transaction_t *CF_FindUnusedTransaction(CF_Channel_t *chan, CF_Direction_t direction);

/************************************************************************/
/** @brief Returns a history structure back to its unused state.
 *
 * @par Description
 *       There's nothing to do currently other than remove the history
 *       from its current queue and put it back on CF_QueueIdx_HIST_FREE.
 *
 * @par Assumptions, External Events, and Notes:
 *       chan must not be NULL. history must not be NULL.
 *
 * @param chan Pointer to the CF channel
 * @param history Pointer to the history entry
 */
void CF_ResetHistory(CF_Channel_t *chan, CF_History_t *history);

/************************************************************************/
/** @brief Frees and resets a transaction and returns it for later use.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL.
 *
 * @param txn Pointer to the transaction object
 * @param chan The channel number which this transaction is associated with
 */
void CF_FreeTransaction(CF_Transaction_t *txn, U8 chan);

/************************************************************************/
/** @brief Finds an active transaction by sequence number.
 *
 * @par Description
 *       This function traverses the active rx, pending, txa, and txw
 *       transaction and looks for the requested transaction.
 *
 * @par Assumptions, External Events, and Notes:
 *       chan must not be NULL.
 *
 * @param chan Pointer to the CF channel
 * @param transaction_sequence_number  Sequence number to find
 * @param src_eid                      Entity ID associated with sequence number
 *
 * @returns Pointer to the given transaction if found
 * @retval  NULL if the transaction is not found
 */
CF_Transaction_t *CF_FindTransactionBySequenceNumber(CF_Channel_t *      chan,
                                                     CF_TransactionSeq_t transaction_sequence_number,
                                                     CF_EntityId_t       src_eid);

/************************************************************************/
/** @brief List traversal function to check if the desired sequence number matches.
 *
 * @par Assumptions, External Events, and Notes:
 *       context must not be NULL. node must not be NULL.
 *
 * @param node         Pointer to node currently being traversed
 * @param context   Pointer to state object passed through from initial call
 *
 * @retval 1 when it's found, which terminates list traversal
 * @retval 0 when it isn't found, which causes list traversal to continue
 *
 */
CF_CListTraverse_Status_t CF_FindTransactionBySequenceNumber_Impl(CF_CListNode_t *node, void *context);

/************************************************************************/
/** @brief Insert a transaction into a priority sorted transaction queue.
 *
 * @par Description
 *       This function works by walking the queue in reverse to find a
 *       transaction with a higher priority than the given transaction.
 *       The given transaction is then inserted after that one, since it
 *       would be the next lower priority.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL.
 *
 * @param txn  Pointer to the transaction object
 * @param queue  Index of queue to insert into
 */
void CF_InsertSortPrio(CF_Transaction_t *txn, CF_QueueIdx_t queue);

/************************************************************************/
/** @brief Traverses all transactions on all active queues and performs an operation on them.
 *
 * @par Assumptions, External Events, and Notes:
 *       chan must not be NULL. fn must be a valid function. context must not be NULL.
 *
 * @param chan       Channel to operate on
 * @param fn      Callback to invoke for all traversed transactions
 * @param context Opaque object to pass to all callbacks
 *
 * @returns Number of transactions traversed
 */
I32 CF_TraverseAllTransactions(CF_Channel_t *chan, CF_TraverseAllTransactions_fn_t fn, void *context);

/************************************************************************/
/** @brief Traverses all transactions on all channels and performs an operation on them.
 *
 * @par Assumptions, External Events, and Notes:
 *       fn must be a valid function. context must not be NULL.
 *
 * @param fn      Callback to invoke for all traversed transactions
 * @param context Opaque object to pass to all callbacks
 *
 * @returns Number of transactions traversed
 */
I32 CF_TraverseAllTransactions_All_Channels(CF_TraverseAllTransactions_fn_t fn, void *context);

/************************************************************************/
/** @brief List traversal function performs operation on every active transaction.
 *
 * @par Description
 *       Called on every transaction via list traversal. Calls another function
 *       on that transaction.
 *
 * @par Assumptions, External Events, and Notes:
 *       node must not be NULL. args must not be NULL.
 *
 * @param node  Node being currently traversed
 * @param arg   Intermediate context object from initial call
 *
 * @retval 0 for do not exit early (always continue)
 */
CF_CListTraverse_Status_t CF_TraverseAllTransactions_Impl(CF_CListNode_t *node, void *arg);

/************************************************************************/
/** @brief Searches for the first transaction with a lower priority than given.
 *
 * @par Assumptions, External Events, and Notes:
 *       node must not be NULL. context must not be NULL.
 *
 * @param node    Node being currently traversed
 * @param context Pointer to CF_Traverse_PriorityArg_t object indicating the priority to search for
 *
 * @retval CF_CLIST_EXIT when it's found, which terminates list traversal
 * @retval CF_CLIST_CONT when it isn't found, which causes list traversal to continue
 *
 */
CF_CListTraverse_Status_t CF_PrioSearch(CF_CListNode_t *node, void *context);

/************************************************************************/
/** @brief Converts the internal transaction status to a CFDP condition code
 *
 * Transaction status is a superset of condition codes, and includes
 * other error conditions for which CFDP will not send FIN/ACK/EOF
 * and thus there is no corresponding condition code.
 *
 * @par Assumptions, External Events, and Notes:
 *        Not all transaction status codes directly correlate to a CFDP CC
 *
 * @param txn_stat   Transaction status
 *
 * @returns CFDP protocol condition code
 */
CF_CFDP_ConditionCode_t CF_TxnStatus_To_ConditionCode(CF_TxnStatus_t txn_stat);

/************************************************************************/
/** @brief Converts a CFDP condition code to an internal transaction status
 *
 * @par Assumptions, External Events, and Notes:
 *       None
 *
 * @param cc   CFDP condition code
 *
 * @returns Transaction status code
 */
CF_TxnStatus_t CF_TxnStatus_From_ConditionCode(CF_CFDP_ConditionCode_t cc);

/************************************************************************/
/** @brief Check if the internal transaction status represents an error
 *
 * @par Assumptions, External Events, and Notes:
 *       Transaction status is a superset of condition codes, and includes
 *       other error conditions for which CFDP will not send FIN/ACK/EOF
 *       and thus there is no corresponding condition code.
 *
 * @param txn_stat   Transaction status
 *
 * @returns Boolean value indicating if the transaction is in an errorred state
 * @retval true if an error has occurred during the transaction
 * @retval false if no error has occurred during the transaction yet
 */
bool CF_TxnStatus_IsError(CF_TxnStatus_t txn_stat);

/************************************************************************/
/** @brief Gets the associated channel struct from a transaction
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be null, and the chan_num must be set
 *
 * @param txn   Transaction
 *
 * @returns Pointer to CF_Channel_t struct associated with the transaction
 * @retval NULL if checks failed
 */
CF_Channel_t *CF_GetChannelFromTxn(CF_Transaction_t *txn);

/************************************************************************/
/** @brief Gets the head of the chunk list for the given channel + direction
 *
 * The chunk list contains structs that are available for tracking the chunks
 * associated with files in transit.  An entry needs to be pulled from this
 * list for every transaction, and returned to this list when the transaction
 * completes.
 *
 * @param chan       Pointer to channel struct
 * @param direction  Whether this is TX or RX
 *
 * @returns Pointer to list head
 */
CF_CListNode_t **CF_GetChunkListHead(CF_Channel_t *chan, U8 direction);

/************************************************************************/
/** @brief Gets the status of this transaction
 *
 * Determines if the transaction is ACTIVE or TERMINATED.
 * (By definition if it has a txn object then it is not UNRECOGNIZED)
 *
 * @param txn   Transaction
 * @returns CF_CFDP_AckTxnStatus_t value corresponding to transaction
 */
CF_CFDP_AckTxnStatus_t CF_CFDP_GetTxnStatus(CF_Transaction_t *txn);

}  // namespace Ccsds
}  // namespace Svc

#endif /* !CF_UTILS_HPP */

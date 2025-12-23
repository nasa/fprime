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
 *  The CF Application general utility functions source file
 *
 *  Various odds and ends are put here.
 */

#include "cf_cfdp.hpp"
#include "cf_utils.hpp"
#include "cf_clist.hpp"

namespace Svc {
namespace Ccsds {
    
/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_utils.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CF_Channel_t *CF_GetChannelFromTxn(CF_Transaction_t *txn)
{
    CF_Channel_t *chan;

    if (txn->chan_num < CF_NUM_CHANNELS)
    {
        chan = &cfdpEngine.channels[txn->chan_num];
    }
    else
    {
        chan = NULL;
    }

    return chan;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_utils.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CF_CListNode_t **CF_GetChunkListHead(CF_Channel_t *chan, U8 direction)
{
    CF_CListNode_t **result;

    if (chan != NULL && direction < CF_Direction_NUM)
    {
        result = &chan->cs[direction];
    }
    else
    {
        result = NULL;
    }

    return result;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_utils.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CF_CFDP_AckTxnStatus_t CF_CFDP_GetTxnStatus(CF_Transaction_t *txn)
{
    CF_CFDP_AckTxnStatus_t LocalStatus;

    /* check if this is still an active Tx (not in holdover or drop etc) */
    /* in theory this should never be called on S1 because there is no fin-ack to send,
     * but including it for completeness (because it is an active txn) */
    if (txn == NULL)
    {
        LocalStatus = CF_CFDP_AckTxnStatus_UNRECOGNIZED;
    }
    else
        switch (txn->state)
        {
            case CF_TxnState_S1:
            case CF_TxnState_R1:
            case CF_TxnState_S2:
            case CF_TxnState_R2:
                LocalStatus = CF_CFDP_AckTxnStatus_ACTIVE;
                break;

            case CF_TxnState_DROP:
            case CF_TxnState_HOLD:
                LocalStatus = CF_CFDP_AckTxnStatus_TERMINATED;
                break;

            default:
                LocalStatus = CF_CFDP_AckTxnStatus_INVALID;
                break;
        }

    return LocalStatus;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_utils.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CF_Transaction_t *CF_FindUnusedTransaction(CF_Channel_t *chan, CF_Direction_t direction)
{
    CF_CListNode_t *  node;
    CF_Transaction_t *txn;
    CF_QueueIdx_t q_index; /* initialized below in if */

    FW_ASSERT(chan);

    if (chan->qs[CF_QueueIdx_FREE])
    {
        node = chan->qs[CF_QueueIdx_FREE];
        txn = container_of_cpp(node, &CF_Transaction_t::cl_node);

        CF_CList_Remove_Ex(chan, CF_QueueIdx_FREE, &txn->cl_node);

        /* now that a transaction is acquired, must also acquire a history slot to go along with it */
        if (chan->qs[CF_QueueIdx_HIST_FREE])
        {
            q_index = CF_QueueIdx_HIST_FREE;
        }
        else
        {
            /* no free history, so take the oldest one from the channel's history queue */
            FW_ASSERT(chan->qs[CF_QueueIdx_HIST]);
            q_index = CF_QueueIdx_HIST;
        }

        txn->history = container_of_cpp(chan->qs[q_index], &CF_History_t::cl_node);

        CF_CList_Remove_Ex(chan, q_index, &txn->history->cl_node);

        /* Indicate that this was freshly pulled from the free list */
        /* notably this state is distinguishable from items still on the free list */
        txn->state        = CF_TxnState_INIT;
        txn->history->dir = direction;
    }
    else
    {
        txn = NULL;
    }

    return txn;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_utils.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_ResetHistory(CF_Channel_t *chan, CF_History_t *history)
{
    CF_CList_Remove_Ex(chan, CF_QueueIdx_HIST, &history->cl_node);
    CF_CList_InsertBack_Ex(chan, CF_QueueIdx_HIST_FREE, &history->cl_node);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_utils.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_FreeTransaction(CF_Transaction_t *txn, U8 chan)
{
    memset(txn, 0, sizeof(*txn));
    txn->chan_num = chan;
    CF_CList_InitNode(&txn->cl_node);
    CF_CList_InsertBack_Ex(&cfdpEngine.channels[chan], CF_QueueIdx_FREE, &txn->cl_node);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_utils.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CfdpStatus::T CF_FindTransactionBySequenceNumber_Impl(CF_CListNode_t *node, CF_Traverse_TransSeqArg_t *context)
{
    CF_Transaction_t *txn = container_of_cpp(node, &CF_Transaction_t::cl_node);
    CfdpStatus::T      ret = CfdpStatus::T::CFDP_SUCCESS;

    if ((txn->history->src_eid == context->src_eid) && (txn->history->seq_num == context->transaction_sequence_number))
    {
        context->txn = txn;
        ret          = CfdpStatus::T::CFDP_ERROR; /* exit early */
    }

    return ret;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_utils.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CF_Transaction_t *CF_FindTransactionBySequenceNumber(CF_Channel_t *      chan,
                                                     CF_TransactionSeq_t transaction_sequence_number,
                                                     CF_EntityId_t       src_eid)
{
    /* need to find transaction by sequence number. It will either be the active transaction (front of Q_PEND),
     * or on Q_TX or Q_RX. Once a transaction moves to history, then it's done.
     *
     * Let's put CF_QueueIdx_RX up front, because most RX packets will be file data PDUs */
    CF_Traverse_TransSeqArg_t ctx    = {transaction_sequence_number, src_eid, NULL};
    CF_CListNode_t *          ptrs[] = {chan->qs[CF_QueueIdx_RX], chan->qs[CF_QueueIdx_PEND], chan->qs[CF_QueueIdx_TXA],
                              chan->qs[CF_QueueIdx_TXW]};
    int                       i;
    CF_Transaction_t *        ret = NULL;

    for (i = 0; i < (sizeof(ptrs) / sizeof(ptrs[0])); ++i)
    {
        CF_CList_Traverse(ptrs[i], (CF_CListFn_t)CF_FindTransactionBySequenceNumber_Impl, &ctx);
        if (ctx.txn)
        {
            ret = ctx.txn;
            break;
        }
    }

    return ret;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_utils.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CF_CListTraverse_Status_t CF_PrioSearch(CF_CListNode_t *node, void *context)
{
    CF_Transaction_t *         txn = container_of_cpp(node, &CF_Transaction_t::cl_node);
    CF_Traverse_PriorityArg_t *arg = (CF_Traverse_PriorityArg_t *)context;

    if (txn->priority <= arg->priority)
    {
        /* found it!
         *
         * the current transaction's prio is less than desired (higher)
         */
        arg->txn = txn;
        return CF_CLIST_EXIT;
    }

    return CF_CLIST_CONT;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_utils.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_InsertSortPrio(CF_Transaction_t *txn, CF_QueueIdx_t queue)
{
    bool          insert_back = false;
    CF_Channel_t *chan        = &cfdpEngine.channels[txn->chan_num];

    FW_ASSERT(txn->chan_num < CF_NUM_CHANNELS, txn->chan_num, CF_NUM_CHANNELS);

    /* look for proper position on PEND queue for this transaction.
     * This is a simple priority sort. */

    if (!chan->qs[queue])
    {
        /* list is empty, so just insert */
        insert_back = true;
    }
    else
    {
        CF_Traverse_PriorityArg_t arg = {NULL, txn->priority};
        CF_CList_Traverse_R(chan->qs[queue], CF_PrioSearch, &arg);
        if (arg.txn)
        {
            CF_CList_InsertAfter_Ex(chan, queue, &arg.txn->cl_node, &txn->cl_node);
        }
        else
        {
            insert_back = true;
        }
    }

    if (insert_back)
    {
        CF_CList_InsertBack_Ex(chan, queue, &txn->cl_node);
    }
    txn->flags.com.q_index = queue;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_utils.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CF_CListTraverse_Status_t CF_TraverseAllTransactions_Impl(CF_CListNode_t *node, void *arg)
{
    CF_TraverseAll_Arg_t *traverse_all = arg;
    CF_Transaction_t *    txn          = container_of_cpp(node, &CF_Transaction_t::cl_node);
    traverse_all->fn(txn, traverse_all->context);
    ++traverse_all->counter;
    return CF_CLIST_CONT;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_utils.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
I32 CF_TraverseAllTransactions(CF_Channel_t *chan, CF_TraverseAllTransactions_fn_t fn, void *context)
{
    CF_TraverseAll_Arg_t args = {fn, context, 0};
    CF_QueueIdx_t        queueidx;
    for (queueidx = CF_QueueIdx_PEND; queueidx <= CF_QueueIdx_RX; ++queueidx)
        CF_CList_Traverse(chan->qs[queueidx], CF_TraverseAllTransactions_Impl, &args);

    return args.counter;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_utils.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
I32 CF_TraverseAllTransactions_All_Channels(CF_TraverseAllTransactions_fn_t fn, void *context)
{
    int   i;
    I32 ret = 0;
    for (i = 0; i < CF_NUM_CHANNELS; ++i)
        ret += CF_TraverseAllTransactions(cfdpEngine.channels + i, fn, context);
    return ret;
}

/*----------------------------------------------------------------
 *
 * Function: CF_TxnStatus_IsError
 *
 * Application-scope internal function
 * See description in cf_utils.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
bool CF_TxnStatus_IsError(CF_TxnStatus_t txn_stat)
{
    /* The value of CF_TxnStatus_UNDEFINED (-1) indicates a transaction is in progress and no error
     * has occurred yet.  This will be set to CF_TxnStatus_NO_ERROR (0) after successful completion
     * of the transaction (FIN/EOF).  Anything else indicates a problem has occurred. */
    return (txn_stat > CF_TxnStatus_NO_ERROR);
}

/*----------------------------------------------------------------
 *
 * Function: CF_TxnStatus_To_ConditionCode
 *
 * Application-scope internal function
 * See description in cf_utils.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CF_CFDP_ConditionCode_t CF_TxnStatus_To_ConditionCode(CF_TxnStatus_t txn_stat)
{
    CF_CFDP_ConditionCode_t result;

    if (!CF_TxnStatus_IsError(txn_stat))
    {
        /* If no status has been set (CF_TxnStatus_UNDEFINED), treat that as NO_ERROR for
         * the purpose of CFDP CC.  This can occur e.g. when sending ACK PDUs and no errors
         * have happened yet, but the transaction is not yet complete and thus not final. */
        result = CF_CFDP_ConditionCode_NO_ERROR;
    }
    else
    {
        switch (txn_stat)
        {
            /* The definition of CF_TxnStatus_t is such that the 4-bit codes (0-15) share the same
             * numeric values as the CFDP condition codes, and can be put directly into the 4-bit
             * CC field of a FIN/ACK/EOF PDU.  Extended codes use the upper bits (>15) to differentiate */
            case CF_TxnStatus_NO_ERROR:
            case CF_TxnStatus_POS_ACK_LIMIT_REACHED:
            case CF_TxnStatus_KEEP_ALIVE_LIMIT_REACHED:
            case CF_TxnStatus_INVALID_TRANSMISSION_MODE:
            case CF_TxnStatus_FILESTORE_REJECTION:
            case CF_TxnStatus_FILE_CHECKSUM_FAILURE:
            case CF_TxnStatus_FILE_SIZE_ERROR:
            case CF_TxnStatus_NAK_LIMIT_REACHED:
            case CF_TxnStatus_INACTIVITY_DETECTED:
            case CF_TxnStatus_INVALID_FILE_STRUCTURE:
            case CF_TxnStatus_CHECK_LIMIT_REACHED:
            case CF_TxnStatus_UNSUPPORTED_CHECKSUM_TYPE:
            case CF_TxnStatus_SUSPEND_REQUEST_RECEIVED:
            case CF_TxnStatus_CANCEL_REQUEST_RECEIVED:
                result = (CF_CFDP_ConditionCode_t)txn_stat;
                break;

                /* Extended status codes below here ---
                 * There are no CFDP CCs to directly represent these status codes. Normally this should
                 * not happen as the engine should not be sending a CFDP CC (FIN/ACK/EOF PDU) for a
                 * transaction that is not in a valid CFDP-defined state.  This should be translated
                 * to the closest CFDP CC per the intent/meaning of the transaction status code. */

            case CF_TxnStatus_ACK_LIMIT_NO_FIN:
            case CF_TxnStatus_ACK_LIMIT_NO_EOF:
                /* this is similar to the inactivity timeout (no fin-ack) */
                result = CF_CFDP_ConditionCode_INACTIVITY_DETECTED;
                break;

            default:
                /* Catch-all: any invalid protocol state will cancel the transaction, and thus this
                 * is the closest CFDP CC in practice for all other unhandled errors. */
                result = CF_CFDP_ConditionCode_CANCEL_REQUEST_RECEIVED;
                break;
        }
    }

    return result;
}

/*----------------------------------------------------------------
 *
 * Function: CF_TxnStatus_From_ConditionCode
 *
 * Application-scope internal function
 * See description in cf_utils.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CF_TxnStatus_t CF_TxnStatus_From_ConditionCode(CF_CFDP_ConditionCode_t cc)
{
    /* All CFDP CC values directly correspond to a Transaction Status of the same numeric value */
    return (CF_TxnStatus_t)cc;
}

}  // namespace Ccsds
}  // namespace Svc
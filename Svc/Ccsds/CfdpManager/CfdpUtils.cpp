// ======================================================================
// \title  CfdpUtils.cpp
// \brief  CFDP utility functions
//
// This file is a port of the cf_utils.c file from the 
// NASA Core Flight System (cFS) CFDP (CF) Application,
// version 3.0.0, adapted for use within the F-Prime (F') framework.
//
// The CFDP general utility functions source file
// 
// Various odds and ends are put here.
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

#include <Svc/Ccsds/CfdpManager/CfdpEngine.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpUtils.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpClist.hpp>

namespace Svc {
namespace Ccsds {

CF_CFDP_AckTxnStatus_t CF_CFDP_GetTxnStatus(CfdpTransaction *txn)
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
        switch (txn->getState())
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

// Static member function - can access private members
CF_CListTraverse_Status_t CfdpTransaction::findBySequenceNumberCallback(CF_CListNode_t *node, void *context)
{
    CfdpTransaction *txn = container_of_cpp(node, &CfdpTransaction::m_cl_node);
    CF_CListTraverse_Status_t ret = CF_CListTraverse_Status_CONTINUE;
    CF_Traverse_TransSeqArg_t* seqContext = static_cast<CF_Traverse_TransSeqArg_t*>(context);

    if (txn->m_history && (txn->m_history->src_eid == seqContext->src_eid) &&
        (txn->m_history->seq_num == seqContext->transaction_sequence_number))
    {
        seqContext->txn = txn;
        ret = CF_CListTraverse_Status_EXIT; /* exit early */
    }

    return ret;
}

// Static member function - can access private members
CF_CListTraverse_Status_t CfdpTransaction::prioritySearchCallback(CF_CListNode_t *node, void *context)
{
    CfdpTransaction *         txn = container_of_cpp(node, &CfdpTransaction::m_cl_node);
    CF_Traverse_PriorityArg_t *arg = static_cast<CF_Traverse_PriorityArg_t *>(context);

    if (txn->m_priority <= arg->priority)
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

// Legacy wrappers for backward compatibility
CF_CListTraverse_Status_t CF_FindTransactionBySequenceNumber_Impl(CF_CListNode_t *node, void *context)
{
    return CfdpTransaction::findBySequenceNumberCallback(node, context);
}

CF_CListTraverse_Status_t CF_PrioSearch(CF_CListNode_t *node, void *context)
{
    return CfdpTransaction::prioritySearchCallback(node, context);
}

bool CF_TxnStatus_IsError(CF_TxnStatus_t txn_stat)
{
    /* The value of CF_TxnStatus_UNDEFINED (-1) indicates a transaction is in progress and no error
     * has occurred yet.  This will be set to CF_TxnStatus_NO_ERROR (0) after successful completion
     * of the transaction (FIN/EOF).  Anything else indicates a problem has occurred. */
    return (txn_stat > CF_TxnStatus_NO_ERROR);
}

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
                result = static_cast<CF_CFDP_ConditionCode_t>(txn_stat);
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

}  // namespace Ccsds
}  // namespace Svc
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

CfdpAckTxnStatus CfdpGetTxnStatus(CfdpTransaction *txn)
{
    CfdpAckTxnStatus LocalStatus;

    /* check if this is still an active Tx (not in holdover or drop etc) */
    /* in theory this should never be called on S1 because there is no fin-ack to send,
     * but including it for completeness (because it is an active txn) */
    if (txn == NULL)
    {
        LocalStatus = CFDP_ACK_TXN_STATUS_UNRECOGNIZED;
    }
    else
        switch (txn->getState())
        {
            case CFDP_TXN_STATE_S1:
            case CFDP_TXN_STATE_R1:
            case CFDP_TXN_STATE_S2:
            case CFDP_TXN_STATE_R2:
                LocalStatus = CFDP_ACK_TXN_STATUS_ACTIVE;
                break;

            case CFDP_TXN_STATE_DROP:
            case CFDP_TXN_STATE_HOLD:
                LocalStatus = CFDP_ACK_TXN_STATUS_TERMINATED;
                break;

            default:
                LocalStatus = CFDP_ACK_TXN_STATUS_INVALID;
                break;
        }

    return LocalStatus;
}

// Static member function - can access private members
CfdpCListTraverseStatus CfdpTransaction::findBySequenceNumberCallback(CfdpCListNode *node, void *context)
{
    CfdpTransaction *txn = container_of_cpp(node, &CfdpTransaction::m_cl_node);
    CfdpCListTraverseStatus ret = CFDP_CLIST_TRAVERSE_CONTINUE;
    CfdpTraverseTransSeqArg* seqContext = static_cast<CfdpTraverseTransSeqArg*>(context);

    if (txn->m_history && (txn->m_history->src_eid == seqContext->src_eid) &&
        (txn->m_history->seq_num == seqContext->transaction_sequence_number))
    {
        seqContext->txn = txn;
        ret = CFDP_CLIST_TRAVERSE_EXIT; /* exit early */
    }

    return ret;
}

// Static member function - can access private members
CfdpCListTraverseStatus CfdpTransaction::prioritySearchCallback(CfdpCListNode *node, void *context)
{
    CfdpTransaction *         txn = container_of_cpp(node, &CfdpTransaction::m_cl_node);
    CfdpTraversePriorityArg *arg = static_cast<CfdpTraversePriorityArg *>(context);

    if (txn->m_priority <= arg->priority)
    {
        /* found it!
         *
         * the current transaction's prio is less than desired (higher)
         */
        arg->txn = txn;
        return CFDP_CLIST_TRAVERSE_EXIT;
    }

    return CFDP_CLIST_TRAVERSE_CONTINUE;
}

// Legacy wrappers for backward compatibility
CfdpCListTraverseStatus CfdpFindTransactionBySequenceNumberImpl(CfdpCListNode *node, void *context)
{
    return CfdpTransaction::findBySequenceNumberCallback(node, context);
}

CfdpCListTraverseStatus CfdpPrioSearch(CfdpCListNode *node, void *context)
{
    return CfdpTransaction::prioritySearchCallback(node, context);
}

bool CfdpTxnStatusIsError(CfdpTxnStatus txn_stat)
{
    /* The value of CFDP_TXN_STATUS_UNDEFINED (-1) indicates a transaction is in progress and no error
     * has occurred yet.  This will be set to CFDP_TXN_STATUS_NO_ERROR (0) after successful completion
     * of the transaction (FIN/EOF).  Anything else indicates a problem has occurred. */
    return (txn_stat > CFDP_TXN_STATUS_NO_ERROR);
}

CfdpConditionCode CfdpTxnStatusToConditionCode(CfdpTxnStatus txn_stat)
{
    CfdpConditionCode result;

    if (!CfdpTxnStatusIsError(txn_stat))
    {
        /* If no status has been set (CFDP_TXN_STATUS_UNDEFINED), treat that as NO_ERROR for
         * the purpose of CFDP CC.  This can occur e.g. when sending ACK PDUs and no errors
         * have happened yet, but the transaction is not yet complete and thus not final. */
        result = CFDP_CONDITION_CODE_NO_ERROR;
    }
    else
    {
        switch (txn_stat)
        {
            /* The definition of CfdpTxnStatus is such that the 4-bit codes (0-15) share the same
             * numeric values as the CFDP condition codes, and can be put directly into the 4-bit
             * CC field of a FIN/ACK/EOF PDU.  Extended codes use the upper bits (>15) to differentiate */
            case CFDP_TXN_STATUS_NO_ERROR:
            case CFDP_TXN_STATUS_POS_ACK_LIMIT_REACHED:
            case CFDP_TXN_STATUS_KEEP_ALIVE_LIMIT_REACHED:
            case CFDP_TXN_STATUS_INVALID_TRANSMISSION_MODE:
            case CFDP_TXN_STATUS_FILESTORE_REJECTION:
            case CFDP_TXN_STATUS_FILE_CHECKSUM_FAILURE:
            case CFDP_TXN_STATUS_FILE_SIZE_ERROR:
            case CFDP_TXN_STATUS_NAK_LIMIT_REACHED:
            case CFDP_TXN_STATUS_INACTIVITY_DETECTED:
            case CFDP_TXN_STATUS_INVALID_FILE_STRUCTURE:
            case CFDP_TXN_STATUS_CHECK_LIMIT_REACHED:
            case CFDP_TXN_STATUS_UNSUPPORTED_CHECKSUM_TYPE:
            case CFDP_TXN_STATUS_SUSPEND_REQUEST_RECEIVED:
            case CFDP_TXN_STATUS_CANCEL_REQUEST_RECEIVED:
                result = static_cast<CfdpConditionCode>(txn_stat);
                break;

                /* Extended status codes below here ---
                 * There are no CFDP CCs to directly represent these status codes. Normally this should
                 * not happen as the engine should not be sending a CFDP CC (FIN/ACK/EOF PDU) for a
                 * transaction that is not in a valid CFDP-defined state.  This should be translated
                 * to the closest CFDP CC per the intent/meaning of the transaction status code. */

            case CFDP_TXN_STATUS_ACK_LIMIT_NO_FIN:
            case CFDP_TXN_STATUS_ACK_LIMIT_NO_EOF:
                /* this is similar to the inactivity timeout (no fin-ack) */
                result = CFDP_CONDITION_CODE_INACTIVITY_DETECTED;
                break;

            default:
                /* Catch-all: any invalid protocol state will cancel the transaction, and thus this
                 * is the closest CFDP CC in practice for all other unhandled errors. */
                result = CFDP_CONDITION_CODE_CANCEL_REQUEST_RECEIVED;
                break;
        }
    }

    return result;
}

}  // namespace Ccsds
}  // namespace Svc
// ======================================================================
// \title  CfdpUtils.hpp
// \brief  CFDP utilities header
//
// This file is a port of CFDP utility functions from the following files
// from the NASA Core Flight System (cFS) CFDP (CF) Application, version 3.0.0,
// adapted for use within the F-Prime (F') framework:
// - cf_utils.h (CFDP utility function declarations)
//
// CFDP utils header file
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

#ifndef CFDP_UTILS_HPP
#define CFDP_UTILS_HPP

#include <Fw/Types/Assert.hpp>

#include <Svc/Ccsds/CfdpManager/CfdpEngine.hpp>

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
    CfdpTransactionSeq transaction_sequence_number;
    CfdpEntityId       src_eid;
    CfdpTransaction *  txn; /**< \brief output transaction pointer */
} CF_Traverse_TransSeqArg_t;

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
    CfdpTransaction *txn; /**< \brief OUT: holds value of transaction with which to call CF_CList_InsertAfter on */
    U8             priority; /**< \brief seeking this priority */
} CF_Traverse_PriorityArg_t;

/************************************************************************/
/** @brief List traversal function to check if the desired sequence number matches.
 *
 * @param node         Pointer to node currently being traversed
 * @param context   Pointer to state object passed through from initial call
 *
 * @retval 1 when it's found, which terminates list traversal
 * @retval 0 when it isn't found, which causes list traversal to continue
 */
CF_CListTraverse_Status_t CF_FindTransactionBySequenceNumber_Impl(CF_CListNode_t *node, void *context);

/************************************************************************/
/** @brief Searches for the first transaction with a lower priority than given.
 *
 * @param node    Node being currently traversed
 * @param context Pointer to CF_Traverse_PriorityArg_t object indicating the priority to search for
 *
 * @retval CF_CLIST_EXIT when it's found, which terminates list traversal
 * @retval CF_CLIST_CONT when it isn't found, which causes list traversal to continue
 */
CF_CListTraverse_Status_t CF_PrioSearch(CF_CListNode_t *node, void *context);

/************************************************************************/
/** @brief Converts the internal transaction status to a CFDP condition code
 *
 * Transaction status is a superset of condition codes, and includes
 * other error conditions for which CFDP will not send FIN/ACK/EOF
 * and thus there is no corresponding condition code.
 *
 * @param txn_stat   Transaction status
 *
 * @returns CFDP protocol condition code
 */
CF_CFDP_ConditionCode_t CF_TxnStatus_To_ConditionCode(CF_TxnStatus_t txn_stat);

/************************************************************************/
/** @brief Check if the internal transaction status represents an error
 *
 * Transaction status is a superset of condition codes, and includes
 * other error conditions for which CFDP will not send FIN/ACK/EOF
 * and thus there is no corresponding condition code.
 *
 * @param txn_stat   Transaction status
 *
 * @returns Boolean value indicating if the transaction is in an errorred state
 * @retval true if an error has occurred during the transaction
 * @retval false if no error has occurred during the transaction yet
 */
bool CF_TxnStatus_IsError(CF_TxnStatus_t txn_stat);

/************************************************************************/
/** @brief Gets the status of this transaction
 *
 * Determines if the transaction is ACTIVE or TERMINATED.
 * (By definition if it has a txn object then it is not UNRECOGNIZED)
 *
 * @param txn   Transaction
 * @returns CF_CFDP_AckTxnStatus_t value corresponding to transaction
 */
CF_CFDP_AckTxnStatus_t CF_CFDP_GetTxnStatus(CfdpTransaction *txn);

}  // namespace Ccsds
}  // namespace Svc

#endif /* !CFDP_UTILS_HPP */

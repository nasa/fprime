// ======================================================================
// \title  Utils.hpp
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

#include <Svc/Ccsds/CfdpManager/Engine.hpp>

namespace Svc {
namespace Ccsds {
namespace Cfdp {

/**
 * @brief Argument structure for use with CList_Traverse()
 *
 * This identifies a specific transaction sequence number and entity ID
 * The transaction pointer is set by the implementation
 */
struct CfdpTraverseTransSeqArg {
    TransactionSeq transaction_sequence_number;
    EntityId src_eid;
    Transaction* txn; /**< \brief output transaction pointer */
};

/**
 * @brief Argument structure for use with CfdpCListTraverseR()
 *
 * This is for searching for transactions of a specific priority
 */
struct CfdpTraversePriorityArg {
    Transaction* txn; /**< \brief OUT: holds value of transaction with which to call CfdpCListInsertAfter on */
    U8 priority;      /**< \brief seeking this priority */
};

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
ConditionCode TxnStatusToConditionCode(TxnStatus txn_stat);

/************************************************************************/
/** @brief Check if the internal transaction status represents an error
 *
 * Transaction status is a superset of condition codes, and includes
 * other error conditions for which CFDP will not send FIN/ACK/EOF
 * and thus there is no corresponding condition code.
 *
 * @param txn_stat   Transaction status
 *
 * @returns Boolean value indicating if the transaction is in an errored state
 * @retval true if an error has occurred during the transaction
 * @retval false if no error has occurred during the transaction yet
 */
bool TxnStatusIsError(TxnStatus txn_stat);

/************************************************************************/
/** @brief Gets the status of this transaction
 *
 * Determines if the transaction is ACTIVE or TERMINATED.
 * (By definition if it has a txn object then it is not UNRECOGNIZED)
 *
 * @param txn   Transaction
 * @returns AckTxnStatus value corresponding to transaction
 */
AckTxnStatus GetTxnStatus(Transaction* txn);

}  // namespace Cfdp
}  // namespace Ccsds
}  // namespace Svc

#endif /* !CFDP_UTILS_HPP */

// ======================================================================
// \title  CfdpDispatch.hpp
// \brief  Common routines to dispatch operations based on a transaction state
//         and/or received PDU type.
//
// This file is a port of the cf_cfdp_dispatch.h file from the 
// NASA Core Flight System (cFS) CFDP (CF) Application,
// version 3.0.0, adapted for use within the F-Prime (F') framework.
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

#ifndef CFDP_DISPATCH_HPP
#define CFDP_DISPATCH_HPP

#include <Svc/Ccsds/CfdpManager/CfdpTypes.hpp>

namespace Svc {
namespace Ccsds {

// Forward declaration
class CfdpTransaction;

/**
 * @brief A function for dispatching actions to a handler, without existing PDU data
 *
 * This allows quick delegation to handler functions using dispatch tables.  This version is
 * used on the transmit side, where a PDU will likely be generated/sent by the handler being
 * invoked.
 *
 * @param[inout] txn  The transaction object
 */
typedef void (*CF_CFDP_StateSendFunc_t)(CfdpTransaction *txn);

/**
 * @brief A function for dispatching actions to a handler, with existing PDU data
 *
 * This allows quick delegation of PDUs to handler functions using dispatch tables.  This version is
 * used on the receive side where a PDU buffer is associated with the activity, which is then
 * interpreted by the handler being invoked.
 *
 * @param[inout] txn  The transaction object
 * @param[inout] ph The PDU buffer currently being received/processed
 */
typedef void (*CF_CFDP_StateRecvFunc_t)(CfdpTransaction *txn, CF_Logical_PduBuffer_t *ph);

/**
 * @brief A table of transmit handler functions based on transaction state
 *
 * This reflects the main dispatch table for the transmit side of a transaction.
 * Each possible state has a corresponding function pointer in the table to implement
 * the PDU transmit action(s) associated with that state.
 */
typedef struct
{
    CF_CFDP_StateSendFunc_t tx[CF_TxnState_INVALID]; /**< \brief Transmit handler function */
} CF_CFDP_TxnSendDispatchTable_t;

/**
 * @brief A table of receive handler functions based on transaction state
 *
 * This reflects the main dispatch table for the receive side of a transaction.
 * Each possible state has a corresponding function pointer in the table to implement
 * the PDU receive action(s) associated with that state.
 */
typedef struct
{
    /** \brief a separate recv handler for each possible file directive PDU in this state */
    CF_CFDP_StateRecvFunc_t rx[CF_TxnState_INVALID];
} CF_CFDP_TxnRecvDispatchTable_t;

/**
 * @brief A table of receive handler functions based on file directive code
 *
 * For PDUs identified as a "file directive" type - generally anything other
 * than file data - this provides a table to branch to a different handler
 * function depending on the value of the file directive code.
 */
typedef struct
{
    /** \brief a separate recv handler for each possible file directive PDU in this state */
    CF_CFDP_StateRecvFunc_t fdirective[CF_CFDP_FileDirective_INVALID_MAX];
} CF_CFDP_FileDirectiveDispatchTable_t;

/**
 * @brief A dispatch table for receive file transactions, receive side
 *
 * This is used for "receive file" transactions upon receipt of a directive PDU.
 * Depending on the sub-state of the transaction, a different action may be taken.
 */
typedef struct
{
    const CF_CFDP_FileDirectiveDispatchTable_t *state[CF_RxSubState_NUM_STATES];
} CF_CFDP_R_SubstateDispatchTable_t;

/**
 * @brief A dispatch table for send file transactions, receive side
 *
 * This is used for "send file" transactions upon receipt of a directive PDU.
 * Depending on the sub-state of the transaction, a different action may be taken.
 */
typedef struct
{
    const CF_CFDP_FileDirectiveDispatchTable_t *substate[CF_TxSubState_NUM_STATES];
} CF_CFDP_S_SubstateRecvDispatchTable_t;

/**
 * @brief A dispatch table for send file transactions, transmit side
 *
 * This is used for "send file" transactions to generate the next PDU to be sent.
 * Depending on the sub-state of the transaction, a different action may be taken.
 */
typedef struct
{
    CF_CFDP_StateSendFunc_t substate[CF_TxSubState_NUM_STATES];
} CF_CFDP_S_SubstateSendDispatchTable_t;

}  // namespace Ccsds
}  // namespace Svc

#endif /* CFDP_DISPATCH_HPP */

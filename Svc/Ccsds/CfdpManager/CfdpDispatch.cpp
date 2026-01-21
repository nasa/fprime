// ======================================================================
// \title  CfdpDispatch.cpp
// \brief  Common routines to dispatch operations based on a transaction state
//         and/or received PDU type.
//
// This file is a port of the cf_cfdp_dispatch.cpp file from the 
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

#include <stdio.h>
#include <string.h>

#include <Svc/Ccsds/CfdpManager/CfdpEngine.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpUtils.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpDispatch.hpp>

namespace Svc {
namespace Ccsds {

void CF_CFDP_R_DispatchRecv(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph,
                            const CF_CFDP_R_SubstateDispatchTable_t *dispatch, CF_CFDP_StateRecvFunc_t fd_fn)
{
    CF_CFDP_StateRecvFunc_t              selected_handler;
    CF_Logical_PduFileDirectiveHeader_t *fdh;
    
    FW_ASSERT(txn->state_data.receive.sub_state < CF_RxSubState_NUM_STATES,
              txn->state_data.receive.sub_state, CF_RxSubState_NUM_STATES);

    selected_handler = NULL;

    /* the CF_CFDP_R_SubstateDispatchTable_t is only used with file directive PDU */
    if (ph->pdu_header.pdu_type == 0)
    {
        fdh = &ph->fdirective;
        if (fdh->directive_code < CF_CFDP_FileDirective_INVALID_MAX)
        {
            if (dispatch->state[txn->state_data.receive.sub_state] != NULL)
            {
                selected_handler = dispatch->state[txn->state_data.receive.sub_state]->fdirective[fdh->directive_code];
            }
        }
        else
        {
            // ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.recv.spurious;
            // CFE_EVS_SendEvent(CF_CFDP_R_DC_INV_ERR_EID, CFE_EVS_EventType_ERROR,
            //                   "CF R%d(%lu:%lu): received PDU with invalid directive code %d for sub-state %d",
            //                   (txn->state == CF_TxnState_R2), (unsigned long)txn->history->src_eid,
            //                   (unsigned long)txn->history->seq_num, fdh->directive_code,
            //                   txn->state_data.receive.sub_state);
        }
    }
    else
    {
        if (!CF_TxnStatus_IsError(txn->history->txn_stat))
        {
            selected_handler = fd_fn;
        }
        else
        {
            // ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.recv.dropped;
        }
    }

    /*
     * NOTE: if no handler is selected, this will drop packets on the floor here,
     * without incrementing any counter.  This was existing behavior.
     */
    if (selected_handler != NULL)
    {
        selected_handler(txn, ph);
    }
}

void CF_CFDP_S_DispatchRecv(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph,
                            const CF_CFDP_S_SubstateRecvDispatchTable_t *dispatch)
{
    const CF_CFDP_FileDirectiveDispatchTable_t *substate_tbl;
    CF_CFDP_StateRecvFunc_t                     selected_handler;
    CF_Logical_PduFileDirectiveHeader_t *       fdh;

    FW_ASSERT(txn->state_data.send.sub_state < CF_TxSubState_NUM_STATES,
              txn->state_data.send.sub_state, CF_TxSubState_NUM_STATES);

    /* send state, so we only care about file directive PDU */
    selected_handler = NULL;
    if (ph->pdu_header.pdu_type == 0)
    {
        fdh = &ph->fdirective;
        if (fdh->directive_code < CF_CFDP_FileDirective_INVALID_MAX)
        {
            /* This should be silent (no event) if no handler is defined in the table */
            substate_tbl = dispatch->substate[txn->state_data.send.sub_state];
            if (substate_tbl != NULL)
            {
                selected_handler = substate_tbl->fdirective[fdh->directive_code];
            }
        }
        else
        {
            // ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.recv.spurious;
            // CFE_EVS_SendEvent(CF_CFDP_S_DC_INV_ERR_EID, CFE_EVS_EventType_ERROR,
            //                   "CF S%d(%lu:%lu): received PDU with invalid directive code %d for sub-state %d",
            //                   (txn->state == CF_TxnState_S2), (unsigned long)txn->history->src_eid,
            //                   (unsigned long)txn->history->seq_num, fdh->directive_code,
            //                   txn->state_data.send.sub_state);
        }
    }
    else
    {
        // CFE_EVS_SendEvent(CF_CFDP_S_NON_FD_PDU_ERR_EID, CFE_EVS_EventType_ERROR,
        //                   "CF S%d(%lu:%lu): received non-file directive PDU", (txn->state == CF_TxnState_S2),
        //                   (unsigned long)txn->history->src_eid, (unsigned long)txn->history->seq_num);
    }

    /* check that there's a valid function pointer. If there isn't,
     * then silently ignore. We may want to discuss if it's worth
     * shutting down the whole transaction if a PDU is received
     * that doesn't make sense to be received (For example,
     * class 1 CFDP receiving a NAK PDU) but for now, we silently
     * ignore the received packet and keep chugging along. */
    if (selected_handler)
    {
        selected_handler(txn, ph);
    }
}

void CF_CFDP_S_DispatchTransmit(CF_Transaction_t *txn, const CF_CFDP_S_SubstateSendDispatchTable_t *dispatch)
{
    CF_CFDP_StateSendFunc_t selected_handler;

    selected_handler = dispatch->substate[txn->state_data.send.sub_state];
    if (selected_handler != NULL)
    {
        selected_handler(txn);
    }
}

void CF_CFDP_TxStateDispatch(CF_Transaction_t *txn, const CF_CFDP_TxnSendDispatchTable_t *dispatch)
{
    CF_CFDP_StateSendFunc_t selected_handler;

    FW_ASSERT(txn->state < CF_TxnState_INVALID, txn->state, CF_TxnState_INVALID);

    selected_handler = dispatch->tx[txn->state];
    if (selected_handler != NULL)
    {
        selected_handler(txn);
    }
}

void CF_CFDP_RxStateDispatch(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph,
                             const CF_CFDP_TxnRecvDispatchTable_t *dispatch)
{
    CF_CFDP_StateRecvFunc_t selected_handler;

    FW_ASSERT(txn->state < CF_TxnState_INVALID, txn->state, CF_TxnState_INVALID);
    selected_handler = dispatch->rx[txn->state];
    if (selected_handler != NULL)
    {
        selected_handler(txn, ph);
    }
}

}  // namespace Ccsds
}  // namespace Svc
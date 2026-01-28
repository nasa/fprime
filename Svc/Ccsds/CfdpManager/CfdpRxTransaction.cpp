// ======================================================================
// \title  CfdpRxTransaction.cpp
// \brief  cpp file for CFDP RX Transaction state machine
//
// This file is a port of the cf_cfdp_r.c file from the
// NASA Core Flight System (cFS) CFDP (CF) Application,
// version 3.0.0, adapted for use within the F-Prime (F') framework.
//
// This file contains various state handling routines for
// transactions which are receiving a file.
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

#include <Svc/Ccsds/CfdpManager/CfdpTransaction.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpEngine.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpRx.hpp>

namespace Svc {
namespace Ccsds {

// ======================================================================
// Construction and Destruction
// ======================================================================

CfdpTransaction::CfdpTransaction() {
    // No state yet - methods operate on CF_Transaction_t*
}

CfdpTransaction::~CfdpTransaction() {
    // No cleanup needed yet
}

// ======================================================================
// RX State Machine - Public Methods
// ======================================================================

void CfdpTransaction::r1Recv(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph) {
    CF_CFDP_R1_Recv(txn, ph);
}

void CfdpTransaction::r2Recv(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph) {
    CF_CFDP_R2_Recv(txn, ph);
}

void CfdpTransaction::rAckTimerTick(CF_Transaction_t *txn) {
    CF_CFDP_R_AckTimerTick(txn);
}

void CfdpTransaction::rTick(CF_Transaction_t *txn, int *cont) {
    CF_CFDP_R_Tick(txn, cont);
}

void CfdpTransaction::rCancel(CF_Transaction_t *txn) {
    CF_CFDP_R_Cancel(txn);
}

void CfdpTransaction::rInit(CF_Transaction_t *txn) {
    CF_CFDP_R_Init(txn);
}

void CfdpTransaction::r2SetFinTxnStatus(CF_Transaction_t *txn, CF_TxnStatus_t txn_stat) {
    CF_CFDP_R2_SetFinTxnStatus(txn, txn_stat);
}

void CfdpTransaction::r1Reset(CF_Transaction_t *txn) {
    CF_CFDP_R1_Reset(txn);
}

void CfdpTransaction::r2Reset(CF_Transaction_t *txn) {
    CF_CFDP_R2_Reset(txn);
}

CfdpStatus::T CfdpTransaction::rCheckCrc(CF_Transaction_t *txn, U32 expected_crc) {
    return CF_CFDP_R_CheckCrc(txn, expected_crc);
}

void CfdpTransaction::r2Complete(CF_Transaction_t *txn, int ok_to_send_nak) {
    CF_CFDP_R2_Complete(txn, ok_to_send_nak);
}

// ======================================================================
// RX State Machine - Private Helper Methods
// ======================================================================

CfdpStatus::T CfdpTransaction::rProcessFd(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph) {
    return CF_CFDP_R_ProcessFd(txn, ph);
}

CfdpStatus::T CfdpTransaction::rSubstateRecvEof(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph) {
    return CF_CFDP_R_SubstateRecvEof(txn, ph);
}

void CfdpTransaction::r1SubstateRecvEof(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph) {
    CF_CFDP_R1_SubstateRecvEof(txn, ph);
}

void CfdpTransaction::r2SubstateRecvEof(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph) {
    CF_CFDP_R2_SubstateRecvEof(txn, ph);
}

void CfdpTransaction::r1SubstateRecvFileData(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph) {
    CF_CFDP_R1_SubstateRecvFileData(txn, ph);
}

void CfdpTransaction::r2SubstateRecvFileData(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph) {
    CF_CFDP_R2_SubstateRecvFileData(txn, ph);
}

void CfdpTransaction::r2GapCompute(const CF_ChunkList_t *chunks, const CF_Chunk_t *chunk, void *opaque) {
    CF_CFDP_R2_GapCompute(chunks, chunk, opaque);
}

CfdpStatus::T CfdpTransaction::rSubstateSendNak(CF_Transaction_t *txn) {
    return CF_CFDP_R_SubstateSendNak(txn);
}

CfdpStatus::T CfdpTransaction::r2CalcCrcChunk(CF_Transaction_t *txn) {
    return CF_CFDP_R2_CalcCrcChunk(txn);
}

CfdpStatus::T CfdpTransaction::r2SubstateSendFin(CF_Transaction_t *txn) {
    return CF_CFDP_R2_SubstateSendFin(txn);
}

void CfdpTransaction::r2RecvFinAck(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph) {
    CF_CFDP_R2_Recv_fin_ack(txn, ph);
}

void CfdpTransaction::r2RecvMd(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph) {
    CF_CFDP_R2_RecvMd(txn, ph);
}

void CfdpTransaction::rSendInactivityEvent(CF_Transaction_t *txn) {
    CF_CFDP_R_SendInactivityEvent(txn);
}

}  // namespace Ccsds
}  // namespace Svc
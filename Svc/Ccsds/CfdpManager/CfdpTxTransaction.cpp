// ======================================================================
// \title  CfdpTxTransaction.cpp
// \brief  cpp file for CFDP TX Transaction state machine
//
// This file is a port of the cf_cfdp_s.c file from the
// NASA Core Flight System (cFS) CFDP (CF) Application,
// version 3.0.0, adapted for use within the F-Prime (F') framework.
//
// This file contains various state handling routines for
// transactions which are sending a file.
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
#include <Svc/Ccsds/CfdpManager/CfdpTx.hpp>

namespace Svc {
namespace Ccsds {

// ======================================================================
// TX State Machine - Public Methods
// ======================================================================

void CfdpTransaction::s1Recv(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph) {
    CF_CFDP_S1_Recv(txn, ph);
}

void CfdpTransaction::s2Recv(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph) {
    CF_CFDP_S2_Recv(txn, ph);
}

void CfdpTransaction::s1Tx(CF_Transaction_t *txn) {
    CF_CFDP_S1_Tx(txn);
}

void CfdpTransaction::s2Tx(CF_Transaction_t *txn) {
    CF_CFDP_S2_Tx(txn);
}

void CfdpTransaction::sAckTimerTick(CF_Transaction_t *txn) {
    CF_CFDP_S_AckTimerTick(txn);
}

void CfdpTransaction::sTick(CF_Transaction_t *txn, int *cont) {
    CF_CFDP_S_Tick(txn, cont);
}

void CfdpTransaction::sTickNak(CF_Transaction_t *txn, int *cont) {
    CF_CFDP_S_Tick_Nak(txn, cont);
}

void CfdpTransaction::sCancel(CF_Transaction_t *txn) {
    CF_CFDP_S_Cancel(txn);
}

// ======================================================================
// TX State Machine - Private Helper Methods
// ======================================================================

CfdpStatus::T CfdpTransaction::sSendEof(CF_Transaction_t *txn) {
    return CF_CFDP_S_SendEof(txn);
}

void CfdpTransaction::s1SubstateSendEof(CF_Transaction_t *txn) {
    CF_CFDP_S1_SubstateSendEof(txn);
}

void CfdpTransaction::s2SubstateSendEof(CF_Transaction_t *txn) {
    CF_CFDP_S2_SubstateSendEof(txn);
}

CfdpStatus::T CfdpTransaction::sSendFileData(CF_Transaction_t *txn, U32 foffs, U32 bytes_to_read, U8 calc_crc) {
    return CF_CFDP_S_SendFileData(txn, foffs, bytes_to_read, calc_crc);
}

void CfdpTransaction::sSubstateSendFileData(CF_Transaction_t *txn) {
    CF_CFDP_S_SubstateSendFileData(txn);
}

CfdpStatus::T CfdpTransaction::sCheckAndRespondNak(CF_Transaction_t *txn, bool* nakProcessed) {
    return CF_CFDP_S_CheckAndRespondNak(txn, nakProcessed);
}

void CfdpTransaction::s2SubstateSendFileData(CF_Transaction_t *txn) {
    CF_CFDP_S2_SubstateSendFileData(txn);
}

void CfdpTransaction::sSubstateSendMetadata(CF_Transaction_t *txn) {
    CF_CFDP_S_SubstateSendMetadata(txn);
}

CfdpStatus::T CfdpTransaction::sSendFinAck(CF_Transaction_t *txn) {
    return CF_CFDP_S_SendFinAck(txn);
}

void CfdpTransaction::s2EarlyFin(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph) {
    CF_CFDP_S2_EarlyFin(txn, ph);
}

void CfdpTransaction::s2Fin(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph) {
    CF_CFDP_S2_Fin(txn, ph);
}

void CfdpTransaction::s2Nak(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph) {
    CF_CFDP_S2_Nak(txn, ph);
}

void CfdpTransaction::s2NakArm(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph) {
    CF_CFDP_S2_Nak_Arm(txn, ph);
}

void CfdpTransaction::s2EofAck(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph) {
    CF_CFDP_S2_EofAck(txn, ph);
}

}  // namespace Ccsds
}  // namespace Svc
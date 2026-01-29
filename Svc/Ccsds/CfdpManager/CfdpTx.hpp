// ======================================================================
// \title  CfdpTx.hpp
// \brief  Implementation related to CFDP Send File transactions
//
// This file is a port of the cf_cfdp_s.h file from the 
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

#ifndef CFDP_TX_HPP
#define CFDP_TX_HPP

#include <Svc/Ccsds/CfdpManager/CfdpTypes.hpp>

namespace Svc {
namespace Ccsds {

/************************************************************************/
/** @brief S1 receive PDU processing.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL.
 *
 * @param txn  Pointer to the transaction object
 * @param ph Pointer to the PDU information
 */
void CF_CFDP_S1_Recv(CfdpTransaction *txn, CF_Logical_PduBuffer_t *ph);

/************************************************************************/
/** @brief S2 receive PDU processing.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL.
 *
 * @param txn  Pointer to the transaction object
 * @param ph Pointer to the PDU information
 */
void CF_CFDP_S2_Recv(CfdpTransaction *txn, CF_Logical_PduBuffer_t *ph);

/************************************************************************/
/** @brief S1 dispatch function.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL.
 *
 * @param txn  Pointer to the transaction object
 */
void CF_CFDP_S1_Tx(CfdpTransaction *txn);

/************************************************************************/
/** @brief S2 dispatch function.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL.
 *
 * @param txn  Pointer to the transaction object
 */
void CF_CFDP_S2_Tx(CfdpTransaction *txn);

/************************************************************************/
/** @brief Perform acknowledgement timer tick (time-based) processing for S transactions.
 *
 * @par Description
 *       This is invoked as part of overall timer tick processing if the transaction
 *       has some sort of acknowledgement pending from the remote.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL
 *
 * @param txn  Pointer to the transaction object
 *
 */
void CF_CFDP_S_AckTimerTick(CfdpTransaction *txn);

/************************************************************************/
/** @brief Perform tick (time-based) processing for S transactions.
 *
 * @par Description
 *       This function is called on every transaction by the engine on
 *       every CF wakeup. This is where flags are checked to send EOF or
 *       FIN-ACK. If nothing else is sent, it checks to see if a NAK
 *       retransmit must occur.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL. cont is unused, so may be NULL
 *
 * @param txn  Pointer to the transaction object
 * @param cont Unused, exists for compatibility with tick processor
 */
void CF_CFDP_S_Tick(CfdpTransaction *txn, int *cont);

/************************************************************************/
/** @brief Perform NAK response for TX transactions
 *
 * @par Description
 *       This function is called at tick processing time to send pending
 *       NAK responses. It indicates "cont" is 1 if there are more responses
 *       left to send.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL. cont must not be NULL.
 *
 * @param txn  Pointer to the transaction object
 * @param cont Set to 1 if a NAK was generated
 */
void CF_CFDP_S_Tick_Nak(CfdpTransaction *txn, int *cont);

/************************************************************************/
/** @brief Cancel an S transaction.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL.
 *
 * @param txn  Pointer to the transaction object
 */
void CF_CFDP_S_Cancel(CfdpTransaction *txn);

/***********************************************************************
 *
 * Handler routines for send-file transactions
 * These are not called from outside this module, but are declared here so they can be unit tested
 *
 ************************************************************************/

/************************************************************************/
/** @brief Send an EOF PDU.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL.
 *
 * @retval CfdpStatus::SUCCESS on success.
 * @retval CfdpStatus::SEND_PDU_NO_BUF_AVAIL_ERROR if message buffer cannot be obtained.
 * @retval SEND_PDU_ERROR if an error occurred while building the packet.
 *
 * @param txn  Pointer to the transaction object
 */
CfdpStatus::T CF_CFDP_S_SendEof(CfdpTransaction *txn);

/************************************************************************/
/** @brief Sends an EOF for S1.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL.
 *
 * @param txn  Pointer to the transaction object
 */
void CF_CFDP_S1_SubstateSendEof(CfdpTransaction *txn);

/************************************************************************/
/** @brief Triggers tick processing to send an EOF and wait for EOF-ACK for S2
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL.
 *
 * @param txn  Pointer to the transaction object
 */
void CF_CFDP_S2_SubstateSendEof(CfdpTransaction *txn);

/************************************************************************/
/** @brief Helper function to populate the PDU with file data and send it.
 *
 * @par Description
 *       This function checks the file offset cache and if the desired
 *       location is where the file offset is, it can skip a seek() call.
 *       The file is read into the filedata PDU and then the PDU is sent.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL.
 *
 * @returns The number of bytes sent in the file data PDU (CfdpStatus::SUCCESS,
 *          i.e. 0, if no bytes were processed), or CfdpStatus::ERROR on error
 *
 * @param txn     Pointer to the transaction object
 * @param foffs Position in file to send data from
 * @param bytes_to_read Number of bytes to send (maximum)
 * @param calc_crc Enable CRC/Checksum calculation
 *
 */
CfdpStatus::T CF_CFDP_S_SendFileData(CfdpTransaction *txn, U32 foffs, U32 bytes_to_read, U8 calc_crc);

/************************************************************************/
/** @brief Standard state function to send the next file data PDU for active transaction.
 *
 * @par Description
 *       During the transfer of active transaction file data PDUs, the file
 *       offset is saved. This function sends the next chunk of data. If
 *       the file offset equals the file size, then transition to the EOF
 *       state.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL.
 *
 * @param txn     Pointer to the transaction object
 */
void CF_CFDP_S_SubstateSendFileData(CfdpTransaction *txn);

/************************************************************************/
/** @brief Respond to a NAK by sending filedata PDUs as response.
 *
 * @par Description
 *       Checks to see if a metadata PDU or filedata re-transmits must
 *       occur.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL.
 *
 * @returns ERROR if error otherwise SUCCESS
 *
 * @param txn          Pointer to the transaction object
 * @param nakProcessed true if a NAK was processed, otherwise false
 */
CfdpStatus::T CF_CFDP_S_CheckAndRespondNak(CfdpTransaction *txn, bool* nakProcessed);

/************************************************************************/
/** @brief Send filedata handling for S2.
 *
 * @par Description
 *       S2 will either respond to a NAK by sending retransmits, or in
 *       absence of a NAK, it will send more of the original file data.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL.
 *
 * @param txn     Pointer to the transaction object
 */
void CF_CFDP_S2_SubstateSendFileData(CfdpTransaction *txn);

/************************************************************************/
/** @brief Send metadata PDU.
 *
 * @par Description
 *       Construct and send a metadata PDU. This function determines the
 *       size of the file to put in the metadata PDU.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL.
 *
 * @param txn     Pointer to the transaction object
 */
void CF_CFDP_S_SubstateSendMetadata(CfdpTransaction *txn);

/************************************************************************/
/** @brief Send FIN-ACK packet for S2.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL.
 *
 * @param txn     Pointer to the transaction object
 */
CfdpStatus::T CF_CFDP_S_SendFinAck(CfdpTransaction *txn);

/************************************************************************/
/** @brief A FIN was received before file complete, so abandon the transaction.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL. ph must not be NULL.
 *
 * @param txn  Pointer to the transaction object
 * @param ph Pointer to the PDU information
 */
void CF_CFDP_S2_EarlyFin(CfdpTransaction *txn, CF_Logical_PduBuffer_t *ph);

/************************************************************************/
/** @brief S2 received FIN, so set flag to send FIN-ACK.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL. ph must not be NULL.
 *
 * @param txn  Pointer to the transaction object
 * @param ph Pointer to the PDU information
 */
void CF_CFDP_S2_Fin(CfdpTransaction *txn, CF_Logical_PduBuffer_t *ph);

/************************************************************************/
/** @brief S2 NAK PDU received handling.
 *
 * @par Description
 *       Stores the segment requests from the NAK packet in the chunks
 *       structure. These can be used to generate re-transmit filedata
 *       PDUs.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL. ph must not be NULL.
 *
 * @param txn  Pointer to the transaction object
 * @param ph Pointer to the PDU information
 */
void CF_CFDP_S2_Nak(CfdpTransaction *txn, CF_Logical_PduBuffer_t *ph);

/************************************************************************/
/** @brief S2 NAK handling but with arming the NAK timer.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL. ph must not be NULL.
 *
 * @param txn  Pointer to the transaction object
 * @param ph Pointer to the PDU information
 */
void CF_CFDP_S2_Nak_Arm(CfdpTransaction *txn, CF_Logical_PduBuffer_t *ph);

/************************************************************************/
/** @brief S2 received ACK PDU.
 *
 * @par Description
 *       Handles reception of an ACK PDU
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL. ph must not be NULL.
 *
 * @param txn  Pointer to the transaction object
 * @param ph Pointer to the PDU information
 */
void CF_CFDP_S2_EofAck(CfdpTransaction *txn, CF_Logical_PduBuffer_t *ph);

}  // namespace Ccsds
}  // namespace Svc

#endif /* !CF_CFDP_S_H */

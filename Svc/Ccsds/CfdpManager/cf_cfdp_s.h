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
 * Implementation related to CFDP Send File transactions
 *
 * This file contains various state handling routines for
 * transactions which are sending a file.
 */

#ifndef CF_CFDP_S_H
#define CF_CFDP_S_H

#include "cf_cfdp_types.h"

/************************************************************************/
/** @brief S1 receive PDU processing.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL.
 *
 * @param txn  Pointer to the transaction object
 * @param ph Pointer to the PDU information
 */
void CF_CFDP_S1_Recv(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);

/************************************************************************/
/** @brief S2 receive PDU processing.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL.
 *
 * @param txn  Pointer to the transaction object
 * @param ph Pointer to the PDU information
 */
void CF_CFDP_S2_Recv(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);

/************************************************************************/
/** @brief S1 dispatch function.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL.
 *
 * @param txn  Pointer to the transaction object
 */
void CF_CFDP_S1_Tx(CF_Transaction_t *txn);

/************************************************************************/
/** @brief S2 dispatch function.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL.
 *
 * @param txn  Pointer to the transaction object
 */
void CF_CFDP_S2_Tx(CF_Transaction_t *txn);

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
void CF_CFDP_S_AckTimerTick(CF_Transaction_t *txn);

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
void CF_CFDP_S_Tick(CF_Transaction_t *txn, int *cont);

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
void CF_CFDP_S_Tick_Nak(CF_Transaction_t *txn, int *cont);

/************************************************************************/
/** @brief Cancel an S transaction.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL.
 *
 * @param txn  Pointer to the transaction object
 */
void CF_CFDP_S_Cancel(CF_Transaction_t *txn);

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
 * @retval CFE_SUCCESS on success.
 * @retval CF_SEND_PDU_NO_BUF_AVAIL_ERROR if message buffer cannot be obtained.
 * @retval CF_SEND_PDU_ERROR if an error occurred while building the packet.
 *
 * @param txn  Pointer to the transaction object
 */
CFE_Status_t CF_CFDP_S_SendEof(CF_Transaction_t *txn);

/************************************************************************/
/** @brief Sends an EOF for S1.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL.
 *
 * @param txn  Pointer to the transaction object
 */
void CF_CFDP_S1_SubstateSendEof(CF_Transaction_t *txn);

/************************************************************************/
/** @brief Triggers tick processing to send an EOF and wait for EOF-ACK for S2
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL.
 *
 * @param txn  Pointer to the transaction object
 */
void CF_CFDP_S2_SubstateSendEof(CF_Transaction_t *txn);

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
 * @returns The number of bytes sent in the file data PDU (CFE_SUCCESS,
 *          i.e. 0, if no bytes were processed), or CF_ERROR on error
 *
 * @param txn     Pointer to the transaction object
 * @param foffs Position in file to send data from
 * @param bytes_to_read Number of bytes to send (maximum)
 * @param calc_crc Enable CRC/Checksum calculation
 *
 */
CFE_Status_t CF_CFDP_S_SendFileData(CF_Transaction_t *txn, uint32 foffs, uint32 bytes_to_read, uint8 calc_crc);

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
void CF_CFDP_S_SubstateSendFileData(CF_Transaction_t *txn);

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
 * @returns CF_ERROR if error.
 * @retval 0 if no NAK processed.
 * @retval 1 if NAK processed.
 *
 * @param txn     Pointer to the transaction object
 */
CFE_Status_t CF_CFDP_S_CheckAndRespondNak(CF_Transaction_t *txn);

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
void CF_CFDP_S2_SubstateSendFileData(CF_Transaction_t *txn);

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
void CF_CFDP_S_SubstateSendMetadata(CF_Transaction_t *txn);

/************************************************************************/
/** @brief Send FIN-ACK packet for S2.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL.
 *
 * @param txn     Pointer to the transaction object
 */
CFE_Status_t CF_CFDP_S_SendFinAck(CF_Transaction_t *txn);

/************************************************************************/
/** @brief A FIN was received before file complete, so abandon the transaction.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL. ph must not be NULL.
 *
 * @param txn  Pointer to the transaction object
 * @param ph Pointer to the PDU information
 */
void CF_CFDP_S2_EarlyFin(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);

/************************************************************************/
/** @brief S2 received FIN, so set flag to send FIN-ACK.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL. ph must not be NULL.
 *
 * @param txn  Pointer to the transaction object
 * @param ph Pointer to the PDU information
 */
void CF_CFDP_S2_Fin(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);

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
void CF_CFDP_S2_Nak(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);

/************************************************************************/
/** @brief S2 NAK handling but with arming the NAK timer.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL. ph must not be NULL.
 *
 * @param txn  Pointer to the transaction object
 * @param ph Pointer to the PDU information
 */
void CF_CFDP_S2_Nak_Arm(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);

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
void CF_CFDP_S2_EofAck(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);

#endif /* !CF_CFDP_S_H */

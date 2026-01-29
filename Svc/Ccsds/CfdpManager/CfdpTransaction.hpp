// ======================================================================
// \title  CfdpTransaction.hpp
// \brief  CFDP Transaction state machine class for TX and RX operations
//
// This file is a port of the cf_cfdp_r.h and cf_cfdp_s.h files from the
// NASA Core Flight System (cFS) CFDP (CF) Application,
// version 3.0.0, adapted for use within the F-Prime (F') framework.
//
// This file contains the unified interface for CFDP transaction state
// machines, encompassing both TX (send) and RX (receive) operations.
// The implementation is split across CfdpTxTransaction.cpp and
// CfdpRxTransaction.cpp for maintainability.
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

#ifndef Svc_Ccsds_CfdpTransaction_HPP
#define Svc_Ccsds_CfdpTransaction_HPP

#include <Svc/Ccsds/CfdpManager/CfdpEngine.hpp>

namespace Svc {
namespace Ccsds {

/**
 * @brief CFDP Transaction state machine class
 *
 * This class provides TX and RX state machine operations for CFDP transactions.
 * Currently, it operates on CF_Transaction_t structures passed as parameters.
 * In a future refactor, CF_Transaction_t members will be migrated to become
 * class members of CfdpTransaction.
 *
 * Implementation is split across multiple files for maintainability:
 * - CfdpTxTransaction.cpp: TX (send) state machine implementation
 * - CfdpRxTransaction.cpp: RX (receive) state machine implementation
 */
class CfdpTransaction {
  public:
    // ----------------------------------------------------------------------
    // Construction and Destruction
    // ----------------------------------------------------------------------

    CfdpTransaction();
    ~CfdpTransaction();

    // ----------------------------------------------------------------------
    // TX State Machine - Implemented in CfdpTxTransaction.cpp
    // ----------------------------------------------------------------------

    /************************************************************************/
    /** @brief S1 receive PDU processing.
     *
     * @par Assumptions, External Events, and Notes:
     *       txn must not be NULL.
     *
     * @param txn  Pointer to the transaction object
     * @param ph Pointer to the PDU information
     */
    void s1Recv(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);

    /************************************************************************/
    /** @brief S2 receive PDU processing.
     *
     * @par Assumptions, External Events, and Notes:
     *       txn must not be NULL.
     *
     * @param txn  Pointer to the transaction object
     * @param ph Pointer to the PDU information
     */
    void s2Recv(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);

    /************************************************************************/
    /** @brief S1 dispatch function.
     *
     * @par Assumptions, External Events, and Notes:
     *       txn must not be NULL.
     *
     * @param txn  Pointer to the transaction object
     */
    void s1Tx(CF_Transaction_t *txn);

    /************************************************************************/
    /** @brief S2 dispatch function.
     *
     * @par Assumptions, External Events, and Notes:
     *       txn must not be NULL.
     *
     * @param txn  Pointer to the transaction object
     */
    void s2Tx(CF_Transaction_t *txn);

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
    void sAckTimerTick(CF_Transaction_t *txn);

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
    void sTick(CF_Transaction_t *txn, int *cont);

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
    void sTickNak(CF_Transaction_t *txn, int *cont);

    /************************************************************************/
    /** @brief Cancel an S transaction.
     *
     * @par Assumptions, External Events, and Notes:
     *       txn must not be NULL.
     *
     * @param txn  Pointer to the transaction object
     */
    void sCancel(CF_Transaction_t *txn);

  private:
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
    CfdpStatus::T sSendEof(CF_Transaction_t *txn);

    /************************************************************************/
    /** @brief Sends an EOF for S1.
     *
     * @par Assumptions, External Events, and Notes:
     *       txn must not be NULL.
     *
     * @param txn  Pointer to the transaction object
     */
    void s1SubstateSendEof(CF_Transaction_t *txn);

    /************************************************************************/
    /** @brief Triggers tick processing to send an EOF and wait for EOF-ACK for S2
     *
     * @par Assumptions, External Events, and Notes:
     *       txn must not be NULL.
     *
     * @param txn  Pointer to the transaction object
     */
    void s2SubstateSendEof(CF_Transaction_t *txn);

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
     * @param bytes_processed Output: actual bytes sent
     *
     */
    CfdpStatus::T sSendFileData(CF_Transaction_t *txn, U32 foffs, U32 bytes_to_read, U8 calc_crc, U32* bytes_processed);

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
    void sSubstateSendFileData(CF_Transaction_t *txn);

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
    CfdpStatus::T sCheckAndRespondNak(CF_Transaction_t *txn, bool* nakProcessed);

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
    void s2SubstateSendFileData(CF_Transaction_t *txn);

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
    void sSubstateSendMetadata(CF_Transaction_t *txn);

    /************************************************************************/
    /** @brief Send FIN-ACK packet for S2.
     *
     * @par Assumptions, External Events, and Notes:
     *       txn must not be NULL.
     *
     * @param txn     Pointer to the transaction object
     */
    CfdpStatus::T sSendFinAck(CF_Transaction_t *txn);

    /************************************************************************/
    /** @brief A FIN was received before file complete, so abandon the transaction.
     *
     * @par Assumptions, External Events, and Notes:
     *       txn must not be NULL. ph must not be NULL.
     *
     * @param txn  Pointer to the transaction object
     * @param ph Pointer to the PDU information
     */
    void s2EarlyFin(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);

    /************************************************************************/
    /** @brief S2 received FIN, so set flag to send FIN-ACK.
     *
     * @par Assumptions, External Events, and Notes:
     *       txn must not be NULL. ph must not be NULL.
     *
     * @param txn  Pointer to the transaction object
     * @param ph Pointer to the PDU information
     */
    void s2Fin(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);

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
    void s2Nak(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);

    /************************************************************************/
    /** @brief S2 NAK handling but with arming the NAK timer.
     *
     * @par Assumptions, External Events, and Notes:
     *       txn must not be NULL. ph must not be NULL.
     *
     * @param txn  Pointer to the transaction object
     * @param ph Pointer to the PDU information
     */
    void s2NakArm(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);

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
    void s2EofAck(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);

  public:
    // ----------------------------------------------------------------------
    // RX State Machine - Implemented in CfdpRxTransaction.cpp
    // ----------------------------------------------------------------------

    /************************************************************************/
    /** @brief R1 receive PDU processing.
     *
     * @par Assumptions, External Events, and Notes:
     *       txn must not be NULL.
     *
     * @param txn  Pointer to the transaction object
     * @param ph Pointer to the PDU information
     */
    void r1Recv(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);

    /************************************************************************/
    /** @brief R2 receive PDU processing.
     *
     * @par Assumptions, External Events, and Notes:
     *       txn must not be NULL.
     *
     * @param txn  Pointer to the transaction object
     * @param ph Pointer to the PDU information
     */
    void r2Recv(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);

    /************************************************************************/
    /** @brief Perform acknowledgement timer tick (time-based) processing for R transactions.
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
    void rAckTimerTick(CF_Transaction_t *txn);

    /************************************************************************/
    /** @brief Perform tick (time-based) processing for R transactions.
     *
     * @par Description
     *       This function is called on every transaction by the engine on
     *       every CF wakeup. This is where flags are checked to send ACK,
     *       NAK, and FIN. It checks for inactivity timer and processes the
     *       ACK timer. The ACK timer is what triggers re-sends of PDUs
     *       that require acknowledgment.
     *
     * @par Assumptions, External Events, and Notes:
     *       txn must not be NULL. cont is unused, so may be NULL
     *
     * @param txn  Pointer to the transaction object
     * @param cont Ignored/Unused
     *
     */
    void rTick(CF_Transaction_t *txn, int *cont);

    /************************************************************************/
    /** @brief Cancel an R transaction.
     *
     * @par Assumptions, External Events, and Notes:
     *       txn must not be NULL.
     *
     * @param txn  Pointer to the transaction object
     */
    void rCancel(CF_Transaction_t *txn);

    /************************************************************************/
    /** @brief Initialize a transaction structure for R.
     *
     * @par Assumptions, External Events, and Notes:
     *       txn must not be NULL.
     *
     * @param txn  Pointer to the transaction object
     */
    void rInit(CF_Transaction_t *txn);

    /************************************************************************/
    /** @brief Helper function to store transaction status code and set send_fin flag.
     *
     * @par Assumptions, External Events, and Notes:
     *       txn must not be NULL.
     *
     * @param txn  Pointer to the transaction object
     * @param txn_stat Status Code value to set within transaction
     */
    void r2SetFinTxnStatus(CF_Transaction_t *txn, CF_TxnStatus_t txn_stat);

    /************************************************************************/
    /** @brief CFDP R1 transaction reset function.
     *
     * @par Description
     *       All R transactions use this call to indicate the transaction
     *       state can be returned to the system. While this function currently
     *       only calls CF_CFDP_ResetTransaction(), it is here as a placeholder.
     *
     * @par Assumptions, External Events, and Notes:
     *       txn must not be NULL.
     *
     * @param txn  Pointer to the transaction object
     */
    void r1Reset(CF_Transaction_t *txn);

    /************************************************************************/
    /** @brief CFDP R2 transaction reset function.
     *
     * @par Description
     *       Handles reset logic for R2, then calls R1 reset logic.
     *
     * @par Assumptions, External Events, and Notes:
     *       txn must not be NULL.
     *
     * @param txn  Pointer to the transaction object
     */
    void r2Reset(CF_Transaction_t *txn);

    /************************************************************************/
    /** @brief Checks that the transaction file's CRC matches expected.
     *
     * @par Assumptions, External Events, and Notes:
     *       txn must not be NULL.
     *
     *
     * @retval CfdpStatus::SUCCESS on CRC match, otherwise CfdpStatus::CFDP_ERROR.
     *
     *
     * @param txn            Pointer to the transaction object
     * @param expected_crc Expected CRC
     */
    CfdpStatus::T rCheckCrc(CF_Transaction_t *txn, U32 expected_crc);

    /************************************************************************/
    /** @brief Checks R2 transaction state for transaction completion status.
     *
     * @par Description
     *       This function is called anywhere there's a desire to know if the
     *       transaction has completed. It may trigger other actions by setting
     *       flags to be handled during tick processing. In order for a
     *       transaction to be complete, it must have had its meta-data PDU
     *       received, the EOF must have been received, and there must be
     *       no gaps in the file. EOF is not checked in this function, because
     *       it's only called from functions after EOF is received.
     *
     * @par Assumptions, External Events, and Notes:
     *       txn must not be NULL.
     *
     * @param txn  Pointer to the transaction object
     * @param ok_to_send_nak If set to 0, suppress sending of a NAK packet
     */
    void r2Complete(CF_Transaction_t *txn, int ok_to_send_nak);

  private:
    /************************************************************************/
    /** @brief Process a filedata PDU on a transaction.
     *
     * @par Assumptions, External Events, and Notes:
     *       txn must not be NULL.
     *
     *
     * @retval CfdpStatus::SUCCESS on success. CfdpStatus::CFDP_ERROR on error.
     *
     *
     * @param txn  Pointer to the transaction object
     * @param ph Pointer to the PDU information
     */
    CfdpStatus::T rProcessFd(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);

    /************************************************************************/
    /** @brief Processing receive EOF common functionality for R1/R2.
     *
     * @par Description
     *       This function is used for both R1 and R2 EOF receive. It calls
     *       the unmarshaling function and then checks known transaction
     *       data against the PDU.
     *
     * @par Assumptions, External Events, and Notes:
     *       txn must not be NULL. ph must not be NULL.
     *
     *
     * @retval CfdpStatus::SUCCESS on success. Returns anything else on error.
     *
     *
     * @param txn  Pointer to the transaction object
     * @param ph Pointer to the PDU information
     */
    CfdpStatus::T rSubstateRecvEof(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);

    /************************************************************************/
    /** @brief Process receive EOF for R1.
     *
     * @par Description
     *       Only need to confirm CRC for R1.
     *
     * @par Assumptions, External Events, and Notes:
     *       txn must not be NULL. ph must not be NULL.
     *
     * @param txn  Pointer to the transaction object
     * @param ph Pointer to the PDU information
     *
     */
    void r1SubstateRecvEof(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);

    /************************************************************************/
    /** @brief Process receive EOF for R2.
     *
     * @par Description
     *       For R2, need to trigger the send of EOF-ACK and then call the
     *       check complete function which will either send NAK or FIN.
     *
     * @par Assumptions, External Events, and Notes:
     *       txn must not be NULL. ph must not be NULL.
     *
     * @param txn  Pointer to the transaction object
     * @param ph Pointer to the PDU information
     *
     */
    void r2SubstateRecvEof(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);

    /************************************************************************/
    /** @brief Process received file data for R1.
     *
     * @par Description
     *       For R1, only need to digest the CRC.
     *
     * @par Assumptions, External Events, and Notes:
     *       txn must not be NULL. ph must not be NULL.
     *
     * @param txn  Pointer to the transaction object
     * @param ph Pointer to the PDU information
     */
    void r1SubstateRecvFileData(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);

    /************************************************************************/
    /** @brief Process received file data for R2.
     *
     * @par Description
     *       For R2, the CRC is checked after the whole file is received
     *       since there may be gaps. Instead, insert file received range
     *       data into chunks. Once NAK has been received, this function
     *       always checks for completion. This function also re-arms
     *       the ACK timer.
     *
     * @par Assumptions, External Events, and Notes:
     *       txn must not be NULL. ph must not be NULL.
     *
     * @param txn  Pointer to the transaction object
     * @param ph Pointer to the PDU information
     */
    void r2SubstateRecvFileData(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);

    /************************************************************************/
    /** @brief Loads a single NAK segment request.
     *
     * @par Description
     *       This is a function callback from CF_ChunkList_ComputeGaps().
     *
     * @par Assumptions, External Events, and Notes:
     *       chunks must not be NULL, chunk must not be NULL, opaque must not be NULL.
     *
     * @param chunks Not used, required for compatibility with CF_ChunkList_ComputeGaps
     * @param chunk      Pointer to a single chunk information
     * @param opaque Pointer to a CF_GapComputeArgs_t object (passed via CF_ChunkList_ComputeGaps)
     */
    void r2GapCompute(const CF_ChunkList_t *chunks, const CF_Chunk_t *chunk, void *opaque);

    /************************************************************************/
    /** @brief Send a NAK PDU for R2.
     *
     * @par Description
     *       NAK PDU is sent when there are gaps in the received data. The
     *       chunks class tracks this and generates the NAK PDU by calculating
     *       gaps internally and calling r2GapCompute(). There is a special
     *       case where if a metadata PDU has not been received, then a NAK
     *       packet will be sent to request another.
     *
     * @par Assumptions, External Events, and Notes:
     *       txn must not be NULL.
     *
     * @retval CfdpStatus::SUCCESS on success. CfdpStatus::CFDP_ERROR on error.
     *
     * @param txn  Pointer to the transaction object
     */
    CfdpStatus::T rSubstateSendNak(CF_Transaction_t *txn);

    /************************************************************************/
    /** @brief Calculate up to the configured amount of bytes of CRC.
     *
     * @par Description
     *       The configuration table has a number of bytes to calculate per
     *       transaction per wakeup. At each wakeup, the file is read and
     *       this number of bytes are calculated. This function will set
     *       the checksum error condition code if the final CRC does not match.
     *
     * @par PTFO
     *       Increase throughput by consuming all CRC bytes per wakeup in
     *       transaction-order. This would require a change to the meaning
     *       of the value in the configuration table.
     *
     * @par Assumptions, External Events, and Notes:
     *       txn must not be NULL.
     *
     * @retval CfdpStatus::SUCCESS on completion.
     * @retval CfdpStatus::CFDP_ERROR on non-completion.
     *
     */
    CfdpStatus::T r2CalcCrcChunk(CF_Transaction_t *txn);

    /************************************************************************/
    /** @brief Send a FIN PDU.
     *
     * @par Assumptions, External Events, and Notes:
     *       txn must not be NULL.
     *
     * @retval CfdpStatus::SUCCESS on success. CfdpStatus::CFDP_ERROR on error.
     *
     * @param txn  Pointer to the transaction object
     *
     */
    CfdpStatus::T r2SubstateSendFin(CF_Transaction_t *txn);

    /************************************************************************/
    /** @brief Process receive FIN-ACK PDU.
     *
     * @par Description
     *       This is the end of an R2 transaction. Simply reset the transaction
     *       state.
     *
     * @par Assumptions, External Events, and Notes:
     *       txn must not be NULL. ph must not be NULL.
     *
     * @param txn  Pointer to the transaction object
     * @param ph Pointer to the PDU information
     */
    void r2RecvFinAck(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);

    /************************************************************************/
    /** @brief Process receive metadata PDU for R2.
     *
     * @par Description
     *       It's possible that metadata PDU was missed in cf_cfdp.c, or that
     *       it was re-sent. This function checks if it was already processed,
     *       and if not, handles it. If there was a temp file opened due to
     *       missed metadata PDU, it will move the file to the correct
     *       destination according to the metadata PDU.
     *
     * @par Assumptions, External Events, and Notes:
     *       txn must not be NULL. ph must not be NULL.
     *
     * @param txn  Pointer to the transaction object
     * @param ph Pointer to the PDU information
     */
    void r2RecvMd(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);

    /************************************************************************/
    /** @brief Sends an inactivity timer expired event to EVS.
     *
     * @par Assumptions, External Events, and Notes:
     *       txn must not be NULL.
     *
     * @param txn  Pointer to the transaction object
     */
    void rSendInactivityEvent(CF_Transaction_t *txn);

  private:
    // ----------------------------------------------------------------------
    // Member Variables
    // ----------------------------------------------------------------------

    // Future: CF_Transaction_t members will be moved here in a later refactor
};

}  // namespace Ccsds
}  // namespace Svc

#endif  // Svc_Ccsds_CfdpTransaction_HPP
// ======================================================================
// \title  CfdpEngine.hpp
// \brief  CFDP Engine header
//
// This file is a port of the cf_cfdp.h file from the
// NASA Core Flight System (cFS) CFDP (CF) Application,
// version 3.0.0, adapted for use within the F-Prime (F') framework.
//
// CFDP engine and packet parsing header file
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

#ifndef CFDP_ENGINE_HPP
#define CFDP_ENGINE_HPP

#include <Fw/FPrimeBasicTypes.hpp>

#include <Svc/Ccsds/CfdpManager/CfdpTypes.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpTransaction.hpp>

// Forward declarations - do NOT include CfdpManager.hpp to avoid circular dependency
namespace Svc {
namespace Ccsds {
    class CfdpManager;
    class CfdpChannel;
}
}

namespace Svc {
namespace Ccsds {

/**
 * @brief Structure for use with the CfdpChannel::cycleTx() function
 */
typedef struct CF_CFDP_CycleTx_args
{
    CfdpChannel *chan;    /**< \brief channel object */
    int          ran_one; /**< \brief should be set to 1 if a transaction was cycled */
} CF_CFDP_CycleTx_args_t;

/**
 * @brief Structure for use with the CfdpChannel::doTick() function
 */
typedef struct CF_CFDP_Tick_args
{
    CfdpChannel *chan;                                /**< \brief channel object */
    void (CfdpTransaction::*fn)(int *);               /**< \brief member function pointer */
    bool early_exit;                                  /**< \brief early exit result */
    int  cont;                                        /**< \brief if 1, then re-traverse the list */
} CF_CFDP_Tick_args_t;

//
// Codec functions - these remain as free functions for now
// TODO: These will be replaced by Pdu classes in a future refactoring
//

/**
 * @brief CFDP Protocol Engine
 *
 * Manages the CFDP protocol engine lifecycle, transactions, and operations.
 * This class owns all CFDP protocol state (formerly global) and provides
 * a clean interface to CfdpManager.
 *
 * Key design points:
 * - Owns engine data
 * - Has access to CfdpManager's protected logging methods via manager_ pointer
 * - Private methods encapsulate all internal CFDP protocol logic
 */
class CfdpEngine {
  public:
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

    /**
     * @brief Construct a new CfdpEngine object
     *
     * @param manager Pointer to parent CfdpManager component
     */
    explicit CfdpEngine(CfdpManager* manager);

    /**
     * @brief Destroy the CfdpEngine object
     */
    ~CfdpEngine();

    // ----------------------------------------------------------------------
    // Public interface
    // ----------------------------------------------------------------------

    /**
     * @brief Initialize the CFDP engine
     */
    void init();

    /**
     * @brief Cycle the engine once per wakeup
     *
     * This drives all CFDP protocol processing
     */
    void cycle();

    /**
     * @brief Receive and process a PDU
     *
     * @param chan_id Channel ID receiving the PDU
     * @param pdu     Reference to decoded PDU
     */
    void receivePdu(U8 chan_id, const Cfdp::Pdu& pdu);

    /**
     * @brief Begin transmit of a file
     *
     * @param src        Local filename
     * @param dst        Remote filename
     * @param cfdp_class Whether to perform a class 1 or class 2 transfer
     * @param keep       Whether to keep or delete the local file after completion
     * @param chan_num   CF channel number to use
     * @param priority   CF priority level
     * @param dest_id    Entity ID of remote receiver
     * @returns Cfdp::Status::SUCCESS on success, error code otherwise
     */
    Cfdp::Status::T txFile(const Fw::String& src, const Fw::String& dst,
                         Cfdp::Class::T cfdp_class, Cfdp::Keep::T keep,
                         U8 chan_num, U8 priority, CfdpEntityId dest_id);

    /**
     * @brief Begin transmit of a directory
     *
     * @param src        Local directory
     * @param dst        Remote directory
     * @param cfdp_class Whether to perform a class 1 or class 2 transfer
     * @param keep       Whether to keep or delete the local file after completion
     * @param chan       CF channel number to use
     * @param priority   CF priority level
     * @param dest_id    Entity ID of remote receiver
     * @returns Cfdp::Status::SUCCESS on success, error code otherwise
     */
    Cfdp::Status::T playbackDir(const Fw::String& src, const Fw::String& dst,
                              Cfdp::Class::T cfdp_class, Cfdp::Keep::T keep,
                              U8 chan, U8 priority, CfdpEntityId dest_id);

    /**
     * @brief Start polling a directory
     *
     * @param chanId      CFDP channel number to use
     * @param pollId      Channel poll directory index to use
     * @param srcDir      Local directory
     * @param dstDir      Remote directory
     * @param cfdp_class  Whether to perform a class 1 or class 2 transfer
     * @param priority    CF priority level
     * @param destEid     Entity ID of remote receiver
     * @param intervalSec Time between directory playbacks in seconds
     * @returns Cfdp::Status::SUCCESS on success, error code otherwise
     */
    Cfdp::Status::T startPollDir(U8 chanId, U8 pollId, const Fw::String& srcDir,
                               const Fw::String& dstDir, Cfdp::Class::T cfdp_class,
                               U8 priority, CfdpEntityId destEid, U32 intervalSec);

    /**
     * @brief Stop polling a directory
     *
     * @param chanId CFDP channel number
     * @param pollId Channel poll directory index
     * @returns Cfdp::Status::SUCCESS on success, error code otherwise
     */
    Cfdp::Status::T stopPollDir(U8 chanId, U8 pollId);

    /**
     * @brief Set channel flow state
     *
     * Called by CfdpManager::SetChannelFlow_cmdHandler()
     *
     * @param channelId Channel index
     * @param flowState Flow state to set (normal or frozen)
     */
    void setChannelFlowState(U8 channelId, Cfdp::Flow::T flowState);

    // ----------------------------------------------------------------------
    // Public Transaction Interface
    // Methods used by CfdpRx/CfdpTx transaction processing
    // ----------------------------------------------------------------------

    /**
     * @brief Finish a transaction
     *
     * This marks the transaction as completed and puts it into a holdover state.
     * After the inactivity timer expires, the resources will be recycled and
     * become available for re-use.
     *
     * Holdover is necessary because even though locally we consider the transaction
     * to be complete, there may be undelivered PDUs still in network queues that
     * get delivered to us late.  By holding this transaction for a bit longer,
     * we can still associate those PDUs with this transaction/seq_num and
     * appropriately handle them as dupes/spurious deliveries.
     *
     * @par Assumptions, External Events, and Notes:
     *       txn must not be NULL.
     *
     * @param txn  Pointer to the transaction object
     * @param keep_history Whether the transaction info should be preserved in history
     */
    void finishTransaction(CfdpTransaction *txn, bool keep_history);

    /**
     * @brief Helper function to store transaction status code only
     *
     * This records the status in the history block but does not set FIN flag
     * or take any other protocol/state machine actions.
     *
     * @par Assumptions, External Events, and Notes:
     *       txn must not be NULL.
     *
     * @param txn  Pointer to the transaction object
     * @param txn_stat Status Code value to set within transaction
     */
    void setTxnStatus(CfdpTransaction *txn, CF_TxnStatus_t txn_stat);

    /**
     * @brief Arm the ACK timer for a transaction
     *
     * Sets the ACK timer duration based on the channel configuration and marks
     * the timer as armed.
     *
     * @param txn  Pointer to the transaction object
     */
    void armAckTimer(CfdpTransaction *txn);

    /**
     * @brief Arm the inactivity timer for a transaction
     *
     * Sets the inactivity timer duration based on the transaction state and
     * channel configuration.
     *
     * @param txn  Pointer to the transaction object
     */
    void armInactTimer(CfdpTransaction *txn);

    /**
     * @brief Build a metadata PDU for transmit
     *
     * @par Assumptions, External Events, and Notes:
     *       txn must not be NULL.
     *
     * @param txn  Pointer to the transaction object
     *
     * @returns Cfdp::Status::T status code
     * @retval Cfdp::Status::SUCCESS on success.
     * @retval Cfdp::Status::SEND_PDU_NO_BUF_AVAIL_ERROR if message buffer cannot be obtained.
     */
    Cfdp::Status::T sendMd(CfdpTransaction *txn);

    /**
     * @brief Send a previously-assembled filedata PDU for transmit
     *
     * @par Assumptions, External Events, and Notes:
     *       txn must not be NULL.
     *
     * @param txn  Pointer to the transaction object
     * @param ph   Pointer to logical PDU buffer content
     *
     * @note Unlike other "send" routines, the file data PDU must be acquired and
     * filled by the caller prior to invoking this routine.  This routine only
     * sends the PDU that was previously allocated and assembled.  As such, the
     * typical failure possibilities do not apply to this call.
     *
     * @returns Cfdp::Status::T status code
     * @retval Cfdp::Status::SUCCESS on success. (error checks not yet implemented)
     */
    Cfdp::Status::T sendFd(CfdpTransaction *txn, Cfdp::Pdu::FileDataPdu& fdPdu);

    /**
     * @brief Build an EOF PDU for transmit
     *
     * @par Assumptions, External Events, and Notes:
     *       txn must not be NULL.
     *
     * @param txn  Pointer to the transaction object
     *
     * @returns Cfdp::Status::T status code
     * @retval Cfdp::Status::SUCCESS on success.
     * @retval Cfdp::Status::SEND_PDU_NO_BUF_AVAIL_ERROR if message buffer cannot be obtained.
     */
    Cfdp::Status::T sendEof(CfdpTransaction *txn);

    /**
     * @brief Build an ACK PDU for transmit
     *
     * @par Assumptions, External Events, and Notes:
     *       txn must not be NULL.
     *
     * @note CF_CFDP_SendAck() takes a CfdpTransactionSeq instead of getting it from transaction history because
     * of the special case where a FIN-ACK must be sent for an unknown transaction. It's better for
     * long term maintenance to not build an incomplete CF_History_t for it.
     *
     * @param txn       Pointer to the transaction object
     * @param ts        Transaction ACK status
     * @param dir_code  File directive code being ACK'ed
     * @param cc        Condition code of transaction
     * @param peer_eid  Remote entity ID
     * @param tsn       Transaction sequence number
     *
     * @returns Cfdp::Status::T status code
     * @retval Cfdp::Status::SUCCESS on success.
     * @retval Cfdp::Status::SEND_PDU_NO_BUF_AVAIL_ERROR if message buffer cannot be obtained.
     */
    Cfdp::Status::T sendAck(CfdpTransaction *txn, Cfdp::AckTxnStatus ts, Cfdp::FileDirective dir_code,
                          Cfdp::ConditionCode cc, CfdpEntityId peer_eid, CfdpTransactionSeq tsn);

    /**
     * @brief Build a FIN PDU for transmit
     *
     * @par Assumptions, External Events, and Notes:
     *       txn must not be NULL.
     *
     * @param txn  Pointer to the transaction object
     * @param dc   Final delivery status code (complete or incomplete)
     * @param fs   Final file status (retained or rejected, etc)
     * @param cc   Final CFDP condition code
     *
     * @returns Cfdp::Status::T status code
     * @retval Cfdp::Status::SUCCESS on success.
     * @retval Cfdp::Status::SEND_PDU_NO_BUF_AVAIL_ERROR if message buffer cannot be obtained.
     */
    Cfdp::Status::T sendFin(CfdpTransaction *txn, CF_CFDP_FinDeliveryCode_t dc, CF_CFDP_FinFileStatus_t fs,
                          Cfdp::ConditionCode cc);

    /**
     * @brief Send a previously-assembled NAK PDU for transmit
     *
     * @par Assumptions, External Events, and Notes:
     *       txn must not be NULL.
     *
     * @param txn  Pointer to the transaction object
     * @param ph   Pointer to logical PDU buffer content
     *
     * @note Unlike other "send" routines, the NAK PDU must be acquired and
     * filled by the caller prior to invoking this routine.  This routine only
     * encodes and sends the previously-assembled PDU buffer.  As such, the
     * typical failure possibilities do not apply to this call.
     *
     * @returns Cfdp::Status::T status code
     * @retval Cfdp::Status::SUCCESS on success.
     * @retval Cfdp::Status::SEND_PDU_NO_BUF_AVAIL_ERROR if message buffer cannot be obtained.
     */
    Cfdp::Status::T sendNak(CfdpTransaction *txn, Cfdp::Pdu::NakPdu& nakPdu);

    /**
     * @brief Handle receipt of metadata PDU
     *
     * This should only be invoked for buffers that have been identified
     * as a metadata PDU.
     *
     * @par Assumptions, External Events, and Notes:
     *       txn must not be NULL.
     *       PDU validation already done in MetadataPdu::fromBuffer
     *
     * @param txn  Pointer to the transaction state
     * @param pdu  The metadata PDU
     */
    void recvMd(CfdpTransaction *txn, const Cfdp::Pdu& pdu);

    /**
     * @brief Unpack a file data PDU from a received message
     *
     * This should only be invoked for buffers that have been identified
     * as a file data PDU.
     *
     * @par Assumptions, External Events, and Notes:
     *       txn must not be NULL.
     *
     * @param txn  Pointer to the transaction state
     * @param ph   The logical PDU buffer being received
     *
     * @returns integer status code
     * @retval Cfdp::Status::SUCCESS on success
     * @retval Cfdp::Status::ERROR for general errors
     * @retval Cfdp::Status::SHORT_PDU_ERROR PDU too short
     */
    Cfdp::Status::T recvFd(CfdpTransaction *txn, const Cfdp::Pdu& pdu);

    /**
     * @brief Unpack an EOF PDU from a received message
     *
     * This should only be invoked for buffers that have been identified
     * as an end of file PDU.
     *
     * @par Assumptions, External Events, and Notes:
     *       txn must not be NULL.
     *
     * @param txn  Pointer to the transaction state
     * @param ph   The logical PDU buffer being received
     *
     * @returns integer status code
     * @retval Cfdp::Status::SUCCESS on success
     * @retval Cfdp::Status::SHORT_PDU_ERROR on error
     */
    Cfdp::Status::T recvEof(CfdpTransaction *txn, const Cfdp::Pdu& pdu);

    /**
     * @brief Unpack an ACK PDU from a received message
     *
     * This should only be invoked for buffers that have been identified
     * as an acknowledgment PDU.
     *
     * @par Assumptions, External Events, and Notes:
     *       txn must not be NULL.
     *
     * @param txn  Pointer to the transaction state
     * @param ph   The logical PDU buffer being received
     *
     * @returns integer status code
     * @retval Cfdp::Status::SUCCESS on success
     * @retval Cfdp::Status::SHORT_PDU_ERROR on error
     */
    Cfdp::Status::T recvAck(CfdpTransaction *txn, const Cfdp::Pdu& pdu);

    /**
     * @brief Unpack an FIN PDU from a received message
     *
     * This should only be invoked for buffers that have been identified
     * as a final PDU.
     *
     * @par Assumptions, External Events, and Notes:
     *       txn must not be NULL.
     *
     * @param txn  Pointer to the transaction state
     * @param ph   The logical PDU buffer being received
     *
     * @returns integer status code
     * @retval Cfdp::Status::SUCCESS on success
     * @retval Cfdp::Status::SHORT_PDU_ERROR on error
     */
    Cfdp::Status::T recvFin(CfdpTransaction *txn, const Cfdp::Pdu& pdu);

    /**
     * @brief Unpack a NAK PDU from a received message
     *
     * This should only be invoked for buffers that have been identified
     * as a negative/non-acknowledgment PDU.
     *
     * @par Assumptions, External Events, and Notes:
     *       txn must not be NULL.
     *
     * @param txn  Pointer to the transaction state
     * @param ph   The logical PDU buffer being received
     *
     * @returns integer status code
     * @retval Cfdp::Status::SUCCESS on success
     * @retval Cfdp::Status::SHORT_PDU_ERROR on error
     */
    Cfdp::Status::T recvNak(CfdpTransaction *txn, const Cfdp::Pdu& pdu);

    /**
     * @brief Initiate a file transfer transaction
     *
     * @param txn          Pointer to the transaction state
     * @param cfdp_class   Set to class 1 or class 2
     * @param keep         Whether to keep the local file
     * @param chan         CF channel number
     * @param priority     Priority of transfer
     * @param dest_id      Destination entity ID
     */
    void txFileInitiate(CfdpTransaction *txn, Cfdp::Class::T cfdp_class, Cfdp::Keep::T keep, U8 chan,
                        U8 priority, CfdpEntityId dest_id);

    /**
     * @brief Initiate playback of a directory
     *
     * @param pb           Playback state
     * @param src_filename Source filename
     * @param dst_filename Destination filename
     * @param cfdp_class   Set to class 1 or class 2
     * @param keep         Whether to keep the local file
     * @param chan         CF channel number
     * @param priority     Priority of transfer
     * @param dest_id      Destination entity ID
     * @returns SUCCESS if initiated, error otherwise
     */
    Cfdp::Status::T playbackDirInitiate(CF_Playback_t *pb, const Fw::String& src_filename, const Fw::String& dst_filename,
                                      Cfdp::Class::T cfdp_class, Cfdp::Keep::T keep, U8 chan, U8 priority, CfdpEntityId dest_id);

    /**
     * @brief Dispatch TX state machine for a transaction
     *
     * Called by CfdpChannel to drive the TX state machine for a transaction.
     *
     * @param txn  Pointer to the transaction state
     */
    void dispatchTx(CfdpTransaction *txn);

  private:
    // ----------------------------------------------------------------------
    // Private member variables
    // ----------------------------------------------------------------------

    //!< Parent component for event and telemetry methods
    CfdpManager* m_manager;       

    //! Channel data structures
    CfdpChannel* m_channels[CF_NUM_CHANNELS];

    //! Sequence number tracker for outgoing transactions
    CfdpTransactionSeq m_seqNum;

    // Note: Transactions, histories, and chunks are now owned by each CfdpChannel

    // ----------------------------------------------------------------------
    // Private helper methods
    // All the non-public CFDP functions converted to methods
    // ----------------------------------------------------------------------

    // Transaction Management

    /**
     * @brief Send an end of transaction packet
     *
     * @par Assumptions, External Events, and Notes:
     *       txn must not be NULL.
     *
     * @param txn  Pointer to the transaction object
     */
    void sendEotPkt(CfdpTransaction *txn);

    /**
     * @brief Cancels a transaction
     *
     * @par Assumptions, External Events, and Notes:
     *       txn must not be NULL.
     *
     * @param txn  Pointer to the transaction state
     */
    void cancelTransaction(CfdpTransaction *txn);

    /**
     * @brief Helper function to start a new RX transaction
     *
     * This sets various fields inside a newly-allocated transaction
     * structure appropriately for receiving a file.  Note that in the
     * receive direction, most fields are unknown until the MD is received,
     * and thus are left in their initial state here (generally 0).
     *
     * If there is no capacity for another RX transaction, this returns NULL.
     *
     * @param chan_num  CF channel number
     * @returns Pointer to new transaction
     */
    CfdpTransaction* startRxTransaction(U8 chan_num);

    // PDU Operations - Send

    // PDU Operations - Receive

    /**
     * @brief Receive state function to ignore a packet
     *
     * @par Description
     *       This function signature must match all receive state functions.
     *       The parameter txn is ignored here.
     *
     * @par Assumptions, External Events, and Notes:
     *       txn must not be NULL.
     *
     * @param txn  Pointer to the transaction state
     * @param ph   The logical PDU buffer being received
     */
    void recvDrop(CfdpTransaction *txn, const Cfdp::Pdu& pdu);

    /**
     * @brief Receive state function during holdover period
     *
     * @par Description
     *       This function signature must match all receive state functions.
     *       Handles any possible spurious PDUs that might come in after the
     *       transaction is considered done.  This can happen if ACKs were
     *       lost in transmission causing the sender to retransmit PDUs even
     *       though we already completed the transaction.
     *
     * @par Assumptions, External Events, and Notes:
     *       txn must not be NULL.
     *
     * @param txn  Pointer to the transaction state
     * @param ph   The logical PDU buffer being received
     */
    void recvHold(CfdpTransaction *txn, const Cfdp::Pdu& pdu);

    /**
     * @brief Receive state function to process new rx transaction
     *
     * @par Description
     *       An idle transaction has never had message processing performed on it.
     *       Typically, the first packet received for a transaction would be
     *       the metadata PDU. There's a special case for R2 where the metadata
     *       PDU could be missed, and filedata comes in instead. In that case,
     *       an R2 transaction must still be started.
     *
     * @par Assumptions, External Events, and Notes:
     *       txn must not be NULL. There must be a received message.
     *
     * @param txn  Pointer to the transaction state
     * @param ph   The logical PDU buffer being received
     */
    void recvInit(CfdpTransaction *txn, const Cfdp::Pdu& pdu);

    // Dispatch

    /**
     * @brief Dispatch received packet to its handler
     *
     * This dispatches the PDU to the appropriate handler
     * based on the transaction state
     *
     * @par Assumptions, External Events, and Notes:
     *       txn must not be null. It must be an initialized transaction.
     *
     * @param txn  Pointer to the transaction state
     * @param ph   The logical PDU buffer being received
     */
    void dispatchRecv(CfdpTransaction *txn, const Cfdp::Pdu& pdu);

    // Channel Processing

    /**
     * @brief Check if source file came from polling directory
     *
     * @par Assumptions, External Events, and Notes:
     *
     * @param src_file Source file path to check
     * @param chan_num Channel number
     *
     * @retval true/false
     */
    bool isPollingDir(const char *src_file, U8 chan_num);

    /**
     * @brief Remove/Move file after transaction
     *
     * This helper is used to handle "not keep" file option after a transaction.
     *
     * @par Assumptions, External Events, and Notes:
     *
     * @param txn  Pointer to the transaction object
     */
    void handleNotKeepFile(CfdpTransaction *txn);

    // Friend declarations for testing
    friend class CfdpManagerTester;
};

}  // namespace Ccsds
}  // namespace Svc

#endif /* !CFDP_ENGINE_HPP */
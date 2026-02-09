// ======================================================================
// \title  Transaction.hpp
// \brief  CFDP Transaction state machine class for TX and RX operations
//
// This file is a port of transaction state machine definitions from the following files
// from the NASA Core Flight System (cFS) CFDP (CF) Application, version 3.0.0,
// adapted for use within the F-Prime (F') framework:
// - cf_cfdp_r.h (receive transaction state machine definitions)
// - cf_cfdp_s.h (send transaction state machine definitions)
// - cf_cfdp_dispatch.h (transaction dispatch definitions)
//
// This file contains the unified interface for CFDP transaction state
// machines, encompassing both TX (send) and RX (receive) operations.
// The implementation is split across TransactionTx.cpp and
// TransactionRx.cpp for maintainability.
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

#include <Fw/Types/BasicTypes.hpp>

#include <Svc/Ccsds/CfdpManager/Types/Types.hpp>
#include <Svc/Ccsds/CfdpManager/Types/PduBase.hpp>

namespace Svc {
namespace Ccsds {
namespace Cfdp {

// Forward declarations
class CfdpManager;
class Engine;
class Channel;
class Transaction;

// ======================================================================
// Dispatch Table Type Definitions
// ======================================================================

/**
 * @brief A member function pointer for dispatching actions to a handler, without existing PDU data
 *
 * This allows quick delegation to handler functions using dispatch tables.  This version is
 * used on the transmit side, where a PDU will likely be generated/sent by the handler being
 * invoked.
 *
 * @note This is a member function pointer - invoke with: (txn->*fn)()
 */
using StateSendFunc = void (Transaction::*)();

/**
 * @brief A member function pointer for dispatching actions to a handler, with existing PDU data
 *
 * This allows quick delegation of PDUs to handler functions using dispatch tables.  This version is
 * used on the receive side where a PDU buffer is associated with the activity, which is then
 * interpreted by the handler being invoked.
 *
 * @param[inout] buffer The buffer containing the PDU currently being received/processed
 * @note This is a member function pointer - invoke with: (txn->*fn)(buffer)
 */
using StateRecvFunc = void (Transaction::*)(const Fw::Buffer& buffer);

/**
 * @brief A table of transmit handler functions based on transaction state
 *
 * This reflects the main dispatch table for the transmit side of a transaction.
 * Each possible state has a corresponding function pointer in the table to implement
 * the PDU transmit action(s) associated with that state.
 */
struct TxnSendDispatchTable
{
    StateSendFunc tx[TXN_STATE_INVALID]; /**< \brief Transmit handler function */
};

/**
 * @brief A table of receive handler functions based on transaction state
 *
 * This reflects the main dispatch table for the receive side of a transaction.
 * Each possible state has a corresponding function pointer in the table to implement
 * the PDU receive action(s) associated with that state.
 */
struct TxnRecvDispatchTable
{
    /** \brief a separate recv handler for each possible file directive PDU in this state */
    StateRecvFunc rx[TXN_STATE_INVALID];
};

/**
 * @brief A table of receive handler functions based on file directive code
 *
 * For PDUs identified as a "file directive" type - generally anything other
 * than file data - this provides a table to branch to a different handler
 * function depending on the value of the file directive code.
 */
struct FileDirectiveDispatchTable
{
    /** \brief a separate recv handler for each possible file directive PDU in this state */
    StateRecvFunc fdirective[FILE_DIRECTIVE_INVALID_MAX];
};

/**
 * @brief A dispatch table for receive file transactions, receive side
 *
 * This is used for "receive file" transactions upon receipt of a directive PDU.
 * Depending on the sub-state of the transaction, a different action may be taken.
 */
struct RSubstateDispatchTable
{
    const FileDirectiveDispatchTable *state[RX_SUB_STATE_NUM_STATES];
};

/**
 * @brief A dispatch table for send file transactions, receive side
 *
 * This is used for "send file" transactions upon receipt of a directive PDU.
 * Depending on the sub-state of the transaction, a different action may be taken.
 */
struct SSubstateRecvDispatchTable
{
    const FileDirectiveDispatchTable *substate[TX_SUB_STATE_NUM_STATES];
};

/**
 * @brief A dispatch table for send file transactions, transmit side
 *
 * This is used for "send file" transactions to generate the next PDU to be sent.
 * Depending on the sub-state of the transaction, a different action may be taken.
 */
struct SSubstateSendDispatchTable
{
    StateSendFunc substate[TX_SUB_STATE_NUM_STATES];
};

/**
 * @brief CFDP Transaction state machine class
 *
 * This class provides TX and RX state machine operations for CFDP transactions.
 * Implementation is split across multiple files for maintainability:
 * - TransactionTx.cpp: TX (send) state machine implementation
 * - TransactionRx.cpp: RX (receive) state machine implementation
 */
class Transaction {
  friend class Engine;
  friend class Channel;
  friend class CfdpManagerTester;

  public:
    // ----------------------------------------------------------------------
    // Construction and Destruction
    // ----------------------------------------------------------------------

    //! Parameterized constructor for channel-bound transaction initialization
    //! @param channel Pointer to the channel this transaction belongs to
    //! @param channelId Channel ID number
    //! @param engine Pointer to the CFDP engine
    //! @param manager Pointer to the CfdpManager component
    Transaction(Channel* channel, U8 channelId, Engine* engine, CfdpManager* manager);

    ~Transaction();

    /**
     * @brief Reset transaction to default state
     *
     * Resets the transaction to a clean state while preserving channel binding.
     * Used when returning a transaction to the free pool for reuse.
     */
    void reset();

    /**
     * @brief Initialize transaction for outgoing file transfer
     *
     * Sets up transaction state for transmitting a file.
     *
     * @param cfdp_class CFDP class (1 or 2)
     * @param keep       Whether to keep file after transfer
     * @param chan       Channel number
     * @param priority   Transaction priority
     */
    void initTxFile(Class::T cfdp_class, Keep::T keep, U8 chan, U8 priority);

    /**
     * @brief Static callback for finding transaction by sequence number
     *
     * C-style callback for list traversal. Used with CfdpCListTraverse.
     *
     * @param node    List node pointer
     * @param context Pointer to CfdpTraverseTransSeqArg
     * @return Traversal status (CONTINUE or EXIT)
     */
    static CListTraverseStatus findBySequenceNumberCallback(CListNode *node, void *context);

    /**
     * @brief Static callback for priority search
     *
     * C-style callback for list traversal. Used with CfdpCListTraverseR.
     *
     * @param node    List node pointer
     * @param context Pointer to CfdpTraversePriorityArg
     * @return Traversal status (CONTINUE or EXIT)
     */
    static CListTraverseStatus prioritySearchCallback(CListNode *node, void *context);

    // ----------------------------------------------------------------------
    // Accessors
    // ----------------------------------------------------------------------

    /**
     * @brief Get transaction history
     * @return Pointer to history structure
     */
    History* getHistory() const { return m_history; }

    /**
     * @brief Get transaction priority
     * @return Priority value
     */
    U8 getPriority() const { return m_priority; }

    /**
     * @brief Get channel ID
     * @return Channel ID number
     */
    U8 getChannelId() const { return m_chan_num; }

    /**
     * @brief Get transaction class (CLASS_1 or CLASS_2)
     * @return Transaction class
     */
    Class::T getClass() const { return m_txn_class; }

    /**
     * @brief Get transaction state
     * @return Transaction state
     */
    TxnState getState() const { return m_state; }

    // ----------------------------------------------------------------------
    // TX State Machine - Implemented in TransactionTx.cpp
    // ----------------------------------------------------------------------

    /************************************************************************/
    /** @brief S1 receive PDU processing.
     *
     * @param buffer The buffer containing the PDU to process
     */
    void s1Recv(const Fw::Buffer& buffer);

    /************************************************************************/
    /** @brief S2 receive PDU processing.
     *
     * @param buffer The buffer containing the PDU to process
     */
    void s2Recv(const Fw::Buffer& buffer);

    /************************************************************************/
    /** @brief S1 dispatch function.
     */
    void s1Tx();

    /************************************************************************/
    /** @brief S2 dispatch function.
     */
    void s2Tx();

    /************************************************************************/
    /** @brief Perform acknowledgement timer tick (time-based) processing for S transactions.
     *
     * This is invoked as part of overall timer tick processing if the transaction
     * has some sort of acknowledgement pending from the remote.
     */
    void sAckTimerTick();

    /************************************************************************/
    /** @brief Perform tick (time-based) processing for S transactions.
     *
     * This function is called on every transaction by the engine on
     * every CFDP wakeup. This is where flags are checked to send EOF or
     * FIN-ACK. If nothing else is sent, it checks to see if a NAK
     * retransmit must occur.
     *
     * @param cont Unused, exists for compatibility with tick processor
     */
    void sTick(int *cont);

    /************************************************************************/
    /** @brief Perform NAK response for TX transactions
     *
     * This function is called at tick processing time to send pending
     * NAK responses. It indicates "cont" is 1 if there are more responses
     * left to send.
     *
     * @param cont Set to 1 if a NAK was generated
     */
    void sTickNak(int *cont);

    /************************************************************************/
    /** @brief Cancel an S transaction.
     */
    void sCancel();

    /************************************************************************/
    /** @brief Sends an EOF for S1.
     */
    void s1SubstateSendEof();

    /************************************************************************/
    /** @brief Triggers tick processing to send an EOF and wait for EOF-ACK for S2
     */
    void s2SubstateSendEof();

    /************************************************************************/
    /** @brief Standard state function to send the next file data PDU for active transaction.
     *
     * During the transfer of active transaction file data PDUs, the file
     * offset is saved. This function sends the next chunk of data. If
     * the file offset equals the file size, then transition to the EOF
     * state.
     */
    void sSubstateSendFileData();

    /************************************************************************/
    /** @brief Send filedata handling for S2.
     *
     * S2 will either respond to a NAK by sending retransmits, or in
     * absence of a NAK, it will send more of the original file data.
     */
    void s2SubstateSendFileData();

    /************************************************************************/
    /** @brief Send metadata PDU.
     *
     * Construct and send a metadata PDU. This function determines the
     * size of the file to put in the metadata PDU.
     */
    void sSubstateSendMetadata();

    /************************************************************************/
    /** @brief A FIN was received before file complete, so abandon the transaction.
     *
     * @param pdu The PDU to process
     */
    void s2EarlyFin(const Fw::Buffer& pdu);

    /************************************************************************/
    /** @brief S2 received FIN, so set flag to send FIN-ACK.
     *
     * @param pdu Buffer containing the FIN PDU to process
     */
    void s2Fin(const Fw::Buffer& pdu);

    /************************************************************************/
    /** @brief S2 NAK PDU received handling.
     *
     * Stores the segment requests from the NAK packet in the chunks
     * structure. These can be used to generate re-transmit filedata
     * PDUs.
     *
     * @param pdu Buffer containing the NAK PDU to process
     */
    void s2Nak(const Fw::Buffer& pdu);

    /************************************************************************/
    /** @brief S2 NAK handling but with arming the NAK timer.
     *
     * @param pdu Buffer containing the NAK PDU to process
     */
    void s2NakArm(const Fw::Buffer& pdu);

    /************************************************************************/
    /** @brief S2 received ACK PDU.
     *
     * Handles reception of an ACK PDU
     *
     * @param pdu Buffer containing the ACK PDU to process
     */
    void s2EofAck(const Fw::Buffer& pdu);

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
     * @retval Cfdp::Status::SUCCESS on success.
     * @retval Cfdp::Status::SEND_PDU_NO_BUF_AVAIL_ERROR if message buffer cannot be obtained.
     * @retval SEND_PDU_ERROR if an error occurred while building the packet.
     */
    Status::T sSendEof();

    Status::T sSendFileData(FileSize foffs, FileSize bytes_to_read, U8 calc_crc, FileSize* bytes_processed);

    Status::T sCheckAndRespondNak(bool* nakProcessed);

    Status::T sSendFinAck();

  public:
    // ----------------------------------------------------------------------
    // RX State Machine - Implemented in TransactionRx.cpp
    // ----------------------------------------------------------------------

    /************************************************************************/
    /** @brief R1 receive PDU processing.
     *
     * @param buffer The buffer containing the PDU to process
     */
    void r1Recv(const Fw::Buffer& buffer);

    /************************************************************************/
    /** @brief R2 receive PDU processing.
     *
     * @param buffer The buffer containing the PDU to process
     */
    void r2Recv(const Fw::Buffer& buffer);

    /************************************************************************/
    /** @brief Perform acknowledgement timer tick (time-based) processing for R transactions.
     *
     * This is invoked as part of overall timer tick processing if the transaction
     * has some sort of acknowledgement pending from the remote.
     */
    void rAckTimerTick();

    /************************************************************************/
    /** @brief Perform tick (time-based) processing for R transactions.
     *
     * This function is called on every transaction by the engine on
     * every CFDP wakeup. This is where flags are checked to send ACK,
     * NAK, and FIN. It checks for inactivity timer and processes the
     * ACK timer. The ACK timer is what triggers re-sends of PDUs
     * that require acknowledgment.
     *
     * @param cont Ignored/Unused
     */
    void rTick(int *cont);

    /************************************************************************/
    /** @brief Cancel an R transaction.
     */
    void rCancel();

    /************************************************************************/
    /** @brief Initialize a transaction structure for R.
     */
    void rInit();

    /************************************************************************/
    /** @brief Helper function to store transaction status code and set send_fin flag.
     *
     * @param txn_stat Status Code value to set within transaction
     */
    void r2SetFinTxnStatus(TxnStatus txn_stat);

    /************************************************************************/
    /** @brief CFDP R1 transaction reset function.
     *
     * All R transactions use this call to indicate the transaction
     * state can be returned to the system. While this function currently
     * only calls the transaction reset logic, it is here as a placeholder.
     */
    void r1Reset();

    /************************************************************************/
    /** @brief CFDP R2 transaction reset function.
     *
     * Handles reset logic for R2, then calls R1 reset logic.
     */
    void r2Reset();

    /************************************************************************/
    /** @brief Checks that the transaction file's CRC matches expected.
     *
     * @retval Cfdp::Status::SUCCESS on CRC match, otherwise Cfdp::Status::CFDP_ERROR.
     *
     * @param expected_crc Expected CRC
     */
    Status::T rCheckCrc(U32 expected_crc);

    /************************************************************************/
    /** @brief Checks R2 transaction state for transaction completion status.
     *
     * This function is called anywhere there's a desire to know if the
     * transaction has completed. It may trigger other actions by setting
     * flags to be handled during tick processing. In order for a
     * transaction to be complete, it must have had its meta-data PDU
     * received, the EOF must have been received, and there must be
     * no gaps in the file. EOF is not checked in this function, because
     * it's only called from functions after EOF is received.
     *
     * @param ok_to_send_nak If set to 0, suppress sending of a NAK packet
     */
    void r2Complete(int ok_to_send_nak);

    // ----------------------------------------------------------------------
    // Dispatch Methods (ported from cf_cfdp_dispatch.c)
    // ----------------------------------------------------------------------

    /************************************************************************/
    /** @brief Dispatch function for received PDUs on receive-file transactions
     *
     * Receive file transactions primarily only react/respond to received PDUs.
     * This function dispatches to the appropriate handler based on the
     * transaction substate and PDU type.
     *
     * @param buffer    Buffer containing the PDU to dispatch
     * @param dispatch  Dispatch table for file directive PDUs
     * @param fd_fn     Function to handle file data PDUs
     */
    void rDispatchRecv(const Fw::Buffer& buffer,
                       const RSubstateDispatchTable *dispatch,
                       StateRecvFunc fd_fn);

    /************************************************************************/
    /** @brief Dispatch function for received PDUs on send-file transactions
     *
     * Send file transactions also react/respond to received PDUs.
     * Note that a file data PDU is not expected here.
     *
     * @param buffer    Buffer containing the PDU to dispatch
     * @param dispatch  Dispatch table for file directive PDUs
     */
    void sDispatchRecv(const Fw::Buffer& buffer,
                       const SSubstateRecvDispatchTable *dispatch);

    /************************************************************************/
    /** @brief Dispatch function to send/generate PDUs on send-file transactions
     *
     * Send file transactions generate PDUs each cycle based on the
     * transaction state. This does not have an existing PDU buffer at
     * the time of dispatch, but one may be generated by the invoked function.
     *
     * @param dispatch  State-based dispatch table
     */
    void sDispatchTransmit(const SSubstateSendDispatchTable *dispatch);

    /************************************************************************/
    /** @brief Top-level Dispatch function to send a PDU based on current state
     *
     * This does not have an existing PDU buffer at the time of dispatch,
     * but one may be generated by the invoked function.
     *
     * @param dispatch  Transaction State-based Dispatch table
     */
    void txStateDispatch(const TxnSendDispatchTable *dispatch);

  private:
    /************************************************************************/
    /** @brief Process a filedata PDU on a transaction.
     *
     * @retval Cfdp::Status::SUCCESS on success. Cfdp::Status::CFDP_ERROR on error.
     *
     * @param pdu Buffer containing the file data PDU to process
     */
    Status::T rProcessFd(const Fw::Buffer& pdu);

    /************************************************************************/
    /** @brief Processing receive EOF common functionality for R1/R2.
     *
     * This function is used for both R1 and R2 EOF receive. It calls
     * the unmarshaling function and then checks known transaction
     * data against the PDU.
     *
     * @retval Cfdp::Status::SUCCESS on success. Returns anything else on error.
     *
     * @param pdu Buffer containing the EOF PDU to process
     */
    Status::T rSubstateRecvEof(const Fw::Buffer& pdu);

    /************************************************************************/
    /** @brief Process receive EOF for R1.
     *
     * Only need to confirm CRC for R1.
     *
     * @param pdu Buffer containing the EOF PDU to process
     */
    void r1SubstateRecvEof(const Fw::Buffer& pdu);

    /************************************************************************/
    /** @brief Process receive EOF for R2.
     *
     * For R2, need to trigger the send of EOF-ACK and then call the
     * check complete function which will either send NAK or FIN.
     *
     * @param pdu Buffer containing the EOF PDU to process
     */
    void r2SubstateRecvEof(const Fw::Buffer& pdu);

    /************************************************************************/
    /** @brief Process received file data for R1.
     *
     * For R1, only need to digest the CRC.
     *
     * @param pdu Buffer containing the file data PDU to process
     */
    void r1SubstateRecvFileData(const Fw::Buffer& pdu);

    /************************************************************************/
    /** @brief Process received file data for R2.
     *
     * For R2, the CRC is checked after the whole file is received
     * since there may be gaps. Instead, insert file received range
     * data into chunks. Once NAK has been received, this function
     * always checks for completion. This function also re-arms
     * the ACK timer.
     *
     * @param pdu Buffer containing the file data PDU to process
     */
    void r2SubstateRecvFileData(const Fw::Buffer& pdu);

    /************************************************************************/
    /** @brief Loads a single NAK segment request.
     *
     * This is a callback function used with CfdpChunkList::computeGaps().
     * For each gap found, this function adds a segment request to the NAK PDU.
     *
     * @param chunk Pointer to the gap chunk information
     * @param nak   Pointer to the NAK PDU being constructed
     */
    void r2GapCompute(const Chunk *chunk, NakPdu& nak);

    /************************************************************************/
    /** @brief Send a NAK PDU for R2.
     *
     * NAK PDU is sent when there are gaps in the received data. The
     * chunks class tracks this and generates the NAK PDU by calculating
     * gaps internally and calling r2GapCompute(). There is a special
     * case where if a metadata PDU has not been received, then a NAK
     * packet will be sent to request another.
     *
     * @retval Cfdp::Status::SUCCESS on success. Cfdp::Status::CFDP_ERROR on error.
     */
    Status::T rSubstateSendNak();

    /************************************************************************/
    /** @brief Calculate up to the configured amount of bytes of CRC.
     *
     * The configuration table has a number of bytes to calculate per
     * transaction per wakeup. At each wakeup, the file is read and
     * this number of bytes are calculated. This function will set
     * the checksum error condition code if the final CRC does not match.
     *
     * @par PTFO
     *       Increase throughput by consuming all CRC bytes per wakeup in
     *       transaction-order. This would require a change to the meaning
     *       of the value in the configuration table.
     *
     * @retval Cfdp::Status::SUCCESS on completion.
     * @retval Cfdp::Status::CFDP_ERROR on non-completion.
     */
    Status::T r2CalcCrcChunk();

    /************************************************************************/
    /** @brief Send a FIN PDU.
     *
     * @retval Cfdp::Status::SUCCESS on success. Cfdp::Status::CFDP_ERROR on error.
     */
    Status::T r2SubstateSendFin();

    /************************************************************************/
    /** @brief Process receive FIN-ACK PDU.
     *
     * This is the end of an R2 transaction. Simply reset the transaction
     * state.
     *
     * @param pdu Buffer containing the ACK PDU to process
     */
    void r2RecvFinAck(const Fw::Buffer& pdu);

    /************************************************************************/
    /** @brief Process receive metadata PDU for R2.
     *
     * It's possible that metadata PDU was missed in cf_cfdp.c, or that
     * it was re-sent. This function checks if it was already processed,
     * and if not, handles it. If there was a temp file opened due to
     * missed metadata PDU, it will move the file to the correct
     * destination according to the metadata PDU.
     *
     * @param pdu Buffer containing the metadata PDU to process
     */
    void r2RecvMd(const Fw::Buffer& pdu);

    /************************************************************************/
    /** @brief Sends an inactivity timer expired event to EVS.
     */
    void rSendInactivityEvent();

  private:
    // ----------------------------------------------------------------------
    // Member Variables
    // ----------------------------------------------------------------------

    /**
     * @brief High-level transaction state
     *
     * Each engine is commanded to do something, which is the overall state.
     */
    TxnState m_state;

    /**
     * @brief Transaction class (CLASS_1 or CLASS_2)
     *
     * Set at initialization and never changes.
     */
    Class::T m_txn_class;

    /**
     * @brief Pointer to history entry
     *
     * Holds active filenames and possibly other info.
     */
    History* m_history;

    /**
     * @brief Pointer to chunk wrapper
     *
     * For gap tracking, only used on class 2.
     */
    CfdpChunkWrapper* m_chunks;

    /**
     * @brief Inactivity timer
     *
     * Set to the overall inactivity timer of a remote.
     */
    Timer m_inactivity_timer;

    /**
     * @brief ACK/NAK timer
     *
     * Called ack_timer, but is also nak_timer.
     */
    Timer m_ack_timer;

    /**
     * @brief File size
     */
    FileSize m_fsize;

    /**
     * @brief File offset for next read
     */
    FileSize m_foffs;

    /**
     * @brief File descriptor
     */
    Os::File m_fd;

    /**
     * @brief CRC checksum object
     */
    CFDP::Checksum m_crc;

    /**
     * @brief Keep file flag
     */
    Keep::T m_keep;

    /**
     * @brief Channel number
     *
     * If ever more than one engine, this may need to change to pointer.
     */
    U8 m_chan_num;

    /**
     * @brief Priority
     */
    U8 m_priority;

    /**
     * @brief Flag indicating if this transaction was initiated via port
     *
     * Set to true for port-initiated transfers so completion can be
     * notified via the FileComplete output port.
     */
    bool m_portInitiated;

    /**
     * @brief Circular list node
     *
     * For connection to a CList (intrusive linked list).
     */
    CListNode m_cl_node;

    /**
     * @brief Pointer to playback entry
     *
     * NULL if transaction does not belong to a playback.
     */
    Playback* m_pb;

    /**
     * @brief State-specific data (TX or RX)
     */
    CfdpStateData m_state_data;

    /**
     * @brief State flags (TX or RX)
     *
     * Note: The flags here look a little strange, because there are different
     * flags for TX and RX. Both types share the same type of flag, though.
     * Since RX flags plus the global flags is over one byte, storing them this
     * way allows 2 bytes to cover all possible flags. Please ignore the
     * duplicate declarations of the "all" flags.
     */
    CfdpStateFlags m_flags;

    /**
     * @brief Reference to the wrapper F' component
     *
     * Used to send PDUs.
     */
    CfdpManager* m_cfdpManager;

    /**
     * @brief Pointer to the channel wrapper
     *
     * The channel this transaction belongs to.
     */
    Channel* m_chan;

    /**
     * @brief Pointer to the CFDP engine
     *
     * The engine this transaction belongs to.
     */
    Engine* m_engine;
};

}  // namespace Cfdp
}  // namespace Ccsds
}  // namespace Svc

#endif  // Svc_Ccsds_CfdpTransaction_HPP
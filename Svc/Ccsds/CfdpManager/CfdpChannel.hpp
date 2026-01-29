// ======================================================================
// \title  CfdpChannel.hpp
// \brief  CFDP Channel operations
//
// This file is a port of channel-specific functions from the following files
// from the NASA Core Flight System (cFS) CFDP (CF) Application, version 3.0.0,
// adapted for use within the F-Prime (F') framework:
// - cf_cfdp.c (channel processing functions)
// - cf_utils.c (channel transaction and resource management)
//
// ======================================================================
//
// NASA Docket No. GSC-18,447-1
//
// Copyright (c) 2019 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// ======================================================================

#ifndef CFDP_CHANNEL_HPP
#define CFDP_CHANNEL_HPP

#include <Fw/Types/Assert.hpp>

#include <Svc/Ccsds/CfdpManager/CfdpTypes.hpp>

namespace Svc {
namespace Ccsds {

// Forward declarations
class CfdpEngine;
class CfdpTransaction;

/**
 * @brief CFDP Channel class
 *
 * Encapsulates channel-specific operations for CFDP protocol processing.
 * Each channel manages its own set of transactions, playback directories,
 * and polling directories.
 */
class CfdpChannel {
  public:
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    /**
     * @brief Construct a CfdpChannel
     *
     * @param engine      Pointer to parent CFDP engine
     * @param channelId   Channel ID (index)
     * @param cfdpManager Pointer to parent CfdpManager component
     */
    CfdpChannel(CfdpEngine* engine, U8 channelId, CfdpManager* cfdpManager);

    /**
     * @brief Destruct a CfdpChannel
     *
     * Frees dynamically allocated resources (transactions, histories, chunks)
     */
    ~CfdpChannel();

    // ----------------------------------------------------------------------
    // Channel Processing
    // ----------------------------------------------------------------------

    /**
     * @brief Cycle the TX side of this channel
     *
     * Processes outgoing transactions and sends PDUs for this channel.
     */
    void cycleTx();

    /**
     * @brief Tick all transactions on this channel
     *
     * Processes timer expirations and retransmissions for all active transactions.
     */
    void tickTransactions();

    /**
     * @brief Process all playback directories for this channel
     */
    void processPlaybackDirectories();

    /**
     * @brief Process all polling directories for this channel
     */
    void processPollingDirectories();

    // ----------------------------------------------------------------------
    // Transaction Management
    // ----------------------------------------------------------------------

    /**
     * @brief Find an unused transaction on this channel
     *
     * @par Assumptions, External Events, and Notes:
     *       None.
     *
     * @param direction Intended direction of data flow (TX or RX)
     *
     * @returns Pointer to a free transaction
     * @retval  NULL if no free transactions available.
     */
    CfdpTransaction* findUnusedTransaction(CF_Direction_t direction);

    /**
     * @brief Finds an active transaction by sequence number
     *
     * @par Description
     *       This function traverses the active rx, pending, txa, and txw
     *       transaction queues and looks for the requested transaction.
     *
     * @par Assumptions, External Events, and Notes:
     *       None.
     *
     * @param transaction_sequence_number  Sequence number to find
     * @param src_eid                      Entity ID associated with sequence number
     *
     * @returns Pointer to the given transaction if found
     * @retval  NULL if the transaction is not found
     */
    CfdpTransaction* findTransactionBySequenceNumber(CfdpTransactionSeq transaction_sequence_number,
                                                     CfdpEntityId src_eid);

    /**
     * @brief Traverses all transactions on all active queues and performs an operation on them
     *
     * @par Assumptions, External Events, and Notes:
     *       fn must be a valid function. context must not be NULL.
     *
     * @param fn      Callback to invoke for all traversed transactions
     * @param context Opaque object to pass to all callbacks
     *
     * @returns Number of transactions traversed
     */
    I32 traverseAllTransactions(CF_TraverseAllTransactions_fn_t fn, void* context);

    /**
     * @brief Returns a history structure back to its unused state
     *
     * @par Description
     *       There's nothing to do currently other than remove the history
     *       from its current queue and put it back on CfdpQueueId::HIST_FREE.
     *
     * @par Assumptions, External Events, and Notes:
     *       history must not be NULL.
     *
     * @param history Pointer to the history entry
     */
    void resetHistory(CF_History_t* history);

    // ----------------------------------------------------------------------
    // Channel State Management
    // ----------------------------------------------------------------------

    /**
     * @brief Get the channel ID
     *
     * @returns Channel ID
     */
    inline U8 getChannelId() const { return m_channelId; }

    /**
     * @brief Get the outgoing PDU counter for this cycle
     *
     * @returns Current outgoing PDU count
     */
    inline U32 getOutgoingCounter() const { return m_outgoingCounter; }

    /**
     * @brief Increment the outgoing PDU counter
     */
    inline void incrementOutgoingCounter() { ++m_outgoingCounter; }

    /**
     * @brief Reset the outgoing PDU counter to zero
     */
    inline void resetOutgoingCounter() { m_outgoingCounter = 0; }

    /**
     * @brief Get the number of commanded TX transactions
     *
     * @returns Number of commanded TX transactions
     */
    inline U32 getNumCmdTx() const { return m_numCmdTx; }

    /**
     * @brief Increment the command TX counter for this channel
     */
    inline void incrementCmdTxCounter() { ++m_numCmdTx; }

    /**
     * @brief Decrement the command TX counter for this channel
     *
     * @par Assumptions, External Events, and Notes:
     *       Counter must be greater than zero
     */
    void decrementCmdTxCounter();

    /**
     * @brief Check if current transaction matches and clear if so
     *
     * @param txn Transaction to check against current
     */
    void clearCurrentIfMatch(CfdpTransaction* txn);

    /**
     * @brief Set the flow state for this channel
     *
     * @param flowState New flow state (NORMAL or FROZEN)
     */
    inline void setFlowState(CfdpFlow::T flowState) { m_flowState = flowState; }

    /**
     * @brief Get the flow state for this channel
     *
     * @returns Current flow state
     */
    inline CfdpFlow::T getFlowState() const { return m_flowState; }

    /**
     * @brief Get a playback directory entry
     *
     * @param index Index of playback directory
     * @returns Pointer to playback directory
     */
    inline CF_Playback_t* getPlayback(U32 index) {
        FW_ASSERT(index < CF_MAX_COMMANDED_PLAYBACK_DIRECTORIES_PER_CHAN);
        return &m_playback[index];
    }

    /**
     * @brief Get a polling directory entry
     *
     * @param index Index of polling directory
     * @returns Pointer to polling directory
     */
    inline CF_PollDir_t* getPollDir(U32 index) {
        FW_ASSERT(index < CF_MAX_POLLING_DIR_PER_CHAN);
        return &m_polldir[index];
    }

    /**
     * @brief Get a transaction by index (for testing)
     *
     * @param index Transaction index within this channel
     * @returns Pointer to transaction
     */
    CfdpTransaction* getTransaction(U32 index);

    /**
     * @brief Get a history by index (for testing)
     *
     * @param index History index within this channel
     * @returns Pointer to history entry
     */
    CF_History_t* getHistory(U32 index);

    // ----------------------------------------------------------------------
    // Resource Management
    // ----------------------------------------------------------------------

    /**
     * @brief Gets the head of the chunk list for this channel + direction
     *
     * The chunk list contains structs that are available for tracking the chunks
     * associated with files in transit.  An entry needs to be pulled from this
     * list for every transaction, and returned to this list when the transaction
     * completes.
     *
     * @param direction  Whether this is TX or RX
     *
     * @returns Pointer to list head
     */
    CF_CListNode_t** getChunkListHead(U8 direction);

    /**
     * @brief Find unused chunks for this channel
     *
     * @param dir  Direction (TX or RX)
     *
     * @returns Pointer to unused chunk wrapper
     * @retval  NULL if no chunks available
     */
    CF_ChunkWrapper_t* findUnusedChunks(CF_Direction_t dir);

    // ----------------------------------------------------------------------
    // Transaction Management
    // ----------------------------------------------------------------------

    /**
     * @brief Free a transaction from the queue it's on
     *
     * @par Description
     *       NOTE: this leaves the transaction in a bad state,
     *       so it must be followed by placing the transaction on
     *       another queue. Need this function because the path of
     *       freeing a transaction (returning to default state)
     *       means that it must be removed from the current queue
     *       otherwise if the structure is zero'd out the queue
     *       will become corrupted due to other nodes on the queue
     *       pointing to an invalid node
     *
     * @par Assumptions, External Events, and Notes:
     *       txn must not be NULL.
     *
     * @param txn Pointer to the transaction object
     */
    void dequeueTransaction(CfdpTransaction* txn);

    /**
     * @brief Move a transaction from one queue to another
     *
     * @par Assumptions, External Events, and Notes:
     *       txn must not be NULL.
     *
     * @param txn   Pointer to the transaction object
     * @param queue Index of destination queue
     */
    void moveTransaction(CfdpTransaction* txn, CfdpQueueId::T queue);

    /**
     * @brief Frees and resets a transaction and returns it for later use
     *
     * @par Assumptions, External Events, and Notes:
     *       txn must not be NULL.
     *
     * @param txn Pointer to the transaction object
     */
    void freeTransaction(CfdpTransaction* txn);

    /**
     * @brief Recover resources associated with a transaction
     *
     * Wipes all data in the transaction struct and returns everything to its
     * relevant FREE list so it can be used again.
     *
     * Notably, should any PDUs arrive after this that is related to this
     * transaction, these PDUs will not be identifiable, and no longer associable
     * to this transaction.
     *
     * @par Assumptions, External Events, and Notes:
     *     It is imperative that nothing uses the txn struct after this call,
     *     as it will now be invalid.  This is effectively like free().
     *
     * @param txn  Pointer to the transaction object
     */
    void recycleTransaction(CfdpTransaction *txn);

    /**
     * @brief Insert a transaction into a priority sorted transaction queue
     *
     * @par Description
     *       This function works by walking the queue in reverse to find a
     *       transaction with a higher priority than the given transaction.
     *       The given transaction is then inserted after that one, since it
     *       would be the next lower priority.
     *
     * @par Assumptions, External Events, and Notes:
     *       txn must not be NULL.
     *
     * @param txn   Pointer to the transaction object
     * @param queue Index of queue to insert into
     */
    void insertSortPrio(CfdpTransaction* txn, CfdpQueueId::T queue);

    // ----------------------------------------------------------------------
    // Queue Management
    // ----------------------------------------------------------------------

    /**
     * @brief Remove a node from a channel queue
     *
     * @param queueidx  Queue index
     * @param node      Node to remove
     */
    inline void removeFromQueue(CfdpQueueId::T queueidx, CF_CListNode_t* node);

    /**
     * @brief Insert a node after another in a channel queue
     *
     * @param queueidx  Queue index
     * @param start     Node to insert after
     * @param after     Node to insert
     */
    inline void insertAfterInQueue(CfdpQueueId::T queueidx, CF_CListNode_t* start, CF_CListNode_t* after);

    /**
     * @brief Insert a node at the back of a channel queue
     *
     * @param queueidx  Queue index
     * @param node      Node to insert
     */
    inline void insertBackInQueue(CfdpQueueId::T queueidx, CF_CListNode_t* node);

    // ----------------------------------------------------------------------
    // Callback methods (public so wrappers can call them)
    // ----------------------------------------------------------------------

    /**
     * @brief Traverse callback for cycling the first active transaction
     *
     * @param node     List node being traversed
     * @param context  Callback context (CF_CFDP_CycleTx_args_t*)
     * @returns Traversal status (CONT or EXIT)
     */
    CF_CListTraverse_Status_t cycleTxFirstActive(CF_CListNode_t* node, void* context);

    /**
     * @brief Traverse callback for ticking a transaction
     *
     * @param node     List node being traversed
     * @param context  Callback context (CF_CFDP_Tick_args_t*)
     * @returns Traversal status (CONT or EXIT)
     */
    CF_CListTraverse_Status_t doTick(CF_CListNode_t* node, void* context);

  private:
    // ----------------------------------------------------------------------
    // Private helper methods
    // ----------------------------------------------------------------------

    /**
     * @brief Step each active playback directory
     *
     * Check if a playback directory needs iterated, and if so does, and
     * if a valid file is found initiates playback on it.
     *
     * @param pb    The playback state
     */
    void processPlaybackDirectory(CF_Playback_t* pb);

    /**
     * @brief Update playback/poll counted state
     *
     * @param pb      Playback state
     * @param up      Whether to increment (1) or decrement (0)
     * @param counter Counter to update
     */
    void updatePollPbCounted(CF_Playback_t* pb, int up, U8* counter);

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    CfdpEngine* m_engine;    //!< Parent CFDP engine

    CF_CListNode_t* m_qs[CfdpQueueId::NUM];    //!< Transaction queues
    CF_CListNode_t* m_cs[CF_Direction_NUM];    //!< Command/history lists

    U32 m_numCmdTx;                            //!< Number of commanded TX transactions

    CF_Playback_t m_playback[CF_MAX_COMMANDED_PLAYBACK_DIRECTORIES_PER_CHAN];  //!< Playback state
    CF_PollDir_t m_polldir[CF_MAX_POLLING_DIR_PER_CHAN];                       //!< Polling directory state

    const CfdpTransaction* m_cur;              //!< Current transaction during channel cycle
    CfdpManager* m_cfdpManager;                //!< Reference to F' component for parameters

    U8 m_tickType;                             //!< Type of tick being processed
    U8 m_channelId;                            //!< Channel ID (index into engine array)

    CfdpFlow::T m_flowState;                   //!< Channel flow state (normal/frozen)
    U32 m_outgoingCounter;                     //!< PDU throttling counter

    // Per-channel resource arrays (dynamically allocated, moved from CfdpEngine)
    CfdpTransaction* m_transactions;           //!< Array of CF_NUM_TRANSACTIONS_PER_CHANNEL
    CF_History_t* m_histories;                 //!< Array of CF_NUM_HISTORIES_PER_CHANNEL
    CF_ChunkWrapper_t* m_chunks;               //!< Array of CF_NUM_TRANSACTIONS_PER_CHANNEL * CF_Direction_NUM
    CF_Chunk_t* m_chunkMem;                    //!< Chunk memory backing store

    // Friend declarations for testing
    friend class CfdpManagerTester;
};

// ----------------------------------------------------------------------
// Inline function implementations
// ----------------------------------------------------------------------

inline void CfdpChannel::removeFromQueue(CfdpQueueId::T queueidx, CF_CListNode_t* node)
{
    CF_CList_Remove(&m_qs[queueidx], node);
}

inline void CfdpChannel::insertAfterInQueue(CfdpQueueId::T queueidx, CF_CListNode_t* start, CF_CListNode_t* after)
{
    CF_CList_InsertAfter(&m_qs[queueidx], start, after);
}

inline void CfdpChannel::insertBackInQueue(CfdpQueueId::T queueidx, CF_CListNode_t* node)
{
    CF_CList_InsertBack(&m_qs[queueidx], node);
}

// ----------------------------------------------------------------------
// Free function wrappers (forward declarations)
// ----------------------------------------------------------------------

CF_CListTraverse_Status_t CF_CFDP_CycleTxFirstActive(CF_CListNode_t* node, void* context);
CF_CListTraverse_Status_t CF_CFDP_DoTick(CF_CListNode_t* node, void* context);
void CF_CFDP_ArmInactTimer(CfdpTransaction *txn);
void CF_MoveTransaction(CfdpTransaction* txn, CfdpQueueId::T queue);

}  // namespace Ccsds
}  // namespace Svc

#endif // CFDP_CHANNEL_HPP
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

#include <Svc/Ccsds/CfdpManager/CfdpTypes.hpp>

namespace Svc {
namespace Ccsds {

// Forward declarations
class CfdpEngine;

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
     * @param engine  Pointer to parent CFDP engine
     * @param chan    Pointer to channel data structure
     */
    CfdpChannel(CfdpEngine* engine, CF_Channel_t* chan);

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
    CF_Transaction_t* findUnusedTransaction(CF_Direction_t direction);

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
    CF_Transaction_t* findTransactionBySequenceNumber(CfdpTransactionSeq transaction_sequence_number,
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
    inline U8 getChannelId() const { return m_channel->channel_id; }

    /**
     * @brief Get the outgoing PDU counter for this cycle
     *
     * @returns Current outgoing PDU count
     */
    inline U32 getOutgoingCounter() const { return m_channel->outgoing_counter; }

    /**
     * @brief Increment the outgoing PDU counter
     */
    inline void incrementOutgoingCounter() { ++m_channel->outgoing_counter; }

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
    void clearCurrentIfMatch(CF_Transaction_t* txn);

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
    void dequeueTransaction(CF_Transaction_t* txn);

    /**
     * @brief Move a transaction from one queue to another
     *
     * @par Assumptions, External Events, and Notes:
     *       txn must not be NULL.
     *
     * @param txn   Pointer to the transaction object
     * @param queue Index of destination queue
     */
    void moveTransaction(CF_Transaction_t* txn, CfdpQueueId::T queue);

    /**
     * @brief Frees and resets a transaction and returns it for later use
     *
     * @par Assumptions, External Events, and Notes:
     *       txn must not be NULL.
     *
     * @param txn Pointer to the transaction object
     */
    void freeTransaction(CF_Transaction_t* txn);

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
    void recycleTransaction(CF_Transaction_t *txn);

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
    void insertSortPrio(CF_Transaction_t* txn, CfdpQueueId::T queue);

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
    // Member variables
    // ----------------------------------------------------------------------

    CfdpEngine* m_engine;    //!< Parent CFDP engine
    CF_Channel_t* m_channel; //!< Channel data structure
};

}  // namespace Ccsds
}  // namespace Svc

#endif // CFDP_CHANNEL_HPP
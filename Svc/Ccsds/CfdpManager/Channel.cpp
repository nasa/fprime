// ======================================================================
// \title  Channel.cpp
// \brief  CFDP Channel operations implementation
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

#include <string.h>
#include <new>

#include <Fw/FPrimeBasicTypes.hpp>

#include <Svc/Ccsds/CfdpManager/CfdpManager.hpp>
#include <Svc/Ccsds/CfdpManager/Channel.hpp>
#include <Svc/Ccsds/CfdpManager/Engine.hpp>
#include <Svc/Ccsds/CfdpManager/Utils.hpp>

namespace Svc {
namespace Ccsds {
namespace Cfdp {

// ----------------------------------------------------------------------
// Construction
// ----------------------------------------------------------------------

Channel::Channel(Engine* engine,
                 U8 channelId,
                 CfdpManager* cfdpManager,
                 Fw::MemAllocator& allocator,
                 FwEnumStoreType memId)
    : m_engine(engine),
      m_numCmdTx(0),
      m_currentTxn(nullptr),
      m_cfdpManager(cfdpManager),
      m_tickType(0),
      m_channelId(channelId),
      m_flowState(Cfdp::Flow::NOT_FROZEN),
      m_outgoingCounter(0),
      m_transactions(nullptr),
      m_histories(nullptr),
      m_chunks(nullptr),
      m_chunkMem(nullptr) {
    FW_ASSERT(engine != nullptr);
    FW_ASSERT(cfdpManager != nullptr);

    // Initialize queue pointers
    for (U32 i = 0; i < QueueId::NUM; i++) {
        m_qs[i] = nullptr;
    }

    // Initialize command/history lists
    for (U32 i = 0; i < static_cast<U32>(Direction::DIRECTION_NUM); i++) {
        m_cs[i] = nullptr;
    }

    // Initialize poll directory playback state
    for (U32 i = 0; i < MaxPollingDirPerChan; i++) {
        m_polldir[i].pb.busy = false;
        m_polldir[i].pb.diropen = false;
        m_polldir[i].pb.counted = false;
        m_polldir[i].pb.num_ts = 0;
        m_polldir[i].pb.pending_file = "";
    }

    // Initialize playback structures
    for (U32 i = 0; i < MaxCommandedPlaybackDirectoriesPerChan; i++) {
        m_playback[i].busy = false;
        m_playback[i].diropen = false;
        m_playback[i].counted = false;
        m_playback[i].num_ts = 0;
        m_playback[i].pending_file = "";
    }

    // Allocate and initialize per-channel resources
    U32 j, k;
    History* history;
    Transaction* txn;
    CfdpChunkWrapper* cw;
    CListNode** list_head;
    U32 chunk_mem_offset = 0;
    U32 total_chunks_needed;

    // Initialize chunk configuration for this channel
    const U32 rxChunksPerChannel[] = CFDP_CHANNEL_NUM_RX_CHUNKS_PER_TRANSACTION;
    const U32 txChunksPerChannel[] = CFDP_CHANNEL_NUM_TX_CHUNKS_PER_TRANSACTION;
    m_dirMaxChunks[static_cast<U32>(Direction::DIRECTION_RX)] = rxChunksPerChannel[m_channelId];
    m_dirMaxChunks[static_cast<U32>(Direction::DIRECTION_TX)] = txChunksPerChannel[m_channelId];

    // Calculate total chunks needed for this channel
    total_chunks_needed = 0;
    for (k = 0; k < static_cast<U32>(Direction::DIRECTION_NUM); ++k) {
        total_chunks_needed += m_dirMaxChunks[k] * CFDP_NUM_TRANSACTIONS_PER_CHANNEL;
    }

    // Allocate arrays using the provided allocator
    FwSizeType transactionsSize = CFDP_NUM_TRANSACTIONS_PER_CHANNEL * sizeof(Transaction);
    m_transactions = static_cast<Transaction*>(allocator.allocate(memId, transactionsSize));
    FW_ASSERT(m_transactions != nullptr);

    FwSizeType chunksSize =
        (CFDP_NUM_TRANSACTIONS_PER_CHANNEL * static_cast<U32>(Direction::DIRECTION_NUM)) * sizeof(CfdpChunkWrapper);
    m_chunks = static_cast<CfdpChunkWrapper*>(allocator.allocate(memId, chunksSize));
    FW_ASSERT(m_chunks != nullptr);

    FwSizeType historiesSize = NumHistoriesPerChannel * sizeof(History);
    m_histories = static_cast<History*>(allocator.allocate(memId, historiesSize));
    FW_ASSERT(m_histories != nullptr);

    FwSizeType chunkMemSize = total_chunks_needed * sizeof(Chunk);
    m_chunkMem = static_cast<Chunk*>(allocator.allocate(memId, chunkMemSize));
    FW_ASSERT(m_chunkMem != nullptr);

    // Initialize transactions using placement new with parameterized constructor
    cw = m_chunks;
    for (j = 0; j < CFDP_NUM_TRANSACTIONS_PER_CHANNEL; ++j) {
        // Construct transaction in-place with parameterized constructor
        txn = new (&m_transactions[j]) Transaction(this, m_channelId, m_engine, m_cfdpManager);

        // Put transaction on free list
        this->freeTransaction(txn);

        // Initialize chunk wrappers for this transaction (TX and RX)
        for (k = 0; k < static_cast<U32>(Direction::DIRECTION_NUM); ++k, ++cw) {
            list_head = this->getChunkListHead(static_cast<U8>(k));

            // Use placement new to construct CfdpChunkWrapper with the new class-based interface
            new (cw) CfdpChunkWrapper(static_cast<ChunkIdx>(m_dirMaxChunks[k]), &m_chunkMem[chunk_mem_offset]);
            chunk_mem_offset += m_dirMaxChunks[k];
            CfdpCListInitNode(&cw->cl_node);
            CfdpCListInsertBack(list_head, &cw->cl_node);
        }
    }

    // Initialize histories using placement new (History contains Fw::String which needs proper construction)
    for (j = 0; j < NumHistoriesPerChannel; ++j) {
        history = new (&m_histories[j]) History();  // Use placement new with default constructor
        CfdpCListInitNode(&history->cl_node);
        this->insertBackInQueue(QueueId::HIST_FREE, &history->cl_node);
    }
}

Channel::~Channel() {
    // Cleanup should have been called before destruction
    // This is enforced by Engine::~Engine()
}

void Channel::cleanup(Fw::MemAllocator& allocator, FwEnumStoreType memId) {
    // Call destructors and deallocate all internal arrays
    if (m_transactions != nullptr) {
        // Manually call destructors since we used placement new
        for (U32 j = 0; j < CFDP_NUM_TRANSACTIONS_PER_CHANNEL; ++j) {
            m_transactions[j].~Transaction();
        }
        allocator.deallocate(memId, m_transactions);
        m_transactions = nullptr;
    }

    if (m_chunks != nullptr) {
        // Manually call destructors since we used placement new
        for (U32 j = 0; j < (CFDP_NUM_TRANSACTIONS_PER_CHANNEL * static_cast<U32>(Direction::DIRECTION_NUM)); ++j) {
            m_chunks[j].~CfdpChunkWrapper();
        }
        allocator.deallocate(memId, m_chunks);
        m_chunks = nullptr;
    }

    if (m_histories != nullptr) {
        // Call destructors on History objects
        for (U32 j = 0; j < NumHistoriesPerChannel; ++j) {
            m_histories[j].~History();
        }
        allocator.deallocate(memId, m_histories);
        m_histories = nullptr;
    }

    if (m_chunkMem != nullptr) {
        allocator.deallocate(memId, m_chunkMem);
        m_chunkMem = nullptr;
    }
}

// ----------------------------------------------------------------------
// Channel Processing
// ----------------------------------------------------------------------

void Channel::cycleTx() {
    Transaction* txn;
    CycleTxArgs args;

    if (m_cfdpManager->getDequeueEnabledParam(m_channelId)) {
        args.chan = this;
        args.ran_one = 0;

        // loop through as long as there are pending transactions, and a message buffer to send their PDUs on

        // NOTE: tick processing is higher priority than sending new filedata PDUs, so only send however many
        // PDUs that can be sent once we get to here
        if (!this->m_currentTxn) {  // don't enter if currentTxn is set, since we need to pick up where we left off on
                                    // tick processing next scheduler cycle

            // Process pending transactions until queue is empty or something runs
            while (true) {
                // Context for static wrapper: pass both Channel* and CycleTxArgs*
                struct CycleTxContext {
                    Channel* channel;
                    CycleTxArgs* args;
                } cycleTxCtx = {this, &args};

                // Attempt to run something on TXA
                CfdpCListTraverse(m_qs[QueueId::TXA], &Channel::cycleTxFirstActiveWrapper, &cycleTxCtx);

                // Keep going until QueueId::PEND is empty or something is run
                if (args.ran_one || m_qs[QueueId::PEND] == nullptr) {
                    break;
                }

                txn = container_of_cpp(m_qs[QueueId::PEND], &Transaction::m_cl_node);

                // Class 2 transactions need a chunklist for NAK processing, get one now.
                // Class 1 transactions don't need chunks since they don't support NAKs.
                if (txn->getClass() == Cfdp::Class::CLASS_2) {
                    if (txn->m_chunks == nullptr) {
                        txn->m_chunks = this->findUnusedChunks(Direction::DIRECTION_TX);
                    }
                    if (txn->m_chunks == nullptr) {
                        // Chunklist unavailable - EVR already emitted by Engine
                        // Leave transaction pending until a chunklist is available.
                        break;
                    }
                }

                m_engine->armInactTimer(txn);
                this->moveTransaction(txn, QueueId::TXA);
            }
        }

        // in case the loop exited due to no message buffers, clear it and start from the top next time
        this->m_currentTxn = nullptr;
    }
}

void Channel::tickTransactions() {
    bool reset = true;

    void (Transaction::* fns[static_cast<U8>(CfdpTickType::CFDP_TICK_TYPE_NUM_TYPES)])(I32*) = {
        &Transaction::rTick, &Transaction::sTick, &Transaction::sTickNak};
    I32 qs[static_cast<U8>(CfdpTickType::CFDP_TICK_TYPE_NUM_TYPES)] = {QueueId::RX, QueueId::TXW, QueueId::TXW};

    FW_ASSERT(m_tickType < static_cast<U8>(CfdpTickType::CFDP_TICK_TYPE_NUM_TYPES), m_tickType);

    for (; m_tickType < static_cast<U8>(CfdpTickType::CFDP_TICK_TYPE_NUM_TYPES); ++m_tickType) {
        TickArgs args = {this, fns[m_tickType], 0, 0};

        // Safety bound: retry loop should not exceed the number of transactions in the queue
        // Each retry processes one transaction that may request continuation
        constexpr U32 maxRetries = MaxSimultaneousRx + MaxCommandedPlaybackFilesPerChan +
                                   (MaxCommandedPlaybackDirectoriesPerChan * NumTransactionsPerPlayback) +
                                   (MaxPollingDirPerChan * NumTransactionsPerPlayback);

        for (U32 retry = 0; retry < maxRetries; ++retry) {
            args.cont = 0;

            // Context for static wrapper: pass both Channel* and TickArgs*
            struct TickContext {
                Channel* channel;
                TickArgs* args;
            } tickCtx = {this, &args};

            CfdpCListTraverse(m_qs[qs[m_tickType]], &Channel::doTickWrapper, &tickCtx);

            if (args.early_exit) {
                // early exit means we ran out of available outgoing messages this scheduler cycle.
                // If current tick type is NAK response, then reset tick type. It would be
                // bad to let NAK response starve out RX or TXW ticks on the next cycle.
                //
                // If RX ticks use up all available messages, then we pick up where we left
                // off on the next cycle. (This causes some RX tick counts to be missed,
                // but that's ok. Precise timing isn't required.)
                //
                // This scheme allows the following priority for use of outgoing messages:
                //
                // RX state messages
                // TXW state messages
                // NAK response (could be many)
                //
                // New file data on TXA
                if (m_tickType != static_cast<U8>(CfdpTickType::CFDP_TICK_TYPE_TXW_NAK)) {
                    reset = false;
                }

                break;
            }

            if (!args.cont) {
                break;  // No continuation requested, exit retry loop
            }
        }

        if (!reset) {
            break;
        }
    }

    if (reset) {
        m_tickType = static_cast<U8>(CfdpTickType::CFDP_TICK_TYPE_RX);  // reset tick type
    }
}

void Channel::processPlaybackDirectories() {
    U32 i;
    U8 playback_count = 0;

    for (i = 0; i < MaxCommandedPlaybackDirectoriesPerChan; ++i) {
        this->processPlaybackDirectory(&m_playback[i]);
        // Count active playback operations
        if (m_playback[i].busy) {
            playback_count++;
        }
    }

    // Update playback counter telemetry
    Cfdp::ChannelTelemetry& tlm = m_engine->getChannelTelemetryRef(m_channelId);
    tlm.set_playbackCounter(playback_count);
}

void Channel::processPollingDirectories() {
    CfdpPollDir* pd;
    U32 i;
    U8 poll_count = 0;
    Status::T status;

    for (i = 0; i < MaxPollingDirPerChan; ++i) {
        pd = &m_polldir[i];

        if (pd->enabled) {
            poll_count++;

            if ((pd->pb.busy == false) && (pd->pb.num_ts == 0)) {
                if ((pd->intervalTimer.getStatus() != Timer::Status::RUNNING) && (pd->intervalSec > 0)) {
                    // timer was not set, so set it now
                    pd->intervalTimer.setTimer(pd->intervalSec);
                } else if (pd->intervalTimer.getStatus() == Timer::Status::EXPIRED) {
                    // the timer has expired
                    status = m_engine->playbackDirInitiate(&pd->pb, pd->srcDir, pd->dstDir, pd->cfdpClass,
                                                           Cfdp::Keep::DELETE, m_channelId, pd->priority, pd->destEid);
                    if (status != Cfdp::Status::SUCCESS) {
                        // error occurred in playback directory, so reset the timer
                        // an event is sent when initiating playback directory so there is no reason to
                        // to have another here
                        pd->intervalTimer.setTimer(pd->intervalSec);
                    }
                } else {
                    pd->intervalTimer.run();
                }
            } else {
                // playback is active, so step it
                this->processPlaybackDirectory(&pd->pb);
            }
        }
    }

    // Update poll counter telemetry
    Cfdp::ChannelTelemetry& tlm = m_engine->getChannelTelemetryRef(m_channelId);
    tlm.set_pollCounter(poll_count);
}

// ----------------------------------------------------------------------
// Transaction Management
// ----------------------------------------------------------------------

Transaction* Channel::findUnusedTransaction(Direction direction) {
    CListNode* node;
    Transaction* txn;
    QueueId::T q_index;  // initialized below in if

    if (m_qs[QueueId::FREE]) {
        node = m_qs[QueueId::FREE];
        txn = container_of_cpp(node, &Transaction::m_cl_node);

        this->removeFromQueue(QueueId::FREE, &txn->m_cl_node);

        // now that a transaction is acquired, must also acquire a history slot to go along with it
        if (m_qs[QueueId::HIST_FREE]) {
            q_index = QueueId::HIST_FREE;
        } else {
            // no free history, so take the oldest one from the channel's history queue
            FW_ASSERT(m_qs[QueueId::HIST]);
            q_index = QueueId::HIST;
        }

        txn->m_history = container_of_cpp(m_qs[q_index], &History::cl_node);

        this->removeFromQueue(q_index, &txn->m_history->cl_node);

        // Reset all history fields to initial state (matches constructor zero-init)
        // This is necessary when recycling from HIST queue to clear stale data
        txn->m_history->txn_stat = TxnStatus::TXN_STATUS_UNDEFINED;  // Critical: prevents error status inheritance
        txn->m_history->src_eid = 0;
        txn->m_history->peer_eid = 0;
        txn->m_history->seq_num = 0;
        txn->m_history->fnames.src_filename = "";
        txn->m_history->fnames.dst_filename = "";
        // Note: cl_node is managed by queue operations (already handled by removeFromQueue)
        // Note: dir is explicitly set below (already handled)

        // Indicate that this was freshly pulled from the free list
        // notably this state is distinguishable from items still on the free list
        txn->m_state = TxnState::TXN_STATE_INIT;

        // Clear the FREE tag now that this transaction has been taken off the FREE
        // list. freeTransaction() marks q_index == QueueId::FREE for anything sitting
        // on the free list; leaving that tag set on an acquired-but-not-yet-enqueued
        // transaction would break the invariant relied on by
        // Engine::finishTransaction()'s double-free guard (a live txn must never look
        // FREE). The caller (startRxTransaction / txFileInitiate) will assign the real
        // queue via insertSortPrio()/direct assignment; until then PEND (0) is the
        // neutral, not-on-FREE-list default that matches reset()'s zeroed m_flags.
        txn->m_flags.com.q_index = QueueId::PEND;

        txn->m_history->dir = direction;
        txn->m_chan = this;  // Set channel pointer

        // Re-initialize the linked list node to clear stale pointers from FREE list
        CfdpCListInitNode(&txn->m_cl_node);
    } else {
        txn = nullptr;
    }

    return txn;
}

Transaction* Channel::findTransactionBySequenceNumber(TransactionSeq transaction_sequence_number, EntityId src_eid) {
    // need to find transaction by sequence number. It will either be the active transaction (front of Q_PEND),
    // or on Q_TX or Q_RX. Once a transaction moves to history, then it's done.
    //
    // Let's put QueueId::RX up front, because most RX packets will be file data PDUs
    CfdpTraverseTransSeqArg ctx = {transaction_sequence_number, src_eid, nullptr};
    CListNode* ptrs[] = {m_qs[QueueId::RX], m_qs[QueueId::PEND], m_qs[QueueId::TXA], m_qs[QueueId::TXW]};
    Transaction* ret = nullptr;

    for (CListNode* head : ptrs) {
        CfdpCListTraverse(head, Transaction::findBySequenceNumberCallback, &ctx);
        if (ctx.txn) {
            ret = ctx.txn;
            break;
        }
    }

    return ret;
}

I32 Channel::traverseAllTransactions(CfdpTraverseAllTransactionsFunc fn, void* context) {
    I32 counter = 0;

    // Context for static wrapper
    struct TraverseAllContext {
        CfdpTraverseAllTransactionsFunc fn;
        void* userContext;
        I32* counter;
    } ctx = {fn, context, &counter};

    for (I32 queueidx = QueueId::PEND; queueidx <= QueueId::RX; ++queueidx) {
        CfdpCListTraverse(m_qs[queueidx], &Channel::traverseAllTransactionsWrapper, &ctx);
    }

    return counter;
}

void Channel::resetHistory(History* history) {
    this->removeFromQueue(QueueId::HIST, &history->cl_node);
    this->insertBackInQueue(QueueId::HIST_FREE, &history->cl_node);
}

// ----------------------------------------------------------------------
// Transaction Queue Management
// ----------------------------------------------------------------------

void Channel::dequeueTransaction(Transaction* txn) {
    FW_ASSERT(txn);
    CfdpCListRemove(&m_qs[txn->m_flags.com.q_index], &txn->m_cl_node);

    // Update queue depth telemetry
    Cfdp::ChannelTelemetry& tlm = m_engine->getChannelTelemetryRef(m_channelId);
    switch (txn->m_flags.com.q_index) {
        case Cfdp::QueueId::FREE:

            tlm.set_queueFree(static_cast<U16>(tlm.get_queueFree() - 1));
            break;
        case Cfdp::QueueId::TXA:

            tlm.set_queueTxActive(static_cast<U16>(tlm.get_queueTxActive() - 1));
            break;
        case Cfdp::QueueId::TXW:

            tlm.set_queueTxWaiting(static_cast<U16>(tlm.get_queueTxWaiting() - 1));
            break;
        case Cfdp::QueueId::RX:

            tlm.set_queueRx(static_cast<U16>(tlm.get_queueRx() - 1));
            break;
        case Cfdp::QueueId::HIST:

            tlm.set_queueHistory(static_cast<U16>(tlm.get_queueHistory() - 1));
            break;
        case Cfdp::QueueId::PEND:
        case Cfdp::QueueId::HIST_FREE:
            // PEND and HIST_FREE queues are not tracked in telemetry
            break;
        default:
            FW_ASSERT(0, txn->m_flags.com.q_index);
    }
}

void Channel::moveTransaction(Transaction* txn, QueueId::T queue) {
    FW_ASSERT(txn);
    Cfdp::ChannelTelemetry& tlm = m_engine->getChannelTelemetryRef(m_channelId);

    // Decrement old queue
    CfdpCListRemove(&m_qs[txn->m_flags.com.q_index], &txn->m_cl_node);
    switch (txn->m_flags.com.q_index) {
        case Cfdp::QueueId::FREE:

            tlm.set_queueFree(static_cast<U16>(tlm.get_queueFree() - 1));
            break;
        case Cfdp::QueueId::TXA:

            tlm.set_queueTxActive(static_cast<U16>(tlm.get_queueTxActive() - 1));
            break;
        case Cfdp::QueueId::TXW:

            tlm.set_queueTxWaiting(static_cast<U16>(tlm.get_queueTxWaiting() - 1));
            break;
        case Cfdp::QueueId::RX:

            tlm.set_queueRx(static_cast<U16>(tlm.get_queueRx() - 1));
            break;
        case Cfdp::QueueId::HIST:

            tlm.set_queueHistory(static_cast<U16>(tlm.get_queueHistory() - 1));
            break;
        case Cfdp::QueueId::PEND:
        case Cfdp::QueueId::HIST_FREE:
            // PEND and HIST_FREE queues are not tracked in telemetry
            break;
        default:
            FW_ASSERT(0, txn->m_flags.com.q_index);
    }

    // Increment new queue
    CfdpCListInsertBack(&m_qs[queue], &txn->m_cl_node);
    txn->m_flags.com.q_index = queue;
    switch (queue) {
        case Cfdp::QueueId::FREE:
            tlm.set_queueFree(static_cast<U16>(tlm.get_queueFree() + 1));
            break;
        case Cfdp::QueueId::TXA:
            tlm.set_queueTxActive(static_cast<U16>(tlm.get_queueTxActive() + 1));
            break;
        case Cfdp::QueueId::TXW:
            tlm.set_queueTxWaiting(static_cast<U16>(tlm.get_queueTxWaiting() + 1));
            break;
        case Cfdp::QueueId::RX:
            tlm.set_queueRx(static_cast<U16>(tlm.get_queueRx() + 1));
            break;
        case Cfdp::QueueId::HIST:
            tlm.set_queueHistory(static_cast<U16>(tlm.get_queueHistory() + 1));
            break;
        case Cfdp::QueueId::PEND:
        case Cfdp::QueueId::HIST_FREE:
            // PEND and HIST_FREE queues are not tracked in telemetry
            break;
        default:
            FW_ASSERT(0, queue);
    }
}

void Channel::freeTransaction(Transaction* txn) {
    // Reset transaction to default state (preserves channel context)
    txn->reset();

    // Initialize the linked list node for the FREE queue
    CfdpCListInitNode(&txn->m_cl_node);
    this->insertBackInQueue(QueueId::FREE, &txn->m_cl_node);

    // Mark the transaction as residing on the FREE list. insertBackInQueue() only
    // performs the list insertion (unlike insertSortPrio(), which also updates
    // q_index), and txn->reset() zeroes m_flags so q_index would otherwise be left
    // at 0 (== QueueId::PEND). Without this, a freed transaction is never tagged
    // FREE, and Engine::finishTransaction()'s double-free guard
    // (q_index == QueueId::FREE) can never fire. Setting it here upholds the
    // invariant: "a transaction on the FREE list has q_index == FREE".
    txn->m_flags.com.q_index = QueueId::FREE;
}

void Channel::recycleTransaction(Transaction* txn) {
    CListNode** chunklist_head;
    QueueId::T hist_destq;

    // File should have been closed by the state machine, but if
    // it still hanging open at this point, close it now so its not leaked.
    // This is not normal/expected so log it if this happens.
    if (true == txn->m_fd.isOpen()) {
        this->m_cfdpManager->log_WARNING_LO_DanglingFileHandleClosed(txn->getChannelId(), txn->m_history->seq_num);
        txn->m_fd.close();
    }

    this->dequeueTransaction(txn);  // this makes it "float" (not in any queue)

    // this should always be
    if (txn->m_history != nullptr) {
        if (txn->m_chunks != nullptr) {
            chunklist_head = this->getChunkListHead(static_cast<U8>(txn->m_history->dir));
            if (chunklist_head != nullptr) {
                // Reset chunk list to clear stale data from previous transaction
                txn->m_chunks->chunks.reset();
                CfdpCListInsertBack(chunklist_head, &txn->m_chunks->cl_node);
                txn->m_chunks = nullptr;
            }
        }

        if (txn->m_flags.com.keep_history) {
            // move transaction history to history queue
            hist_destq = QueueId::HIST;
        } else {
            hist_destq = QueueId::HIST_FREE;
        }
        this->insertBackInQueue(hist_destq, &txn->m_history->cl_node);
        txn->m_history = nullptr;
    }

    // this wipes it and puts it back onto the list to be found by
    // Channel::findUnusedTransaction().  Need to preserve the chan_num
    // and keep it associated with this channel, though.
    this->freeTransaction(txn);
}

void Channel::insertSortPrio(Transaction* txn, QueueId::T queue) {
    bool insert_back = false;

    FW_ASSERT(txn);

    // look for proper position on PEND queue for this transaction.
    // This is a simple priority sort.

    if (!m_qs[queue]) {
        // list is empty, so just insert
        insert_back = true;
    } else {
        CfdpTraversePriorityArg arg = {nullptr, txn->getPriority()};
        CfdpCListTraverseR(m_qs[queue], Transaction::prioritySearchCallback, &arg);
        if (arg.txn) {
            this->insertAfterInQueue(queue, &arg.txn->m_cl_node, &txn->m_cl_node);
        } else {
            insert_back = true;
        }
    }

    if (insert_back) {
        this->insertBackInQueue(queue, &txn->m_cl_node);
    }
    txn->m_flags.com.q_index = queue;
}

// ----------------------------------------------------------------------
// Channel State Management
// ----------------------------------------------------------------------

void Channel::decrementCmdTxCounter() {
    FW_ASSERT(m_numCmdTx);  // sanity check
    --m_numCmdTx;
}

void Channel::clearCurrentIfMatch(Transaction* txn) {
    // Done with this TX transaction
    if (this->m_currentTxn == txn) {
        this->m_currentTxn = nullptr;
    }
}

void Channel::setCurrentTxn(const Transaction* txn) {
    this->m_currentTxn = txn;
}

// ----------------------------------------------------------------------
// Resource Management
// ----------------------------------------------------------------------

CListNode** Channel::getChunkListHead(U8 direction) {
    CListNode** result;

    if (direction < static_cast<U32>(Direction::DIRECTION_NUM)) {
        result = &m_cs[direction];
    } else {
        result = nullptr;
    }

    return result;
}

CfdpChunkWrapper* Channel::findUnusedChunks(Direction dir) {
    CfdpChunkWrapper* ret = nullptr;
    CListNode* node;
    CListNode** chunklist_head;

    chunklist_head = this->getChunkListHead(static_cast<U8>(dir));

    // this should never be null
    FW_ASSERT(chunklist_head);

    if (*chunklist_head != nullptr) {
        node = CfdpCListPop(chunklist_head);
        if (node != nullptr) {
            ret = container_of_cpp(node, &CfdpChunkWrapper::cl_node);
        }
    }

    return ret;
}

// ----------------------------------------------------------------------
// Private helper methods
// ----------------------------------------------------------------------

void Channel::processPlaybackDirectory(Playback* pb) {
    Transaction* txn;
    Fw::StringTemplate<MaxFilePathSize> path;
    Os::Directory::Status status;

    // either there's no transaction (first one) or the last one was finished, so check for a new one

    while (pb->diropen && (pb->num_ts < NumTransactionsPerPlayback)) {
        if (pb->pending_file.length() == 0) {
            status = pb->dir.read(path);
            if (status == Os::Directory::NO_MORE_FILES) {
                // Directory playback complete - success reported via TxFileTransferCompleted EVR
                pb->dir.close();
                pb->diropen = false;
                break;
            }
            if (status != Os::Directory::OP_OK) {
                // Directory read error - emit EVR and close playback
                this->m_cfdpManager->log_WARNING_LO_PlaybackDirReadFailed(pb->fnames.src_filename,
                                                                          static_cast<I32>(status));
                pb->dir.close();
                pb->diropen = false;
                break;
            }

            pb->pending_file = path;
        } else {
            txn = this->findUnusedTransaction(Direction::DIRECTION_TX);
            if (txn == nullptr) {
                // while not expected this can certainly happen, because
                // rx transactions consume in these as well.
                // should not need to do anything special, will come back next tick
                break;
            }

            // Append file name to source/destination folders
            txn->m_history->fnames.src_filename = pb->fnames.src_filename;
            txn->m_history->fnames.src_filename += "/";
            txn->m_history->fnames.src_filename += pb->pending_file;

            txn->m_history->fnames.dst_filename = pb->fnames.dst_filename;
            txn->m_history->fnames.dst_filename += "/";
            txn->m_history->fnames.dst_filename += pb->pending_file;

            m_engine->txFileInitiate(txn, pb->cfdp_class, pb->keep, m_channelId, pb->priority, pb->dest_id);

            txn->m_pb = pb;
            ++pb->num_ts;

            pb->pending_file = "";  // continue reading dir
        }
    }

    if (!pb->diropen && !pb->num_ts) {
        // the directory has been exhausted, and there are no more active transactions
        // for this playback -- so mark it as not busy
        pb->busy = false;
    }
}

void Channel::updatePollPbCounted(Playback* pb, I32 up, U8* counter) {
    if (pb->counted != up) {
        // only handle on state change
        pb->counted = !!up;  // !! ensure 0 or 1, should be optimized out

        if (up) {
            ++*counter;
        } else {
            FW_ASSERT(*counter);  // sanity check it isn't zero
            --*counter;
        }
    }
}

CListTraverseStatus Channel::cycleTxFirstActive(CListNode* node, void* context) {
    CycleTxArgs* args = static_cast<CycleTxArgs*>(context);
    Transaction* txn = container_of_cpp(node, &Transaction::m_cl_node);
    CListTraverseStatus ret = CLIST_TRAVERSE_EXIT;  // default option is exit traversal

    if (txn->m_flags.com.suspended) {
        ret = CLIST_TRAVERSE_CONTINUE;  // suspended, so move on to next
    } else {
        FW_ASSERT(txn->m_flags.com.q_index == QueueId::TXA);  // huh?

        // if no more messages, then chan->m_currentTxn will be set.
        // If the transaction sent the last filedata PDU and EOF, it will move itself
        // off the active queue. Run until either of these occur.
        while (!this->m_currentTxn && txn->m_flags.com.q_index == QueueId::TXA) {
            m_engine->dispatchTx(txn);
        }

        args->ran_one = 1;
    }

    return ret;
}

CListTraverseStatus Channel::doTick(CListNode* node, void* context) {
    CListTraverseStatus ret =
        CLIST_TRAVERSE_CONTINUE;  // CLIST_TRAVERSE_CONTINUE means don't tick one, keep looking for currentTxn
    TickArgs* args = static_cast<TickArgs*>(context);
    Transaction* txn = container_of_cpp(node, &Transaction::m_cl_node);
    if (!this->m_currentTxn || (this->m_currentTxn == txn)) {
        // found where we left off, so clear that and move on
        this->m_currentTxn = nullptr;
        if (!txn->m_flags.com.suspended) {
            (txn->*args->fn)(&args->cont);
        }

        // if this->m_currentTxn was set to not-nullptr above, then exit early
        // NOTE: if channel is frozen, then tick processing won't have been entered.
        //     so there is no need to check it here
        if (this->m_currentTxn) {
            ret = CLIST_TRAVERSE_EXIT;
            args->early_exit = true;
        }
    }

    return ret;  // don't tick one, keep looking for currentTxn
}

Transaction* Channel::getTransaction(U32 index) {
    FW_ASSERT(index < CFDP_NUM_TRANSACTIONS_PER_CHANNEL);
    return &m_transactions[index];
}

History* Channel::getHistory(U32 index) {
    FW_ASSERT(index < NumHistoriesPerChannel);
    return &m_histories[index];
}

// ----------------------------------------------------------------------
// Static callback wrapper implementations
// ----------------------------------------------------------------------

CListTraverseStatus Channel::cycleTxFirstActiveWrapper(CListNode* node, void* context) {
    struct CycleTxContext {
        Channel* channel;
        CycleTxArgs* args;
    };
    CycleTxContext* ctx = static_cast<CycleTxContext*>(context);
    return ctx->channel->cycleTxFirstActive(node, ctx->args);
}

CListTraverseStatus Channel::doTickWrapper(CListNode* node, void* context) {
    struct TickContext {
        Channel* channel;
        TickArgs* args;
    };
    TickContext* ctx = static_cast<TickContext*>(context);
    return ctx->channel->doTick(node, ctx->args);
}

CListTraverseStatus Channel::traverseAllTransactionsWrapper(CListNode* node, void* context) {
    struct TraverseAllContext {
        CfdpTraverseAllTransactionsFunc fn;
        void* userContext;
        I32* counter;
    };
    TraverseAllContext* ctx = static_cast<TraverseAllContext*>(context);
    Transaction* txn = container_of_cpp(node, &Transaction::m_cl_node);
    ctx->fn(txn, ctx->userContext);
    ++(*ctx->counter);
    return CLIST_TRAVERSE_CONTINUE;
}

}  // namespace Cfdp
}  // namespace Ccsds
}  // namespace Svc

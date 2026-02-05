// ======================================================================
// \title  CfdpChannel.cpp
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
#include <Svc/Ccsds/CfdpManager/CfdpChannel.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpEngine.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpUtils.hpp>

namespace Svc {
namespace Ccsds {

// ----------------------------------------------------------------------
// Construction
// ----------------------------------------------------------------------

CfdpChannel::CfdpChannel(CfdpEngine* engine, U8 channelId, CfdpManager* cfdpManager) :
    m_engine(engine),
    m_numCmdTx(0),
    m_cur(nullptr),
    m_cfdpManager(cfdpManager),
    m_tickType(0),
    m_channelId(channelId),
    m_flowState(Cfdp::Flow::NOT_FROZEN),
    m_outgoingCounter(0),
    m_transactions(nullptr),
    m_histories(nullptr),
    m_chunks(nullptr),
    m_chunkMem(nullptr)
{
    FW_ASSERT(engine != nullptr);
    FW_ASSERT(cfdpManager != nullptr);

    // Initialize queue pointers
    for (U32 i = 0; i < Cfdp::QueueId::NUM; i++) {
        m_qs[i] = nullptr;
    }

    // Initialize command/history lists
    for (U32 i = 0; i < CFDP_DIRECTION_NUM; i++) {
        m_cs[i] = nullptr;
    }

    // Initialize poll directory playback state
    for (U32 i = 0; i < CFDP_MAX_POLLING_DIR_PER_CHAN; i++) {
        m_polldir[i].pb.busy = false;
        m_polldir[i].pb.diropen = false;
        m_polldir[i].pb.counted = false;
        m_polldir[i].pb.num_ts = 0;
        m_polldir[i].pb.pending_file[0] = '\0';
    }

    // Initialize playback structures
    for (U32 i = 0; i < CFDP_MAX_COMMANDED_PLAYBACK_DIRECTORIES_PER_CHAN; i++) {
        m_playback[i].busy = false;
        m_playback[i].diropen = false;
        m_playback[i].counted = false;
        m_playback[i].num_ts = 0;
        m_playback[i].pending_file[0] = '\0';
    }

    // Allocate and initialize per-channel resources
    U32 j, k;
    CfdpHistory* history;
    CfdpTransaction* txn;
    CfdpChunkWrapper* cw;
    CfdpCListNode** list_head;
    U32 chunk_mem_offset = 0;
    U32 total_chunks_needed;

    // Initialize chunk configuration for this channel
    const U32 rxChunksPerChannel[] = CFDP_CHANNEL_NUM_RX_CHUNKS_PER_TRANSACTION;
    const U32 txChunksPerChannel[] = CFDP_CHANNEL_NUM_TX_CHUNKS_PER_TRANSACTION;
    m_dirMaxChunks[CFDP_DIRECTION_RX] = rxChunksPerChannel[m_channelId];
    m_dirMaxChunks[CFDP_DIRECTION_TX] = txChunksPerChannel[m_channelId];

    // Calculate total chunks needed for this channel
    total_chunks_needed = 0;
    for (k = 0; k < CFDP_DIRECTION_NUM; ++k) {
        total_chunks_needed += m_dirMaxChunks[k] * CFDP_NUM_TRANSACTIONS_PER_CHANNEL;
    }

    // Allocate arrays
    // Use operator new for raw memory (for types requiring placement new with constructor params)
    m_transactions = static_cast<CfdpTransaction*>(
        ::operator new(CFDP_NUM_TRANSACTIONS_PER_CHANNEL * sizeof(CfdpTransaction))
    );
    m_chunks = static_cast<CfdpChunkWrapper*>(
        ::operator new((CFDP_NUM_TRANSACTIONS_PER_CHANNEL * CFDP_DIRECTION_NUM) * sizeof(CfdpChunkWrapper))
    );
    // Regular new for simple types
    m_histories = new CfdpHistory[CFDP_NUM_HISTORIES_PER_CHANNEL];
    m_chunkMem = new CfdpChunk[total_chunks_needed];

    // Initialize transactions using placement new with parameterized constructor
    cw = m_chunks;
    for (j = 0; j < CFDP_NUM_TRANSACTIONS_PER_CHANNEL; ++j)
    {
        // Construct transaction in-place with parameterized constructor
        txn = new (&m_transactions[j]) CfdpTransaction(this, m_channelId, m_engine, m_cfdpManager);

        // Put transaction on free list
        this->freeTransaction(txn);

        // Initialize chunk wrappers for this transaction (TX and RX)
        for (k = 0; k < CFDP_DIRECTION_NUM; ++k, ++cw)
        {
            list_head = this->getChunkListHead(static_cast<U8>(k));

            // Use placement new to construct CfdpChunkWrapper with the new class-based interface
            new (cw) CfdpChunkWrapper(static_cast<CfdpChunkIdx>(m_dirMaxChunks[k]), &m_chunkMem[chunk_mem_offset]);
            chunk_mem_offset += m_dirMaxChunks[k];
            CfdpCListInitNode(&cw->cl_node);
            CfdpCListInsertBack(list_head, &cw->cl_node);
        }
    }

    // Initialize histories
    for (j = 0; j < CFDP_NUM_HISTORIES_PER_CHANNEL; ++j)
    {
        history = &m_histories[j];
        // Zero-initialize using aggregate initialization
        *history = {};
        CfdpCListInitNode(&history->cl_node);
        this->insertBackInQueue(Cfdp::QueueId::HIST_FREE, &history->cl_node);
    }
}

CfdpChannel::~CfdpChannel()
{
    // Free dynamically allocated resources
    if (m_transactions != nullptr) {
        // Manually call destructors since we used placement new
        for (U32 j = 0; j < CFDP_NUM_TRANSACTIONS_PER_CHANNEL; ++j) {
            m_transactions[j].~CfdpTransaction();
        }
        // Free raw memory allocated with operator new
        ::operator delete(m_transactions);
        m_transactions = nullptr;
    }
    if (m_histories != nullptr) {
        delete[] m_histories;
        m_histories = nullptr;
    }
    if (m_chunks != nullptr) {
        // Manually call destructors since we used placement new
        for (U32 j = 0; j < (CFDP_NUM_TRANSACTIONS_PER_CHANNEL * CFDP_DIRECTION_NUM); ++j) {
            m_chunks[j].~CfdpChunkWrapper();
        }
        // Free raw memory allocated with operator new
        ::operator delete(m_chunks);
        m_chunks = nullptr;
    }
    if (m_chunkMem != nullptr) {
        delete[] m_chunkMem;
        m_chunkMem = nullptr;
    }
}

// ----------------------------------------------------------------------
// Channel Processing
// ----------------------------------------------------------------------

void CfdpChannel::cycleTx()
{
    CfdpTransaction* txn;
    CfdpCycleTxArgs args;

    if (m_cfdpManager->getDequeueEnabledParam(m_channelId))
    {
        args.chan = this;
        args.ran_one = 0;

        /* loop through as long as there are pending transactions, and a message buffer to send their PDUs on */

        /* NOTE: tick processing is higher priority than sending new filedata PDUs, so only send however many
         * PDUs that can be sent once we get to here */
        if (!this->m_cur)
        { /* don't enter if cur is set, since we need to pick up where we left off on tick processing next wakeup */

            // TODO BPC: refactor all while loops
            while (true)
            {
                /* Attempt to run something on TXA */
                CfdpCListTraverse(m_qs[Cfdp::QueueId::TXA],
                                  [this](CfdpCListNode* node, void* context) -> CfdpCListTraverseStatus {
                                      return this->cycleTxFirstActive(node, context);
                                  },
                                  &args);

                /* Keep going until Cfdp::QueueId::PEND is empty or something is run */
                if (args.ran_one || m_qs[Cfdp::QueueId::PEND] == NULL)
                {
                    break;
                }

                txn = container_of_cpp(m_qs[Cfdp::QueueId::PEND], &CfdpTransaction::m_cl_node);

                /* Class 2 transactions need a chunklist for NAK processing, get one now.
                 * Class 1 transactions don't need chunks since they don't support NAKs. */
                if (txn->getClass() == Cfdp::Class::CLASS_2)
                {
                    if (txn->m_chunks == NULL)
                    {
                        txn->m_chunks = this->findUnusedChunks(CFDP_DIRECTION_TX);
                    }
                    if (txn->m_chunks == NULL)
                    {
                        // TODO BPC: Emit EVR
                        // Leave transaction pending until a chunklist is available.
                        break;
                    }
                }

                m_engine->armInactTimer(txn);
                this->moveTransaction(txn, Cfdp::QueueId::TXA);
            }
        }

        /* in case the loop exited due to no message buffers, clear it and start from the top next time */
        this->m_cur = NULL;
    }
}

void CfdpChannel::tickTransactions()
{
    bool reset = true;

    void (CfdpTransaction::*fns[CFDP_TICK_TYPE_NUM_TYPES])(int*) = {&CfdpTransaction::rTick, &CfdpTransaction::sTick,
                                                                  &CfdpTransaction::sTickNak};
    int qs[CFDP_TICK_TYPE_NUM_TYPES]                              = {Cfdp::QueueId::RX, Cfdp::QueueId::TXW, Cfdp::QueueId::TXW};

    FW_ASSERT(m_tickType < CFDP_TICK_TYPE_NUM_TYPES, m_tickType);

    for (; m_tickType < CFDP_TICK_TYPE_NUM_TYPES; ++m_tickType)
    {
        CfdpTickArgs args = {this, fns[m_tickType], 0, 0};

        do
        {
            args.cont = 0;
            CfdpCListTraverse(m_qs[qs[m_tickType]],
                              [this](CfdpCListNode* node, void* context) -> CfdpCListTraverseStatus {
                                  return this->doTick(node, context);
                              },
                              &args);

            if (args.early_exit)
            {
                /* early exit means we ran out of available outgoing messages this wakeup.
                 * If current tick type is NAK response, then reset tick type. It would be
                 * bad to let NAK response starve out RX or TXW ticks on the next cycle.
                 *
                 * If RX ticks use up all available messages, then we pick up where we left
                 * off on the next cycle. (This causes some RX tick counts to be missed,
                 * but that's ok. Precise timing isn't required.)
                 *
                 * This scheme allows the following priority for use of outgoing messages:
                 *
                 * RX state messages
                 * TXW state messages
                 * NAK response (could be many)
                 *
                 * New file data on TXA
                 */
                if (m_tickType != CFDP_TICK_TYPE_TXW_NAK)
                {
                    reset = false;
                }

                break;
            }
        }
        while (args.cont);

        if (!reset)
        {
            break;
        }
    }

    if (reset)
    {
        m_tickType = CFDP_TICK_TYPE_RX; /* reset tick type */
    }
}

void CfdpChannel::processPlaybackDirectories()
{
    U32 i;
    // const int chan_index = (m_channel - m_engine->m_engineData.channels);

    for (i = 0; i < CFDP_MAX_COMMANDED_PLAYBACK_DIRECTORIES_PER_CHAN; ++i)
    {
        this->processPlaybackDirectory(&m_playback[i]);
        // this->updatePollPbCounted(&m_playback[i], m_playback[i].busy,
        //                             &CF_AppData.hk.Payload.channel_hk[chan_index].playback_counter);
    }
}

void CfdpChannel::processPollingDirectories()
{
    CfdpPollDir* pd;
    U32 i;
    // TODO BPC: count_check is only used for telemetry
    // I32 count_check;
    Cfdp::Status::T status;

    for (i = 0; i < CFDP_MAX_POLLING_DIR_PER_CHAN; ++i)
    {
        pd = &m_polldir[i];
        // count_check = 0;

        if (pd->enabled)
        {
            if ((pd->pb.busy == false) && (pd->pb.num_ts == 0))
            {
                if ((pd->intervalTimer.getStatus() != CfdpTimer::Status::RUNNING) && (pd->intervalSec > 0))
                {
                    /* timer was not set, so set it now */
                    pd->intervalTimer.setTimer(pd->intervalSec);
                }
                else if (pd->intervalTimer.getStatus() == CfdpTimer::Status::EXPIRED)
                {
                    /* the timer has expired */
                    status = m_engine->playbackDirInitiate(&pd->pb, pd->srcDir, pd->dstDir, pd->cfdpClass,
                                                          Cfdp::Keep::DELETE, m_channelId, pd->priority,
                                                          pd->destEid);
                    if (status != Cfdp::Status::SUCCESS)
                    {
                        /* error occurred in playback directory, so reset the timer */
                        /* an event is sent when initiating playback directory so there is no reason to
                         * to have another here */
                        pd->intervalTimer.setTimer(pd->intervalSec);
                    }
                }
                else
                {
                    pd->intervalTimer.run();
                }
            }
            else
            {
                /* playback is active, so step it */
                this->processPlaybackDirectory(&pd->pb);
            }

            // count_check = 1;
        }

        // this->updatePollPbCounted(&poll->pb, count_check, &CF_AppData.hk.Payload.channel_hk[chan_index].poll_counter);
    }
}

// ----------------------------------------------------------------------
// Transaction Management
// ----------------------------------------------------------------------

CfdpTransaction* CfdpChannel::findUnusedTransaction(CfdpDirection direction)
{
    CfdpCListNode*  node;
    CfdpTransaction* txn;
    Cfdp::QueueId::T q_index; /* initialized below in if */

    if (m_qs[Cfdp::QueueId::FREE])
    {
        node = m_qs[Cfdp::QueueId::FREE];
        txn = container_of_cpp(node, &CfdpTransaction::m_cl_node);

        this->removeFromQueue(Cfdp::QueueId::FREE, &txn->m_cl_node);

        /* now that a transaction is acquired, must also acquire a history slot to go along with it */
        if (m_qs[Cfdp::QueueId::HIST_FREE])
        {
            q_index = Cfdp::QueueId::HIST_FREE;
        }
        else
        {
            /* no free history, so take the oldest one from the channel's history queue */
            FW_ASSERT(m_qs[Cfdp::QueueId::HIST]);
            q_index = Cfdp::QueueId::HIST;
        }

        txn->m_history = container_of_cpp(m_qs[q_index], &CfdpHistory::cl_node);

        this->removeFromQueue(q_index, &txn->m_history->cl_node);

        /* Indicate that this was freshly pulled from the free list */
        /* notably this state is distinguishable from items still on the free list */
        txn->m_state        = CFDP_TXN_STATE_INIT;
        txn->m_history->dir = direction;
        txn->m_chan         = this;  /* Set channel pointer */

        /* Re-initialize the linked list node to clear stale pointers from FREE list */
        CfdpCListInitNode(&txn->m_cl_node);
    }
    else
    {
        txn = NULL;
    }

    return txn;
}

CfdpTransaction* CfdpChannel::findTransactionBySequenceNumber(CfdpTransactionSeq transaction_sequence_number,
                                                               CfdpEntityId src_eid)
{
    /* need to find transaction by sequence number. It will either be the active transaction (front of Q_PEND),
     * or on Q_TX or Q_RX. Once a transaction moves to history, then it's done.
     *
     * Let's put Cfdp::QueueId::RX up front, because most RX packets will be file data PDUs */
    CfdpTraverseTransSeqArg ctx    = {transaction_sequence_number, src_eid, NULL};
    CfdpCListNode*          ptrs[] = {m_qs[Cfdp::QueueId::RX], m_qs[Cfdp::QueueId::PEND], m_qs[Cfdp::QueueId::TXA],
                              m_qs[Cfdp::QueueId::TXW]};
    CfdpTransaction*        ret = NULL;

    for (CfdpCListNode* head : ptrs)
    {
        CfdpCListTraverse(head, CfdpTransaction::findBySequenceNumberCallback, &ctx);
        if (ctx.txn)
        {
            ret = ctx.txn;
            break;
        }
    }

    return ret;
}

I32 CfdpChannel::traverseAllTransactions(CfdpTraverseAllTransactionsFunc fn, void* context)
{
    CfdpTraverseAllArg args = {fn, context, 0};
    for (I32 queueidx = Cfdp::QueueId::PEND; queueidx <= Cfdp::QueueId::RX; ++queueidx)
    {
        CfdpCListTraverse(m_qs[queueidx],
                          [&args](CfdpCListNode* node, void*) -> CfdpCListTraverseStatus {
                              CfdpTransaction* txn = container_of_cpp(node, &CfdpTransaction::m_cl_node);
                              args.fn(txn, args.context);
                              ++args.counter;
                              return CFDP_CLIST_TRAVERSE_CONTINUE;
                          },
                          nullptr);
    }

    return args.counter;
}

void CfdpChannel::resetHistory(CfdpHistory* history)
{
    this->removeFromQueue(Cfdp::QueueId::HIST, &history->cl_node);
    this->insertBackInQueue(Cfdp::QueueId::HIST_FREE, &history->cl_node);
}

// ----------------------------------------------------------------------
// Transaction Queue Management
// ----------------------------------------------------------------------

void CfdpChannel::dequeueTransaction(CfdpTransaction* txn)
{
    FW_ASSERT(txn);
    CfdpCListRemove(&m_qs[txn->m_flags.com.q_index], &txn->m_cl_node);
    // FW_ASSERT(CF_AppData.hk.Payload.channel_hk[txn->chan_num].q_size[txn->flags.com.q_index]); /* sanity check */
    // --CF_AppData.hk.Payload.channel_hk[txn->chan_num].q_size[txn->flags.com.q_index];
}

void CfdpChannel::moveTransaction(CfdpTransaction* txn, Cfdp::QueueId::T queue)
{
    FW_ASSERT(txn);
    CfdpCListRemove(&m_qs[txn->m_flags.com.q_index], &txn->m_cl_node);
    // FW_ASSERT(CF_AppData.hk.Payload.channel_hk[txn->chan_num].q_size[txn->flags.com.q_index]); /* sanity check */
    // --CF_AppData.hk.Payload.channel_hk[txn->chan_num].q_size[txn->flags.com.q_index];
    CfdpCListInsertBack(&m_qs[queue], &txn->m_cl_node);
    txn->m_flags.com.q_index = queue;
    // ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].q_size[txn->flags.com.q_index];
}

void CfdpChannel::freeTransaction(CfdpTransaction* txn)
{
    // Reset transaction to default state (preserves channel context)
    txn->reset();

    // Initialize the linked list node for the FREE queue
    CfdpCListInitNode(&txn->m_cl_node);
    this->insertBackInQueue(Cfdp::QueueId::FREE, &txn->m_cl_node);
}

void CfdpChannel::recycleTransaction(CfdpTransaction *txn)
{
    CfdpCListNode **chunklist_head;
    Cfdp::QueueId::T    hist_destq;

    /* File should have been closed by the state machine, but if
     * it still hanging open at this point, close it now so its not leaked.
     * This is not normal/expected so log it if this happens. */
    if (true == txn->m_fd.isOpen())
    {
        // CFE_ES_WriteToSysLog("%s(): Closing dangling file handle: %lu\n", __func__, OS_ObjectIdToInteger(txn->fd));
        txn->m_fd.close();
    }

    this->dequeueTransaction(txn); /* this makes it "float" (not in any queue) */

    /* this should always be */
    if (txn->m_history != NULL)
    {
        if (txn->m_chunks != NULL)
        {
            chunklist_head = this->getChunkListHead(txn->m_history->dir);
            if (chunklist_head != NULL)
            {
                CfdpCListInsertBack(chunklist_head, &txn->m_chunks->cl_node);
                txn->m_chunks = NULL;
            }
        }

        if (txn->m_flags.com.keep_history)
        {
            /* move transaction history to history queue */
            hist_destq = Cfdp::QueueId::HIST;
        }
        else
        {
            hist_destq = Cfdp::QueueId::HIST_FREE;
        }
        this->insertBackInQueue(hist_destq, &txn->m_history->cl_node);
        txn->m_history = NULL;
    }

    /* this wipes it and puts it back onto the list to be found by
     * CfdpChannel::findUnusedTransaction().  Need to preserve the chan_num
     * and keep it associated with this channel, though. */
    this->freeTransaction(txn);
}

void CfdpChannel::insertSortPrio(CfdpTransaction* txn, Cfdp::QueueId::T queue)
{
    bool insert_back = false;

    FW_ASSERT(txn);

    /* look for proper position on PEND queue for this transaction.
     * This is a simple priority sort. */

    if (!m_qs[queue])
    {
        /* list is empty, so just insert */
        insert_back = true;
    }
    else
    {
        CfdpTraversePriorityArg arg = {NULL, txn->getPriority()};
        CfdpCListTraverseR(m_qs[queue], CfdpTransaction::prioritySearchCallback, &arg);
        if (arg.txn)
        {
            this->insertAfterInQueue(queue, &arg.txn->m_cl_node, &txn->m_cl_node);
        }
        else
        {
            insert_back = true;
        }
    }

    if (insert_back)
    {
        this->insertBackInQueue(queue, &txn->m_cl_node);
    }
    txn->m_flags.com.q_index = queue;
}

// ----------------------------------------------------------------------
// Channel State Management
// ----------------------------------------------------------------------

void CfdpChannel::decrementCmdTxCounter()
{
    FW_ASSERT(m_numCmdTx); // sanity check
    --m_numCmdTx;
}

void CfdpChannel::clearCurrentIfMatch(CfdpTransaction* txn)
{
    // Done with this TX transaction
    if (this->m_cur == txn)
    {
        this->m_cur = NULL;
    }
}

// ----------------------------------------------------------------------
// Resource Management
// ----------------------------------------------------------------------

CfdpCListNode** CfdpChannel::getChunkListHead(U8 direction)
{
    CfdpCListNode** result;

    if (direction < CFDP_DIRECTION_NUM)
    {
        result = &m_cs[direction];
    }
    else
    {
        result = NULL;
    }

    return result;
}

CfdpChunkWrapper* CfdpChannel::findUnusedChunks(CfdpDirection dir)
{
    CfdpChunkWrapper* ret = NULL;
    CfdpCListNode* node;
    CfdpCListNode** chunklist_head;

    chunklist_head = this->getChunkListHead(dir);

    /* this should never be null */
    FW_ASSERT(chunklist_head);

    if (*chunklist_head != NULL)
    {
        node = CfdpCListPop(chunklist_head);
        if (node != NULL)
        {
            ret = container_of_cpp(node, &CfdpChunkWrapper::cl_node);
        }
    }

    return ret;
}

// ----------------------------------------------------------------------
// Private helper methods
// ----------------------------------------------------------------------

void CfdpChannel::processPlaybackDirectory(CfdpPlayback* pb)
{
    CfdpTransaction* txn;
    char path[CfdpManagerMaxFileSize];
    Os::Directory::Status status;

    /* either there's no transaction (first one) or the last one was finished, so check for a new one */

    memset(&path, 0, sizeof(path));

    while (pb->diropen && (pb->num_ts < CFDP_NUM_TRANSACTIONS_PER_PLAYBACK))
    {
        if (pb->pending_file[0] == 0)
        {
            status = pb->dir.read(path, CfdpManagerMaxFileSize);
            if (status == Os::Directory::NO_MORE_FILES)
            {
                // TODO BPC: Emit playback success EVR
                pb->dir.close();
                pb->diropen = false;
                break;
            }
            if (status != Os::Directory::OP_OK)
            {
                // TODO BPC: emit playback error EVR
                pb->dir.close();
                pb->diropen = false;
                break;
            }

            strncpy(pb->pending_file, path, sizeof(pb->pending_file) - 1);
            pb->pending_file[sizeof(pb->pending_file) - 1] = 0;
        }
        else
        {
            txn = this->findUnusedTransaction(CFDP_DIRECTION_TX);
            if (txn == NULL)
            {
                /* while not expected this can certainly happen, because
                 * rx transactions consume in these as well. */
                /* should not need to do anything special, will come back next tick */
                break;
            }

            // Append file name to source/destination folders
            txn->m_history->fnames.src_filename = pb->fnames.src_filename;
            txn->m_history->fnames.src_filename += "/";
            txn->m_history->fnames.src_filename += pb->pending_file;

            txn->m_history->fnames.dst_filename = pb->fnames.dst_filename;
            txn->m_history->fnames.dst_filename += "/";
            txn->m_history->fnames.dst_filename += pb->pending_file;

            m_engine->txFileInitiate(txn, pb->cfdp_class, pb->keep, m_channelId, pb->priority,
                                    pb->dest_id);

            txn->m_pb = pb;
            ++pb->num_ts;

            pb->pending_file[0] = 0; /* continue reading dir */
        }
    }

    if (!pb->diropen && !pb->num_ts)
    {
        /* the directory has been exhausted, and there are no more active transactions
         * for this playback -- so mark it as not busy */
        pb->busy = false;
    }
}

void CfdpChannel::updatePollPbCounted(CfdpPlayback* pb, int up, U8* counter)
{
    if (pb->counted != up)
    {
        /* only handle on state change */
        pb->counted = !!up; /* !! ensure 0 or 1, should be optimized out */

        if (up)
        {
            ++*counter;
        }
        else
        {
            FW_ASSERT(*counter); /* sanity check it isn't zero */
            --*counter;
        }
    }
}

CfdpCListTraverseStatus CfdpChannel::cycleTxFirstActive(CfdpCListNode* node, void* context)
{
    CfdpCycleTxArgs*  args = static_cast<CfdpCycleTxArgs*>(context);
    CfdpTransaction*        txn  = container_of_cpp(node, &CfdpTransaction::m_cl_node);
    CfdpCListTraverseStatus ret  = CFDP_CLIST_TRAVERSE_EXIT; /* default option is exit traversal */

    if (txn->m_flags.com.suspended)
    {
        ret = CFDP_CLIST_TRAVERSE_CONTINUE; /* suspended, so move on to next */
    }
    else
    {
        FW_ASSERT(txn->m_flags.com.q_index == Cfdp::QueueId::TXA); /* huh? */

        /* if no more messages, then chan->m_cur will be set.
         * If the transaction sent the last filedata PDU and EOF, it will move itself
         * off the active queue. Run until either of these occur. */
        while (!this->m_cur && txn->m_flags.com.q_index == Cfdp::QueueId::TXA)
        {
            m_engine->dispatchTx(txn);
        }

        args->ran_one = 1;
    }

    return ret;
}

CfdpCListTraverseStatus CfdpChannel::doTick(CfdpCListNode* node, void* context)
{
    CfdpCListTraverseStatus ret  = CFDP_CLIST_TRAVERSE_CONTINUE; /* CFDP_CLIST_TRAVERSE_CONTINUE means don't tick one, keep looking for cur */
    CfdpTickArgs*     args = static_cast<CfdpTickArgs*>(context);
    CfdpTransaction*        txn  = container_of_cpp(node, &CfdpTransaction::m_cl_node);
    if (!this->m_cur || (this->m_cur == txn))
    {
        /* found where we left off, so clear that and move on */
        this->m_cur = NULL;
        if (!txn->m_flags.com.suspended)
        {
            (txn->*args->fn)(&args->cont);
        }

        /* if this->m_cur was set to not-NULL above, then exit early */
        /* NOTE: if channel is frozen, then tick processing won't have been entered.
         *     so there is no need to check it here */
        if (this->m_cur)
        {
            ret              = CFDP_CLIST_TRAVERSE_EXIT;
            args->early_exit = true;
        }
    }

    return ret; /* don't tick one, keep looking for cur */
}

CfdpTransaction* CfdpChannel::getTransaction(U32 index)
{
    FW_ASSERT(index < CFDP_NUM_TRANSACTIONS_PER_CHANNEL);
    return &m_transactions[index];
}

CfdpHistory* CfdpChannel::getHistory(U32 index)
{
    FW_ASSERT(index < CFDP_NUM_HISTORIES_PER_CHANNEL);
    return &m_histories[index];
}

}  // namespace Ccsds
}  // namespace Svc

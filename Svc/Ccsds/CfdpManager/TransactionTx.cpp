// ======================================================================
// \title  TransactionTx.cpp
// \brief  cpp file for CFDP TX Transaction state machine
//
// This file is a port of TX transaction state machine operations from the following files
// from the NASA Core Flight System (cFS) CFDP (CF) Application, version 3.0.0,
// adapted for use within the F-Prime (F') framework:
// - cf_cfdp_s.c (send-file transaction state handling routines)
// - cf_cfdp_dispatch.c (TX state machine dispatch functions)
//
// This file contains various state handling routines for
// transactions which are sending a file, as well as dispatch
// functions for TX state machines and top-level transaction dispatch.
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

#include <stdio.h>
#include <string.h>

#include <Svc/Ccsds/CfdpManager/Transaction.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpManager.hpp>
#include <Svc/Ccsds/CfdpManager/Engine.hpp>
#include <Svc/Ccsds/CfdpManager/Channel.hpp>
#include <Svc/Ccsds/CfdpManager/Chunk.hpp>
#include <Svc/Ccsds/CfdpManager/Utils.hpp>
#include <Svc/Ccsds/CfdpManager/Timer.hpp>

namespace Svc {
namespace Ccsds {
namespace Cfdp {

// ======================================================================
// TX State Machine - Private Helper (anonymous namespace)
// ======================================================================

namespace {

// Helper to build dispatch tables
FileDirectiveDispatchTable makeFileDirectiveTable(
    StateRecvFunc fin,
    StateRecvFunc ack,
    StateRecvFunc nak
)
{
    FileDirectiveDispatchTable table = {};
    memset(&table, 0, sizeof(table));

    table.fdirective[FILE_DIRECTIVE_FIN] = fin;
    table.fdirective[FILE_DIRECTIVE_ACK] = ack;
    table.fdirective[FILE_DIRECTIVE_NAK] = nak;

    return table;
}

}  // anonymous namespace

// ======================================================================
// TX State Machine - Public Methods
// ======================================================================

void Transaction::s1Recv(const Fw::Buffer& buffer) {
    // s1 doesn't need to receive anything
    static const SSubstateRecvDispatchTable substate_fns = {{NULL}};
    this->sDispatchRecv(buffer, &substate_fns);
}

void Transaction::s2Recv(const Fw::Buffer& buffer) {
    static const FileDirectiveDispatchTable s2_meta =
        makeFileDirectiveTable(
            &Transaction::s2EarlyFin,
            nullptr,
            nullptr
        );

    static const FileDirectiveDispatchTable s2_fd_or_eof =
        makeFileDirectiveTable(
            &Transaction::s2EarlyFin,
            nullptr,
            &Transaction::s2Nak
        );

    static const FileDirectiveDispatchTable s2_wait_ack =
        makeFileDirectiveTable(
            &Transaction::s2Fin,
            &Transaction::s2EofAck,
            &Transaction::s2NakArm
        );

    static const SSubstateRecvDispatchTable substate_fns = {
        {
            &s2_meta,      /* TX_SUB_STATE_METADATA */
            &s2_fd_or_eof, /* TX_SUB_STATE_FILEDATA */
            &s2_fd_or_eof, /* TX_SUB_STATE_EOF */
            &s2_wait_ack   /* TX_SUB_STATE_CLOSEOUT_SYNC */
        }
    };

    this->sDispatchRecv(buffer, &substate_fns);
}

void Transaction::initTxFile(Class::T cfdp_class, Keep::T keep, U8 chan, U8 priority)
{
    m_chan_num = chan;
    m_priority = priority;
    m_keep = keep;
    m_txn_class = cfdp_class;
    m_state = (cfdp_class == Cfdp::Class::CLASS_2) ? TXN_STATE_S2 : TXN_STATE_S1;
    m_state_data.send.sub_state = TX_SUB_STATE_METADATA;
}

void Transaction::s1Tx() {
    static const SSubstateSendDispatchTable substate_fns = {{
        &Transaction::sSubstateSendMetadata, // TX_SUB_STATE_METADATA
        &Transaction::sSubstateSendFileData, // TX_SUB_STATE_FILEDATA
        &Transaction::s1SubstateSendEof, // TX_SUB_STATE_EOF
        nullptr // TX_SUB_STATE_CLOSEOUT_SYNC
    }};

    this->sDispatchTransmit(&substate_fns);
}

void Transaction::s2Tx() {
    static const SSubstateSendDispatchTable substate_fns = {{
        &Transaction::sSubstateSendMetadata, // TX_SUB_STATE_METADATA
        &Transaction::s2SubstateSendFileData, // TX_SUB_STATE_FILEDATA
        &Transaction::s2SubstateSendEof, // TX_SUB_STATE_EOF
        nullptr // TX_SUB_STATE_CLOSEOUT_SYNC
    }};

    this->sDispatchTransmit(&substate_fns);
}

void Transaction::sAckTimerTick() {
    U8 ack_limit = 0;

    // note: the ack timer is only ever relevant on class 2
    if (this->m_state != TXN_STATE_S2 || !this->m_flags.com.ack_timer_armed)
    {
        // nothing to do
        return;
    }

    if (this->m_ack_timer.getStatus() == Timer::Status::RUNNING)
    {
        this->m_ack_timer.run();
    }
    else if (this->m_state_data.send.sub_state == TX_SUB_STATE_CLOSEOUT_SYNC)
    {
        // Check limit and handle if needed
        ack_limit = this->m_cfdpManager->getAckLimitParam(this->m_chan_num);
        if (this->m_state_data.send.s2.acknak_count >= ack_limit)
        {
            this->m_cfdpManager->log_WARNING_HI_TxAckLimitReached(
                Cfdp::getClassDisplay(this->getClass()),
                this->m_history->src_eid,
                this->m_history->seq_num);
            this->m_engine->setTxnStatus(this, TXN_STATUS_ACK_LIMIT_NO_EOF);
            // ++CF_AppData.hk.Payload.channel_hk[this->m_chan_num].counters.fault.ack_limit;

            // give up on this
            this->m_engine->finishTransaction(this, true);
            this->m_flags.com.ack_timer_armed = false;
        }
        else
        {
            // Increment acknak counter
            ++this->m_state_data.send.s2.acknak_count;

            // If the peer sent FIN that is an implicit EOF ack, it is not supposed
            // to send it before EOF unless an error occurs, and either way we do not
            // re-transmit anything after FIN unless we get another FIN
            if (!this->m_flags.tx.eof_ack_recv && !this->m_flags.tx.fin_recv)
            {
                this->m_flags.tx.send_eof = true;
            }
            else
            {
                // no response is pending
                this->m_flags.com.ack_timer_armed = false;
            }
        }

        // reset the ack timer if still waiting on something
        if (this->m_flags.com.ack_timer_armed)
        {
            this->m_engine->armAckTimer(this);
        }
    }
    else
    {
        // if we are not waiting for anything, why is the ack timer armed?
        this->m_flags.com.ack_timer_armed = false;
    }
}

void Transaction::sTick(int *cont /* unused */) {
    bool pending_send;

    pending_send = true; // maybe; tbd, will be reset if not

    // at each tick, various timers used by S are checked
    // first, check inactivity timer
    if (!this->m_flags.com.inactivity_fired)
    {
        if (this->m_inactivity_timer.getStatus() == Timer::Status::RUNNING)
        {
            this->m_inactivity_timer.run();
        }
        else
        {
            this->m_flags.com.inactivity_fired = true;

            // HOLD state is the normal path to recycle transaction objects, not an error
            // inactivity is abnormal in any other state
            if (this->m_state != TXN_STATE_HOLD && this->m_state == TXN_STATE_S2)
            {
                this->m_cfdpManager->log_WARNING_HI_TxInactivityTimeout(
                    Cfdp::getClassDisplay(this->getClass()),
                    this->m_history->src_eid,
                    this->m_history->seq_num);
                this->m_engine->setTxnStatus(this, TXN_STATUS_INACTIVITY_DETECTED);

                // ++CF_AppData.hk.Payload.channel_hk[this->m_chan_num].counters.fault.inactivity_timer;
            }
        }
    }

    // tx maintenance: possibly process send_eof, or send_fin_ack
    if (this->m_flags.tx.send_eof)
    {
        if (this->sSendEof() == Cfdp::Status::SUCCESS)
        {
            this->m_flags.tx.send_eof = false;
        }
    }
    else if (this->m_flags.tx.send_fin_ack)
    {
        if (this->sSendFinAck() == Cfdp::Status::SUCCESS)
        {
            this->m_flags.tx.send_fin_ack = false;
        }
    }
    else
    {
        pending_send = false;
    }

    // if the inactivity timer ran out, then there is no sense
    // pending for responses for anything.  Send out anything
    // that we need to send (i.e. the EOF) just in case the sender
    // is still listening to us but do not expect any future ACKs
    if (this->m_flags.com.inactivity_fired && !pending_send)
    {
        // the transaction is now recyclable - this means we will
        // no longer have a record of this transaction seq.  If the sender
        // wakes up or if the network delivers severely delayed PDUs at
        // some future point, then they will be seen as spurious.  They
        // will no longer be associable with this transaction at all
        this->m_chan->recycleTransaction(this);

        // NOTE: this must be the last thing in here.  Do not use txn after this
    }
    else
    {
        // transaction still valid so process the ACK timer, if relevant
        this->sAckTimerTick();
    }
}

void Transaction::sTickNak(int *cont) {
    bool nakProcessed = false;
    Status::T status;

    // Only Class 2 transactions should process NAKs
    if (this->m_txn_class == Cfdp::Class::CLASS_2)
    {
        status = this->sCheckAndRespondNak(&nakProcessed);
        if ((status == Cfdp::Status::SUCCESS) && nakProcessed)
        {
            *cont = 1; // cause dispatcher to re-enter this wakeup
        }
    }
}

void Transaction::sCancel() {
    if (this->m_state_data.send.sub_state < TX_SUB_STATE_EOF)
    {
        // if state has not reached TX_SUB_STATE_EOF, then set it to TX_SUB_STATE_EOF now.
        this->m_state_data.send.sub_state = TX_SUB_STATE_EOF;
    }
}

// ======================================================================
// TX State Machine - Private Helper Methods
// ======================================================================

Status::T Transaction::sSendEof() {
    // note the crc is "finalized" regardless of success or failure of the txn
    // this is OK as we still need to put some value into the EOF
    if (!this->m_flags.com.crc_calc)
    {
        // The F' version does not have an equivalent finalize call as it
        // - Never stores a partial word internally
        // - Never needs to "flush" anything
        // - Always accounts for padding at update time
        this->m_flags.com.crc_calc = true;
    }
    return this->m_engine->sendEof(this);
}

void Transaction::s1SubstateSendEof() {
    // set the flag, the EOF is sent by the tick handler
    this->m_flags.tx.send_eof = true;

    // In class 1 this is the end of normal operation
    // NOTE: this is not always true, as class 1 can request an EOF ack.
    // In this case we could change state to CLOSEOUT_SYNC instead and wait,
    // but right now we do not request an EOF ack in S1
    this->m_engine->finishTransaction(this, true);
}

void Transaction::s2SubstateSendEof() {
    // set the flag, the EOF is sent by the tick handler
    this->m_flags.tx.send_eof = true;

    // wait for remaining responses to close out the state machine
    this->m_state_data.send.sub_state = TX_SUB_STATE_CLOSEOUT_SYNC;

    // always move the transaction onto the wait queue now
    this->m_chan->dequeueTransaction(this);
    this->m_chan->insertSortPrio(this, QueueId::TXW);

    // the ack timer is armed in class 2 only
    this->m_engine->armAckTimer(this);
}

Status::T Transaction::sSendFileData(FileSize foffs, FileSize bytes_to_read, U8 calc_crc, FileSize* bytes_processed) {
    FW_ASSERT(bytes_processed != NULL);
    *bytes_processed = 0;

    Status::T status = Cfdp::Status::SUCCESS;

    // Local buffer for file data
    U8 fileDataBuffer[MaxPduSize];

    // Create File Data PDU
    FileDataPdu fdPdu;
    Cfdp::PduDirection direction = DIRECTION_TOWARD_RECEIVER;

    // Calculate maximum data size we can send, accounting for PDU overhead
    U32 maxDataCapacity = fdPdu.getMaxFileDataSize();

    // Limited by: bytes_to_read, outgoing_file_chunk_size, and maxDataCapacity
    FileSize outgoing_file_chunk_size = this->m_cfdpManager->getOutgoingFileChunkSizeParam();
    FileSize max_data_bytes = bytes_to_read;
    if (max_data_bytes > outgoing_file_chunk_size) {
        max_data_bytes = outgoing_file_chunk_size;
    }
    if (max_data_bytes > maxDataCapacity) {
        max_data_bytes = maxDataCapacity;
    }

    // Seek to file offset if needed
    FwSizeType actual_bytes = max_data_bytes;
    if (status == Cfdp::Status::SUCCESS) {
        if (this->m_state_data.send.cached_pos != foffs) {
            Os::File::Status fileStatus = this->m_fd.seek(foffs, Os::File::SeekType::ABSOLUTE);
            if (fileStatus != Os::File::OP_OK) {
                status = Cfdp::Status::ERROR;
            }
        }
    }

    // Read file data
    if (status == Cfdp::Status::SUCCESS) {
        Os::File::Status fileStatus = this->m_fd.read(fileDataBuffer, actual_bytes, Os::File::WaitType::WAIT);
        if (fileStatus != Os::File::OP_OK) {
            status = Cfdp::Status::ERROR;
        }
    }

    // Initialize and send PDU
    if (status == Cfdp::Status::SUCCESS) {
        fdPdu.initialize(
            direction,
            this->getClass(),  // transmission mode
            this->m_cfdpManager->getLocalEidParam(),  // source EID
            this->m_history->seq_num,  // transaction sequence number
            this->m_history->peer_eid,  // destination EID
            foffs,  // file offset
            static_cast<U16>(actual_bytes),  // data size
            fileDataBuffer  // data pointer
        );

        status = this->m_engine->sendFd(this, fdPdu);
    }

    // Update state and CRC
    if (status == Cfdp::Status::SUCCESS) {
        this->m_state_data.send.cached_pos += static_cast<FileSize>(actual_bytes);

        FW_ASSERT((foffs + actual_bytes) <= this->m_fsize, foffs, static_cast<FwAssertArgType>(actual_bytes), this->m_fsize);

        if (calc_crc) {
            this->m_crc.update(fileDataBuffer, foffs, static_cast<U32>(actual_bytes));
        }

        *bytes_processed = static_cast<U32>(actual_bytes);
    }

    return status;
}

void Transaction::sSubstateSendFileData() {
    FileSize bytes_processed = 0;
    Status::T status = this->sSendFileData(this->m_foffs, (this->m_fsize - this->m_foffs), 1, &bytes_processed);

    if(status != Cfdp::Status::SUCCESS)
    {
        // IO error -- change state and send EOF
        this->m_engine->setTxnStatus(this, TXN_STATUS_FILESTORE_REJECTION);
        this->m_state_data.send.sub_state = TX_SUB_STATE_EOF;
    }
    else if (bytes_processed > 0)
    {
        this->m_foffs += bytes_processed;
        if (this->m_foffs == this->m_fsize)
        {
            // file is done
            this->m_state_data.send.sub_state = TX_SUB_STATE_EOF;
        }
    }
    else
    {
        // don't care about other cases
    }
}

Status::T Transaction::sCheckAndRespondNak(bool* nakProcessed) {
    const Chunk *chunk;
    Status::T sret;
    Status::T ret = Cfdp::Status::SUCCESS;
    FileSize bytes_processed = 0;

    FW_ASSERT(nakProcessed != NULL);
    *nakProcessed = false;

    // Class 2 transactions must have had chunks allocated
    FW_ASSERT(this->m_chunks != NULL);

    if (this->m_flags.tx.md_need_send)
    {
        sret = this->m_engine->sendMd(this);
        if (sret == Cfdp::Status::SEND_PDU_ERROR)
        {
            ret = Cfdp::Status::ERROR; // error occurred
        }
        else
        {
            if (sret == Cfdp::Status::SUCCESS)
            {
                this->m_flags.tx.md_need_send = false;
            }
            // unless SEND_PDU_ERROR, return 1 to keep caller from sending file data
            *nakProcessed = true; // nak processed, so don't send filedata

        }
    }
    else
    {
        // Get first chunk and process if available
        chunk = this->m_chunks->chunks.getFirstChunk();
        if (chunk != nullptr)
        {
            ret = this->sSendFileData(chunk->offset, chunk->size, 0, &bytes_processed);
            if(ret != Cfdp::Status::SUCCESS)
            {
                // error occurred
                ret = Cfdp::Status::ERROR; // error occurred
            }
            else if (bytes_processed > 0)
            {
                this->m_chunks->chunks.removeFromFirst(bytes_processed);
                *nakProcessed = true; // nak processed, so caller doesn't send file data
            }
        }
    }

    return ret;
}

void Transaction::s2SubstateSendFileData() {
    Status::T status;
    bool nakProcessed = false;

    status = this->sCheckAndRespondNak(&nakProcessed);
    if (status != Cfdp::Status::SUCCESS)
    {
        this->m_engine->setTxnStatus(this, TXN_STATUS_NAK_RESPONSE_ERROR);
        this->m_flags.tx.send_eof = true; /* do not leave the remote hanging */
        this->m_engine->finishTransaction(this, true);
        return;
    }

    if (!nakProcessed)
    {
        this->sSubstateSendFileData();
    }
    else
    {
        // NAK was processed, so do not send filedata
    }
}

void Transaction::sSubstateSendMetadata() {
    Status::T status;
    Os::File::Status fileStatus;
    bool success = true;

    if (false == this->m_fd.isOpen())
    {
        fileStatus = this->m_fd.open(this->m_history->fnames.src_filename.toChar(), Os::File::OPEN_READ);
        if (fileStatus != Os::File::OP_OK)
        {
            this->m_cfdpManager->log_WARNING_HI_TxFileOpenFailed(
                Cfdp::getClassDisplay(this->getClass()),
                this->m_history->src_eid,
                this->m_history->seq_num,
                this->m_history->fnames.src_filename,
                fileStatus);
            // ++CF_AppData.hk.Payload.channel_hk[this->m_chan_num].counters.fault.file_open;
            // this->m_fd = OS_OBJECT_ID_UNDEFINED; /* just in case */
            success = false;
        }

        if (success)
        {
            FwSizeType file_size;
            fileStatus = this->m_fd.size(file_size);
            this->m_fsize = static_cast<FileSize>(file_size);
            if (fileStatus != Os::File::Status::OP_OK)
            {
                this->m_cfdpManager->log_WARNING_HI_TxFileSeekFailed(
                    Cfdp::getClassDisplay(this->getClass()),
                    this->m_history->src_eid,
                    this->m_history->seq_num,
                    fileStatus);
                // ++CF_AppData.hk.Payload.channel_hk[this->m_chan_num].counters.fault.file_seek;
                success = false;
            }
            else
            {
                // Check that file size is well formed
                FW_ASSERT(this->m_fsize > 0, this->m_fsize);
            }
        }
    }

    if (success)
    {
        status = this->m_engine->sendMd(this);
        if (status == Cfdp::Status::SEND_PDU_ERROR)
        {
            /* failed to send md */
            this->m_cfdpManager->log_WARNING_HI_TxSendMetadataFailed(
                Cfdp::getClassDisplay(this->getClass()),
                this->m_history->src_eid,
                this->m_history->seq_num);
            success = false;
        }
        else if (status == Cfdp::Status::SUCCESS)
        {
            /* once metadata is sent, switch to filedata mode */
            this->m_state_data.send.sub_state = TX_SUB_STATE_FILEDATA;
        }
        /* if status==Cfdp::Status::SEND_PDU_NO_BUF_AVAIL_ERROR, then try to send md again next cycle */
    }

    if (!success)
    {
        this->m_engine->setTxnStatus(this, TXN_STATUS_FILESTORE_REJECTION);
        this->m_engine->finishTransaction(this, true);
    }

    // don't need to reset the CRC since its taken care of by reset_cfdp()
}

Status::T Transaction::sSendFinAck() {
    Status::T ret = this->m_engine->sendAck(this,
                           static_cast<AckTxnStatus>(GetTxnStatus(this)),
                           FILE_DIRECTIVE_FIN,
                           static_cast<ConditionCode>(this->m_state_data.send.s2.fin_cc),
                           this->m_history->peer_eid, this->m_history->seq_num);
    return ret;
}

void Transaction::s2EarlyFin(const Fw::Buffer& buffer) {
    // received early fin, so just cancel
    this->m_cfdpManager->log_WARNING_HI_TxEarlyFinReceived(
        Cfdp::getClassDisplay(this->getClass()),
        this->m_history->src_eid,
        this->m_history->seq_num);
    this->m_engine->setTxnStatus(this, TXN_STATUS_EARLY_FIN);

    this->m_state_data.send.sub_state = TX_SUB_STATE_CLOSEOUT_SYNC;

    // otherwise do normal fin processing
    this->s2Fin(buffer);
}

void Transaction::s2Fin(const Fw::Buffer& buffer) {
    // Deserialize FIN PDU from buffer
    FinPdu fin;
    Fw::SerialBuffer sb(const_cast<U8*>(buffer.getData()), buffer.getSize());
    sb.setBuffLen(buffer.getSize());

    Fw::SerializeStatus deserStatus = fin.deserializeFrom(sb);
    if (deserStatus != Fw::FW_SERIALIZE_OK) {
        // Bad FIN PDU
        this->m_cfdpManager->log_WARNING_LO_FailFinPduDeserialization(this->getChannelId(), static_cast<I32>(deserStatus));
        return;
    }

    if (!this->m_engine->recvFin(this, fin))
    {
        // set the CC only on the first time we get the FIN.  If this is a dupe
        // then re-ack but otherwise ignore it
        if (!this->m_flags.tx.fin_recv)
        {

            this->m_flags.tx.fin_recv               = true;
            this->m_state_data.send.s2.fin_cc       = static_cast<ConditionCode>(fin.getConditionCode());
            this->m_state_data.send.s2.acknak_count = 0; // in case retransmits had occurred

            // note this is a no-op unless the status was unset previously
            this->m_engine->setTxnStatus(this, static_cast<TxnStatus>(this->m_state_data.send.s2.fin_cc));

            // Generally FIN is the last exchange in an S2 transaction, the remote is not supposed
            // to send it until after the EOF+ACK.  So at this point we stop trying to send anything
            // to the peer, regardless of whether we got every ACK we expected.
            this->m_engine->finishTransaction(this, true);
        }
        this->m_flags.tx.send_fin_ack = true;
    }
}

void Transaction::s2Nak(const Fw::Buffer& buffer) {
    U8 counter;
    U8 bad_sr;

    bad_sr = 0;

    // Deserialize NAK PDU from buffer
    NakPdu nak;
    Fw::SerialBuffer sb(const_cast<U8*>(buffer.getData()), buffer.getSize());
    sb.setBuffLen(buffer.getSize());

    Fw::SerializeStatus deserStatus = nak.deserializeFrom(sb);
    if (deserStatus != Fw::FW_SERIALIZE_OK) {
        // Bad NAK PDU
        this->m_cfdpManager->log_WARNING_LO_FailNakPduDeserialization(this->getChannelId(), static_cast<I32>(deserStatus));
        // ++CF_AppData.hk.Payload.channel_hk[this->m_chan_num].counters.recv.error;
        return;
    }

    // this function is only invoked for NAK PDU types
    if (this->m_engine->recvNak(this, nak) == Cfdp::Status::SUCCESS && nak.getNumSegments() > 0)
    {
        for (counter = 0; counter < nak.getNumSegments(); ++counter)
        {
            const Cfdp::SegmentRequest& sr = nak.getSegment(counter);

            if (sr.offsetStart == 0 && sr.offsetEnd == 0)
            {
                // need to re-send metadata PDU
                this->m_flags.tx.md_need_send = true;
            }
            else
            {
                if (sr.offsetEnd < sr.offsetStart)
                {
                    ++bad_sr;
                    continue;
                }

                // overflow probably won't be an issue
                if (sr.offsetEnd > this->m_fsize)
                {
                    ++bad_sr;
                    continue;
                }

                // insert gap data in chunks
                this->m_chunks->chunks.add(sr.offsetStart, sr.offsetEnd - sr.offsetStart);
            }
        }

        // CF_AppData.hk.Payload.channel_hk[this->m_chan_num].counters.recv.nak_segment_requests +=
        //     nak->segment_list.num_segments;
        if (bad_sr)
        {
            this->m_cfdpManager->log_WARNING_LO_TxInvalidSegmentRequests(
                Cfdp::getClassDisplay(this->getClass()),
                this->m_history->src_eid,
                this->m_history->seq_num,
                bad_sr);
        }
    }
    else
    {
        this->m_cfdpManager->log_WARNING_HI_TxInvalidNakPdu(
            Cfdp::getClassDisplay(this->getClass()),
            this->m_history->src_eid,
            this->m_history->seq_num);
        // ++CF_AppData.hk.Payload.channel_hk[this->m_chan_num].counters.recv.error;
    }
}

void Transaction::s2NakArm(const Fw::Buffer& buffer) {
    this->m_engine->armAckTimer(this);
    this->s2Nak(buffer);
}

void Transaction::s2EofAck(const Fw::Buffer& buffer) {
    // Deserialize ACK PDU from buffer
    AckPdu ack;
    Fw::SerialBuffer sb(const_cast<U8*>(buffer.getData()), buffer.getSize());
    sb.setBuffLen(buffer.getSize());

    Fw::SerializeStatus deserStatus = ack.deserializeFrom(sb);
    if (deserStatus != Fw::FW_SERIALIZE_OK) {
        // Bad ACK PDU
        this->m_cfdpManager->log_WARNING_LO_FailAckPduDeserialization(this->getChannelId(), static_cast<I32>(deserStatus));
        return;
    }

    if (!this->m_engine->recvAck(this, ack) &&
        ack.getDirectiveCode() == FILE_DIRECTIVE_END_OF_FILE)
    {
        this->m_flags.tx.eof_ack_recv           = true;
        this->m_flags.com.ack_timer_armed       = false; // just wait for FIN now, nothing to re-send
        this->m_state_data.send.s2.acknak_count = 0;     // in case EOF retransmits had occurred

        // if FIN was also received then we are done (these can come out of order)
        if (this->m_flags.tx.fin_recv)
        {
            this->m_engine->finishTransaction(this, true);
        }
    }
}

// ======================================================================
// Dispatch Methods (ported from cf_cfdp_dispatch.c)
// ======================================================================

void Transaction::sDispatchRecv(const Fw::Buffer& buffer,
                                    const SSubstateRecvDispatchTable *dispatch)
{
    const FileDirectiveDispatchTable *substate_tbl;
    StateRecvFunc                     selected_handler;

    FW_ASSERT(this->m_state_data.send.sub_state < TX_SUB_STATE_NUM_STATES,
              this->m_state_data.send.sub_state, TX_SUB_STATE_NUM_STATES);

    // Peek at PDU type from buffer
    Cfdp::PduTypeEnum pduType = Cfdp::peekPduType(buffer);

    // send state, so we only care about file directive PDU
    selected_handler = NULL;

    if (pduType == Cfdp::T_FILE_DATA)
    {
        this->m_cfdpManager->log_WARNING_LO_TxNonFileDirectivePduReceived(
            Cfdp::getClassDisplay(this->getClass()),
            this->m_history->src_eid,
            this->m_history->seq_num);
    }
    else if (pduType != Cfdp::T_NONE)
    {
        // It's a directive PDU - parse header to get directive code
        Fw::SerialBuffer sb(const_cast<U8*>(buffer.getData()), buffer.getSize());
        sb.setBuffLen(buffer.getSize());

        Cfdp::PduHeader header;
        if (header.fromSerialBuffer(sb) == Fw::FW_SERIALIZE_OK)
        {
            // Read directive code (first byte after header for directive PDUs)
            U8 directiveCodeByte;
            if (sb.deserializeTo(directiveCodeByte) == Fw::FW_SERIALIZE_OK)
            {
                FileDirective directiveCode = static_cast<FileDirective>(directiveCodeByte);

                if (directiveCode < FILE_DIRECTIVE_INVALID_MAX)
                {
                    // This should be silent (no event) if no handler is defined in the table
                    substate_tbl = dispatch->substate[this->m_state_data.send.sub_state];
                    if (substate_tbl != NULL)
                    {
                        selected_handler = substate_tbl->fdirective[directiveCode];
                    }
                }
                else
                {
                    this->m_cfdpManager->log_WARNING_LO_TxInvalidDirectiveCode(
                        Cfdp::getClassDisplay(this->getClass()),
                        this->m_history->src_eid,
                        this->m_history->seq_num,
                        directiveCodeByte,
                        this->m_state_data.send.sub_state);
                }
            }
        }
    }

    // check that there's a valid function pointer. If there isn't,
    // then silently ignore. We may want to discuss if it's worth
    // shutting down the whole transaction if a PDU is received
    // that doesn't make sense to be received (For example,
    // class 1 CFDP receiving a NAK PDU) but for now, we silently
    // ignore the received packet and keep chugging along.
    if (selected_handler)
    {
        (this->*selected_handler)(buffer);
    }
}

void Transaction::sDispatchTransmit(const SSubstateSendDispatchTable *dispatch)
{
    StateSendFunc selected_handler;

    selected_handler = dispatch->substate[this->m_state_data.send.sub_state];
    if (selected_handler != NULL)
    {
        (this->*selected_handler)();
    }
}

void Transaction::txStateDispatch(const TxnSendDispatchTable *dispatch)
{
    StateSendFunc selected_handler;

    FW_ASSERT(this->m_state < TXN_STATE_INVALID, this->m_state, TXN_STATE_INVALID);

    selected_handler = dispatch->tx[this->m_state];
    if (selected_handler != NULL)
    {
        (this->*selected_handler)();
    }
}

}  // namespace Cfdp
}  // namespace Ccsds
}  // namespace Svc
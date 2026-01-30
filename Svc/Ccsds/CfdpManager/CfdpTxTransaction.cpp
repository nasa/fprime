// ======================================================================
// \title  CfdpTxTransaction.cpp
// \brief  cpp file for CFDP TX Transaction state machine
//
// This file is a port of the cf_cfdp_s.c and cf_cfdp_dispatch.c files from the
// NASA Core Flight System (cFS) CFDP (CF) Application,
// version 3.0.0, adapted for use within the F-Prime (F') framework.
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

#include <Svc/Ccsds/CfdpManager/CfdpTransaction.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpManager.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpEngine.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpChannel.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpChunk.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpUtils.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpTimer.hpp>

namespace Svc {
namespace Ccsds {

// ======================================================================
// TX State Machine - Private Helper (anonymous namespace)
// ======================================================================

namespace {

// Helper to build dispatch tables
CF_CFDP_FileDirectiveDispatchTable_t makeFileDirectiveTable(
    CF_CFDP_StateRecvFunc_t fin,
    CF_CFDP_StateRecvFunc_t ack,
    CF_CFDP_StateRecvFunc_t nak
)
{
    CF_CFDP_FileDirectiveDispatchTable_t table = {};
    memset(&table, 0, sizeof(table));

    table.fdirective[Cfdp::FILE_DIRECTIVE_FIN] = fin;
    table.fdirective[Cfdp::FILE_DIRECTIVE_ACK] = ack;
    table.fdirective[Cfdp::FILE_DIRECTIVE_NAK] = nak;

    return table;
}

}  // anonymous namespace

// ======================================================================
// TX State Machine - Public Methods
// ======================================================================

void CfdpTransaction::s1Recv(const Cfdp::Pdu& pdu) {
    /* s1 doesn't need to receive anything */
    static const CF_CFDP_S_SubstateRecvDispatchTable_t substate_fns = {{NULL}};
    this->sDispatchRecv(pdu, &substate_fns);
}

void CfdpTransaction::s2Recv(const Cfdp::Pdu& pdu) {
    static const CF_CFDP_FileDirectiveDispatchTable_t s2_meta =
        makeFileDirectiveTable(
            &CfdpTransaction::s2EarlyFin,
            nullptr,
            nullptr
        );

    static const CF_CFDP_FileDirectiveDispatchTable_t s2_fd_or_eof =
        makeFileDirectiveTable(
            &CfdpTransaction::s2EarlyFin,
            nullptr,
            &CfdpTransaction::s2Nak
        );

    static const CF_CFDP_FileDirectiveDispatchTable_t s2_wait_ack =
        makeFileDirectiveTable(
            &CfdpTransaction::s2Fin,
            &CfdpTransaction::s2EofAck,
            &CfdpTransaction::s2NakArm
        );

    static const CF_CFDP_S_SubstateRecvDispatchTable_t substate_fns = {
        {
            &s2_meta,      /* CF_TxSubState_METADATA */
            &s2_fd_or_eof, /* CF_TxSubState_FILEDATA */
            &s2_fd_or_eof, /* CF_TxSubState_EOF */
            &s2_wait_ack   /* CF_TxSubState_CLOSEOUT_SYNC */
        }
    };

    this->sDispatchRecv(pdu, &substate_fns);
}

void CfdpTransaction::initTxFile(Cfdp::Class::T cfdp_class, Cfdp::Keep::T keep, U8 chan, U8 priority)
{
    m_chan_num = chan;
    m_priority = priority;
    m_keep = keep;
    m_txn_class = cfdp_class;
    m_state = (cfdp_class == Cfdp::Class::CLASS_2) ? CF_TxnState_S2 : CF_TxnState_S1;
    m_state_data.send.sub_state = CF_TxSubState_METADATA;
}

void CfdpTransaction::s1Tx() {
    static const CF_CFDP_S_SubstateSendDispatchTable_t substate_fns = {{
        &CfdpTransaction::sSubstateSendMetadata, // CF_TxSubState_METADATA
        &CfdpTransaction::sSubstateSendFileData, // CF_TxSubState_FILEDATA
        &CfdpTransaction::s1SubstateSendEof, // CF_TxSubState_EOF
        nullptr // CF_TxSubState_CLOSEOUT_SYNC
    }};

    this->sDispatchTransmit(&substate_fns);
}

void CfdpTransaction::s2Tx() {
    static const CF_CFDP_S_SubstateSendDispatchTable_t substate_fns = {{
        &CfdpTransaction::sSubstateSendMetadata, // CF_TxSubState_METADATA
        &CfdpTransaction::s2SubstateSendFileData, // CF_TxSubState_FILEDATA
        &CfdpTransaction::s2SubstateSendEof, // CF_TxSubState_EOF
        nullptr // CF_TxSubState_CLOSEOUT_SYNC
    }};

    this->sDispatchTransmit(&substate_fns);
}

void CfdpTransaction::sAckTimerTick() {
    U8 ack_limit = 0;

    /* note: the ack timer is only ever relevant on class 2 */
    if (this->m_state != CF_TxnState_S2 || !this->m_flags.com.ack_timer_armed)
    {
        /* nothing to do */
        return;
    }

    if (this->m_ack_timer.getStatus() == CfdpTimer::Status::RUNNING)
    {
        this->m_ack_timer.run();
    }
    else if (this->m_state_data.send.sub_state == CF_TxSubState_CLOSEOUT_SYNC)
    {
        /* Check limit and handle if needed */
        ack_limit = this->m_cfdpManager->getAckLimitParam(this->m_chan_num);
        if (this->m_state_data.send.s2.acknak_count >= ack_limit)
        {
            // CFE_EVS_SendEvent(CF_CFDP_S_ACK_LIMIT_ERR_EID, CFE_EVS_EventType_ERROR,
            //                   "CF S2(%lu:%lu), ack limit reached, no eof-ack", (unsigned long)this->m_history->src_eid,
            //                   (unsigned long)this->m_history->seq_num);
            this->m_engine->setTxnStatus(this, CF_TxnStatus_ACK_LIMIT_NO_EOF);
            // ++CF_AppData.hk.Payload.channel_hk[this->m_chan_num].counters.fault.ack_limit;

            /* give up on this */
            this->m_engine->finishTransaction(this, true);
            this->m_flags.com.ack_timer_armed = false;
        }
        else
        {
            /* Increment acknak counter */
            ++this->m_state_data.send.s2.acknak_count;

            /* If the peer sent FIN that is an implicit EOF ack, it is not supposed
             * to send it before EOF unless an error occurs, and either way we do not
             * re-transmit anything after FIN unless we get another FIN */
            if (!this->m_flags.tx.eof_ack_recv && !this->m_flags.tx.fin_recv)
            {
                this->m_flags.tx.send_eof = true;
            }
            else
            {
                /* no response is pending */
                this->m_flags.com.ack_timer_armed = false;
            }
        }

        /* reset the ack timer if still waiting on something */
        if (this->m_flags.com.ack_timer_armed)
        {
            this->m_engine->armAckTimer(this);
        }
    }
    else
    {
        /* if we are not waiting for anything, why is the ack timer armed? */
        this->m_flags.com.ack_timer_armed = false;
    }
}

void CfdpTransaction::sTick(int *cont /* unused */) {
    bool pending_send;

    pending_send = true; /* maybe; tbd, will be reset if not */

    /* at each tick, various timers used by S are checked */
    /* first, check inactivity timer */
    if (!this->m_flags.com.inactivity_fired)
    {
        if (this->m_inactivity_timer.getStatus() == CfdpTimer::Status::RUNNING)
        {
            this->m_inactivity_timer.run();
        }
        else
        {
            this->m_flags.com.inactivity_fired = true;

            /* HOLD state is the normal path to recycle transaction objects, not an error */
            /* inactivity is abnormal in any other state */
            if (this->m_state != CF_TxnState_HOLD && this->m_state == CF_TxnState_S2)
            {
                // CFE_EVS_SendEvent(CF_CFDP_S_INACT_TIMER_ERR_EID, CFE_EVS_EventType_ERROR,
                //                   "CF S2(%lu:%lu): inactivity timer expired", (unsigned long)this->m_history->src_eid,
                //                   (unsigned long)this->m_history->seq_num);
                this->m_engine->setTxnStatus(this, CF_TxnStatus_INACTIVITY_DETECTED);

                // ++CF_AppData.hk.Payload.channel_hk[this->m_chan_num].counters.fault.inactivity_timer;
            }
        }
    }

    /* tx maintenance: possibly process send_eof, or send_fin_ack */
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

    /* if the inactivity timer ran out, then there is no sense
     * pending for responses for anything.  Send out anything
     * that we need to send (i.e. the EOF) just in case the sender
     * is still listening to us but do not expect any future ACKs */
    if (this->m_flags.com.inactivity_fired && !pending_send)
    {
        /* the transaction is now recycleable - this means we will
         * no longer have a record of this transaction seq.  If the sender
         * wakes up or if the network delivers severely delayed PDUs at
         * some future point, then they will be seen as spurious.  They
         * will no longer be associable with this transaction at all */
        this->m_chan->recycleTransaction(this);

        /* NOTE: this must be the last thing in here.  Do not use txn after this */
    }
    else
    {
        /* transaction still valid so process the ACK timer, if relevant */
        this->sAckTimerTick();
    }
}

void CfdpTransaction::sTickNak(int *cont) {
    bool nakProcessed = false;
    Cfdp::Status::T status;

    // Only Class 2 transactions should process NAKs
    if (this->m_txn_class == Cfdp::Class::CLASS_2)
    {
        status = this->sCheckAndRespondNak(&nakProcessed);
        if ((status == Cfdp::Status::SUCCESS) && nakProcessed)
        {
            *cont = 1; /* cause dispatcher to re-enter this wakeup */
        }
    }
}

void CfdpTransaction::sCancel() {
    if (this->m_state_data.send.sub_state < CF_TxSubState_EOF)
    {
        /* if state has not reached CF_TxSubState_EOF, then set it to CF_TxSubState_EOF now. */
        this->m_state_data.send.sub_state = CF_TxSubState_EOF;
    }
}

// ======================================================================
// TX State Machine - Private Helper Methods
// ======================================================================

Cfdp::Status::T CfdpTransaction::sSendEof() {
    /* note the crc is "finalized" regardless of success or failure of the txn */
    /* this is OK as we still need to put some value into the EOF */
    if (!this->m_flags.com.crc_calc)
    {
        // The F' version does not have an equivelent finalize call as it
        // - Never stores a partial word internally
        // - Never needs to "flush" anything
        // - Always accounts for padding at update time
        this->m_flags.com.crc_calc = true;
    }
    return this->m_engine->sendEof(this);
}

void CfdpTransaction::s1SubstateSendEof() {
    /* set the flag, the EOF is sent by the tick handler */
    this->m_flags.tx.send_eof = true;

    /* In class 1 this is the end of normal operation */
    /* NOTE: this is not always true, as class 1 can request an EOF ack.
     * In this case we could change state to CLOSEOUT_SYNC instead and wait,
     * but right now we do not request an EOF ack in S1 */
    this->m_engine->finishTransaction(this, true);
}

void CfdpTransaction::s2SubstateSendEof() {
    /* set the flag, the EOF is sent by the tick handler */
    this->m_flags.tx.send_eof = true;

    /* wait for remaining responses to close out the state machine */
    this->m_state_data.send.sub_state = CF_TxSubState_CLOSEOUT_SYNC;

    /* always move the transaction onto the wait queue now */
    this->m_chan->dequeueTransaction(this);
    this->m_chan->insertSortPrio(this, Cfdp::QueueId::TXW);

    /* the ack timer is armed in class 2 only */
    this->m_engine->armAckTimer(this);
}

Cfdp::Status::T CfdpTransaction::sSendFileData(U32 foffs, U32 bytes_to_read, U8 calc_crc, U32* bytes_processed) {
    FW_ASSERT(bytes_processed != NULL);
    *bytes_processed = 0;

    Cfdp::Status::T status = Cfdp::Status::SUCCESS;

    // Local buffer for file data
    U8 fileDataBuffer[CF_MAX_PDU_SIZE];

    // Create File Data PDU
    Cfdp::Pdu::FileDataPdu fdPdu;
    Cfdp::Direction direction = Cfdp::DIRECTION_TOWARD_RECEIVER;

    // Calculate maximum data size we can send, accounting for PDU overhead
    U32 maxDataCapacity = fdPdu.getMaxFileDataSize();

    // Limited by: bytes_to_read, outgoing_file_chunk_size, and maxDataCapacity
    U32 outgoing_file_chunk_size = this->m_cfdpManager->getOutgoingFileChunkSizeParam();
    U32 max_data_bytes = bytes_to_read;
    if (max_data_bytes > outgoing_file_chunk_size) {
        max_data_bytes = outgoing_file_chunk_size;
    }
    if (max_data_bytes > maxDataCapacity) {
        max_data_bytes = maxDataCapacity;
    }

    // Seek to file offset if needed
    if (this->m_state_data.send.cached_pos != foffs) {
        Os::File::Status fileStatus = this->m_fd.seek(foffs, Os::File::SeekType::ABSOLUTE);
        if (fileStatus != Os::File::OP_OK) {
            return Cfdp::Status::ERROR;
        }
    }

    // Read file data
    FwSizeType actual_bytes = max_data_bytes;
    Os::File::Status fileStatus = this->m_fd.read(fileDataBuffer, actual_bytes, Os::File::WaitType::WAIT);
    if (fileStatus != Os::File::OP_OK) {
        return Cfdp::Status::ERROR;
    }

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

    // Send the PDU
    status = this->m_engine->sendFd(this, fdPdu);

    if (status == Cfdp::Status::SUCCESS) {
        this->m_state_data.send.cached_pos += static_cast<U32>(actual_bytes);

        FW_ASSERT((foffs + actual_bytes) <= this->m_fsize, foffs, static_cast<FwAssertArgType>(actual_bytes), this->m_fsize);

        if (calc_crc) {
            this->m_crc.update(fileDataBuffer, foffs, static_cast<U32>(actual_bytes));
        }

        *bytes_processed = static_cast<U32>(actual_bytes);
    }

    return status;
}

void CfdpTransaction::sSubstateSendFileData() {
    U32 bytes_processed = 0;
    Cfdp::Status::T status = this->sSendFileData(this->m_foffs, (this->m_fsize - this->m_foffs), 1, &bytes_processed);

    if(status != Cfdp::Status::SUCCESS)
    {
        /* IO error -- change state and send EOF */
        this->m_engine->setTxnStatus(this, CF_TxnStatus_FILESTORE_REJECTION);
        this->m_state_data.send.sub_state = CF_TxSubState_EOF;
    }
    else if (bytes_processed > 0)
    {
        this->m_foffs += bytes_processed;
        if (this->m_foffs == this->m_fsize)
        {
            /* file is done */
            this->m_state_data.send.sub_state = CF_TxSubState_EOF;
        }
    }
    else
    {
        /* don't care about other cases */
    }
}

Cfdp::Status::T CfdpTransaction::sCheckAndRespondNak(bool* nakProcessed) {
    const CF_Chunk_t *chunk;
    Cfdp::Status::T sret;
    Cfdp::Status::T ret = Cfdp::Status::SUCCESS;
    U32 bytes_processed = 0;

    FW_ASSERT(nakProcessed != NULL);
    *nakProcessed = false;

    // Class 2 transactions must have had chunks allocated
    FW_ASSERT(this->m_chunks != NULL);

    if (this->m_flags.tx.md_need_send)
    {
        sret = this->m_engine->sendMd(this);
        if (sret == Cfdp::Status::SEND_PDU_ERROR)
        {
            ret = Cfdp::Status::ERROR; /* error occurred */
        }
        else
        {
            if (sret == Cfdp::Status::SUCCESS)
            {
                this->m_flags.tx.md_need_send = false;
            }
            /* unless SEND_PDU_ERROR, return 1 to keep caller from sending file data */
            *nakProcessed = true; /* nak processed, so don't send filedata */

        }
    }
    else
    {
        /* Get first chunk and process if available */
        chunk = this->m_chunks->chunks.getFirstChunk();
        if (chunk != nullptr)
        {
            ret = this->sSendFileData(chunk->offset, chunk->size, 0, &bytes_processed);
            if(ret != Cfdp::Status::SUCCESS)
            {
                /* error occurred */
                ret = Cfdp::Status::ERROR; /* error occurred */
            }
            else if (bytes_processed > 0)
            {
                this->m_chunks->chunks.removeFromFirst(bytes_processed);
                *nakProcessed = true; /* nak processed, so caller doesn't send file data */
            }
        }
    }

    return ret;
}

void CfdpTransaction::s2SubstateSendFileData() {
    Cfdp::Status::T status;
    bool nakProcessed = false;

    status = this->sCheckAndRespondNak(&nakProcessed);
    if (status != Cfdp::Status::SUCCESS)
    {
        this->m_engine->setTxnStatus(this, CF_TxnStatus_NAK_RESPONSE_ERROR);
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
        /* NAK was processed, so do not send filedata */
    }
}

void CfdpTransaction::sSubstateSendMetadata() {
    Cfdp::Status::T status;
    Os::File::Status fileStatus;
    bool success = true;

    if (false == this->m_fd.isOpen())
    {
        fileStatus = this->m_fd.open(this->m_history->fnames.src_filename.toChar(), Os::File::OPEN_READ);
        if (fileStatus != Os::File::OP_OK)
        {
            // CFE_EVS_SendEvent(CF_CFDP_S_OPEN_ERR_EID, CFE_EVS_EventType_ERROR,
            //                   "CF S%d(%lu:%lu): failed to open file %s, error=%ld", (this->m_state == CF_TxnState_S2),
            //                   (unsigned long)this->m_history->src_eid, (unsigned long)this->m_history->seq_num,
            //                   this->m_history->fnames.src_filename, (long)ret);
            // ++CF_AppData.hk.Payload.channel_hk[this->m_chan_num].counters.fault.file_open;
            // this->m_fd = OS_OBJECT_ID_UNDEFINED; /* just in case */
            success = false;
        }

        if (success)
        {
            FwSizeType file_size;
            fileStatus = this->m_fd.size(file_size);
            this->m_fsize = static_cast<CfdpFileSize>(file_size);
            if (fileStatus != Os::File::Status::OP_OK)
            {
                // CFE_EVS_SendEvent(CF_CFDP_S_SEEK_BEG_ERR_EID, CFE_EVS_EventType_ERROR,
                //                   "CF S%d(%lu:%lu): failed to seek begin file %s, got %ld",
                //                   (this->m_state == CF_TxnState_S2), (unsigned long)this->m_history->src_eid,
                //                   (unsigned long)this->m_history->seq_num, this->m_history->fnames.src_filename,
                //                   (long)status);
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
            // CFE_EVS_SendEvent(CF_CFDP_S_SEND_MD_ERR_EID, CFE_EVS_EventType_ERROR, "CF S%d(%lu:%lu): failed to send md",
            //                   (this->m_state == CF_TxnState_S2), (unsigned long)this->m_history->src_eid,
            //                   (unsigned long)this->m_history->seq_num);
            success = false;
        }
        else if (status == Cfdp::Status::SUCCESS)
        {
            /* once metadata is sent, switch to filedata mode */
            this->m_state_data.send.sub_state = CF_TxSubState_FILEDATA;
        }
        /* if status==Cfdp::Status::SEND_PDU_NO_BUF_AVAIL_ERROR, then try to send md again next cycle */
    }

    if (!success)
    {
        this->m_engine->setTxnStatus(this, CF_TxnStatus_FILESTORE_REJECTION);
        this->m_engine->finishTransaction(this, true);
    }

    /* don't need to reset the CRC since its taken care of by reset_cfdp() */
}

Cfdp::Status::T CfdpTransaction::sSendFinAck() {
    Cfdp::Status::T ret = this->m_engine->sendAck(this,
                           static_cast<Cfdp::AckTxnStatus>(CF_CFDP_GetTxnStatus(this)),
                           Cfdp::FILE_DIRECTIVE_FIN,
                           static_cast<Cfdp::ConditionCode>(this->m_state_data.send.s2.fin_cc),
                           this->m_history->peer_eid, this->m_history->seq_num);
    return ret;
}

void CfdpTransaction::s2EarlyFin(const Cfdp::Pdu& pdu) {
    /* received early fin, so just cancel */
    // CFE_EVS_SendEvent(CF_CFDP_S_EARLY_FIN_ERR_EID, CFE_EVS_EventType_ERROR,
    //                   "CF S%d(%lu:%lu): got early FIN -- cancelling", (this->m_state == CF_TxnState_S2),
    //                   (unsigned long)this->m_history->src_eid, (unsigned long)this->m_history->seq_num);
    this->m_engine->setTxnStatus(this, CF_TxnStatus_EARLY_FIN);

    this->m_state_data.send.sub_state = CF_TxSubState_CLOSEOUT_SYNC;

    /* otherwise do normal fin processing */
    this->s2Fin(pdu);
}

void CfdpTransaction::s2Fin(const Cfdp::Pdu& pdu) {
    if (!this->m_engine->recvFin(this, pdu))
    {
        /* set the CC only on the first time we get the FIN.  If this is a dupe
         * then re-ack but otherwise ignore it */
        if (!this->m_flags.tx.fin_recv)
        {
            const Cfdp::Pdu::FinPdu& fin = pdu.asFinPdu();

            this->m_flags.tx.fin_recv               = true;
            this->m_state_data.send.s2.fin_cc       = static_cast<CF_CFDP_ConditionCode_t>(fin.getConditionCode());
            this->m_state_data.send.s2.acknak_count = 0; /* in case retransmits had occurred */

            /* note this is a no-op unless the status was unset previously */
            this->m_engine->setTxnStatus(this, static_cast<CF_TxnStatus_t>(this->m_state_data.send.s2.fin_cc));

            /* Generally FIN is the last exchange in an S2 transaction, the remote is not supposed
             * to send it until after the EOF+ACK.  So at this point we stop trying to send anything
             * to the peer, regardless of whether we got every ACK we expected. */
            this->m_engine->finishTransaction(this, true);
        }
        this->m_flags.tx.send_fin_ack = true;
    }
}

void CfdpTransaction::s2Nak(const Cfdp::Pdu& pdu) {
    U8 counter;
    U8 bad_sr;

    bad_sr = 0;

    /* this function is only invoked for NAK PDU types */
    const Cfdp::Pdu::NakPdu& nak = pdu.asNakPdu();

    if (this->m_engine->recvNak(this, pdu) == Cfdp::Status::SUCCESS && nak.getNumSegments() > 0)
    {
        for (counter = 0; counter < nak.getNumSegments(); ++counter)
        {
            const Cfdp::Pdu::SegmentRequest& sr = nak.getSegment(counter);

            if (sr.offsetStart == 0 && sr.offsetEnd == 0)
            {
                /* need to re-send metadata PDU */
                this->m_flags.tx.md_need_send = true;
            }
            else
            {
                if (sr.offsetEnd < sr.offsetStart)
                {
                    ++bad_sr;
                    continue;
                }

                /* overflow probably won't be an issue */
                if (sr.offsetEnd > this->m_fsize)
                {
                    ++bad_sr;
                    continue;
                }

                /* insert gap data in chunks */
                this->m_chunks->chunks.add(sr.offsetStart, sr.offsetEnd - sr.offsetStart);
            }
        }

        // CF_AppData.hk.Payload.channel_hk[this->m_chan_num].counters.recv.nak_segment_requests +=
        //     nak->segment_list.num_segments;
        if (bad_sr)
        {
            // CFE_EVS_SendEvent(CF_CFDP_S_INVALID_SR_ERR_EID, CFE_EVS_EventType_ERROR,
            //                   "CF S%d(%lu:%lu): received %d invalid NAK segment requests",
            //                   (this->m_state == CF_TxnState_S2), (unsigned long)this->m_history->src_eid,
            //                   (unsigned long)this->m_history->seq_num, bad_sr);
        }
    }
    else
    {
        // CFE_EVS_SendEvent(CF_CFDP_S_PDU_NAK_ERR_EID, CFE_EVS_EventType_ERROR,
        //                   "CF S%d(%lu:%lu): received invalid NAK PDU", (this->m_state == CF_TxnState_S2),
        //                   (unsigned long)this->m_history->src_eid, (unsigned long)this->m_history->seq_num);
        // ++CF_AppData.hk.Payload.channel_hk[this->m_chan_num].counters.recv.error;
    }
}

void CfdpTransaction::s2NakArm(const Cfdp::Pdu& pdu) {
    this->m_engine->armAckTimer(this);
    this->s2Nak(pdu);
}

void CfdpTransaction::s2EofAck(const Cfdp::Pdu& pdu) {
    const Cfdp::Pdu::AckPdu& ack = pdu.asAckPdu();

    if (!this->m_engine->recvAck(this, pdu) &&
        ack.getDirectiveCode() == Cfdp::FILE_DIRECTIVE_END_OF_FILE)
    {
        this->m_flags.tx.eof_ack_recv           = true;
        this->m_flags.com.ack_timer_armed       = false; /* just wait for FIN now, nothing to re-send */
        this->m_state_data.send.s2.acknak_count = 0;     /* in case EOF retransmits had occurred */

        /* if FIN was also received then we are done (these can come out of order) */
        if (this->m_flags.tx.fin_recv)
        {
            this->m_engine->finishTransaction(this, true);
        }
    }
}

// ======================================================================
// Dispatch Methods (ported from cf_cfdp_dispatch.c)
// ======================================================================

void CfdpTransaction::sDispatchRecv(const Cfdp::Pdu& pdu,
                                    const CF_CFDP_S_SubstateRecvDispatchTable_t *dispatch)
{
    const CF_CFDP_FileDirectiveDispatchTable_t *substate_tbl;
    CF_CFDP_StateRecvFunc_t                     selected_handler;

    FW_ASSERT(this->m_state_data.send.sub_state < CF_TxSubState_NUM_STATES,
              this->m_state_data.send.sub_state, CF_TxSubState_NUM_STATES);

    // Extract PDU type from header
    const Cfdp::Pdu::Header& header = pdu.asHeader();
    Cfdp::Pdu::Type pduType = header.getType();
    Cfdp::FileDirective directiveCode = pdu.getDirectiveCode();

    /* send state, so we only care about file directive PDU */
    selected_handler = NULL;

    if (pduType == Cfdp::Pdu::T_FILE_DATA)
    {
        // CFE_EVS_SendEvent(CF_CFDP_S_NON_FD_PDU_ERR_EID, CFE_EVS_EventType_ERROR,
        //                   "CF S%d(%lu:%lu): received non-file directive PDU", (this->m_state == CF_TxnState_S2),
        //                   (unsigned long)this->m_history->src_eid, (unsigned long)this->m_history->seq_num);
    }
    else if (directiveCode < Cfdp::FILE_DIRECTIVE_INVALID_MAX)
    {
        /* This should be silent (no event) if no handler is defined in the table */
        substate_tbl = dispatch->substate[this->m_state_data.send.sub_state];
        if (substate_tbl != NULL)
        {
            selected_handler = substate_tbl->fdirective[directiveCode];
        }
    }
    else
    {
        // ++CF_AppData.hk.Payload.channel_hk[this->m_chan_num].counters.recv.spurious;
        // CFE_EVS_SendEvent(CF_CFDP_S_DC_INV_ERR_EID, CFE_EVS_EventType_ERROR,
        //                   "CF S%d(%lu:%lu): received PDU with invalid directive code %d for sub-state %d",
        //                   (this->m_state == CF_TxnState_S2), (unsigned long)this->m_history->src_eid,
        //                   (unsigned long)this->m_history->seq_num, directiveCode,
        //                   this->m_state_data.send.sub_state);
    }

    /* check that there's a valid function pointer. If there isn't,
     * then silently ignore. We may want to discuss if it's worth
     * shutting down the whole transaction if a PDU is received
     * that doesn't make sense to be received (For example,
     * class 1 CFDP receiving a NAK PDU) but for now, we silently
     * ignore the received packet and keep chugging along. */
    if (selected_handler)
    {
        (this->*selected_handler)(pdu);
    }
}

void CfdpTransaction::sDispatchTransmit(const CF_CFDP_S_SubstateSendDispatchTable_t *dispatch)
{
    CF_CFDP_StateSendFunc_t selected_handler;

    selected_handler = dispatch->substate[this->m_state_data.send.sub_state];
    if (selected_handler != NULL)
    {
        (this->*selected_handler)();
    }
}

void CfdpTransaction::txStateDispatch(const CF_CFDP_TxnSendDispatchTable_t *dispatch)
{
    CF_CFDP_StateSendFunc_t selected_handler;

    FW_ASSERT(this->m_state < CF_TxnState_INVALID, this->m_state, CF_TxnState_INVALID);

    selected_handler = dispatch->tx[this->m_state];
    if (selected_handler != NULL)
    {
        (this->*selected_handler)();
    }
}

}  // namespace Ccsds
}  // namespace Svc
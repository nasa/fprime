// ======================================================================
// \title  TransactionRx.cpp
// \brief  cpp file for CFDP RX Transaction state machine
//
// This file is a port of RX transaction state machine operations from the following files
// from the NASA Core Flight System (cFS) CFDP (CF) Application, version 3.0.0,
// adapted for use within the F-Prime (F') framework:
// - cf_cfdp_r.c (receive-file transaction state handling routines)
// - cf_cfdp_dispatch.c (RX state machine dispatch functions)
//
// This file contains various state handling routines for
// transactions which are receiving a file, as well as dispatch
// functions for RX state machines and top-level transaction dispatch.
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

#include <Fw/Types/SuccessEnumAc.hpp>
#include <Os/FileSystem.hpp>

#include <Svc/Ccsds/CfdpManager/Transaction.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpManager.hpp>
#include <Svc/Ccsds/CfdpManager/Engine.hpp>
#include <Svc/Ccsds/CfdpManager/Channel.hpp>
#include <Svc/Ccsds/CfdpManager/Utils.hpp>
#include <Svc/Ccsds/CfdpManager/Chunk.hpp>

namespace Svc {
namespace Ccsds {
namespace Cfdp {

// ======================================================================
// Construction and Destruction
// ======================================================================

Transaction::Transaction(Channel* channel, U8 channelId, Engine* engine, CfdpManager* manager) :
    m_state(TXN_STATE_UNDEF),
    m_txn_class(Cfdp::Class::CLASS_1),
    m_history(nullptr),
    m_chunks(nullptr),
    m_inactivity_timer(),
    m_ack_timer(),
    m_fsize(0),
    m_foffs(0),
    m_fd(),
    m_crc(),
    m_keep(Cfdp::Keep::KEEP),
    m_chan_num(channelId),          // Initialize from parameter
    m_priority(0),
    m_initType(INIT_BY_COMMAND),
    m_cl_node{},
    m_pb(nullptr),
    m_state_data{},
    m_flags{},
    m_cfdpManager(manager),         // Initialize from parameter
    m_chan(channel),                // Initialize from parameter
    m_engine(engine)                // Initialize from parameter
{
    // All members initialized via member initializer list above
}

Transaction::~Transaction() { }

void Transaction::reset()
{
    // Reset transaction state to default values
    this->m_state = TXN_STATE_UNDEF;
    this->m_txn_class = Cfdp::Class::CLASS_1;
    this->m_fsize = 0;
    this->m_foffs = 0;
    this->m_keep = Cfdp::Keep::KEEP;
    this->m_priority = 0;
    this->m_initType = INIT_BY_COMMAND;
    this->m_crc = CFDP::Checksum(0);
    this->m_pb = nullptr;

    // Use aggregate initialization to zero out unions
    this->m_state_data = {};
    this->m_flags = {};

    // Close the file if it is open
    if(this->m_fd.isOpen())
    {
        this->m_fd.close();
    }

    // The following state information is PRESERVED across reset (NOT modified):
    // - this->m_cfdpManager  // Channel binding
    // - this->m_chan         // Channel binding
    // - this->m_engine       // Channel binding
    // - this->m_chan_num     // Channel binding
    // - this->m_history      // Assigned when transaction is activated
    // - this->m_chunks       // Assigned when transaction is activated
    // - this->m_ack_timer    // Timer state preserved
    // - this->m_inactivity_timer // Timer state preserved
    // - this->m_cl_node      // Managed by queue operations in freeTransaction()
}

// ======================================================================
// RX State Machine - Public Methods
// ======================================================================

void Transaction::r1Recv(const Fw::Buffer& buffer) {
    static const FileDirectiveDispatchTable r1_fdir_handlers = {
        {
            nullptr, /* CFDP_FileDirective_INVALID_MIN */
            nullptr, /* 1 is unused in the CFDP_FileDirective_t enum */
            nullptr, /* 2 is unused in the CFDP_FileDirective_t enum */
            nullptr, /* 3 is unused in the CFDP_FileDirective_t enum */
            &Transaction::r1SubstateRecvEof, /* CFDP_FileDirective_EOF */
            nullptr, /* CFDP_FileDirective_FIN */
            nullptr, /* CFDP_FileDirective_ACK */
            nullptr, /* CFDP_FileDirective_METADATA */
            nullptr, /* CFDP_FileDirective_NAK */
            nullptr, /* CFDP_FileDirective_PROMPT */
            nullptr, /* 10 is unused in the CFDP_FileDirective_t enum */
            nullptr, /* 11 is unused in the CFDP_FileDirective_t enum */
            nullptr, /* CFDP_FileDirective_KEEP_ALIVE */
        }
    };

    static const RSubstateDispatchTable substate_fns = {
        {
            &r1_fdir_handlers, /* RX_SUB_STATE_FILEDATA */
            &r1_fdir_handlers, /* RX_SUB_STATE_EOF */
            &r1_fdir_handlers, /* RX_SUB_STATE_CLOSEOUT_SYNC */
        }
    };

    this->rDispatchRecv(buffer, &substate_fns, &Transaction::r1SubstateRecvFileData);
}

void Transaction::r2Recv(const Fw::Buffer& buffer) {
    static const FileDirectiveDispatchTable r2_fdir_handlers_normal = {
        {
            nullptr, /* CFDP_FileDirective_INVALID_MIN */
            nullptr, /* 1 is unused in the CFDP_FileDirective_t enum */
            nullptr, /* 2 is unused in the CFDP_FileDirective_t enum */
            nullptr, /* 3 is unused in the CFDP_FileDirective_t enum */
            &Transaction::r2SubstateRecvEof, /* CFDP_FileDirective_EOF */
            nullptr, /* CFDP_FileDirective_FIN */
            nullptr, /* CFDP_FileDirective_ACK */
            &Transaction::r2RecvMd, /* CFDP_FileDirective_METADATA */
            nullptr, /* CFDP_FileDirective_NAK */
            nullptr, /* CFDP_FileDirective_PROMPT */
            nullptr, /* 10 is unused in the CFDP_FileDirective_t enum */
            nullptr, /* 11 is unused in the CFDP_FileDirective_t enum */
            nullptr, /* CFDP_FileDirective_KEEP_ALIVE */
        }
    };
    static const FileDirectiveDispatchTable r2_fdir_handlers_finack = {
        {
            nullptr, /* CFDP_FileDirective_INVALID_MIN */
            nullptr, /* 1 is unused in the CFDP_FileDirective_t enum */
            nullptr, /* 2 is unused in the CFDP_FileDirective_t enum */
            nullptr, /* 3 is unused in the CFDP_FileDirective_t enum */
            &Transaction::r2SubstateRecvEof, /* CFDP_FileDirective_EOF */
            nullptr, /* CFDP_FileDirective_FIN */
            &Transaction::r2RecvFinAck, /* CFDP_FileDirective_ACK */
            nullptr, /* CFDP_FileDirective_METADATA */
            nullptr, /* CFDP_FileDirective_NAK */
            nullptr, /* CFDP_FileDirective_PROMPT */
            nullptr, /* 10 is unused in the CFDP_FileDirective_t enum */
            nullptr, /* 11 is unused in the CFDP_FileDirective_t enum */
            nullptr, /* CFDP_FileDirective_KEEP_ALIVE */
        }
    };

    static const RSubstateDispatchTable substate_fns = {
        {
            &r2_fdir_handlers_normal, /* RX_SUB_STATE_FILEDATA */
            &r2_fdir_handlers_normal, /* RX_SUB_STATE_EOF */
            &r2_fdir_handlers_finack, /* RX_SUB_STATE_CLOSEOUT_SYNC */
        }
    };

    this->rDispatchRecv(buffer, &substate_fns, &Transaction::r2SubstateRecvFileData);
}

void Transaction::rAckTimerTick() {
    U8 ack_limit = 0;

    /* note: the ack timer is only ever armed on class 2 */
    if (this->m_state != TXN_STATE_R2 || !this->m_flags.com.ack_timer_armed)
    {
        /* nothing to do */
        return;
    }

    if (this->m_ack_timer.getStatus() == Timer::Status::RUNNING)
    {
        this->m_ack_timer.run();
    }
    else
    {
        /* ACK timer expired, so check for completion */
        if (!this->m_flags.rx.complete)
        {
            this->r2Complete(true);
        }
        else if (this->m_state_data.receive.sub_state == RX_SUB_STATE_CLOSEOUT_SYNC)
        {
            /* Increment acknak counter */
            ++this->m_state_data.receive.r2.acknak_count;

            /* Check limit and handle if needed */
            ack_limit = this->m_cfdpManager->getAckLimitParam(this->m_chan_num);
            if (this->m_state_data.receive.r2.acknak_count >= ack_limit)
            {
                this->m_cfdpManager->log_WARNING_HI_RxAckLimitReached(
                    Cfdp::getClassDisplay(this->getClass()),
                    this->m_history->src_eid,
                    this->m_history->seq_num);
                this->m_engine->setTxnStatus(this, TXN_STATUS_ACK_LIMIT_NO_FIN);
                // ++CF_AppData.hk.Payload.channel_hk[this->m_chan_num].counters.fault.ack_limit;

                /* give up on this */
                this->m_engine->finishTransaction(this, true);
                this->m_flags.com.ack_timer_armed = false;
            }
            else
            {
                this->m_flags.rx.send_fin = true;
            }
        }

        /* re-arm the timer if it is still pending */
        if (this->m_flags.com.ack_timer_armed)
        {
            /* whether sending FIN or waiting for more filedata, need ACK timer armed */
            this->m_engine->armAckTimer(this);
        }
    }
}

void Transaction::rTick(int *cont /* unused */) {
    /* Steven is not real happy with this function. There should be a better way to separate out
     * the logic by state so that it isn't a bunch of if statements for different flags
     */

    Status::T sret;
    bool         pending_send;

    if (!this->m_flags.com.inactivity_fired)
    {
        if (this->m_inactivity_timer.getStatus() == Timer::Status::RUNNING)
        {
            this->m_inactivity_timer.run();
        }
        else
        {
            this->m_flags.com.inactivity_fired = true;

            /* HOLD state is the normal path to recycle transaction objects, not an error */
            /* inactivity is abnormal in any other state */
            if (this->m_state != TXN_STATE_HOLD)
            {
                this->rSendInactivityEvent();

                /* in class 2 this also triggers sending an early FIN response */
                if (this->m_state == TXN_STATE_R2)
                {
                    this->r2SetFinTxnStatus(TXN_STATUS_INACTIVITY_DETECTED);
                }
            }
        }
    }

    pending_send = true; /* maybe; tbd */

    /* rx maintenance: possibly process send_eof_ack, send_nak or send_fin */
    if (this->m_flags.rx.send_eof_ack)
    {
        sret = this->m_engine->sendAck(this, ACK_TXN_STATUS_ACTIVE, FILE_DIRECTIVE_END_OF_FILE,
                               static_cast<ConditionCode>(this->m_state_data.receive.r2.eof_cc),
                               this->m_history->peer_eid, this->m_history->seq_num);
        FW_ASSERT(sret != Cfdp::Status::SEND_PDU_ERROR);

        /* if Cfdp::Status::SUCCESS, then move on in the state machine. CFDP_SendAck does not return
         * SEND_PDU_ERROR */
        if (sret != Cfdp::Status::SEND_PDU_NO_BUF_AVAIL_ERROR)
        {
            this->m_flags.rx.send_eof_ack = false;
        }
    }
    else if (this->m_flags.rx.send_nak)
    {
        if (!this->rSubstateSendNak())
        {
            this->m_flags.rx.send_nak = false; /* will re-enter on error */
        }
    }
    else if (this->m_flags.rx.send_fin)
    {
        if (!this->r2SubstateSendFin())
        {
            this->m_flags.rx.send_fin = false; /* will re-enter on error */
        }
    }
    else
    {
        /* no pending responses to the sender */
        pending_send = false;
    }

    /* if the inactivity timer ran out, then there is no sense
     * pending for responses for anything.  Send out anything
     * that we need to send (i.e. the FIN) just in case the sender
     * is still listening to us but do not expect any future ACKs */
    if (this->m_flags.com.inactivity_fired && !pending_send)
    {
        /* the transaction is now recyclable - this means we will
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
        this->rAckTimerTick();
    }
}

void Transaction::rCancel() {
    /* for cancel, only need to send FIN if R2 */
    if ((this->m_state == TXN_STATE_R2) && (this->m_state_data.receive.sub_state < RX_SUB_STATE_CLOSEOUT_SYNC))
    {
        this->m_flags.rx.send_fin = true;
    }
    else
    {
        this->r1Reset(); /* if R1, just call it quits */
    }
}

void Transaction::rInit() {
    Os::File::Status status;
    Fw::String tmpDir;
    Fw::String dst;

    if (this->m_state == TXN_STATE_R2)
    {
        if (!this->m_flags.rx.md_recv)
        {
            tmpDir = this->m_cfdpManager->getTmpDirParam();
            /* we need to make a temp file and then do a NAK for md PDU */
            /* the transaction already has a history, and that has a buffer that we can use to
             * hold the temp filename which is defined by the sequence number and the source entity ID */

            // Create destination filepath with format: <tmpDir>/<src_eid>:<seq_num>.tmp
            dst.format("%s/%" CFDP_PRI_ENTITY_ID ":%" CFDP_PRI_TRANSACTION_SEQ ".tmp",
                       tmpDir.toChar(),
                       this->m_history->src_eid,
                       this->m_history->seq_num);

            this->m_history->fnames.dst_filename = dst;

            this->m_cfdpManager->log_ACTIVITY_LO_RxTempFileCreated(
                Cfdp::getClassDisplay(this->getClass()),
                this->m_history->src_eid,
                this->m_history->seq_num,
                this->m_history->fnames.dst_filename);
        }

        this->m_engine->armAckTimer(this);
    }

    status = this->m_fd.open(this->m_history->fnames.dst_filename.toChar(), Os::File::OPEN_CREATE, Os::File::OVERWRITE);
    if (status != Os::File::OP_OK)
    {
        this->m_cfdpManager->log_WARNING_HI_RxFileCreateFailed(
            Cfdp::getClassDisplay(this->getClass()),
            this->m_history->src_eid,
            this->m_history->seq_num,
            this->m_history->fnames.dst_filename,
            status);
        // ++CF_AppData.hk.Payload.channel_hk[this->m_chan_num].counters.fault.file_open;
        // this->m_fd = OS_OBJECT_ID_UNDEFINED; /* just in case */
        if (this->m_state == TXN_STATE_R2)
        {
            this->r2SetFinTxnStatus(TXN_STATUS_FILESTORE_REJECTION);
        }
        else
        {
            this->r1Reset();
        }
    }
    else
    {
        this->m_state_data.receive.sub_state = RX_SUB_STATE_FILEDATA;
    }
}

void Transaction::r2SetFinTxnStatus(TxnStatus txn_stat) {
    this->m_engine->setTxnStatus(this, txn_stat);
    this->m_flags.rx.send_fin = true;
}

void Transaction::r1Reset() {
    this->m_engine->finishTransaction(this, true);
}

void Transaction::r2Reset() {
    if ((this->m_state_data.receive.sub_state == RX_SUB_STATE_CLOSEOUT_SYNC) ||
        (this->m_state_data.receive.r2.eof_cc != CONDITION_CODE_NO_ERROR) ||
        TxnStatusIsError(this->m_history->txn_stat) || this->m_flags.com.canceled)
    {
        this->r1Reset(); /* it's done */
    }
    else
    {
        /* not waiting for FIN ACK, so trigger send FIN */
        this->m_flags.rx.send_fin = true;
    }
}

Status::T Transaction::rCheckCrc(U32 expected_crc) {
    Status::T ret = Cfdp::Status::SUCCESS;
    U32 crc_result;

    // The F' version does not have an equivalent finalize call as it
    // - Never stores a partial word internally
    // - Never needs to "flush" anything
    // - Always accounts for padding at update time
    crc_result = this->m_crc.getValue();
    if (crc_result != expected_crc)
    {
        this->m_cfdpManager->log_WARNING_HI_RxCrcMismatch(
            Cfdp::getClassDisplay(this->getClass()),
            this->m_history->src_eid,
            this->m_history->seq_num,
            expected_crc,
            crc_result);
        // ++CF_AppData.hk.Payload.channel_hk[this->m_chan_num].counters.fault.crc_mismatch;
        ret = Cfdp::Status::ERROR;
    }

    return ret;
}

void Transaction::r2Complete(int ok_to_send_nak) {
    U32 ret;
    bool send_nak = false;
    bool send_fin = false;
    U8 nack_limit = 0;
    /* checking if r2 is complete. Check NAK list, and send NAK if appropriate */
    /* if all data is present, then there will be no gaps in the chunk */

    if (!TxnStatusIsError(this->m_history->txn_stat))
    {
        /* first, check if md is received. If not, send specialized NAK */
        if (!this->m_flags.rx.md_recv)
        {
            send_nak = true;
        }
        else
        {
            /* only look for 1 gap, since the goal here is just to know that there are gaps */
            ret = this->m_chunks->chunks.computeGaps(1, this->m_fsize, 0, nullptr, nullptr);

            if (ret)
            {
                /* there is at least 1 gap, so send a NAK */
                send_nak = true;
            }
            else if (this->m_flags.rx.eof_recv)
            {
                /* the EOF was received, and there are no NAKs -- process completion in send FIN state */
                send_fin = true;
            }
        }

        if (send_nak && ok_to_send_nak)
        {
            /* Increment the acknak counter */
            ++this->m_state_data.receive.r2.acknak_count;

            /* Check limit and handle if needed */
            nack_limit = this->m_cfdpManager->getNackLimitParam(this->m_chan_num);
            if (this->m_state_data.receive.r2.acknak_count >= nack_limit)
            {
                this->m_cfdpManager->log_WARNING_HI_RxNakLimitReached(
                    Cfdp::getClassDisplay(this->getClass()),
                    this->m_history->src_eid,
                    this->m_history->seq_num);
                send_fin = true;
                // ++CF_AppData.hk.Payload.channel_hk[this->m_chan_num].counters.fault.nak_limit;
                /* don't use CFDP_R2_SetFinTxnStatus because many places in this function set send_fin */
                this->m_engine->setTxnStatus(this, TXN_STATUS_NAK_LIMIT_REACHED);
                this->m_state_data.receive.r2.acknak_count = 0; /* reset for fin/ack */
            }
            else
            {
                this->m_flags.rx.send_nak = true;
            }
        }

        if (send_fin)
        {
            this->m_flags.rx.complete = true; /* latch completeness, since send_fin is cleared later */

            /* the transaction is now considered complete, but this will not overwrite an
             * error status code if there was one set */
            this->r2SetFinTxnStatus(TXN_STATUS_NO_ERROR);
        }

        /* always go to RX_SUB_STATE_FILEDATA, and let tick change state */
        this->m_state_data.receive.sub_state = RX_SUB_STATE_FILEDATA;
    }
}

// ======================================================================
// RX State Machine - Private Helper Methods
// ======================================================================

Status::T Transaction::rProcessFd(const Fw::Buffer& buffer) {
    Status::T ret = Cfdp::Status::SUCCESS;

    /* this function is only entered for data PDUs */
    // Deserialize FileData PDU from buffer
    FileDataPdu fd;
    Fw::SerialBuffer sb(const_cast<U8*>(buffer.getData()), buffer.getSize());
    sb.setBuffLen(buffer.getSize());

    Fw::SerializeStatus deserStatus = fd.deserializeFrom(sb);
    if (deserStatus != Fw::FW_SERIALIZE_OK) {
        this->m_cfdpManager->log_WARNING_LO_FailFileDataPduDeserialization(this->getChannelId(), static_cast<I32>(deserStatus));
        ret = Cfdp::Status::ERROR;
    }

    /*
     * NOTE: The decode routine should have left a direct pointer to the data and actual data length
     * within the PDU.  The length has already been verified, too.  Should not need to make any
     * adjustments here, just write it.
     */

    FileSize offset = fd.getOffset();
    U16 dataSize = fd.getDataSize();
    const U8* dataPtr = fd.getData();

    // Seek to file offset if needed
    if (ret == Cfdp::Status::SUCCESS) {
        if (this->m_state_data.receive.cached_pos != offset)
        {
            Os::File::Status status = this->m_fd.seek(offset, Os::File::SeekType::ABSOLUTE);
            if (status != Os::File::OP_OK)
            {
                this->m_cfdpManager->log_WARNING_HI_RxSeekFailed(
                    Cfdp::getClassDisplay(this->getClass()),
                    this->m_history->src_eid,
                    this->m_history->seq_num,
                    offset,
                    status);
                this->m_engine->setTxnStatus(this, TXN_STATUS_FILE_SIZE_ERROR);
                // ++CF_AppData.hk.Payload.channel_hk[this->m_chan_num].counters.fault.file_seek;
                ret = Cfdp::Status::ERROR;
            }
        }
    }

    // Write file data
    if (ret == Cfdp::Status::SUCCESS)
    {
        FwSizeType write_size = dataSize;
        Os::File::Status status = this->m_fd.write(dataPtr, write_size, Os::File::WaitType::WAIT);
        if (status != Os::File::OP_OK)
        {
            this->m_cfdpManager->log_WARNING_HI_RxWriteFailed(
                Cfdp::getClassDisplay(this->getClass()),
                this->m_history->src_eid,
                this->m_history->seq_num,
                dataSize,
                static_cast<I32>(write_size));
            this->m_engine->setTxnStatus(this, TXN_STATUS_FILESTORE_REJECTION);
            // ++CF_AppData.hk.Payload.channel_hk[this->m_chan_num].counters.fault.file_write;
            ret = Cfdp::Status::ERROR;
        }
        else
        {
            this->m_state_data.receive.cached_pos = static_cast<FileSize>(dataSize) + offset;
            // CF_AppData.hk.Payload.channel_hk[this->m_chan_num].counters.recv.file_data_bytes += pdu->data_len;
        }
    }

    return ret;
}

Status::T Transaction::rSubstateRecvEof(const Fw::Buffer& buffer) {
    Status::T ret = Cfdp::Status::SUCCESS;

    // Deserialize EOF PDU from buffer
    EofPdu eof;
    Fw::SerialBuffer sb(const_cast<U8*>(buffer.getData()), buffer.getSize());
    sb.setBuffLen(buffer.getSize());

    Fw::SerializeStatus deserStatus = eof.deserializeFrom(sb);
    if (deserStatus != Fw::FW_SERIALIZE_OK) {
        this->m_cfdpManager->log_WARNING_LO_FailEofPduDeserialization(this->getChannelId(), static_cast<I32>(deserStatus));
        ret = Cfdp::Status::REC_PDU_BAD_EOF_ERROR;
    }

    if (ret == Cfdp::Status::SUCCESS)
    {
        if (!this->m_engine->recvEof(this, eof))
        {
            /* this function is only entered for PDUs identified as EOF type */

            /* only check size if MD received, otherwise it's still OK */
            if (this->m_flags.rx.md_recv && (eof.getFileSize() != this->m_fsize))
            {
                this->m_cfdpManager->log_WARNING_HI_RxFileSizeMismatch(
                    Cfdp::getClassDisplay(this->getClass()),
                    this->m_history->src_eid,
                    this->m_history->seq_num,
                    this->m_fsize,
                    eof.getFileSize());
                // ++CF_AppData.hk.Payload.channel_hk[this->m_chan_num].counters.fault.file_size_mismatch;
                ret = Cfdp::Status::REC_PDU_FSIZE_MISMATCH_ERROR;
            }
        }
        else
        {
            this->m_cfdpManager->log_WARNING_LO_RxInvalidEofPdu(
                Cfdp::getClassDisplay(this->getClass()),
                this->m_history->src_eid,
                this->m_history->seq_num);
            // ++CF_AppData.hk.Payload.channel_hk[this->m_chan_num].counters.recv.error;
            ret = Cfdp::Status::REC_PDU_BAD_EOF_ERROR;
        }
    }

    return ret;
}

void Transaction::r1SubstateRecvEof(const Fw::Buffer& buffer) {
    // Deserialize EOF PDU from buffer
    EofPdu eof;
    Fw::SerialBuffer sb(const_cast<U8*>(buffer.getData()), buffer.getSize());
    sb.setBuffLen(buffer.getSize());

    Fw::SerializeStatus deserStatus = eof.deserializeFrom(sb);
    if (deserStatus != Fw::FW_SERIALIZE_OK) {
        // Bad EOF, reset transaction
        this->m_cfdpManager->log_WARNING_LO_FailEofPduDeserialization(this->getChannelId(), static_cast<I32>(deserStatus));
        this->r1Reset();
        return;
    }

    Status::T ret = this->rSubstateRecvEof(buffer);
    U32 crc;

    /* this function is only entered for PDUs identified as EOF type */
    crc = eof.getChecksum();

    if (ret == Cfdp::Status::SUCCESS)
    {
        /* Verify CRC */
        if (this->rCheckCrc(crc) == Cfdp::Status::SUCCESS)
        {
            /* successfully processed the file */
            this->m_keep = Cfdp::Keep::KEEP; /* save the file */
        }
        /* if file failed to process, there's nothing to do. CFDP_R_CheckCrc() generates an event on failure */
    }

    /* after exit, always reset since we are done */
    /* reset even if the EOF failed -- class 1, so it won't come again! */
    this->r1Reset();
}

void Transaction::r2SubstateRecvEof(const Fw::Buffer& buffer) {
    Status::T ret;

    if (!this->m_flags.rx.eof_recv)
    {
        // Deserialize EOF PDU from buffer
        EofPdu eof;
        Fw::SerialBuffer sb(const_cast<U8*>(buffer.getData()), buffer.getSize());
        sb.setBuffLen(buffer.getSize());

        Fw::SerializeStatus deserStatus = eof.deserializeFrom(sb);
        if (deserStatus != Fw::FW_SERIALIZE_OK) {
            // Bad EOF, return to FILEDATA substate
            this->m_cfdpManager->log_WARNING_LO_FailEofPduDeserialization(this->getChannelId(), static_cast<I32>(deserStatus));
            this->m_state_data.receive.sub_state = RX_SUB_STATE_FILEDATA;
            return;
        }

        ret = this->rSubstateRecvEof(buffer);

        /* did receiving EOF succeed? */
        if (ret == Cfdp::Status::SUCCESS)
        {

            this->m_flags.rx.eof_recv = true;

            /* need to remember the EOF CRC for later */
            this->m_state_data.receive.r2.eof_crc  = eof.getChecksum();
            this->m_state_data.receive.r2.eof_size = eof.getFileSize();

            /* always ACK the EOF, even if we're not done */
            this->m_state_data.receive.r2.eof_cc = static_cast<U8>(eof.getConditionCode());
            this->m_flags.rx.send_eof_ack        = true; /* defer sending ACK to tick handling */

            /* only check for complete if EOF with no errors */
            if (this->m_state_data.receive.r2.eof_cc == CONDITION_CODE_NO_ERROR)
            {
                this->r2Complete(true); /* CFDP_R2_Complete() will change state */
            }
            else
            {
                /* All CFDP CC values directly correspond to a Transaction Status of the same numeric value */
                this->m_engine->setTxnStatus(this, static_cast<TxnStatus>(static_cast<I32>(this->m_state_data.receive.r2.eof_cc)));
                this->r2Reset();
            }
        }
        else
        {
            /* bad EOF sent? */
            if (ret == Cfdp::Status::REC_PDU_FSIZE_MISMATCH_ERROR)
            {
                this->r2SetFinTxnStatus(TXN_STATUS_FILE_SIZE_ERROR);
            }
            else
            {
                /* can't do anything with this bad EOF, so return to FILEDATA */
                this->m_state_data.receive.sub_state = RX_SUB_STATE_FILEDATA;
            }
        }
    }
}

void Transaction::r1SubstateRecvFileData(const Fw::Buffer& buffer) {
    Status::T ret;

    // Deserialize FileData PDU from buffer
    FileDataPdu fd;
    Fw::SerialBuffer sb(const_cast<U8*>(buffer.getData()), buffer.getSize());
    sb.setBuffLen(buffer.getSize());

    Fw::SerializeStatus deserStatus = fd.deserializeFrom(sb);
    if (deserStatus != Fw::FW_SERIALIZE_OK) {
        // Bad file data PDU, reset transaction
        this->m_cfdpManager->log_WARNING_LO_FailFileDataPduDeserialization(this->getChannelId(), static_cast<I32>(deserStatus));
        this->r1Reset();
        return;
    }

    /* got file data PDU? */
    ret = this->m_engine->recvFd(this, fd);
    if (ret == Cfdp::Status::SUCCESS)
    {
        ret = this->rProcessFd(buffer);
    }

    if (ret == Cfdp::Status::SUCCESS)
    {
        /* class 1 digests CRC */
        this->m_crc.update(fd.getData(), fd.getOffset(),
                        static_cast<U32>(fd.getDataSize()));
    }
    else
    {
        /* Reset transaction on failure */
        this->r1Reset();
    }
}

void Transaction::r2SubstateRecvFileData(const Fw::Buffer& buffer) {
    Status::T ret;

    // If CRC calculation has started (file reopened in READ mode), ignore late FileData PDUs.
    // This can happen if retransmitted FileData arrives after EOF was received and CRC began.
    if (this->m_state_data.receive.r2.rx_crc_calc_bytes > 0)
    {
        // Silently ignore - file is complete and we're calculating CRC
        // TODO BPC: do we want a throttled EVR here?
        return;
    }

    // Deserialize FileData PDU from buffer
    FileDataPdu fd;
    Fw::SerialBuffer sb(const_cast<U8*>(buffer.getData()), buffer.getSize());
    sb.setBuffLen(buffer.getSize());

    Fw::SerializeStatus deserStatus = fd.deserializeFrom(sb);
    if (deserStatus != Fw::FW_SERIALIZE_OK) {
        // Bad file data PDU, reset transaction
        this->m_cfdpManager->log_WARNING_LO_FailFileDataPduDeserialization(this->getChannelId(), static_cast<I32>(deserStatus));
        this->r2Reset();
        return;
    }

    /* got file data PDU? */
    ret = this->m_engine->recvFd(this, fd);
    if (ret == Cfdp::Status::SUCCESS)
    {
        ret = this->rProcessFd(buffer);
    }

    if (ret == Cfdp::Status::SUCCESS)
    {
        /* class 2 does CRC at FIN, but track gaps */
        this->m_chunks->chunks.add(fd.getOffset(), static_cast<FileSize>(fd.getDataSize()));

        if (this->m_flags.rx.fd_nak_sent)
        {
            this->r2Complete(false); /* once nak-retransmit received, start checking for completion at each fd */
        }

        if (!this->m_flags.rx.complete)
        {
            this->m_engine->armAckTimer(this); /* re-arm ACK timer, since we got data */
        }

        this->m_state_data.receive.r2.acknak_count = 0;
    }
    else
    {
        /* Reset transaction on failure */
        this->r2Reset();
    }
}

void Transaction::r2GapCompute(const Chunk *chunk, NakPdu& nak) {
    FW_ASSERT(chunk->size > 0, chunk->size);

    // Calculate segment offsets relative to scope start
    FileSize offsetStart = chunk->offset - nak.getScopeStart();
    FileSize offsetEnd = offsetStart + chunk->size;

    // Add segment to NAK PDU (returns false if array is full)
    nak.addSegment(offsetStart, offsetEnd);
}

Status::T Transaction::rSubstateSendNak() {
    Status::T status = Cfdp::Status::SUCCESS;

    // Create and initialize NAK PDU
    NakPdu nakPdu;
    Cfdp::PduDirection direction = DIRECTION_TOWARD_SENDER;

    if (this->m_flags.rx.md_recv) {
        // We have metadata, so send NAK with file data gaps
        nakPdu.initialize(
            direction,
            this->getClass(),  // transmission mode
            this->m_history->peer_eid,  // source EID (receiver)
            this->m_history->seq_num,  // transaction sequence number
            this->m_cfdpManager->getLocalEidParam(),  // destination EID (sender)
            0,  // scope start
            0   // scope end
        );

        // Compute gaps and add segments to NAK PDU
        U32 chunkCount = this->m_chunks->chunks.getCount();
        U32 maxChunks = this->m_chunks->chunks.getMaxChunks();
        U32 gapLimit = (chunkCount < maxChunks) ? maxChunks : (maxChunks - 1);

        // For each gap found, add it as a segment to the NAK PDU via callback
        U32 gapCount = this->m_chunks->chunks.computeGaps(
            static_cast<ChunkIdx>(gapLimit),
            this->m_fsize,
            0,
            [this, &nakPdu](const Chunk* chunk, void* opaque) {
                this->r2GapCompute(chunk, nakPdu);
            },
            nullptr);

        if (!gapCount) {
            // No gaps left, file reception is complete
            this->m_flags.rx.complete = true;
            status = Cfdp::Status::SUCCESS;
        } else {
            // Gaps are present, send the NAK PDU
            status = this->m_engine->sendNak(this, nakPdu);
            if (status == Cfdp::Status::SUCCESS) {
                this->m_flags.rx.fd_nak_sent = true;
                // CF_AppData.hk.Payload.channel_hk[this->m_chan_num].counters.sent.nak_segment_requests += gapCount;
            }
        }
    } else {
        // Need to send NAK to request metadata PDU again
        // Special case: scope start/end and segment[0] all zeros requests metadata
        nakPdu.initialize(
            direction,
            this->getClass(),  // transmission mode
            this->m_history->peer_eid,  // source EID (receiver)
            this->m_history->seq_num,  // transaction sequence number
            this->m_cfdpManager->getLocalEidParam(),  // destination EID (sender)
            0,  // scope start (special value)
            0   // scope end (special value)
        );

        // Add special segment [0,0] to request metadata
        nakPdu.addSegment(0, 0);

        status = this->m_engine->sendNak(this, nakPdu);
    }

    return status;
}

Status::T Transaction::r2CalcCrcChunk() {
    U8 buf[CFDP_R2_CRC_CHUNK_SIZE];
    FileSize count_bytes;
    FileSize want_offs_size;
    FwSizeType read_size;
    Os::File::Status fileStatus;
    Status::T ret = Cfdp::Status::SUCCESS;
    FileSize rx_crc_calc_bytes_per_wakeup = 0;

    memset(buf, 0, sizeof(buf));

    count_bytes = 0;

    // Open file for CRC calculation if needed
    if (ret == Cfdp::Status::SUCCESS) {
        if (this->m_state_data.receive.r2.rx_crc_calc_bytes == 0)
        {
            this->m_crc = CFDP::Checksum(0);

            // For Class 2 RX, the file was opened in WRITE mode for receiving FileData PDUs.
            // Now we need to READ it for CRC calculation. Close and reopen in READ mode.
            if (this->m_fd.isOpen())
            {
                this->m_fd.close();
            }

            fileStatus = this->m_fd.open(this->m_history->fnames.dst_filename.toChar(), Os::File::OPEN_READ);
            if (fileStatus != Os::File::OP_OK)
            {
                this->m_engine->setTxnStatus(this, TXN_STATUS_FILE_SIZE_ERROR);
                ret = Cfdp::Status::ERROR;
            } else {
                // Reset cached position since we just reopened the file
                this->m_state_data.receive.cached_pos = 0;
            }
        }
    }

    // Process file in chunks
    if (ret == Cfdp::Status::SUCCESS) {
        rx_crc_calc_bytes_per_wakeup = this->m_cfdpManager->getRxCrcCalcBytesPerWakeupParam();

        while ((ret == Cfdp::Status::SUCCESS) &&
               (count_bytes < rx_crc_calc_bytes_per_wakeup) &&
               (this->m_state_data.receive.r2.rx_crc_calc_bytes < this->m_fsize))
        {
            want_offs_size = this->m_state_data.receive.r2.rx_crc_calc_bytes + sizeof(buf);

            if (want_offs_size > this->m_fsize)
            {
                read_size = this->m_fsize - this->m_state_data.receive.r2.rx_crc_calc_bytes;
            }
            else
            {
                read_size = sizeof(buf);
            }

            if (this->m_state_data.receive.cached_pos != this->m_state_data.receive.r2.rx_crc_calc_bytes)
            {
                fileStatus = this->m_fd.seek(this->m_state_data.receive.r2.rx_crc_calc_bytes, Os::File::SeekType::ABSOLUTE);
                if (fileStatus != Os::File::OP_OK)
                {
                    this->m_cfdpManager->log_WARNING_HI_RxSeekCrcFailed(
                        Cfdp::getClassDisplay(this->getClass()),
                        this->m_history->src_eid,
                        this->m_history->seq_num,
                        this->m_state_data.receive.r2.rx_crc_calc_bytes,
                        fileStatus);
                    // this->m_engine->setTxnStatus(this, TXN_STATUS_FILE_SIZE_ERROR);
                    // ++CF_AppData.hk.Payload.channel_hk[this->m_chan_num].counters.fault.file_seek;
                    ret = Cfdp::Status::ERROR;
                }
            }

            if (ret == Cfdp::Status::SUCCESS) {
                FwSizeType expected_read_size = read_size;
                fileStatus = this->m_fd.read(buf, read_size, Os::File::WaitType::WAIT);
                if (fileStatus != Os::File::OP_OK)
                {
                    this->m_cfdpManager->log_WARNING_HI_RxReadCrcFailed(
                        Cfdp::getClassDisplay(this->getClass()),
                        this->m_history->src_eid,
                        this->m_history->seq_num,
                        static_cast<U32>(expected_read_size),
                        static_cast<I32>(read_size));
                    this->m_engine->setTxnStatus(this, TXN_STATUS_FILE_SIZE_ERROR);
                    // ++CF_AppData.hk.Payload.channel_hk[this->m_chan_num].counters.fault.file_read;
                    ret = Cfdp::Status::ERROR;
                } else {
                    this->m_crc.update(buf, this->m_state_data.receive.r2.rx_crc_calc_bytes, static_cast<U32>(read_size));
                    this->m_state_data.receive.r2.rx_crc_calc_bytes += static_cast<FileSize>(read_size);
                    this->m_state_data.receive.cached_pos = this->m_state_data.receive.r2.rx_crc_calc_bytes;
                    count_bytes += static_cast<FileSize>(read_size);
                }
            }
        }
    }

    // Check final CRC if all bytes processed
    if (ret == Cfdp::Status::SUCCESS) {
        if (this->m_state_data.receive.r2.rx_crc_calc_bytes == this->m_fsize)
        {
            /* all bytes calculated, so now check */
            if (this->rCheckCrc(this->m_state_data.receive.r2.eof_crc) == Cfdp::Status::SUCCESS)
            {
                /* CRC matched! We are happy */
                this->m_keep = Cfdp::Keep::KEEP; /* save the file */

                /* set FIN PDU status */
                this->m_state_data.receive.r2.dc = FIN_DELIVERY_CODE_COMPLETE;
                this->m_state_data.receive.r2.fs = FIN_FILE_STATUS_RETAINED;
            }
            else
            {
                this->r2SetFinTxnStatus(TXN_STATUS_FILE_CHECKSUM_FAILURE);
            }

            this->m_flags.com.crc_calc = true;
        } else {
            // Not all bytes processed yet, return ERROR to signal need to continue
            ret = Cfdp::Status::ERROR;
        }
    }

    return ret;
}

Status::T Transaction::r2SubstateSendFin() {
    Status::T sret;
    Status::T ret = Cfdp::Status::SUCCESS;

    if (!TxnStatusIsError(this->m_history->txn_stat) && !this->m_flags.com.crc_calc)
    {
        /* no error, and haven't checked CRC -- so start checking it */
        if (this->r2CalcCrcChunk())
        {
            ret = Cfdp::Status::ERROR; /* signal to caller to re-enter next tick */
        }
    }

    if (ret != Cfdp::Status::ERROR)
    {
        sret = this->m_engine->sendFin(this, this->m_state_data.receive.r2.dc, this->m_state_data.receive.r2.fs,
                               static_cast<ConditionCode>(TxnStatusToConditionCode(this->m_history->txn_stat)));
        /* CFDP_SendFin does not return SEND_PDU_ERROR */
        FW_ASSERT(sret != Cfdp::Status::SEND_PDU_ERROR);
        this->m_state_data.receive.sub_state =
            RX_SUB_STATE_CLOSEOUT_SYNC; /* whether or not FIN send successful, ok to transition state */
        if (sret != Cfdp::Status::SUCCESS)
        {
            ret = Cfdp::Status::ERROR;
        }
    }

    /* if no message, then try again next time */
    return ret;
}

void Transaction::r2RecvFinAck(const Fw::Buffer& buffer) {
    // Deserialize ACK PDU from buffer
    AckPdu ack;
    Fw::SerialBuffer sb(const_cast<U8*>(buffer.getData()), buffer.getSize());
    sb.setBuffLen(buffer.getSize());

    Fw::SerializeStatus deserStatus = ack.deserializeFrom(sb);
    if (deserStatus != Fw::FW_SERIALIZE_OK) {
        // Bad ACK PDU
        this->m_cfdpManager->log_WARNING_LO_FailAckPduDeserialization(this->getChannelId(), static_cast<I32>(deserStatus));
        // ++CF_AppData.hk.Payload.channel_hk[this->m_chan_num].counters.recv.error;
        return;
    }

    if (!this->m_engine->recvAck(this, ack))
    {
        /* got fin-ack, so time to close the state */
        this->r2Reset();
    }
    else
    {
        // TODO JMP Not converted to EVR because this is unreachable code. Verify implementation of recvAck
        // CFE Event: CFDP_R_PDU_FINACK_ERR_EID, EventType_ERROR, "CF R%d(%lu:%lu): invalid fin-ack",
        //            (this->m_state == TXN_STATE_R2), (unsigned long)this->m_history->src_eid,
        //            (unsigned long)this->m_history->seq_num);
        // ++CF_AppData.hk.Payload.channel_hk[this->m_chan_num].counters.recv.error;
    }
}

void Transaction::r2RecvMd(const Fw::Buffer& buffer) {
    Fw::String fname;
    Os::File::Status fileStatus;
    Os::FileSystem::Status fileSysStatus;
    bool success = true;

    /* it isn't an error to get another MD PDU, right? */
    if (!this->m_flags.rx.md_recv)
    {
        /* NOTE: this->m_flags.rx.md_recv always 1 in R1, so this is R2 only */
        /* parse the md PDU. this will overwrite the transaction's history, which contains our filename. so let's
         * save the filename in a local buffer so it can be used with moveFile upon successful parsing of
         * the md PDU */
        fname = this->m_history->fnames.dst_filename;

        // Deserialize Metadata PDU from buffer
        MetadataPdu md;
        Fw::SerialBuffer sb(const_cast<U8*>(buffer.getData()), buffer.getSize());
        sb.setBuffLen(buffer.getSize());

        Fw::SerializeStatus deserStatus = md.deserializeFrom(sb);
        if (deserStatus != Fw::FW_SERIALIZE_OK) {
            // Bad metadata PDU
            this->m_cfdpManager->log_WARNING_LO_FailMetadataPduDeserialization(this->getChannelId(), static_cast<I32>(deserStatus));
            return;
        }

        // PDU validation already done during deserialization
        this->m_engine->recvMd(this, md);

        /* successfully obtained md PDU */
        if (this->m_flags.rx.eof_recv)
        {
            /* EOF was received, so check that md and EOF sizes match */
            if (this->m_state_data.receive.r2.eof_size != this->m_fsize)
            {
                this->m_cfdpManager->log_WARNING_HI_RxEofMdSizeMismatch(
                    Cfdp::getClassDisplay(this->getClass()),
                    this->m_history->src_eid,
                    this->m_history->seq_num,
                    this->m_fsize,
                    this->m_state_data.receive.r2.eof_size);
                // ++CF_AppData.hk.Payload.channel_hk[this->m_chan_num].counters.fault.file_size_mismatch;
                this->r2SetFinTxnStatus(TXN_STATUS_FILE_SIZE_ERROR);
                success = false;
            }
        }

        if (success)
        {
            /* close and rename file */
            this->m_fd.close();

            fileSysStatus = Os::FileSystem::moveFile(fname.toChar(),
                                                     this->m_history->fnames.dst_filename.toChar());
            if (fileSysStatus != Os::FileSystem::OP_OK)
            {
                this->m_cfdpManager->log_WARNING_HI_RxFileRenameFailed(
                    Cfdp::getClassDisplay(this->getClass()),
                    this->m_history->src_eid,
                    this->m_history->seq_num,
                    fname,
                    this->m_history->fnames.dst_filename,
                    fileSysStatus);
                // this->m_fd = OS_OBJECT_ID_UNDEFINED;
                this->r2SetFinTxnStatus(TXN_STATUS_FILESTORE_REJECTION);
                // ++CF_AppData.hk.Payload.channel_hk[this->m_chan_num].counters.fault.file_rename;
                success = false;
            }
            else
            {
                 // File was successfully renamed, open for writing
                fileStatus = this->m_fd.open(this->m_history->fnames.dst_filename.toChar(), Os::File::OPEN_WRITE);
                if (fileStatus != Os::File::OP_OK)
                {
                    this->m_cfdpManager->log_WARNING_HI_RxFileReopenFailed(
                        Cfdp::getClassDisplay(this->getClass()),
                        this->m_history->src_eid,
                        this->m_history->seq_num,
                        this->m_history->fnames.dst_filename,
                        fileStatus);
                    this->r2SetFinTxnStatus(TXN_STATUS_FILESTORE_REJECTION);
                    // ++CF_AppData.hk.Payload.channel_hk[this->m_chan_num].counters.fault.file_open;
                    // this->m_fd = OS_OBJECT_ID_UNDEFINED; /* just in case */
                    success = false;
                }
            }

            if (success)
            {
                this->m_state_data.receive.cached_pos      = 0; /* reset psn due to open */
                this->m_flags.rx.md_recv                   = true;
                this->m_state_data.receive.r2.acknak_count = 0; /* in case part of NAK */
                this->r2Complete(true);                 /* check for completion now that md is received */
            }
        }
    }
}

void Transaction::rSendInactivityEvent() {
    this->m_cfdpManager->log_WARNING_HI_RxInactivityTimeout(
        Cfdp::getClassDisplay(this->getClass()),
        this->m_history->src_eid,
        this->m_history->seq_num);
    // ++CF_AppData.hk.Payload.channel_hk[this->m_chan_num].counters.fault.inactivity_timer;
}

// ======================================================================
// Dispatch Methods
// ======================================================================

void Transaction::rDispatchRecv(const Fw::Buffer& buffer,
                                    const RSubstateDispatchTable *dispatch,
                                    StateRecvFunc fd_fn)
{
    StateRecvFunc selected_handler;

    FW_ASSERT(this->m_state_data.receive.sub_state < RX_SUB_STATE_NUM_STATES,
              this->m_state_data.receive.sub_state, RX_SUB_STATE_NUM_STATES);

    selected_handler = NULL;

    // Peek at PDU type from buffer
    Cfdp::PduTypeEnum pduType = Cfdp::peekPduType(buffer);

    // Special handling for file data PDU
    if (pduType == Cfdp::T_FILE_DATA)
    {
        /* For file data PDU, use the provided fd_fn */
        if (!TxnStatusIsError(this->m_history->txn_stat))
        {
            selected_handler = fd_fn;
        }
        else
        {
            // ++CF_AppData.hk.Payload.channel_hk[this->m_chan_num].counters.recv.dropped;
        }
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
                    /* the CFDP_R_SubstateDispatchTable_t is only used with file directive PDU */
                    if (dispatch->state[this->m_state_data.receive.sub_state] != NULL)
                    {
                        selected_handler = dispatch->state[this->m_state_data.receive.sub_state]->fdirective[directiveCode];
                    }
                }
                else
                {
                    // ++CF_AppData.hk.Payload.channel_hk[this->m_chan_num].counters.recv.spurious;
                    this->m_cfdpManager->log_WARNING_LO_RxInvalidDirectiveCode(
                        Cfdp::getClassDisplay(this->getClass()),
                        this->m_history->src_eid,
                        this->m_history->seq_num,
                        directiveCodeByte,
                        this->m_state_data.receive.sub_state);
                }
            }
        }
    }

    /*
     * NOTE: if no handler is selected, this will drop packets on the floor here,
     * without incrementing any counter.  This was existing behavior.
     */
    if (selected_handler != NULL)
    {
        (this->*selected_handler)(buffer);
    }
}

}  // namespace Cfdp
}  // namespace Ccsds
}  // namespace Svc

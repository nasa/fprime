// ======================================================================
// \title  CfdpRxTransaction.cpp
// \brief  cpp file for CFDP RX Transaction state machine
//
// This file is a port of the cf_cfdp_r.c and cf_cfdp_dispatch.c files from the
// NASA Core Flight System (cFS) CFDP (CF) Application,
// version 3.0.0, adapted for use within the F-Prime (F') framework.
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

#include <Svc/Ccsds/CfdpManager/CfdpTransaction.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpManager.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpEngine.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpChannel.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpUtils.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpChunk.hpp>

namespace Svc {
namespace Ccsds {

// ======================================================================
// Construction and Destruction
// ======================================================================

CfdpTransaction::CfdpTransaction(CfdpChannel* channel, U8 channelId, CfdpEngine* engine, CfdpManager* manager) :
    m_state(CF_TxnState_UNDEF),
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

CfdpTransaction::~CfdpTransaction() { }

void CfdpTransaction::reset()
{
    // Reset transaction state to default values
    this->m_state = CF_TxnState_UNDEF;
    this->m_txn_class = Cfdp::Class::CLASS_1;
    this->m_fsize = 0;
    this->m_foffs = 0;
    this->m_keep = Cfdp::Keep::KEEP;
    this->m_priority = 0;
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

void CfdpTransaction::r1Recv(CF_Logical_PduBuffer_t *ph) {
    static const CF_CFDP_FileDirectiveDispatchTable_t r1_fdir_handlers = {
        {
            nullptr, /* CF_CFDP_FileDirective_INVALID_MIN */
            nullptr, /* 1 is unused in the CF_CFDP_FileDirective_t enum */
            nullptr, /* 2 is unused in the CF_CFDP_FileDirective_t enum */
            nullptr, /* 3 is unused in the CF_CFDP_FileDirective_t enum */
            &CfdpTransaction::r1SubstateRecvEof, /* CF_CFDP_FileDirective_EOF */
            nullptr, /* CF_CFDP_FileDirective_FIN */
            nullptr, /* CF_CFDP_FileDirective_ACK */
            nullptr, /* CF_CFDP_FileDirective_METADATA */
            nullptr, /* CF_CFDP_FileDirective_NAK */
            nullptr, /* CF_CFDP_FileDirective_PROMPT */
            nullptr, /* 10 is unused in the CF_CFDP_FileDirective_t enum */
            nullptr, /* 11 is unused in the CF_CFDP_FileDirective_t enum */
            nullptr, /* CF_CFDP_FileDirective_KEEP_ALIVE */
        }
    };

    static const CF_CFDP_R_SubstateDispatchTable_t substate_fns = {
        {
            &r1_fdir_handlers, /* CF_RxSubState_FILEDATA */
            &r1_fdir_handlers, /* CF_RxSubState_EOF */
            &r1_fdir_handlers, /* CF_RxSubState_CLOSEOUT_SYNC */
        }
    };

    this->rDispatchRecv(ph, &substate_fns, &CfdpTransaction::r1SubstateRecvFileData);
}

void CfdpTransaction::r2Recv(CF_Logical_PduBuffer_t *ph) {
    static const CF_CFDP_FileDirectiveDispatchTable_t r2_fdir_handlers_normal = {
        {
            nullptr, /* CF_CFDP_FileDirective_INVALID_MIN */
            nullptr, /* 1 is unused in the CF_CFDP_FileDirective_t enum */
            nullptr, /* 2 is unused in the CF_CFDP_FileDirective_t enum */
            nullptr, /* 3 is unused in the CF_CFDP_FileDirective_t enum */
            &CfdpTransaction::r2SubstateRecvEof, /* CF_CFDP_FileDirective_EOF */
            nullptr, /* CF_CFDP_FileDirective_FIN */
            nullptr, /* CF_CFDP_FileDirective_ACK */
            &CfdpTransaction::r2RecvMd, /* CF_CFDP_FileDirective_METADATA */
            nullptr, /* CF_CFDP_FileDirective_NAK */
            nullptr, /* CF_CFDP_FileDirective_PROMPT */
            nullptr, /* 10 is unused in the CF_CFDP_FileDirective_t enum */
            nullptr, /* 11 is unused in the CF_CFDP_FileDirective_t enum */
            nullptr, /* CF_CFDP_FileDirective_KEEP_ALIVE */
        }
    };
    static const CF_CFDP_FileDirectiveDispatchTable_t r2_fdir_handlers_finack = {
        {
            nullptr, /* CF_CFDP_FileDirective_INVALID_MIN */
            nullptr, /* 1 is unused in the CF_CFDP_FileDirective_t enum */
            nullptr, /* 2 is unused in the CF_CFDP_FileDirective_t enum */
            nullptr, /* 3 is unused in the CF_CFDP_FileDirective_t enum */
            &CfdpTransaction::r2SubstateRecvEof, /* CF_CFDP_FileDirective_EOF */
            nullptr, /* CF_CFDP_FileDirective_FIN */
            &CfdpTransaction::r2RecvFinAck, /* CF_CFDP_FileDirective_ACK */
            nullptr, /* CF_CFDP_FileDirective_METADATA */
            nullptr, /* CF_CFDP_FileDirective_NAK */
            nullptr, /* CF_CFDP_FileDirective_PROMPT */
            nullptr, /* 10 is unused in the CF_CFDP_FileDirective_t enum */
            nullptr, /* 11 is unused in the CF_CFDP_FileDirective_t enum */
            nullptr, /* CF_CFDP_FileDirective_KEEP_ALIVE */
        }
    };

    static const CF_CFDP_R_SubstateDispatchTable_t substate_fns = {
        {
            &r2_fdir_handlers_normal, /* CF_RxSubState_FILEDATA */
            &r2_fdir_handlers_normal, /* CF_RxSubState_EOF */
            &r2_fdir_handlers_finack, /* CF_RxSubState_CLOSEOUT_SYNC */
        }
    };

    this->rDispatchRecv(ph, &substate_fns, &CfdpTransaction::r2SubstateRecvFileData);
}

void CfdpTransaction::rAckTimerTick() {
    U8 ack_limit = 0;

    /* note: the ack timer is only ever armed on class 2 */
    if (this->m_state != CF_TxnState_R2 || !this->m_flags.com.ack_timer_armed)
    {
        /* nothing to do */
        return;
    }

    if (this->m_ack_timer.getStatus() == CfdpTimer::Status::RUNNING)
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
        else if (this->m_state_data.receive.sub_state == CF_RxSubState_CLOSEOUT_SYNC)
        {
            /* Increment acknak counter */
            ++this->m_state_data.receive.r2.acknak_count;

            /* Check limit and handle if needed */
            ack_limit = this->m_cfdpManager->getAckLimitParam(this->m_chan_num);
            if (this->m_state_data.receive.r2.acknak_count >= ack_limit)
            {
                // CFE_EVS_SendEvent(CF_CFDP_R_ACK_LIMIT_ERR_EID, CFE_EVS_EventType_ERROR,
                //                   "CF R2(%lu:%lu): ACK limit reached, no fin-ack", (unsigned long)this->m_history->src_eid,
                //                   (unsigned long)this->m_history->seq_num);
                this->m_engine->setTxnStatus(this, CF_TxnStatus_ACK_LIMIT_NO_FIN);
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

void CfdpTransaction::rTick(int *cont /* unused */) {
    /* Steven is not real happy with this function. There should be a better way to separate out
     * the logic by state so that it isn't a bunch of if statements for different flags
     */

    Cfdp::Status::T sret;
    bool         pending_send;

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
            if (this->m_state != CF_TxnState_HOLD)
            {
                this->rSendInactivityEvent();

                /* in class 2 this also triggers sending an early FIN response */
                if (this->m_state == CF_TxnState_R2)
                {
                    this->r2SetFinTxnStatus(CF_TxnStatus_INACTIVITY_DETECTED);
                }
            }
        }
    }

    pending_send = true; /* maybe; tbd */

    /* rx maintenance: possibly process send_eof_ack, send_nak or send_fin */
    if (this->m_flags.rx.send_eof_ack)
    {
        sret = this->m_engine->sendAck(this, CF_CFDP_AckTxnStatus_ACTIVE, CF_CFDP_FileDirective_EOF,
                               static_cast<CF_CFDP_ConditionCode_t>(this->m_state_data.receive.r2.eof_cc),
                               this->m_history->peer_eid, this->m_history->seq_num);
        FW_ASSERT(sret != Cfdp::Status::SEND_PDU_ERROR);

        /* if Cfdp::Status::SUCCESS, then move on in the state machine. CF_CFDP_SendAck does not return
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
        this->rAckTimerTick();
    }
}

void CfdpTransaction::rCancel() {
    /* for cancel, only need to send FIN if R2 */
    if ((this->m_state == CF_TxnState_R2) && (this->m_state_data.receive.sub_state < CF_RxSubState_CLOSEOUT_SYNC))
    {
        this->m_flags.rx.send_fin = true;
    }
    else
    {
        this->r1Reset(); /* if R1, just call it quits */
    }
}

void CfdpTransaction::rInit() {
    Os::File::Status status;
    Fw::String tmpDir;
    Fw::String dst;

    if (this->m_state == CF_TxnState_R2)
    {
        if (!this->m_flags.rx.md_recv)
        {
            tmpDir = this->m_cfdpManager->getTmpDirParam();
            /* we need to make a temp file and then do a NAK for md PDU */
            /* the transaction already has a history, and that has a buffer that we can use to
             * hold the temp filename which is defined by the sequence number and the source entity ID */

            // Create destination filepath with format: <tmpDir>/<src_eid>:<seq_num>.tmp
            dst.format("%s/%" CF_PRI_ENTITY_ID ":%" CF_PRI_TRANSACTION_SEQ ".tmp",
                       tmpDir.toChar(),
                       this->m_history->src_eid,
                       this->m_history->seq_num);

            this->m_history->fnames.dst_filename = dst;

            // CFE_EVS_SendEvent(CF_CFDP_R_TEMP_FILE_INF_EID, CFE_EVS_EventType_INFORMATION,
            //                   "CF R%d(%lu:%lu): making temp file %s for transaction without MD",
            //                   (this->m_state == CF_TxnState_R2), (unsigned long)this->m_history->src_eid,
            //                   (unsigned long)this->m_history->seq_num, this->m_history->fnames.dst_filename);
        }

        this->m_engine->armAckTimer(this);
    }

    status = this->m_fd.open(this->m_history->fnames.dst_filename.toChar(), Os::File::OPEN_CREATE, Os::File::OVERWRITE);
    if (status != Os::File::OP_OK)
    {
        // CFE_EVS_SendEvent(CF_CFDP_R_CREAT_ERR_EID, CFE_EVS_EventType_ERROR,
        //                   "CF R%d(%lu:%lu): failed to create file %s for writing, error=%ld",
        //                   (this->m_state == CF_TxnState_R2), (unsigned long)this->m_history->src_eid,
        //                   (unsigned long)this->m_history->seq_num, this->m_history->fnames.dst_filename, (long)ret);
        // ++CF_AppData.hk.Payload.channel_hk[this->m_chan_num].counters.fault.file_open;
        // this->m_fd = OS_OBJECT_ID_UNDEFINED; /* just in case */
        if (this->m_state == CF_TxnState_R2)
        {
            this->r2SetFinTxnStatus(CF_TxnStatus_FILESTORE_REJECTION);
        }
        else
        {
            this->r1Reset();
        }
    }
    else
    {
        this->m_state_data.receive.sub_state = CF_RxSubState_FILEDATA;
    }
}

void CfdpTransaction::r2SetFinTxnStatus(CF_TxnStatus_t txn_stat) {
    this->m_engine->setTxnStatus(this, txn_stat);
    this->m_flags.rx.send_fin = true;
}

void CfdpTransaction::r1Reset() {
    this->m_engine->finishTransaction(this, true);
}

void CfdpTransaction::r2Reset() {
    if ((this->m_state_data.receive.sub_state == CF_RxSubState_CLOSEOUT_SYNC) ||
        (this->m_state_data.receive.r2.eof_cc != CF_CFDP_ConditionCode_NO_ERROR) ||
        CF_TxnStatus_IsError(this->m_history->txn_stat) || this->m_flags.com.canceled)
    {
        this->r1Reset(); /* it's done */
    }
    else
    {
        /* not waiting for FIN ACK, so trigger send FIN */
        this->m_flags.rx.send_fin = true;
    }
}

Cfdp::Status::T CfdpTransaction::rCheckCrc(U32 expected_crc) {
    Cfdp::Status::T ret = Cfdp::Status::SUCCESS;
    U32 crc_result;

    // The F' version does not have an equivelent finalize call as it
    // - Never stores a partial word internally
    // - Never needs to "flush" anything
    // - Always accounts for padding at update time
    crc_result = this->m_crc.getValue();
    if (crc_result != expected_crc)
    {
        // CFE_EVS_SendEvent(CF_CFDP_R_CRC_ERR_EID, CFE_EVS_EventType_ERROR,
        //                   "CF R%d(%lu:%lu): CRC mismatch for R trans. got 0x%08lx expected 0x%08lx",
        //                   (this->m_state == CF_TxnState_R2), (unsigned long)this->m_history->src_eid,
        //                   (unsigned long)this->m_history->seq_num, (unsigned long)crc_result,
        //                   (unsigned long)expected_crc);
        // ++CF_AppData.hk.Payload.channel_hk[this->m_chan_num].counters.fault.crc_mismatch;
        ret = Cfdp::Status::ERROR;
    }

    return ret;
}

void CfdpTransaction::r2Complete(int ok_to_send_nak) {
    U32 ret;
    bool send_nak = false;
    bool send_fin = false;
    U8 nack_limit = 0;
    /* checking if r2 is complete. Check NAK list, and send NAK if appropriate */
    /* if all data is present, then there will be no gaps in the chunk */

    if (!CF_TxnStatus_IsError(this->m_history->txn_stat))
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
                // CFE_EVS_SendEvent(CF_CFDP_R_NAK_LIMIT_ERR_EID, CFE_EVS_EventType_ERROR,
                //                   "CF R%d(%lu:%lu): NAK limited reach", (this->m_state == CF_TxnState_R2),
                //                   (unsigned long)this->m_history->src_eid, (unsigned long)this->m_history->seq_num);
                send_fin = true;
                // ++CF_AppData.hk.Payload.channel_hk[this->m_chan_num].counters.fault.nak_limit;
                /* don't use CF_CFDP_R2_SetFinTxnStatus because many places in this function set send_fin */
                this->m_engine->setTxnStatus(this, CF_TxnStatus_NAK_LIMIT_REACHED);
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
            this->r2SetFinTxnStatus(CF_TxnStatus_NO_ERROR);
        }

        /* always go to CF_RxSubState_FILEDATA, and let tick change state */
        this->m_state_data.receive.sub_state = CF_RxSubState_FILEDATA;
    }
}

// ======================================================================
// RX State Machine - Private Helper Methods
// ======================================================================

Cfdp::Status::T CfdpTransaction::rProcessFd(CF_Logical_PduBuffer_t *ph) {
    const CF_Logical_PduFileDataHeader_t *pdu;
    Os::File::Status status;
    Cfdp::Status::T ret;

    /* this function is only entered for data PDUs */
    pdu = &ph->int_header.fd;
    ret = Cfdp::Status::SUCCESS;

    /*
     * NOTE: The decode routine should have left a direct pointer to the data and actual data length
     * within the PDU.  The length has already been verified, too.  Should not need to make any
     * adjustments here, just write it.
     */

    // TODO BPC: get rid of pdu->offset in favor of Os::File::position()
    if (this->m_state_data.receive.cached_pos != pdu->offset)
    {
        status = this->m_fd.seek(pdu->offset, Os::File::SeekType::ABSOLUTE);
        if (status != Os::File::OP_OK)
        {
            // CFE_EVS_SendEvent(CF_CFDP_R_SEEK_FD_ERR_EID, CFE_EVS_EventType_ERROR,
            //                   "CF R%d(%lu:%lu): failed to seek offset %ld, got %ld", (this->m_state == CF_TxnState_R2),
            //                   (unsigned long)this->m_history->src_eid, (unsigned long)this->m_history->seq_num,
            //                   (long)pdu->offset, (long)fret);
            this->m_engine->setTxnStatus(this, CF_TxnStatus_FILE_SIZE_ERROR);
            // ++CF_AppData.hk.Payload.channel_hk[this->m_chan_num].counters.fault.file_seek;
            ret = Cfdp::Status::ERROR; /* connection will reset in caller */
        }
    }

    if (ret != Cfdp::Status::ERROR)
    {
        FwSizeType write_size = pdu->data_len;
        status = this->m_fd.write(pdu->data_ptr, write_size, Os::File::WaitType::WAIT);
        if (status != Os::File::OP_OK)
        {
            // CFE_EVS_SendEvent(CF_CFDP_R_WRITE_ERR_EID, CFE_EVS_EventType_ERROR,
            //                   "CF R%d(%lu:%lu): OS_write expected %ld, got %ld", (this->m_state == CF_TxnState_R2),
            //                   (unsigned long)this->m_history->src_eid, (unsigned long)this->m_history->seq_num,
            //                   (long)pdu->data_len, (long)fret);
            this->m_engine->setTxnStatus(this, CF_TxnStatus_FILESTORE_REJECTION);
            // ++CF_AppData.hk.Payload.channel_hk[this->m_chan_num].counters.fault.file_write;
            ret = Cfdp::Status::ERROR; /* connection will reset in caller */
        }
        else
        {
            this->m_state_data.receive.cached_pos = static_cast<U32>(pdu->data_len) + pdu->offset;
            // CF_AppData.hk.Payload.channel_hk[this->m_chan_num].counters.recv.file_data_bytes += pdu->data_len;
        }
    }

    return ret;
}

Cfdp::Status::T CfdpTransaction::rSubstateRecvEof(CF_Logical_PduBuffer_t *ph) {
    Cfdp::Status::T               ret = Cfdp::Status::SUCCESS;
    const CF_Logical_PduEof_t *eof;

    if (!this->m_engine->recvEof(this, ph))
    {
        /* this function is only entered for PDUs identified as EOF type */
        eof = &ph->int_header.eof;

        /* only check size if MD received, otherwise it's still OK */
        if (this->m_flags.rx.md_recv && (eof->size != this->m_fsize))
        {
            // CFE_EVS_SendEvent(CF_CFDP_R_SIZE_MISMATCH_ERR_EID, CFE_EVS_EventType_ERROR,
            //                   "CF R%d(%lu:%lu): EOF file size mismatch: got %lu expected %lu",
            //                   (this->m_state == CF_TxnState_R2), (unsigned long)this->m_history->src_eid,
            //                   (unsigned long)this->m_history->seq_num, (unsigned long)eof->size,
            //                   (unsigned long)this->m_fsize);
            // ++CF_AppData.hk.Payload.channel_hk[this->m_chan_num].counters.fault.file_size_mismatch;
            ret = Cfdp::Status::REC_PDU_FSIZE_MISMATCH_ERROR;
        }
    }
    else
    {
        // CFE_EVS_SendEvent(CF_CFDP_R_PDU_EOF_ERR_EID, CFE_EVS_EventType_ERROR, "CF R%d(%lu:%lu): invalid EOF packet",
        //                   (this->m_state == CF_TxnState_R2), (unsigned long)this->m_history->src_eid,
        //                   (unsigned long)this->m_history->seq_num);
        // ++CF_AppData.hk.Payload.channel_hk[this->m_chan_num].counters.recv.error;
        ret = Cfdp::Status::REC_PDU_BAD_EOF_ERROR;
    }

    return ret;
}

void CfdpTransaction::r1SubstateRecvEof(CF_Logical_PduBuffer_t *ph) {
    int                        ret = this->rSubstateRecvEof(ph);
    U32                     crc;
    const CF_Logical_PduEof_t *eof;

    /* this function is only entered for PDUs identified as EOF type */
    eof = &ph->int_header.eof;
    crc = eof->crc;

    if (ret == Cfdp::Status::SUCCESS)
    {
        /* Verify CRC */
        if (this->rCheckCrc(crc) == Cfdp::Status::SUCCESS)
        {
            /* successfully processed the file */
            this->m_keep = Cfdp::Keep::KEEP; /* save the file */
        }
        /* if file failed to process, there's nothing to do. CF_CFDP_R_CheckCrc() generates an event on failure */
    }

    /* after exit, always reset since we are done */
    /* reset even if the EOF failed -- class 1, so it won't come again! */
    this->r1Reset();
}

void CfdpTransaction::r2SubstateRecvEof(CF_Logical_PduBuffer_t *ph) {
    const CF_Logical_PduEof_t *eof;
    int                        ret;

    if (!this->m_flags.rx.eof_recv)
    {
        ret = this->rSubstateRecvEof(ph);

        /* did receiving EOF succeed? */
        if (ret == Cfdp::Status::SUCCESS)
        {
            eof = &ph->int_header.eof;

            this->m_flags.rx.eof_recv = true;

            /* need to remember the EOF CRC for later */
            this->m_state_data.receive.r2.eof_crc  = eof->crc;
            this->m_state_data.receive.r2.eof_size = eof->size;

            /* always ACK the EOF, even if we're not done */
            this->m_state_data.receive.r2.eof_cc = eof->cc;
            this->m_flags.rx.send_eof_ack        = true; /* defer sending ACK to tick handling */

            /* only check for complete if EOF with no errors */
            if (this->m_state_data.receive.r2.eof_cc == CF_CFDP_ConditionCode_NO_ERROR)
            {
                this->r2Complete(true); /* CF_CFDP_R2_Complete() will change state */
            }
            else
            {
                /* All CFDP CC values directly correspond to a Transaction Status of the same numeric value */
                this->m_engine->setTxnStatus(this, static_cast<CF_TxnStatus_t>(this->m_state_data.receive.r2.eof_cc));
                this->r2Reset();
            }
        }
        else
        {
            /* bad EOF sent? */
            if (ret == Cfdp::Status::REC_PDU_FSIZE_MISMATCH_ERROR)
            {
                this->r2SetFinTxnStatus(CF_TxnStatus_FILE_SIZE_ERROR);
            }
            else
            {
                /* can't do anything with this bad EOF, so return to FILEDATA */
                this->m_state_data.receive.sub_state = CF_RxSubState_FILEDATA;
            }
        }
    }
}

void CfdpTransaction::r1SubstateRecvFileData(CF_Logical_PduBuffer_t *ph) {
    int ret;

    /* got file data PDU? */
    ret = this->m_engine->recvFd(this, ph);
    if (ret == Cfdp::Status::SUCCESS)
    {
        ret = this->rProcessFd(ph);
    }

    if (ret == Cfdp::Status::SUCCESS)
    {
        /* class 1 digests CRC */
        this->m_crc.update(ph->int_header.fd.data_ptr, ph->int_header.fd.offset,
                        static_cast<U32>(ph->int_header.fd.data_len));
    }
    else
    {
        /* Reset transaction on failure */
        this->r1Reset();
    }
}

void CfdpTransaction::r2SubstateRecvFileData(CF_Logical_PduBuffer_t *ph) {
    const CF_Logical_PduFileDataHeader_t *fd;
    int                                   ret;

    /* this function is only entered for data PDUs */
    fd = &ph->int_header.fd;

    // If CRC calculation has started (file reopened in READ mode), ignore late FileData PDUs.
    // This can happen if retransmitted FileData arrives after EOF was received and CRC began.
    if (this->m_state_data.receive.r2.rx_crc_calc_bytes > 0)
    {
        // Silently ignore - file is complete and we're calculating CRC
        // TODO BPC: do we want a throttled EVR here?
        return;
    }

    /* got file data PDU? */
    ret = this->m_engine->recvFd(this, ph);
    if (ret == Cfdp::Status::SUCCESS)
    {
        ret = this->rProcessFd(ph);
    }

    if (ret == Cfdp::Status::SUCCESS)
    {
        /* class 2 does CRC at FIN, but track gaps */
        this->m_chunks->chunks.add(fd->offset, static_cast<CfdpFileSize>(fd->data_len));

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

void CfdpTransaction::r2GapCompute(const CF_Chunk_t *chunk, Cfdp::Pdu::NakPdu& nak) {
    FW_ASSERT(chunk->size > 0, chunk->size);

    // Calculate segment offsets relative to scope start
    CfdpFileSize offsetStart = chunk->offset - nak.getScopeStart();
    CfdpFileSize offsetEnd = offsetStart + chunk->size;

    // Add segment to NAK PDU (returns false if array is full)
    nak.addSegment(offsetStart, offsetEnd);
}

Cfdp::Status::T CfdpTransaction::rSubstateSendNak() {
    Cfdp::Status::T status = Cfdp::Status::SUCCESS;

    // Create and initialize NAK PDU
    Cfdp::Pdu::NakPdu nakPdu;
    Cfdp::Direction direction = Cfdp::DIRECTION_TOWARD_SENDER;

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
            gapLimit,
            this->m_fsize,
            0,
            [this, &nakPdu](const CF_Chunk_t* chunk, void* opaque) {
                this->r2GapCompute(chunk, nakPdu);
            },
            nullptr);

        if (!gapCount) {
            // No gaps left, file reception is complete
            this->m_flags.rx.complete = true;
            return Cfdp::Status::SUCCESS;
        }

        // Gaps are present, send the NAK PDU
        status = this->m_engine->sendNak(this, nakPdu);
        if (status == Cfdp::Status::SUCCESS) {
            this->m_flags.rx.fd_nak_sent = true;
            // CF_AppData.hk.Payload.channel_hk[this->m_chan_num].counters.sent.nak_segment_requests += gapCount;
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

Cfdp::Status::T CfdpTransaction::r2CalcCrcChunk() {
    U8 buf[CF_R2_CRC_CHUNK_SIZE];
    size_t count_bytes;
    size_t want_offs_size;
    FwSizeType read_size;
    Os::File::Status fileStatus;
    Cfdp::Status::T ret;
    bool success = true;
    U32 rx_crc_calc_bytes_per_wakeup = 0;

    memset(buf, 0, sizeof(buf));

    count_bytes = 0;
    ret         = Cfdp::Status::ERROR;

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
            this->m_engine->setTxnStatus(this, CF_TxnStatus_FILE_SIZE_ERROR);
            return Cfdp::Status::ERROR;
        }

        // Reset cached position since we just reopened the file
        this->m_state_data.receive.cached_pos = 0;
    }

    rx_crc_calc_bytes_per_wakeup = this->m_cfdpManager->getRxCrcCalcBytesPerWakeupParam();

    while ((count_bytes < rx_crc_calc_bytes_per_wakeup) &&
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
                // CFE_EVS_SendEvent(CF_CFDP_R_SEEK_CRC_ERR_EID, CFE_EVS_EventType_ERROR,
                //                   "CF R%d(%lu:%lu): failed to seek offset %lu, got %ld", (this->m_state == CF_TxnState_R2),
                //                   (unsigned long)this->m_history->src_eid, (unsigned long)this->m_history->seq_num,
                //                   (unsigned long)this->m_state_data.receive.r2.rx_crc_calc_bytes, (long)fret);
                // this->m_engine->setTxnStatus(this, CF_TxnStatus_FILE_SIZE_ERROR);
                // ++CF_AppData.hk.Payload.channel_hk[this->m_chan_num].counters.fault.file_seek;
                success = false;
                break;
            }
        }

        fileStatus = this->m_fd.read(buf, read_size, Os::File::WaitType::WAIT);
        if (fileStatus != Os::File::OP_OK)
        {
            // CFE_EVS_SendEvent(CF_CFDP_R_READ_ERR_EID, CFE_EVS_EventType_ERROR,
            //                   "CF R%d(%lu:%lu): failed to read file expected %lu, got %ld",
            //                   (this->m_state == CF_TxnState_R2), (unsigned long)this->m_history->src_eid,
            //                   (unsigned long)this->m_history->seq_num, (unsigned long)read_size, (long)fret);
            this->m_engine->setTxnStatus(this, CF_TxnStatus_FILE_SIZE_ERROR);
            // ++CF_AppData.hk.Payload.channel_hk[this->m_chan_num].counters.fault.file_read;
            success = false;
            break;
        }

        this->m_crc.update(buf, this->m_state_data.receive.r2.rx_crc_calc_bytes, static_cast<U32>(read_size));
        this->m_state_data.receive.r2.rx_crc_calc_bytes += static_cast<U32>(read_size);
        this->m_state_data.receive.cached_pos = this->m_state_data.receive.r2.rx_crc_calc_bytes;
        count_bytes += read_size;
    }

    if (success && this->m_state_data.receive.r2.rx_crc_calc_bytes == this->m_fsize)
    {
        /* all bytes calculated, so now check */
        if (this->rCheckCrc(this->m_state_data.receive.r2.eof_crc) == Cfdp::Status::SUCCESS)
        {
            /* CRC matched! We are happy */
            this->m_keep = Cfdp::Keep::KEEP; /* save the file */

            /* set FIN PDU status */
            this->m_state_data.receive.r2.dc = CF_CFDP_FinDeliveryCode_COMPLETE;
            this->m_state_data.receive.r2.fs = CF_CFDP_FinFileStatus_RETAINED;
        }
        else
        {
            this->r2SetFinTxnStatus(CF_TxnStatus_FILE_CHECKSUM_FAILURE);
        }

        this->m_flags.com.crc_calc = true;

        ret = Cfdp::Status::SUCCESS;
    }

    return ret;
}

Cfdp::Status::T CfdpTransaction::r2SubstateSendFin() {
    Cfdp::Status::T sret;
    Cfdp::Status::T ret = Cfdp::Status::SUCCESS;

    if (!CF_TxnStatus_IsError(this->m_history->txn_stat) && !this->m_flags.com.crc_calc)
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
                               CF_TxnStatus_To_ConditionCode(this->m_history->txn_stat));
        /* CF_CFDP_SendFin does not return SEND_PDU_ERROR */
        FW_ASSERT(sret != Cfdp::Status::SEND_PDU_ERROR);
        this->m_state_data.receive.sub_state =
            CF_RxSubState_CLOSEOUT_SYNC; /* whether or not FIN send successful, ok to transition state */
        if (sret != Cfdp::Status::SUCCESS)
        {
            ret = Cfdp::Status::ERROR;
        }
    }

    /* if no message, then try again next time */
    return ret;
}

void CfdpTransaction::r2RecvFinAck(CF_Logical_PduBuffer_t *ph) {
    if (!this->m_engine->recvAck(this, ph))
    {
        /* got fin-ack, so time to close the state */
        this->r2Reset();
    }
    else
    {
        // CFE_EVS_SendEvent(CF_CFDP_R_PDU_FINACK_ERR_EID, CFE_EVS_EventType_ERROR, "CF R%d(%lu:%lu): invalid fin-ack",
        //                   (this->m_state == CF_TxnState_R2), (unsigned long)this->m_history->src_eid,
        //                   (unsigned long)this->m_history->seq_num);
        // ++CF_AppData.hk.Payload.channel_hk[this->m_chan_num].counters.recv.error;
    }
}

void CfdpTransaction::r2RecvMd(CF_Logical_PduBuffer_t *ph) {
    Fw::String fname;
    Cfdp::Status::T status;
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

        status = this->m_engine->recvMd(this, ph);
        if (status == Cfdp::Status::SUCCESS)
        {
            /* successfully obtained md PDU */
            if (this->m_flags.rx.eof_recv)
            {
                /* EOF was received, so check that md and EOF sizes match */
                if (this->m_state_data.receive.r2.eof_size != this->m_fsize)
                {
                    // CFE_EVS_SendEvent(CF_CFDP_R_EOF_MD_SIZE_ERR_EID, CFE_EVS_EventType_ERROR,
                    //                   "CF R%d(%lu:%lu): EOF/md size mismatch md: %lu, EOF: %lu",
                    //                   (this->m_state == CF_TxnState_R2), (unsigned long)this->m_history->src_eid,
                    //                   (unsigned long)this->m_history->seq_num, (unsigned long)this->m_fsize,
                    //                   (unsigned long)this->m_state_data.receive.r2.eof_size);
                    // ++CF_AppData.hk.Payload.channel_hk[this->m_chan_num].counters.fault.file_size_mismatch;
                    this->r2SetFinTxnStatus(CF_TxnStatus_FILE_SIZE_ERROR);
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
                    // CFE_EVS_SendEvent(CF_CFDP_R_RENAME_ERR_EID, CFE_EVS_EventType_ERROR,
                    //                   "CF R%d(%lu:%lu): failed to rename file in R2, error=%ld",
                    //                   (this->m_state == CF_TxnState_R2), (unsigned long)this->m_history->src_eid,
                    //                   (unsigned long)this->m_history->seq_num, (long)status);
                    // this->m_fd = OS_OBJECT_ID_UNDEFINED;
                    this->r2SetFinTxnStatus(CF_TxnStatus_FILESTORE_REJECTION);
                    // ++CF_AppData.hk.Payload.channel_hk[this->m_chan_num].counters.fault.file_rename;
                    success = false;
                }
                else
                {
                     // TODO BPC: flags = OS_FILE_FLAG_NONE, access = OS_READ_WRITE
                     // File was succesfully renamed, open for writing
                    fileStatus = this->m_fd.open(this->m_history->fnames.dst_filename.toChar(), Os::File::OPEN_WRITE);
                    if (fileStatus != Os::File::OP_OK)
                    {
                        // CFE_EVS_SendEvent(CF_CFDP_R_OPEN_ERR_EID, CFE_EVS_EventType_ERROR,
                        //                   "CF R%d(%lu:%lu): failed to open renamed file in R2, error=%ld",
                        //                   (this->m_state == CF_TxnState_R2), (unsigned long)this->m_history->src_eid,
                        //                   (unsigned long)this->m_history->seq_num, (long)ret);
                        this->r2SetFinTxnStatus(CF_TxnStatus_FILESTORE_REJECTION);
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
        else
        {
            // CFE_EVS_SendEvent(CF_CFDP_R_PDU_MD_ERR_EID, CFE_EVS_EventType_ERROR, "CF R%d(%lu:%lu): invalid md received",
            //                   (this->m_state == CF_TxnState_R2), (unsigned long)this->m_history->src_eid,
            //                   (unsigned long)this->m_history->seq_num);
            // ++CF_AppData.hk.Payload.channel_hk[this->m_chan_num].counters.recv.error;
            /* do nothing here, since it will be NAK'd again later */
        }
    }
}

void CfdpTransaction::rSendInactivityEvent() {
    // CFE_EVS_SendEvent(CF_CFDP_R_INACT_TIMER_ERR_EID, CFE_EVS_EventType_ERROR,
    //                   "CF R%d(%lu:%lu): inactivity timer expired", (this->m_state == CF_TxnState_R2),
    //                   (unsigned long)this->m_history->src_eid, (unsigned long)this->m_history->seq_num);
    // ++CF_AppData.hk.Payload.channel_hk[this->m_chan_num].counters.fault.inactivity_timer;
}

// ======================================================================
// Dispatch Methods (ported from cf_cfdp_dispatch.c)
// ======================================================================

void CfdpTransaction::rDispatchRecv(CF_Logical_PduBuffer_t *ph,
                                    const CF_CFDP_R_SubstateDispatchTable_t *dispatch,
                                    CF_CFDP_StateRecvFunc_t fd_fn)
{
    CF_CFDP_StateRecvFunc_t              selected_handler;
    CF_Logical_PduFileDirectiveHeader_t *fdh;

    FW_ASSERT(this->m_state_data.receive.sub_state < CF_RxSubState_NUM_STATES,
              this->m_state_data.receive.sub_state, CF_RxSubState_NUM_STATES);

    selected_handler = NULL;

    /* the CF_CFDP_R_SubstateDispatchTable_t is only used with file directive PDU */
    if (ph->pdu_header.pdu_type == 0)
    {
        fdh = &ph->fdirective;
        if (fdh->directive_code < CF_CFDP_FileDirective_INVALID_MAX)
        {
            if (dispatch->state[this->m_state_data.receive.sub_state] != NULL)
            {
                selected_handler = dispatch->state[this->m_state_data.receive.sub_state]->fdirective[fdh->directive_code];
            }
        }
        else
        {
            // ++CF_AppData.hk.Payload.channel_hk[this->m_chan_num].counters.recv.spurious;
            // CFE_EVS_SendEvent(CF_CFDP_R_DC_INV_ERR_EID, CFE_EVS_EventType_ERROR,
            //                   "CF R%d(%lu:%lu): received PDU with invalid directive code %d for sub-state %d",
            //                   (this->m_state == CF_TxnState_R2), (unsigned long)this->m_history->src_eid,
            //                   (unsigned long)this->m_history->seq_num, fdh->directive_code,
            //                   this->m_state_data.receive.sub_state);
        }
    }
    else
    {
        if (!CF_TxnStatus_IsError(this->m_history->txn_stat))
        {
            selected_handler = fd_fn;
        }
        else
        {
            // ++CF_AppData.hk.Payload.channel_hk[this->m_chan_num].counters.recv.dropped;
        }
    }

    /*
     * NOTE: if no handler is selected, this will drop packets on the floor here,
     * without incrementing any counter.  This was existing behavior.
     */
    if (selected_handler != NULL)
    {
        (this->*selected_handler)(ph);
    }
}

void CfdpTransaction::rxStateDispatch(CF_Logical_PduBuffer_t *ph,
                                      const CF_CFDP_TxnRecvDispatchTable_t *dispatch)
{
    CF_CFDP_StateRecvFunc_t selected_handler;

    FW_ASSERT(this->m_state < CF_TxnState_INVALID, this->m_state, CF_TxnState_INVALID);
    selected_handler = dispatch->rx[this->m_state];
    if (selected_handler != NULL)
    {
        (this->*selected_handler)(ph);
    }
}

}  // namespace Ccsds
}  // namespace Svc

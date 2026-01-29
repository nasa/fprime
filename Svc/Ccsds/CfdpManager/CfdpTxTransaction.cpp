// ======================================================================
// \title  CfdpTxTransaction.cpp
// \brief  cpp file for CFDP TX Transaction state machine
//
// This file is a port of the cf_cfdp_s.c file from the
// NASA Core Flight System (cFS) CFDP (CF) Application,
// version 3.0.0, adapted for use within the F-Prime (F') framework.
//
// This file contains various state handling routines for
// transactions which are sending a file.
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
#include <Svc/Ccsds/CfdpManager/CfdpTx.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpCodec.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpChunk.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpUtils.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpDispatch.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpTimer.hpp>

namespace Svc {
namespace Ccsds {

// ======================================================================
// TX State Machine - Public Methods
// ======================================================================

void CfdpTransaction::s1Recv(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph) {
    /* s1 doesn't need to receive anything */
    CF_CFDP_S1_Recv(txn, ph);
}

void CfdpTransaction::s2Recv(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph) {
    CF_CFDP_S2_Recv(txn, ph);
}

void CfdpTransaction::s1Tx(CF_Transaction_t *txn) {
    CF_CFDP_S1_Tx(txn);
}

void CfdpTransaction::s2Tx(CF_Transaction_t *txn) {
    CF_CFDP_S2_Tx(txn);
}

void CfdpTransaction::sAckTimerTick(CF_Transaction_t *txn) {
    U8 ack_limit = 0;

    /* note: the ack timer is only ever relevant on class 2 */
    if (txn->state != CF_TxnState_S2 || !txn->flags.com.ack_timer_armed)
    {
        /* nothing to do */
        return;
    }

    if (txn->ack_timer.getStatus() == CfdpTimer::Status::RUNNING)
    {
        txn->ack_timer.run();
    }
    else if (txn->state_data.send.sub_state == CF_TxSubState_CLOSEOUT_SYNC)
    {
        /* Check limit and handle if needed */
        ack_limit = txn->cfdpManager->getAckLimitParam(txn->chan_num);
        if (txn->state_data.send.s2.acknak_count >= ack_limit)
        {
            // CFE_EVS_SendEvent(CF_CFDP_S_ACK_LIMIT_ERR_EID, CFE_EVS_EventType_ERROR,
            //                   "CF S2(%lu:%lu), ack limit reached, no eof-ack", (unsigned long)txn->history->src_eid,
            //                   (unsigned long)txn->history->seq_num);
            txn->engine->setTxnStatus(txn, CF_TxnStatus_ACK_LIMIT_NO_EOF);
            // ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.fault.ack_limit;

            /* give up on this */
            txn->engine->finishTransaction(txn, true);
            txn->flags.com.ack_timer_armed = false;
        }
        else
        {
            /* Increment acknak counter */
            ++txn->state_data.send.s2.acknak_count;

            /* If the peer sent FIN that is an implicit EOF ack, it is not supposed
             * to send it before EOF unless an error occurs, and either way we do not
             * re-transmit anything after FIN unless we get another FIN */
            if (!txn->flags.tx.eof_ack_recv && !txn->flags.tx.fin_recv)
            {
                txn->flags.tx.send_eof = true;
            }
            else
            {
                /* no response is pending */
                txn->flags.com.ack_timer_armed = false;
            }
        }

        /* reset the ack timer if still waiting on something */
        if (txn->flags.com.ack_timer_armed)
        {
            txn->engine->armAckTimer(txn);
        }
    }
    else
    {
        /* if we are not waiting for anything, why is the ack timer armed? */
        txn->flags.com.ack_timer_armed = false;
    }
}

void CfdpTransaction::sTick(CF_Transaction_t *txn, int *cont /* unused */) {
    bool pending_send;

    pending_send = true; /* maybe; tbd, will be reset if not */

    /* at each tick, various timers used by S are checked */
    /* first, check inactivity timer */
    if (!txn->flags.com.inactivity_fired)
    {
        if (txn->inactivity_timer.getStatus() == CfdpTimer::Status::RUNNING)
        {
            txn->inactivity_timer.run();
        }
        else
        {
            txn->flags.com.inactivity_fired = true;

            /* HOLD state is the normal path to recycle transaction objects, not an error */
            /* inactivity is abnormal in any other state */
            if (txn->state != CF_TxnState_HOLD && txn->state == CF_TxnState_S2)
            {
                // CFE_EVS_SendEvent(CF_CFDP_S_INACT_TIMER_ERR_EID, CFE_EVS_EventType_ERROR,
                //                   "CF S2(%lu:%lu): inactivity timer expired", (unsigned long)txn->history->src_eid,
                //                   (unsigned long)txn->history->seq_num);
                txn->engine->setTxnStatus(txn, CF_TxnStatus_INACTIVITY_DETECTED);

                // ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.fault.inactivity_timer;
            }
        }
    }

    /* tx maintenance: possibly process send_eof, or send_fin_ack */
    if (txn->flags.tx.send_eof)
    {
        if (this->sSendEof(txn) == CfdpStatus::SUCCESS)
        {
            txn->flags.tx.send_eof = false;
        }
    }
    else if (txn->flags.tx.send_fin_ack)
    {
        if (this->sSendFinAck(txn) == CfdpStatus::SUCCESS)
        {
            txn->flags.tx.send_fin_ack = false;
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
    if (txn->flags.com.inactivity_fired && !pending_send)
    {
        /* the transaction is now recycleable - this means we will
         * no longer have a record of this transaction seq.  If the sender
         * wakes up or if the network delivers severely delayed PDUs at
         * some future point, then they will be seen as spurious.  They
         * will no longer be associable with this transaction at all */
        txn->chan->recycleTransaction(txn);

        /* NOTE: this must be the last thing in here.  Do not use txn after this */
    }
    else
    {
        /* transaction still valid so process the ACK timer, if relevant */
        this->sAckTimerTick(txn);
    }
}

void CfdpTransaction::sTickNak(CF_Transaction_t *txn, int *cont) {
    bool nakProcessed = false;
    CfdpStatus::T status;

    // Only Class 2 transactions should process NAKs
    if (txn->txn_class == CfdpClass::CLASS_2)
    {
        status = this->sCheckAndRespondNak(txn, &nakProcessed);
        if ((status == CfdpStatus::SUCCESS) && nakProcessed)
        {
            *cont = 1; /* cause dispatcher to re-enter this wakeup */
        }
    }
}

void CfdpTransaction::sCancel(CF_Transaction_t *txn) {
    if (txn->state_data.send.sub_state < CF_TxSubState_EOF)
    {
        /* if state has not reached CF_TxSubState_EOF, then set it to CF_TxSubState_EOF now. */
        txn->state_data.send.sub_state = CF_TxSubState_EOF;
    }
}

// ======================================================================
// TX State Machine - Private Helper Methods
// ======================================================================

CfdpStatus::T CfdpTransaction::sSendEof(CF_Transaction_t *txn) {
    /* note the crc is "finalized" regardless of success or failure of the txn */
    /* this is OK as we still need to put some value into the EOF */
    if (!txn->flags.com.crc_calc)
    {
        // The F' version does not have an equivelent finalize call as it
        // - Never stores a partial word internally
        // - Never needs to "flush" anything
        // - Always accounts for padding at update time
        txn->flags.com.crc_calc = true;
    }
    return txn->engine->sendEof(txn);
}

void CfdpTransaction::s1SubstateSendEof(CF_Transaction_t *txn) {
    /* set the flag, the EOF is sent by the tick handler */
    txn->flags.tx.send_eof = true;

    /* In class 1 this is the end of normal operation */
    /* NOTE: this is not always true, as class 1 can request an EOF ack.
     * In this case we could change state to CLOSEOUT_SYNC instead and wait,
     * but right now we do not request an EOF ack in S1 */
    txn->engine->finishTransaction(txn, true);
}

void CfdpTransaction::s2SubstateSendEof(CF_Transaction_t *txn) {
    /* set the flag, the EOF is sent by the tick handler */
    txn->flags.tx.send_eof = true;

    /* wait for remaining responses to close out the state machine */
    txn->state_data.send.sub_state = CF_TxSubState_CLOSEOUT_SYNC;

    /* always move the transaction onto the wait queue now */
    txn->chan->dequeueTransaction(txn);
    txn->chan->insertSortPrio(txn, CfdpQueueId::TXW);

    /* the ack timer is armed in class 2 only */
    txn->engine->armAckTimer(txn);
}

CfdpStatus::T CfdpTransaction::sSendFileData(CF_Transaction_t *txn, U32 foffs, U32 bytes_to_read, U8 calc_crc, U32* bytes_processed) {
    I32 status = 0;
    CfdpStatus::T ret = CfdpStatus::SUCCESS;
    CF_Logical_PduBuffer_t * ph = NULL;
    CF_Logical_PduFileDataHeader_t *fd;
    size_t actual_bytes;
    U8* data_ptr;
    U32 outgoing_file_chunk_size;

    FW_ASSERT(bytes_processed != NULL);
    *bytes_processed = 0;

    ph = txn->engine->constructPduHeader(txn, CF_CFDP_FileDirective_INVALID_MIN, txn->cfdpManager->getLocalEidParam(),
                                    txn->history->peer_eid, 0, txn->history->seq_num, true);
    if (!ph)
    {
        ret = CfdpStatus::SUCCESS; /* couldn't get message, so no bytes sent. Will try again next time */
    }
    else
    {
        fd = &ph->int_header.fd;

        /* need to encode data header up to this point to figure out where data needs to get copied to */
        fd->offset = foffs;
        CF_CFDP_EncodeFileDataHeader(ph->penc, ph->pdu_header.segment_meta_flag, fd);

        /*
         * the actual bytes to read is the smallest of these:
         *  - amount of space actually available in the PDU after encoding the headers
         *  - passed-in size
         *  - outgoing_file_chunk_size from configuration
         */
        actual_bytes = CF_CODEC_GET_REMAIN(ph->penc);
        if (actual_bytes > bytes_to_read)
        {
            actual_bytes = bytes_to_read;
        }
        outgoing_file_chunk_size = txn->cfdpManager->getOutgoingFileChunkSizeParam();
        if (actual_bytes > outgoing_file_chunk_size)
        {
            actual_bytes = outgoing_file_chunk_size;
        }

        /*
         * The call to CF_CFDP_DoEncodeChunk() should never fail because actual_bytes is
         * guaranteed to be less than or equal to the remaining space in the encode buffer.
         */
        data_ptr = CF_CFDP_DoEncodeChunk(ph->penc, actual_bytes);

        /*
         * save off a pointer to the data for future reference.
         * This isn't encoded into the output PDU, but it allows a future step (such as CRC)
         * to easily find and read the data blob in this PDU.
         */
        fd->data_len = actual_bytes;
        fd->data_ptr = data_ptr;

        if (txn->state_data.send.cached_pos != foffs)
        {
            status = txn->fd.seek(foffs, Os::File::SeekType::ABSOLUTE);
            if (status != Os::File::OP_OK)
            {
                // CFE_EVS_SendEvent(CF_CFDP_S_SEEK_FD_ERR_EID, CFE_EVS_EventType_ERROR,
                //                   "CF S%d(%lu:%lu): error seeking to offset %ld, got %ld",
                //                   (txn->state == CF_TxnState_S2), (unsigned long)txn->history->src_eid,
                //                   (unsigned long)txn->history->seq_num, (long)foffs, (long)status);
                // ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.fault.file_seek;
                ret = CfdpStatus::ERROR;
            }
        }

        if (ret == CfdpStatus::SUCCESS)
        {
            status = txn->fd.read(data_ptr, actual_bytes, Os::File::WaitType::WAIT);
            if (status != Os::File::OP_OK)
            {
                // CFE_EVS_SendEvent(CF_CFDP_S_READ_ERR_EID, CFE_EVS_EventType_ERROR,
                //                   "CF S%d(%lu:%lu): error reading bytes: expected %ld, got %ld",
                //                   (txn->state == CF_TxnState_S2), (unsigned long)txn->history->src_eid,
                //                   (unsigned long)txn->history->seq_num, (long)actual_bytes, (long)status);
                // ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.fault.file_read;
                ret = CfdpStatus::ERROR;
            }
        }

        if (ret == CfdpStatus::SUCCESS)
        {
            txn->state_data.send.cached_pos += status;
            txn->engine->sendFd(txn, ph); /* CF_CFDP_SendFd only returns CfdpStatus::SUCCESS */

            // CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.sent.file_data_bytes += actual_bytes;
            FW_ASSERT((foffs + actual_bytes) <= txn->fsize, foffs, static_cast<FwAssertArgType>(actual_bytes), txn->fsize); /* sanity check */
            if (calc_crc)
            {
                txn->crc.update(fd->data_ptr, fd->offset, static_cast<U32>(fd->data_len));
            }

            *bytes_processed = static_cast<U32>(actual_bytes);
        }
        else
        {
            // PDU was not sent, so return the buffer allocated by CF_CFDP_ConstructPduHeader()
            txn->cfdpManager->returnPduBuffer(txn->chan_num, ph);
        }
    }

    return ret;
}

void CfdpTransaction::sSubstateSendFileData(CF_Transaction_t *txn) {
    U32 bytes_processed = 0;
    CfdpStatus::T status = this->sSendFileData(txn, txn->foffs, (txn->fsize - txn->foffs), 1, &bytes_processed);

    if(status != CfdpStatus::SUCCESS)
    {
        /* IO error -- change state and send EOF */
        txn->engine->setTxnStatus(txn, CF_TxnStatus_FILESTORE_REJECTION);
        txn->state_data.send.sub_state = CF_TxSubState_EOF;
    }
    else if (bytes_processed > 0)
    {
        txn->foffs += bytes_processed;
        if (txn->foffs == txn->fsize)
        {
            /* file is done */
            txn->state_data.send.sub_state = CF_TxSubState_EOF;
        }
    }
    else
    {
        /* don't care about other cases */
    }
}

CfdpStatus::T CfdpTransaction::sCheckAndRespondNak(CF_Transaction_t *txn, bool* nakProcessed) {
    const CF_Chunk_t *chunk;
    CfdpStatus::T sret;
    CfdpStatus::T ret = CfdpStatus::SUCCESS;
    U32 bytes_processed = 0;

    FW_ASSERT(nakProcessed != NULL);
    *nakProcessed = false;

    // Class 2 transactions must have had chunks allocated
    FW_ASSERT(txn->chunks != NULL);

    if (txn->flags.tx.md_need_send)
    {
        sret = txn->engine->sendMd(txn);
        if (sret == CfdpStatus::SEND_PDU_ERROR)
        {
            ret = CfdpStatus::ERROR; /* error occurred */
        }
        else
        {
            if (sret == CfdpStatus::SUCCESS)
            {
                txn->flags.tx.md_need_send = false;
            }
            /* unless SEND_PDU_ERROR, return 1 to keep caller from sending file data */
            *nakProcessed = true; /* nak processed, so don't send filedata */

        }
    }
    else
    {
        /* Get first chunk and process if available */
        chunk = CF_ChunkList_GetFirstChunk(&txn->chunks->chunks);
        if (chunk != NULL)
        {
            ret = this->sSendFileData(txn, chunk->offset, chunk->size, 0, &bytes_processed);
            if(ret != CfdpStatus::SUCCESS)
            {
                /* error occurred */
                ret = CfdpStatus::ERROR; /* error occurred */
            }
            else if (bytes_processed > 0)
            {
                CF_ChunkList_RemoveFromFirst(&txn->chunks->chunks, bytes_processed);
                *nakProcessed = true; /* nak processed, so caller doesn't send file data */
            }
        }
    }

    return ret;
}

void CfdpTransaction::s2SubstateSendFileData(CF_Transaction_t *txn) {
    CfdpStatus::T status;
    bool nakProcessed = false;

    status = this->sCheckAndRespondNak(txn, &nakProcessed);
    if (status != CfdpStatus::SUCCESS)
    {
        txn->engine->setTxnStatus(txn, CF_TxnStatus_NAK_RESPONSE_ERROR);
        txn->flags.tx.send_eof = true; /* do not leave the remote hanging */
        txn->engine->finishTransaction(txn, true);
        return;
    }

    if (!nakProcessed)
    {
        this->sSubstateSendFileData(txn);
    }
    else
    {
        /* NAK was processed, so do not send filedata */
    }
}

void CfdpTransaction::sSubstateSendMetadata(CF_Transaction_t *txn) {
    CfdpStatus::T status;
    Os::File::Status fileStatus;
    bool success = true;

    if (false == txn->fd.isOpen())
    {
        fileStatus = txn->fd.open(txn->history->fnames.src_filename.toChar(), Os::File::OPEN_READ);
        if (fileStatus != Os::File::OP_OK)
        {
            // CFE_EVS_SendEvent(CF_CFDP_S_OPEN_ERR_EID, CFE_EVS_EventType_ERROR,
            //                   "CF S%d(%lu:%lu): failed to open file %s, error=%ld", (txn->state == CF_TxnState_S2),
            //                   (unsigned long)txn->history->src_eid, (unsigned long)txn->history->seq_num,
            //                   txn->history->fnames.src_filename, (long)ret);
            // ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.fault.file_open;
            // txn->fd = OS_OBJECT_ID_UNDEFINED; /* just in case */
            success = false;
        }

        if (success)
        {
            FwSizeType file_size;
            fileStatus = txn->fd.size(file_size);
            txn->fsize = static_cast<CfdpFileSize>(file_size);
            if (fileStatus != Os::File::Status::OP_OK)
            {
                // CFE_EVS_SendEvent(CF_CFDP_S_SEEK_BEG_ERR_EID, CFE_EVS_EventType_ERROR,
                //                   "CF S%d(%lu:%lu): failed to seek begin file %s, got %ld",
                //                   (txn->state == CF_TxnState_S2), (unsigned long)txn->history->src_eid,
                //                   (unsigned long)txn->history->seq_num, txn->history->fnames.src_filename,
                //                   (long)status);
                // ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.fault.file_seek;
                success = false;
            }
            else
            {
                // Check that file size is well formed
                FW_ASSERT(txn->fsize > 0, txn->fsize);
            }
        }
    }

    if (success)
    {
        status = txn->engine->sendMd(txn);
        if (status == CfdpStatus::SEND_PDU_ERROR)
        {
            /* failed to send md */
            // CFE_EVS_SendEvent(CF_CFDP_S_SEND_MD_ERR_EID, CFE_EVS_EventType_ERROR, "CF S%d(%lu:%lu): failed to send md",
            //                   (txn->state == CF_TxnState_S2), (unsigned long)txn->history->src_eid,
            //                   (unsigned long)txn->history->seq_num);
            success = false;
        }
        else if (status == CfdpStatus::SUCCESS)
        {
            /* once metadata is sent, switch to filedata mode */
            txn->state_data.send.sub_state = CF_TxSubState_FILEDATA;
        }
        /* if status==CfdpStatus::SEND_PDU_NO_BUF_AVAIL_ERROR, then try to send md again next cycle */
    }

    if (!success)
    {
        txn->engine->setTxnStatus(txn, CF_TxnStatus_FILESTORE_REJECTION);
        txn->engine->finishTransaction(txn, true);
    }

    /* don't need to reset the CRC since its taken care of by reset_cfdp() */
}

CfdpStatus::T CfdpTransaction::sSendFinAck(CF_Transaction_t *txn) {
    CfdpStatus::T ret = txn->engine->sendAck(txn, CF_CFDP_GetTxnStatus(txn), CF_CFDP_FileDirective_FIN,
                           static_cast<CF_CFDP_ConditionCode_t>(txn->state_data.send.s2.fin_cc),
                           txn->history->peer_eid, txn->history->seq_num);
    return ret;
}

void CfdpTransaction::s2EarlyFin(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph) {
    /* received early fin, so just cancel */
    // CFE_EVS_SendEvent(CF_CFDP_S_EARLY_FIN_ERR_EID, CFE_EVS_EventType_ERROR,
    //                   "CF S%d(%lu:%lu): got early FIN -- cancelling", (txn->state == CF_TxnState_S2),
    //                   (unsigned long)txn->history->src_eid, (unsigned long)txn->history->seq_num);
    txn->engine->setTxnStatus(txn, CF_TxnStatus_EARLY_FIN);

    txn->state_data.send.sub_state = CF_TxSubState_CLOSEOUT_SYNC;

    /* otherwise do normal fin processing */
    this->s2Fin(txn, ph);
}

void CfdpTransaction::s2Fin(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph) {
    if (!txn->engine->recvFin(txn, ph))
    {
        /* set the CC only on the first time we get the FIN.  If this is a dupe
         * then re-ack but otherwise ignore it */
        if (!txn->flags.tx.fin_recv)
        {
            txn->flags.tx.fin_recv               = true;
            txn->state_data.send.s2.fin_cc       = ph->int_header.fin.cc;
            txn->state_data.send.s2.acknak_count = 0; /* in case retransmits had occurred */

            /* note this is a no-op unless the status was unset previously */
            txn->engine->setTxnStatus(txn, static_cast<CF_TxnStatus_t>(ph->int_header.fin.cc));

            /* Generally FIN is the last exchange in an S2 transaction, the remote is not supposed
             * to send it until after the EOF+ACK.  So at this point we stop trying to send anything
             * to the peer, regardless of whether we got every ACK we expected. */
            txn->engine->finishTransaction(txn, true);
        }
        txn->flags.tx.send_fin_ack = true;
    }
}

void CfdpTransaction::s2Nak(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph) {
    const CF_Logical_SegmentRequest_t *sr;
    const CF_Logical_PduNak_t *        nak;
    U8                              counter;
    U8                              bad_sr;

    bad_sr = 0;

    /* this function is only invoked for NAK PDU types */
    nak = &ph->int_header.nak;

    if (txn->engine->recvNak(txn, ph) == CfdpStatus::SUCCESS && nak->segment_list.num_segments > 0)
    {
        for (counter = 0; counter < nak->segment_list.num_segments; ++counter)
        {
            sr = &nak->segment_list.segments[counter];

            if (sr->offset_start == 0 && sr->offset_end == 0)
            {
                /* need to re-send metadata PDU */
                txn->flags.tx.md_need_send = true;
            }
            else
            {
                if (sr->offset_end < sr->offset_start)
                {
                    ++bad_sr;
                    continue;
                }

                /* overflow probably won't be an issue */
                if (sr->offset_end > txn->fsize)
                {
                    ++bad_sr;
                    continue;
                }

                /* insert gap data in chunks */
                CF_ChunkListAdd(&txn->chunks->chunks, sr->offset_start, sr->offset_end - sr->offset_start);
            }
        }

        // CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.recv.nak_segment_requests +=
        //     nak->segment_list.num_segments;
        if (bad_sr)
        {
            // CFE_EVS_SendEvent(CF_CFDP_S_INVALID_SR_ERR_EID, CFE_EVS_EventType_ERROR,
            //                   "CF S%d(%lu:%lu): received %d invalid NAK segment requests",
            //                   (txn->state == CF_TxnState_S2), (unsigned long)txn->history->src_eid,
            //                   (unsigned long)txn->history->seq_num, bad_sr);
        }
    }
    else
    {
        // CFE_EVS_SendEvent(CF_CFDP_S_PDU_NAK_ERR_EID, CFE_EVS_EventType_ERROR,
        //                   "CF S%d(%lu:%lu): received invalid NAK PDU", (txn->state == CF_TxnState_S2),
        //                   (unsigned long)txn->history->src_eid, (unsigned long)txn->history->seq_num);
        // ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.recv.error;
    }
}

void CfdpTransaction::s2NakArm(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph) {
    txn->engine->armAckTimer(txn);
    this->s2Nak(txn, ph);
}

void CfdpTransaction::s2EofAck(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph) {
    if (!txn->engine->recvAck(txn, ph) && ph->int_header.ack.ack_directive_code == CF_CFDP_FileDirective_EOF)
    {
        txn->flags.tx.eof_ack_recv           = true;
        txn->flags.com.ack_timer_armed       = false; /* just wait for FIN now, nothing to re-send */
        txn->state_data.send.s2.acknak_count = 0;     /* in case EOF retransmits had occurred */

        /* if FIN was also received then we are done (these can come out of order) */
        if (txn->flags.tx.fin_recv)
        {
            txn->engine->finishTransaction(txn, true);
        }
    }
}

}  // namespace Ccsds
}  // namespace Svc
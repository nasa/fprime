/************************************************************************
 * NASA Docket No. GSC-18,447-1, and identified as “CFS CFDP (CF)
 * Application version 3.0.0”
 *
 * Copyright (c) 2019 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ************************************************************************/

/**
 * @file
 *
 *  The CF Application CFDP send logic source file
 *
 *  Handles all CFDP engine functionality specific to TX transactions.
 */

#include "cfe.h"
#include "cf_verify.h"
#include "cf_app.h"
#include "cf_events.h"
#include "cf_perfids.h"
#include "cf_cfdp.h"
#include "cf_utils.h"

#include "cf_cfdp_s.h"
#include "cf_cfdp_dispatch.h"

#include <stdio.h>
#include <string.h>
#include "cf_assert.h"

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_s.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_CFDP_S_SendEof(CF_Transaction_t *txn)
{
    /* note the crc is "finalized" regardless of success or failure of the txn */
    /* this is OK as we still need to put some value into the EOF */
    if (!txn->flags.com.crc_calc)
    {
        CF_CRC_Finalize(&txn->crc);
        txn->flags.com.crc_calc = true;
    }
    return CF_CFDP_SendEof(txn);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_s.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_S1_SubstateSendEof(CF_Transaction_t *txn)
{
    /* set the flag, the EOF is sent by the tick handler */
    txn->flags.tx.send_eof = true;

    /* In class 1 this is the end of normal operation */
    /* NOTE: this is not always true, as class 1 can request an EOF ack.
     * In this case we could change state to CLOSEOUT_SYNC instead and wait,
     * but right now we do not request an EOF ack in S1 */
    CF_CFDP_FinishTransaction(txn, true);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_s.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_S2_SubstateSendEof(CF_Transaction_t *txn)
{
    /* set the flag, the EOF is sent by the tick handler */
    txn->flags.tx.send_eof = true;

    /* wait for remaining responses to close out the state machine */
    txn->state_data.send.sub_state = CF_TxSubState_CLOSEOUT_SYNC;

    /* always move the transaction onto the wait queue now */
    CF_DequeueTransaction(txn);
    CF_InsertSortPrio(txn, CF_QueueIdx_TXW);

    /* the ack timer is armed in class 2 only */
    CF_CFDP_ArmAckTimer(txn);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_s.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_CFDP_S_SendFileData(CF_Transaction_t *txn, uint32 foffs, uint32 bytes_to_read, uint8 calc_crc)
{
    bool                            success = true;
    int                             status  = 0;
    CFE_Status_t                    ret     = CF_ERROR;
    CF_Logical_PduBuffer_t *        ph      = CF_CFDP_ConstructPduHeader(txn, 0, CF_AppData.config_table->local_eid,
                                                            txn->history->peer_eid, 0, txn->history->seq_num, 1);
    CF_Logical_PduFileDataHeader_t *fd;
    size_t                          actual_bytes;
    void *                          data_ptr;

    if (!ph)
    {
        ret     = CFE_SUCCESS; /* couldn't get message, so no bytes sent. Will try again next time */
        success = false;
    }
    else
    {
        fd = &ph->int_header.fd;

        /* need to encode data header up to this point to figure out where data needs to get copied to */
        fd->offset = foffs;
        CF_CFDP_EncodeFileDataHeader(ph->penc, ph->pdu_header.segment_meta_flag, fd);

        /*
         * the actual bytes to read is the smallest of these:
         *  - passed-in size
         *  - outgoing_file_chunk_size from configuration
         *  - amount of space actually available in the PDU after encoding the headers
         */
        actual_bytes = CF_CODEC_GET_REMAIN(ph->penc);
        if (actual_bytes > bytes_to_read)
        {
            actual_bytes = bytes_to_read;
        }
        if (actual_bytes > CF_AppData.config_table->outgoing_file_chunk_size)
        {
            actual_bytes = CF_AppData.config_table->outgoing_file_chunk_size;
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
            status = CF_WrappedLseek(txn->fd, foffs, OS_SEEK_SET);
            if (status != foffs)
            {
                CFE_EVS_SendEvent(CF_CFDP_S_SEEK_FD_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "CF S%d(%lu:%lu): error seeking to offset %ld, got %ld",
                                  (txn->state == CF_TxnState_S2), (unsigned long)txn->history->src_eid,
                                  (unsigned long)txn->history->seq_num, (long)foffs, (long)status);
                ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.fault.file_seek;
                success = false;
            }
        }

        if (success)
        {
            status = CF_WrappedRead(txn->fd, data_ptr, actual_bytes);
            if (status != actual_bytes)
            {
                CFE_EVS_SendEvent(CF_CFDP_S_READ_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "CF S%d(%lu:%lu): error reading bytes: expected %ld, got %ld",
                                  (txn->state == CF_TxnState_S2), (unsigned long)txn->history->src_eid,
                                  (unsigned long)txn->history->seq_num, (long)actual_bytes, (long)status);
                ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.fault.file_read;
                success = false;
            }
        }

        if (success)
        {
            txn->state_data.send.cached_pos += status;
            CF_CFDP_SendFd(txn, ph); /* CF_CFDP_SendFd only returns CFE_SUCCESS */

            CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.sent.file_data_bytes += actual_bytes;
            CF_Assert((foffs + actual_bytes) <= txn->fsize); /* sanity check */
            if (calc_crc)
            {
                CF_CRC_Digest(&txn->crc, fd->data_ptr, fd->data_len);
            }

            ret = actual_bytes;
        }
    }

    return ret;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_s.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_S_SubstateSendFileData(CF_Transaction_t *txn)
{
    int32 bytes_processed = CF_CFDP_S_SendFileData(txn, txn->foffs, (txn->fsize - txn->foffs), 1);

    if (bytes_processed > 0)
    {
        txn->foffs += bytes_processed;
        if (txn->foffs == txn->fsize)
        {
            /* file is done */
            txn->state_data.send.sub_state = CF_TxSubState_EOF;
        }
    }
    else if (bytes_processed < 0)
    {
        /* IO error -- change state and send EOF */
        CF_CFDP_SetTxnStatus(txn, CF_TxnStatus_FILESTORE_REJECTION);
        txn->state_data.send.sub_state = CF_TxSubState_EOF;
    }
    else
    {
        /* don't care about other cases */
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_s.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_CFDP_S_CheckAndRespondNak(CF_Transaction_t *txn)
{
    const CF_Chunk_t *chunk;
    CFE_Status_t      sret;
    CFE_Status_t      ret = CFE_SUCCESS;

    if (txn->flags.tx.md_need_send)
    {
        sret = CF_CFDP_SendMd(txn);
        if (sret == CF_SEND_PDU_ERROR)
        {
            ret = CF_ERROR; /* error occurred */
        }
        else
        {
            if (sret == CFE_SUCCESS)
            {
                txn->flags.tx.md_need_send = false;
            }
            /* unless CF_SEND_PDU_ERROR, return 1 to keep caller from sending file data */
            ret = 1; /* 1 means nak processed, so don't send filedata */
        }
    }
    else
    {
        /* Get first chunk and process if available */
        chunk = CF_ChunkList_GetFirstChunk(&txn->chunks->chunks);
        if (chunk != NULL)
        {
            ret = CF_CFDP_S_SendFileData(txn, chunk->offset, chunk->size, 0);
            if (ret > 0)
            {
                CF_ChunkList_RemoveFromFirst(&txn->chunks->chunks, ret);
                ret = 1; /* processed nak, so caller doesn't send file data */
            }
            else if (ret < 0)
            {
                ret = CF_ERROR; /* error occurred */
            }
            else
            {
                /* nothing to do if ret==0, since nothing was sent */
            }
        }
    }

    return ret;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_s.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_S2_SubstateSendFileData(CF_Transaction_t *txn)
{
    int ret = CF_CFDP_S_CheckAndRespondNak(txn);

    if (!ret)
    {
        CF_CFDP_S_SubstateSendFileData(txn);
    }
    else if (ret < 0)
    {
        CF_CFDP_SetTxnStatus(txn, CF_TxnStatus_NAK_RESPONSE_ERROR);
        txn->flags.tx.send_eof = true; /* do not leave the remote hanging */
        CF_CFDP_FinishTransaction(txn, true);
    }
    else
    {
        /* don't care about other cases */
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_s.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_S_SubstateSendMetadata(CF_Transaction_t *txn)
{
    CFE_Status_t sret;
    int32        ret;
    int          status  = 0;
    bool         success = true;

    if (!OS_ObjectIdDefined(txn->fd))
    {
        if (OS_FileOpenCheck(txn->history->fnames.src_filename) == OS_SUCCESS)
        {
            CFE_EVS_SendEvent(CF_CFDP_S_ALREADY_OPEN_ERR_EID, CFE_EVS_EventType_ERROR,
                              "CF S%d(%lu:%lu): file %s already open", (txn->state == CF_TxnState_S2),
                              (unsigned long)txn->history->src_eid, (unsigned long)txn->history->seq_num,
                              txn->history->fnames.src_filename);
            ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.fault.file_open;
            success = false;
        }

        if (success)
        {
            ret = CF_WrappedOpenCreate(&txn->fd, txn->history->fnames.src_filename, OS_FILE_FLAG_NONE, OS_READ_ONLY);
            if (ret < 0)
            {
                CFE_EVS_SendEvent(CF_CFDP_S_OPEN_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "CF S%d(%lu:%lu): failed to open file %s, error=%ld", (txn->state == CF_TxnState_S2),
                                  (unsigned long)txn->history->src_eid, (unsigned long)txn->history->seq_num,
                                  txn->history->fnames.src_filename, (long)ret);
                ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.fault.file_open;
                txn->fd = OS_OBJECT_ID_UNDEFINED; /* just in case */
                success = false;
            }
        }

        if (success)
        {
            status = CF_WrappedLseek(txn->fd, 0, OS_SEEK_END);
            if (status < 0)
            {
                CFE_EVS_SendEvent(CF_CFDP_S_SEEK_END_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "CF S%d(%lu:%lu): failed to seek end file %s, error=%ld",
                                  (txn->state == CF_TxnState_S2), (unsigned long)txn->history->src_eid,
                                  (unsigned long)txn->history->seq_num, txn->history->fnames.src_filename,
                                  (long)status);
                ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.fault.file_seek;
                success = false;
            }
        }

        if (success)
        {
            txn->fsize = status;

            status = CF_WrappedLseek(txn->fd, 0, OS_SEEK_SET);
            if (status != 0)
            {
                CFE_EVS_SendEvent(CF_CFDP_S_SEEK_BEG_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "CF S%d(%lu:%lu): failed to seek begin file %s, got %ld",
                                  (txn->state == CF_TxnState_S2), (unsigned long)txn->history->src_eid,
                                  (unsigned long)txn->history->seq_num, txn->history->fnames.src_filename,
                                  (long)status);
                ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.fault.file_seek;
                success = false;
            }
        }
    }

    if (success)
    {
        sret = CF_CFDP_SendMd(txn);
        if (sret == CF_SEND_PDU_ERROR)
        {
            /* failed to send md */
            CFE_EVS_SendEvent(CF_CFDP_S_SEND_MD_ERR_EID, CFE_EVS_EventType_ERROR, "CF S%d(%lu:%lu): failed to send md",
                              (txn->state == CF_TxnState_S2), (unsigned long)txn->history->src_eid,
                              (unsigned long)txn->history->seq_num);
            success = false;
        }
        else if (sret == CFE_SUCCESS)
        {
            /* once metadata is sent, switch to filedata mode */
            txn->state_data.send.sub_state = CF_TxSubState_FILEDATA;
        }
        /* if sret==CF_SEND_PDU_NO_BUF_AVAIL_ERROR, then try to send md again next cycle */
    }

    if (!success)
    {
        CF_CFDP_SetTxnStatus(txn, CF_TxnStatus_FILESTORE_REJECTION);
        CF_CFDP_FinishTransaction(txn, true);
    }

    /* don't need CF_CRC_Start() since taken care of by reset_cfdp() */
    /*CF_CRC_Start(&txn->crc);*/
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_s.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_CFDP_S_SendFinAck(CF_Transaction_t *txn)
{
    return CF_CFDP_SendAck(txn, CF_CFDP_GetTxnStatus(txn), CF_CFDP_FileDirective_FIN, txn->state_data.send.s2.fin_cc,
                           txn->history->peer_eid, txn->history->seq_num);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_s.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_S2_EarlyFin(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    /* received early fin, so just cancel */
    CFE_EVS_SendEvent(CF_CFDP_S_EARLY_FIN_ERR_EID, CFE_EVS_EventType_ERROR,
                      "CF S%d(%lu:%lu): got early FIN -- cancelling", (txn->state == CF_TxnState_S2),
                      (unsigned long)txn->history->src_eid, (unsigned long)txn->history->seq_num);
    CF_CFDP_SetTxnStatus(txn, CF_TxnStatus_EARLY_FIN);

    txn->state_data.send.sub_state = CF_TxSubState_CLOSEOUT_SYNC;

    /* otherwise do normal fin processing */
    CF_CFDP_S2_Fin(txn, ph);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_s.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_S2_Fin(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    if (!CF_CFDP_RecvFin(txn, ph))
    {
        /* set the CC only on the first time we get the FIN.  If this is a dupe
         * then re-ack but otherwise ignore it */
        if (!txn->flags.tx.fin_recv)
        {
            txn->flags.tx.fin_recv               = true;
            txn->state_data.send.s2.fin_cc       = ph->int_header.fin.cc;
            txn->state_data.send.s2.acknak_count = 0; /* in case retransmits had occurred */

            /* note this is a no-op unless the status was unset previously */
            CF_CFDP_SetTxnStatus(txn, ph->int_header.fin.cc);

            /* Generally FIN is the last exchange in an S2 transaction, the remote is not supposed
             * to send it until after the EOF+ACK.  So at this point we stop trying to send anything
             * to the peer, regardless of whether we got every ACK we expected. */
            CF_CFDP_FinishTransaction(txn, true);
        }
        txn->flags.tx.send_fin_ack = true;
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_s.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_S2_Nak(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    const CF_Logical_SegmentRequest_t *sr;
    const CF_Logical_PduNak_t *        nak;
    uint8                              counter;
    uint8                              bad_sr;

    bad_sr = 0;

    /* this function is only invoked for NAK PDU types */
    nak = &ph->int_header.nak;

    if (CF_CFDP_RecvNak(txn, ph) == CFE_SUCCESS && nak->segment_list.num_segments > 0)
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

        CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.recv.nak_segment_requests +=
            nak->segment_list.num_segments;
        if (bad_sr)
        {
            CFE_EVS_SendEvent(CF_CFDP_S_INVALID_SR_ERR_EID, CFE_EVS_EventType_ERROR,
                              "CF S%d(%lu:%lu): received %d invalid NAK segment requests",
                              (txn->state == CF_TxnState_S2), (unsigned long)txn->history->src_eid,
                              (unsigned long)txn->history->seq_num, bad_sr);
        }
    }
    else
    {
        CFE_EVS_SendEvent(CF_CFDP_S_PDU_NAK_ERR_EID, CFE_EVS_EventType_ERROR,
                          "CF S%d(%lu:%lu): received invalid NAK PDU", (txn->state == CF_TxnState_S2),
                          (unsigned long)txn->history->src_eid, (unsigned long)txn->history->seq_num);
        ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.recv.error;
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_s.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_S2_Nak_Arm(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    CF_CFDP_ArmAckTimer(txn);
    CF_CFDP_S2_Nak(txn, ph);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_s.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_S2_EofAck(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    if (!CF_CFDP_RecvAck(txn, ph) && ph->int_header.ack.ack_directive_code == CF_CFDP_FileDirective_EOF)
    {
        txn->flags.tx.eof_ack_recv           = true;
        txn->flags.com.ack_timer_armed       = false; /* just wait for FIN now, nothing to re-send */
        txn->state_data.send.s2.acknak_count = 0;     /* in case EOF retransmits had occurred */

        /* if FIN was also received then we are done (these can come out of order) */
        if (txn->flags.tx.fin_recv)
        {
            CF_CFDP_FinishTransaction(txn, true);
        }
    }
    else
    {
        CFE_EVS_SendEvent(CF_CFDP_S_PDU_EOF_ERR_EID, CFE_EVS_EventType_ERROR,
                          "CF S%d(%lu:%lu): received invalid EOF-ACK PDU", (txn->state == CF_TxnState_S2),
                          (unsigned long)txn->history->src_eid, (unsigned long)txn->history->seq_num);
        ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.recv.error;
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_s.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_S1_Recv(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    /* s1 doesn't need to receive anything */
    static const CF_CFDP_S_SubstateRecvDispatchTable_t substate_fns = {{NULL}};
    CF_CFDP_S_DispatchRecv(txn, ph, &substate_fns);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_s.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_S2_Recv(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    static const CF_CFDP_FileDirectiveDispatchTable_t s2_meta      = {.fdirective = {
                                                                     [CF_CFDP_FileDirective_FIN] = CF_CFDP_S2_EarlyFin,
                                                                 }};
    static const CF_CFDP_FileDirectiveDispatchTable_t s2_fd_or_eof = {
        .fdirective = {
            [CF_CFDP_FileDirective_FIN] = CF_CFDP_S2_EarlyFin, [CF_CFDP_FileDirective_NAK] = CF_CFDP_S2_Nak}};
    static const CF_CFDP_FileDirectiveDispatchTable_t s2_wait_ack = {
        .fdirective = {[CF_CFDP_FileDirective_FIN] = CF_CFDP_S2_Fin,
                       [CF_CFDP_FileDirective_ACK] = CF_CFDP_S2_EofAck,
                       [CF_CFDP_FileDirective_NAK] = CF_CFDP_S2_Nak_Arm}};

    static const CF_CFDP_S_SubstateRecvDispatchTable_t substate_fns = {
        .substate = {[CF_TxSubState_METADATA]      = &s2_meta,
                     [CF_TxSubState_FILEDATA]      = &s2_fd_or_eof,
                     [CF_TxSubState_EOF]           = &s2_fd_or_eof,
                     [CF_TxSubState_CLOSEOUT_SYNC] = &s2_wait_ack}};

    CF_CFDP_S_DispatchRecv(txn, ph, &substate_fns);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_s.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_S1_Tx(CF_Transaction_t *txn)
{
    static const CF_CFDP_S_SubstateSendDispatchTable_t substate_fns = {
        .substate = {
            [CF_TxSubState_METADATA] = CF_CFDP_S_SubstateSendMetadata,
            [CF_TxSubState_FILEDATA] = CF_CFDP_S_SubstateSendFileData,
            [CF_TxSubState_EOF]      = CF_CFDP_S1_SubstateSendEof,
        }};

    CF_CFDP_S_DispatchTransmit(txn, &substate_fns);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_s.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_S2_Tx(CF_Transaction_t *txn)
{
    static const CF_CFDP_S_SubstateSendDispatchTable_t substate_fns = {
        .substate = {
            [CF_TxSubState_METADATA] = CF_CFDP_S_SubstateSendMetadata,
            [CF_TxSubState_FILEDATA] = CF_CFDP_S2_SubstateSendFileData,
            [CF_TxSubState_EOF]      = CF_CFDP_S2_SubstateSendEof,
        }};

    CF_CFDP_S_DispatchTransmit(txn, &substate_fns);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_s.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_S_Cancel(CF_Transaction_t *txn)
{
    if (txn->state_data.send.sub_state < CF_TxSubState_EOF)
    {
        /* if state has not reached CF_TxSubState_EOF, then set it to CF_TxSubState_EOF now. */
        txn->state_data.send.sub_state = CF_TxSubState_EOF;
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_s.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_S_AckTimerTick(CF_Transaction_t *txn)
{
    /* note: the ack timer is only ever relevant on class 2 */
    if (txn->state != CF_TxnState_S2 || !txn->flags.com.ack_timer_armed)
    {
        /* nothing to do */
        return;
    }

    if (!CF_Timer_Expired(&txn->ack_timer))
    {
        CF_Timer_Tick(&txn->ack_timer);
    }
    else if (txn->state_data.send.sub_state == CF_TxSubState_CLOSEOUT_SYNC)
    {
        /* Check limit and handle if needed */
        if (txn->state_data.send.s2.acknak_count >= CF_AppData.config_table->chan[txn->chan_num].ack_limit)
        {
            CFE_EVS_SendEvent(CF_CFDP_S_ACK_LIMIT_ERR_EID, CFE_EVS_EventType_ERROR,
                              "CF S2(%lu:%lu), ack limit reached, no eof-ack", (unsigned long)txn->history->src_eid,
                              (unsigned long)txn->history->seq_num);
            CF_CFDP_SetTxnStatus(txn, CF_TxnStatus_ACK_LIMIT_NO_EOF);
            ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.fault.ack_limit;

            /* give up on this */
            CF_CFDP_FinishTransaction(txn, true);
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
            CF_CFDP_ArmAckTimer(txn);
        }
    }
    else
    {
        /* if we are not waiting for anything, why is the ack timer armed? */
        txn->flags.com.ack_timer_armed = false;
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_s.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_S_Tick(CF_Transaction_t *txn, int *cont /* unused */)
{
    bool pending_send;

    pending_send = true; /* maybe; tbd, will be reset if not */

    /* at each tick, various timers used by S are checked */
    /* first, check inactivity timer */
    if (!txn->flags.com.inactivity_fired)
    {
        if (!CF_Timer_Expired(&txn->inactivity_timer))
        {
            CF_Timer_Tick(&txn->inactivity_timer);
        }
        else
        {
            txn->flags.com.inactivity_fired = true;

            /* HOLD state is the normal path to recycle transaction objects, not an error */
            /* inactivity is abnormal in any other state */
            if (txn->state != CF_TxnState_HOLD && txn->state == CF_TxnState_S2)
            {
                CFE_EVS_SendEvent(CF_CFDP_S_INACT_TIMER_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "CF S2(%lu:%lu): inactivity timer expired", (unsigned long)txn->history->src_eid,
                                  (unsigned long)txn->history->seq_num);
                CF_CFDP_SetTxnStatus(txn, CF_TxnStatus_INACTIVITY_DETECTED);

                ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.fault.inactivity_timer;
            }
        }
    }

    /* tx maintenance: possibly process send_eof, or send_fin_ack */
    if (txn->flags.tx.send_eof)
    {
        if (CF_CFDP_S_SendEof(txn) == CFE_SUCCESS)
        {
            txn->flags.tx.send_eof = false;
        }
    }
    else if (txn->flags.tx.send_fin_ack)
    {
        if (CF_CFDP_S_SendFinAck(txn) == CFE_SUCCESS)
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
        CF_CFDP_RecycleTransaction(txn);

        /* NOTE: this must be the last thing in here.  Do not use txn after this */
    }
    else
    {
        /* transaction still valid so process the ACK timer, if relevant */
        CF_CFDP_S_AckTimerTick(txn);
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_s.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_S_Tick_Nak(CF_Transaction_t *txn, int *cont)
{
    int ret = CF_CFDP_S_CheckAndRespondNak(txn);

    if (ret == 1)
        *cont = 1; /* cause dispatcher to re-enter this wakeup */
}

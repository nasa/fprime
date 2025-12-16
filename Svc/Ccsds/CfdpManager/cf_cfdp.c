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
 *  The CF Application main CFDP engine and PDU parsing implementation
 *
 *  This file contains two sets of functions. The first is what is needed
 *  to deal with CFDP PDUs. Specifically validating them for correctness
 *  and ensuring the byte-order is correct for the target. The second
 *  is incoming and outgoing CFDP PDUs pass through here. All receive
 *  CFDP PDU logic is performed here and the data is passed to the
 *  R (rx) and S (tx) logic.
 */

#include "cfe.h"
#include "cf_verify.h"
#include "cf_app.h"
#include "cf_events.h"
#include "cf_perfids.h"
#include "cf_cfdp.h"
#include "cf_utils.h"

#include "cf_cfdp_r.h"
#include "cf_cfdp_s.h"
#include "cf_cfdp_dispatch.h"
#include "cf_cfdp_sbintf.h"

#include <string.h>
#include "cf_assert.h"

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_EncodeStart(CF_EncoderState_t *penc, void *msgbuf, CF_Logical_PduBuffer_t *ph, size_t encap_hdr_size,
                         size_t total_size)
{
    /* Clear the PDU buffer structure to start */
    memset(ph, 0, sizeof(*ph));

    /* attach encoder object to PDU buffer which is attached to SB (encapsulation) buffer */
    penc->base = (uint8 *)msgbuf;
    ph->penc   = penc;

    CF_CFDP_CodecReset(&penc->codec_state, total_size);

    /*
     * adjust so that the base points to the actual PDU Header, this makes the offset
     * refer to the real offset within the CFDP PDU, rather than the offset of the SB
     * msg container.
     */
    if (total_size > encap_hdr_size)
    {
        penc->codec_state.max_size -= encap_hdr_size;
        penc->base += encap_hdr_size;
    }
    else
    {
        CF_CFDP_CodecSetDone(&penc->codec_state);
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_DecodeStart(CF_DecoderState_t *pdec, const void *msgbuf, CF_Logical_PduBuffer_t *ph, size_t encap_hdr_size,
                         size_t total_size)
{
    /* Clear the PDU buffer structure to start */
    memset(ph, 0, sizeof(*ph));

    /* attach decoder object to PDU buffer which is attached to SB (encapsulation) buffer */
    pdec->base = (const uint8 *)msgbuf;
    ph->pdec   = pdec;

    CF_CFDP_CodecReset(&pdec->codec_state, total_size);

    /*
     * adjust so that the base points to the actual PDU Header, this makes the offset
     * refer to the real offset within the CFDP PDU, rather than the offset of the SB
     * msg container.
     */
    if (total_size > encap_hdr_size)
    {
        pdec->codec_state.max_size -= encap_hdr_size;
        pdec->base += encap_hdr_size;
    }
    else
    {
        CF_CFDP_CodecSetDone(&pdec->codec_state);
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_ArmAckTimer(CF_Transaction_t *txn)
{
    CF_Timer_InitRelSec(&txn->ack_timer, CF_AppData.config_table->chan[txn->chan_num].ack_timer_s);
    txn->flags.com.ack_timer_armed = true;
}

/*----------------------------------------------------------------
 *
 * Internal helper routine only, not part of API.
 *
 *-----------------------------------------------------------------*/
static inline CF_CFDP_Class_t CF_CFDP_GetClass(const CF_Transaction_t *txn)
{
    CF_Assert(txn->flags.com.q_index != CF_QueueIdx_FREE);
    return !!((txn->state == CF_TxnState_S2) || (txn->state == CF_TxnState_R2));
}

/*----------------------------------------------------------------
 *
 * Internal helper routine only, not part of API.
 *
 *-----------------------------------------------------------------*/
static inline bool CF_CFDP_IsSender(CF_Transaction_t *txn)
{
    CF_Assert(txn->history);

    return (txn->history->dir == CF_Direction_TX);
}

/*----------------------------------------------------------------
 *
 * Internal helper routine only, not part of API.
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_ArmInactTimer(CF_Transaction_t *txn)
{
    CF_Timer_Seconds_t Sec;

    /* select timeout based on the state */
    if (CF_CFDP_GetTxnStatus(txn) == CF_CFDP_AckTxnStatus_ACTIVE)
    {
        /* in an active transaction, we expect traffic so use the normal inactivity timer */
        Sec = CF_AppData.config_table->chan[txn->chan_num].inactivity_timer_s;
    }
    else
    {
        /* in an inactive transaction, we do NOT expect traffic, and this timer is now used
         * just in case any late straggler PDUs dp get delivered.  In this case the
         * time should be longer than the retransmit time (ack timer) but less than the full
         * inactivity timer (because again, we are not expecting traffic, so waiting the full
         * timeout would hold resources longer than needed).  Using double the ack timer should
         * ensure that if the remote retransmitted anything, we will see it, and avoids adding
         * another config option just for this. */
        Sec = CF_AppData.config_table->chan[txn->chan_num].ack_timer_s * 2;
    }

    CF_Timer_InitRelSec(&txn->inactivity_timer, Sec);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_DispatchRecv(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    static const CF_CFDP_TxnRecvDispatchTable_t state_fns = {.rx = {[CF_TxnState_INIT] = CF_CFDP_RecvInit,
                                                                    [CF_TxnState_R1]   = CF_CFDP_R1_Recv,
                                                                    [CF_TxnState_S1]   = CF_CFDP_S1_Recv,
                                                                    [CF_TxnState_R2]   = CF_CFDP_R2_Recv,
                                                                    [CF_TxnState_S2]   = CF_CFDP_S2_Recv,
                                                                    [CF_TxnState_DROP] = CF_CFDP_RecvDrop,
                                                                    [CF_TxnState_HOLD] = CF_CFDP_RecvHold}};

    CF_CFDP_RxStateDispatch(txn, ph, &state_fns);
    CF_CFDP_ArmInactTimer(txn); /* whenever a packet was received by the other size, always arm its inactivity timer */
}

/*----------------------------------------------------------------
 *
 * Internal helper routine only, not part of API.
 *
 *-----------------------------------------------------------------*/
static void CF_CFDP_DispatchTx(CF_Transaction_t *txn)
{
    static const CF_CFDP_TxnSendDispatchTable_t state_fns = {
        .tx = {[CF_TxnState_S1] = CF_CFDP_S1_Tx, [CF_TxnState_S2] = CF_CFDP_S2_Tx}};

    CF_CFDP_TxStateDispatch(txn, &state_fns);
}

/*----------------------------------------------------------------
 *
 * Internal helper routine only, not part of API.
 *
 *-----------------------------------------------------------------*/
static CF_ChunkWrapper_t *CF_CFDP_FindUnusedChunks(CF_Channel_t *chan, CF_Direction_t dir)
{
    CF_ChunkWrapper_t *ret;
    CF_CListNode_t **  chunklist_head;

    chunklist_head = CF_GetChunkListHead(chan, dir);

    /* this should never be null */
    CF_Assert(chunklist_head);

    if (*chunklist_head == NULL)
    {
        ret = NULL;
    }
    else
    {
        ret = container_of(CF_CList_Pop(chunklist_head), CF_ChunkWrapper_t, cl_node);
    }

    return ret;
}

/*----------------------------------------------------------------
 *
 * Internal helper routine only, not part of API.
 *
 *-----------------------------------------------------------------*/
static void CF_CFDP_SetPduLength(CF_Logical_PduBuffer_t *ph)
{
    uint16 final_pos;

    /* final position of the encoder state should reflect the entire PDU length */
    final_pos = CF_CODEC_GET_POSITION(ph->penc);

    if (final_pos >= ph->pdu_header.header_encoded_length)
    {
        /* the value that goes into the packet is length _after_ header */
        ph->pdu_header.data_encoded_length = final_pos - ph->pdu_header.header_encoded_length;
    }

    CF_CFDP_EncodeHeaderFinalSize(ph->penc, &ph->pdu_header);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CF_Logical_PduBuffer_t *CF_CFDP_ConstructPduHeader(const CF_Transaction_t *txn, CF_CFDP_FileDirective_t directive_code,
                                                   CF_EntityId_t src_eid, CF_EntityId_t dst_eid, bool towards_sender,
                                                   CF_TransactionSeq_t tsn, bool silent)
{
    /* directive_code == 0 if file data */
    CF_Logical_PduBuffer_t *ph;
    CF_Logical_PduHeader_t *hdr;
    uint8                   eid_len;

    ph = CF_CFDP_MsgOutGet(txn, silent);

    if (ph)
    {
        hdr = &ph->pdu_header;

        hdr->version   = 1;
        hdr->pdu_type  = (directive_code == 0);     /* set to '1' for file data PDU, '0' for a directive PDU */
        hdr->direction = (towards_sender != false); /* set to '1' for toward sender, '0' for toward receiver */
        hdr->txm_mode  = (CF_CFDP_GetClass(txn) == CF_CFDP_CLASS_1); /* set to '1' for class 1 data, '0' for class 2 */

        /* choose the larger of the two EIDs to determine size */
        if (src_eid > dst_eid)
        {
            eid_len = CF_CFDP_GetValueEncodedSize(src_eid);
        }
        else
        {
            eid_len = CF_CFDP_GetValueEncodedSize(dst_eid);
        }

        /*
         * This struct holds the "real" length - when assembled into the final packet
         * this is encoded as 1 less than this value
         */
        hdr->eid_length     = eid_len;
        hdr->txn_seq_length = CF_CFDP_GetValueEncodedSize(tsn);

        hdr->source_eid      = src_eid;
        hdr->destination_eid = dst_eid;
        hdr->sequence_num    = tsn;

        /*
         * encode the known parts so far.  total_size field cannot be
         * included yet because its value is not known, but the basic
         * encoding of the other stuff needs to be done so the position
         * of any data fields can be determined.
         */
        CF_CFDP_EncodeHeaderWithoutSize(ph->penc, hdr);

        /* If directive code is zero, the PDU is a file data PDU which has no directive code field.
         * So only set if non-zero, otherwise it will write a 0 to a byte in a file data PDU where we
         * don't necessarily want a 0. */
        if (directive_code)
        {
            /* set values which can be determined at this time */
            ph->fdirective.directive_code = directive_code;

            CF_CFDP_EncodeFileDirectiveHeader(ph->penc, &ph->fdirective);
        }
    }

    return ph;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_CFDP_SendMd(CF_Transaction_t *txn)
{
    CF_Logical_PduBuffer_t *ph =
        CF_CFDP_ConstructPduHeader(txn, CF_CFDP_FileDirective_METADATA, CF_AppData.config_table->local_eid,
                                   txn->history->peer_eid, 0, txn->history->seq_num, 0);
    CF_Logical_PduMd_t *md;
    CFE_Status_t        sret = CFE_SUCCESS;

    if (!ph)
    {
        sret = CF_SEND_PDU_NO_BUF_AVAIL_ERROR;
    }
    else
    {
        md = &ph->int_header.md;

        CF_Assert((txn->state == CF_TxnState_S1) || (txn->state == CF_TxnState_S2));

        md->size = txn->fsize;

        /* at this point, need to append filenames into md packet */
        /* this does not actually copy here - that is done during encode */
        md->source_filename.length =
            OS_strnlen(txn->history->fnames.src_filename, sizeof(txn->history->fnames.src_filename));
        md->source_filename.data_ptr = txn->history->fnames.src_filename;
        md->dest_filename.length =
            OS_strnlen(txn->history->fnames.dst_filename, sizeof(txn->history->fnames.dst_filename));
        md->dest_filename.data_ptr = txn->history->fnames.dst_filename;

        CF_CFDP_EncodeMd(ph->penc, md);
        CF_CFDP_SetPduLength(ph);
        CF_CFDP_Send(txn->chan_num, ph);
    }

    return sret;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_CFDP_SendFd(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    /* NOTE: SendFd does not need a call to CF_CFDP_MsgOutGet, as the caller already has it */
    CFE_Status_t ret = CFE_SUCCESS;

    /* this should check if any encoding error occurred */

    /* update PDU length */
    CF_CFDP_SetPduLength(ph);
    CF_CFDP_Send(txn->chan_num, ph);

    return ret;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_AppendTlv(CF_Logical_TlvList_t *ptlv_list, CF_CFDP_TlvType_t tlv_type)
{
    CF_Logical_Tlv_t *ptlv;

    if (ptlv_list->num_tlv < CF_PDU_MAX_TLV)
    {
        ptlv = &ptlv_list->tlv[ptlv_list->num_tlv];
        ++ptlv_list->num_tlv;
    }
    else
    {
        ptlv = NULL;
    }

    if (ptlv)
    {
        ptlv->type = tlv_type;

        if (tlv_type == CF_CFDP_TLV_TYPE_ENTITY_ID)
        {
            ptlv->data.eid = CF_AppData.config_table->local_eid;
            ptlv->length   = CF_CFDP_GetValueEncodedSize(ptlv->data.eid);
        }
        else
        {
            ptlv->data.data_ptr = NULL;
            ptlv->length        = 0;
        }
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_CFDP_SendEof(CF_Transaction_t *txn)
{
    CF_Logical_PduBuffer_t *ph =
        CF_CFDP_ConstructPduHeader(txn, CF_CFDP_FileDirective_EOF, CF_AppData.config_table->local_eid,
                                   txn->history->peer_eid, 0, txn->history->seq_num, 0);
    CF_Logical_PduEof_t *eof;
    CFE_Status_t         ret = CFE_SUCCESS;

    if (!ph)
    {
        ret = CF_SEND_PDU_NO_BUF_AVAIL_ERROR;
    }
    else
    {
        eof = &ph->int_header.eof;

        eof->cc   = CF_TxnStatus_To_ConditionCode(txn->history->txn_stat);
        eof->crc  = txn->crc.result;
        eof->size = txn->fsize;

        if (eof->cc != CF_CFDP_ConditionCode_NO_ERROR)
        {
            CF_CFDP_AppendTlv(&eof->tlv_list, CF_CFDP_TLV_TYPE_ENTITY_ID);
        }

        CF_CFDP_EncodeEof(ph->penc, eof);
        CF_CFDP_SetPduLength(ph);
        CF_CFDP_Send(txn->chan_num, ph);
    }

    return ret;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_CFDP_SendAck(CF_Transaction_t *txn, CF_CFDP_AckTxnStatus_t ts, CF_CFDP_FileDirective_t dir_code,
                             CF_CFDP_ConditionCode_t cc, CF_EntityId_t peer_eid, CF_TransactionSeq_t tsn)
{
    CF_Logical_PduBuffer_t *ph;
    CF_Logical_PduAck_t *   ack;
    CFE_Status_t            ret = CFE_SUCCESS;
    CF_EntityId_t           src_eid;
    CF_EntityId_t           dst_eid;

    CF_Assert((dir_code == CF_CFDP_FileDirective_EOF) || (dir_code == CF_CFDP_FileDirective_FIN));

    if (CF_CFDP_IsSender(txn))
    {
        src_eid = CF_AppData.config_table->local_eid;
        dst_eid = peer_eid;
    }
    else
    {
        src_eid = peer_eid;
        dst_eid = CF_AppData.config_table->local_eid;
    }

    ph = CF_CFDP_ConstructPduHeader(txn, CF_CFDP_FileDirective_ACK, src_eid, dst_eid,
                                    (dir_code == CF_CFDP_FileDirective_EOF), tsn, 0);
    if (!ph)
    {
        ret = CF_SEND_PDU_NO_BUF_AVAIL_ERROR;
    }
    else
    {
        ack = &ph->int_header.ack;

        ack->ack_directive_code = dir_code;
        ack->ack_subtype_code   = 1; /* looks like always 1 if not extended features */
        ack->cc                 = cc;
        ack->txn_status         = ts;

        CF_CFDP_EncodeAck(ph->penc, ack);
        CF_CFDP_SetPduLength(ph);
        CF_CFDP_Send(txn->chan_num, ph);
    }

    return ret;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_CFDP_SendFin(CF_Transaction_t *txn, CF_CFDP_FinDeliveryCode_t dc, CF_CFDP_FinFileStatus_t fs,
                             CF_CFDP_ConditionCode_t cc)
{
    CF_Logical_PduBuffer_t *ph =
        CF_CFDP_ConstructPduHeader(txn, CF_CFDP_FileDirective_FIN, txn->history->peer_eid,
                                   CF_AppData.config_table->local_eid, 1, txn->history->seq_num, 0);
    CF_Logical_PduFin_t *fin;
    CFE_Status_t         ret = CFE_SUCCESS;

    if (!ph)
    {
        ret = CF_SEND_PDU_NO_BUF_AVAIL_ERROR;
    }
    else
    {
        fin = &ph->int_header.fin;

        fin->cc            = cc;
        fin->delivery_code = dc;
        fin->file_status   = fs;

        if (cc != CF_CFDP_ConditionCode_NO_ERROR)
        {
            CF_CFDP_AppendTlv(&fin->tlv_list, CF_CFDP_TLV_TYPE_ENTITY_ID);
        }

        CF_CFDP_EncodeFin(ph->penc, fin);
        CF_CFDP_SetPduLength(ph);
        CF_CFDP_Send(txn->chan_num, ph);
    }

    return ret;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_CFDP_SendNak(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    CF_Logical_PduNak_t *nak;
    CFE_Status_t         ret = CFE_SUCCESS;

    if (!ph)
    {
        ret = CF_SEND_PDU_NO_BUF_AVAIL_ERROR;
    }
    else
    {
        CF_Assert(CF_CFDP_GetClass(txn) == CF_CFDP_CLASS_2);

        nak = &ph->int_header.nak;

        /*
         * NOTE: the caller should have already initialized all the fields.
         * This does not need to add anything more to the NAK here
         */

        CF_CFDP_EncodeNak(ph->penc, nak);
        CF_CFDP_SetPduLength(ph);
        CF_CFDP_Send(txn->chan_num, ph);
    }

    return ret;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_CFDP_RecvPh(uint8 chan_num, CF_Logical_PduBuffer_t *ph)
{
    CFE_Status_t ret = CFE_SUCCESS;

    CF_Assert(chan_num < CF_NUM_CHANNELS);
    /*
     * If the source eid, destination eid, or sequence number fields
     * are larger than the sizes configured in the cf platform config
     * file, then reject the PDU.
     */
    if (CF_CFDP_DecodeHeader(ph->pdec, &ph->pdu_header) != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(CF_PDU_TRUNCATION_ERR_EID, CFE_EVS_EventType_ERROR,
                          "CF: PDU rejected due to EID/seq number field truncation");
        ++CF_AppData.hk.Payload.channel_hk[chan_num].counters.recv.error;
        ret = CF_ERROR;
    }
    /*
     * The "large file" flag is not supported by this implementation yet.
     * This means file sizes and offsets will be 64 bits, so codec routines
     * will need to be updated to understand this.  OSAL also doesn't support
     * 64-bit file access yet.
     */
    else if (CF_CODEC_IS_OK(ph->pdec) && ph->pdu_header.large_flag)
    {
        CFE_EVS_SendEvent(CF_PDU_LARGE_FILE_ERR_EID, CFE_EVS_EventType_ERROR,
                          "CF: PDU with large file bit received (unsupported)");
        ++CF_AppData.hk.Payload.channel_hk[chan_num].counters.recv.error;
        ret = CF_ERROR;
    }
    else
    {
        if (CF_CODEC_IS_OK(ph->pdec) && ph->pdu_header.pdu_type == 0)
        {
            CF_CFDP_DecodeFileDirectiveHeader(ph->pdec, &ph->fdirective);
        }

        if (!CF_CODEC_IS_OK(ph->pdec))
        {
            CFE_EVS_SendEvent(CF_PDU_SHORT_HEADER_ERR_EID, CFE_EVS_EventType_ERROR, "CF: PDU too short (%lu received)",
                              (unsigned long)CF_CODEC_GET_SIZE(ph->pdec));
            ++CF_AppData.hk.Payload.channel_hk[chan_num].counters.recv.error;
            ret = CF_SHORT_PDU_ERROR;
        }
        else
        {
            /* PDU is ok, so continue processing */
            ++CF_AppData.hk.Payload.channel_hk[chan_num].counters.recv.pdu;
        }
    }

    return ret;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_CFDP_RecvMd(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    const CF_Logical_PduMd_t *md = &ph->int_header.md;
    int                       lv_ret;
    CFE_Status_t              ret = CFE_SUCCESS;

    CF_CFDP_DecodeMd(ph->pdec, &ph->int_header.md);
    if (!CF_CODEC_IS_OK(ph->pdec))
    {
        CFE_EVS_SendEvent(CF_PDU_MD_SHORT_ERR_EID, CFE_EVS_EventType_ERROR,
                          "CF: metadata packet too short: %lu bytes received",
                          (unsigned long)CF_CODEC_GET_SIZE(ph->pdec));
        ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.recv.error;
        ret = CF_PDU_METADATA_ERROR;
    }
    else
    {
        /* store the expected file size in transaction */
        txn->fsize = md->size;

        /*
         * store the filenames in transaction.
         *
         * NOTE: The "CF_CFDP_CopyStringFromLV()" now knows that the data is supposed to be a C string,
         * and ensures that the output content is properly terminated, so this only needs to check that
         * it worked.
         */
        lv_ret = CF_CFDP_CopyStringFromLV(txn->history->fnames.src_filename, sizeof(txn->history->fnames.src_filename),
                                          &md->source_filename);
        if (lv_ret < 0)
        {
            CFE_EVS_SendEvent(CF_PDU_INVALID_SRC_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
                              "CF: metadata PDU rejected due to invalid length in source filename of 0x%02x",
                              md->source_filename.length);
            ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.recv.error;
            ret = CF_PDU_METADATA_ERROR;
        }
        else
        {
            lv_ret = CF_CFDP_CopyStringFromLV(txn->history->fnames.dst_filename,
                                              sizeof(txn->history->fnames.dst_filename), &md->dest_filename);
            if (lv_ret < 0)
            {
                CFE_EVS_SendEvent(CF_PDU_INVALID_DST_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "CF: metadata PDU rejected due to invalid length in dest filename of 0x%02x",
                                  md->dest_filename.length);
                ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.recv.error;
                ret = CF_PDU_METADATA_ERROR;
            }
            else
            {
                CFE_EVS_SendEvent(CF_PDU_MD_RECVD_INF_EID, CFE_EVS_EventType_INFORMATION,
                                  "CF: md received for source: %s, dest: %s", txn->history->fnames.src_filename,
                                  txn->history->fnames.dst_filename);
            }
        }
    }

    return ret;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_CFDP_RecvFd(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    CFE_Status_t ret = CFE_SUCCESS;

    CF_CFDP_DecodeFileDataHeader(ph->pdec, ph->pdu_header.segment_meta_flag, &ph->int_header.fd);

    /* if the CRC flag is set, need to deduct the size of the CRC from the data - always 32 bits */
    if (CF_CODEC_IS_OK(ph->pdec) && ph->pdu_header.crc_flag)
    {
        if (ph->int_header.fd.data_len < sizeof(CF_CFDP_uint32_t))
        {
            CF_CODEC_SET_DONE(ph->pdec);
        }
        else
        {
            ph->int_header.fd.data_len -= sizeof(CF_CFDP_uint32_t);
        }
    }

    if (!CF_CODEC_IS_OK(ph->pdec))
    {
        CFE_EVS_SendEvent(CF_PDU_FD_SHORT_ERR_EID, CFE_EVS_EventType_ERROR,
                          "CF: filedata PDU too short: %lu bytes received", (unsigned long)CF_CODEC_GET_SIZE(ph->pdec));
        CF_CFDP_SetTxnStatus(txn, CF_TxnStatus_PROTOCOL_ERROR);
        ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.recv.error;
        ret = CF_SHORT_PDU_ERROR;
    }
    else if (ph->pdu_header.segment_meta_flag)
    {
        /* If recv PDU has the "segment_meta_flag" set, this is not currently handled in CF. */
        CFE_EVS_SendEvent(CF_PDU_FD_UNSUPPORTED_ERR_EID, CFE_EVS_EventType_ERROR,
                          "CF: filedata PDU with segment metadata received");
        CF_CFDP_SetTxnStatus(txn, CF_TxnStatus_PROTOCOL_ERROR);
        ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.recv.error;
        ret = CF_ERROR;
    }

    return ret;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_CFDP_RecvEof(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    CFE_Status_t ret = CFE_SUCCESS;

    CF_CFDP_DecodeEof(ph->pdec, &ph->int_header.eof);

    if (!CF_CODEC_IS_OK(ph->pdec))
    {
        CFE_EVS_SendEvent(CF_PDU_EOF_SHORT_ERR_EID, CFE_EVS_EventType_ERROR,
                          "CF: EOF PDU too short: %lu bytes received", (unsigned long)CF_CODEC_GET_SIZE(ph->pdec));
        ret = CF_SHORT_PDU_ERROR;
    }

    return ret;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_CFDP_RecvAck(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    CFE_Status_t ret = CFE_SUCCESS;

    CF_CFDP_DecodeAck(ph->pdec, &ph->int_header.ack);

    if (!CF_CODEC_IS_OK(ph->pdec))
    {
        CFE_EVS_SendEvent(CF_PDU_ACK_SHORT_ERR_EID, CFE_EVS_EventType_ERROR,
                          "CF: ACK PDU too short: %lu bytes received", (unsigned long)CF_CODEC_GET_SIZE(ph->pdec));
        ret = CF_SHORT_PDU_ERROR;
    }

    /* nothing to do for this one, as all fields are bytes */
    return ret;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_CFDP_RecvFin(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    CFE_Status_t ret = CFE_SUCCESS;

    CF_CFDP_DecodeFin(ph->pdec, &ph->int_header.fin);

    if (!CF_CODEC_IS_OK(ph->pdec))
    {
        CFE_EVS_SendEvent(CF_PDU_FIN_SHORT_ERR_EID, CFE_EVS_EventType_ERROR,
                          "CF: FIN PDU too short: %lu bytes received", (unsigned long)CF_CODEC_GET_SIZE(ph->pdec));
        ret = CF_SHORT_PDU_ERROR;
    }

    /* NOTE: right now we don't care about the fault location */
    /* nothing to do for this one. All fields are bytes */
    return ret;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_CFDP_RecvNak(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    CFE_Status_t ret = CFE_SUCCESS;

    CF_CFDP_DecodeNak(ph->pdec, &ph->int_header.nak);

    if (!CF_CODEC_IS_OK(ph->pdec))
    {
        CFE_EVS_SendEvent(CF_PDU_NAK_SHORT_ERR_EID, CFE_EVS_EventType_ERROR,
                          "CF: NAK PDU too short: %lu bytes received", (unsigned long)CF_CODEC_GET_SIZE(ph->pdec));
        ret = CF_SHORT_PDU_ERROR;
    }

    return ret;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_RecvDrop(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.recv.dropped;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_RecvHold(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    /* anything received in this state is considered spurious */
    ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.recv.spurious;

    /*
     * Normally we do not expect PDUs for a transaction in holdover, because
     * from the local point of view it is completed and done.  But the reason
     * for the holdover is because the remote side might not have gotten all
     * the acks and could still be [re-]sending us PDUs for anything it does
     * not know we got already.
     *
     * If an R2 sent FIN, it's possible that the peer missed the
     * FIN-ACK and is sending another FIN. In that case we need to send
     * another ACK.
     */

    /* currently the only thing we will re-ack is the FIN. */
    if (ph->fdirective.directive_code == CF_CFDP_FileDirective_FIN)
    {
        if (!CF_CFDP_RecvFin(txn, ph))
        {
            CF_CFDP_SendAck(txn, CF_CFDP_AckTxnStatus_TERMINATED, CF_CFDP_FileDirective_FIN, ph->int_header.fin.cc,
                            ph->pdu_header.destination_eid, ph->pdu_header.sequence_num);
        }
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_RecvInit(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    CF_Logical_PduFileDirectiveHeader_t *fdh;
    int                                  status;

    /* only RX transactions dare tread here */
    txn->history->seq_num = ph->pdu_header.sequence_num;

    /* peer_eid is always the remote partner. src_eid is always the transaction source.
     * in this case, they are the same */
    txn->history->peer_eid = ph->pdu_header.source_eid;
    txn->history->src_eid  = ph->pdu_header.source_eid;

    /* all RX transactions will need a chunk list to track file segments */
    if (txn->chunks == NULL)
    {
        txn->chunks = CF_CFDP_FindUnusedChunks(CF_GetChannelFromTxn(txn), CF_Direction_RX);
    }
    if (txn->chunks == NULL)
    {
        CFE_EVS_SendEvent(CF_CFDP_NO_CHUNKLIST_AVAIL_EID, CFE_EVS_EventType_ERROR,
                          "CF: cannot get chunklist -- abandoning transaction %u\n",
                          (unsigned int)ph->pdu_header.sequence_num);
    }
    else if (ph->pdu_header.pdu_type)
    {
        /* file data PDU */
        /* being idle and receiving a file data PDU means that no active transaction knew
         * about the transaction in progress, so most likely PDUs were missed. */

        /* if class 2, switch into R2 state and let it handle */
        /* don't forget to bind the transaction */
        if (ph->pdu_header.txm_mode)
        {
            /* R1, can't do anything without metadata first */
            txn->state = CF_TxnState_DROP; /* drop all incoming */
            /* use inactivity timer to ultimately free the state */
        }
        else
        {
            /* R2 can handle missing metadata, so go ahead and create a temp file */
            txn->state = CF_TxnState_R2;
            CF_CFDP_R_Init(txn);
            CF_CFDP_DispatchRecv(txn, ph); /* re-dispatch to enter r2 */
        }
    }
    else
    {
        fdh = &ph->fdirective;

        /* file directive PDU, but we are in an idle state. It only makes sense right now to accept metadata PDU. */
        switch (fdh->directive_code)
        {
            case CF_CFDP_FileDirective_METADATA:
                status = CF_CFDP_RecvMd(txn, ph);
                if (!status)
                {
                    /* NOTE: whether or not class 1 or 2, get a free chunks. It's cheap, and simplifies cleanup path */
                    txn->state            = ph->pdu_header.txm_mode ? CF_TxnState_R1 : CF_TxnState_R2;
                    txn->flags.rx.md_recv = true;
                    CF_CFDP_R_Init(txn); /* initialize R */
                }
                else
                {
                    CFE_EVS_SendEvent(CF_CFDP_IDLE_MD_ERR_EID, CFE_EVS_EventType_ERROR,
                                      "CF: got invalid md PDU -- abandoning transaction");
                    ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.recv.error;
                    /* leave state as idle, which will reset below */
                }
                break;
            default:
                CFE_EVS_SendEvent(CF_CFDP_FD_UNHANDLED_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "CF: unhandled file directive code 0x%02x in idle state", fdh->directive_code);
                ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.recv.error;
                break;
        }
    }

    if (txn->state == CF_TxnState_INIT)
    {
        /* state was not changed, so free the transaction */
        CF_CFDP_FinishTransaction(txn, false);
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_CFDP_InitEngine(void)
{
    /* initialize all transaction nodes */
    CF_History_t *     history;
    CF_Transaction_t * txn = CF_AppData.engine.transactions;
    CF_ChunkWrapper_t *cw  = CF_AppData.engine.chunks;
    CF_CListNode_t **  list_head;
    CFE_Status_t       ret              = CFE_SUCCESS;
    int                chunk_mem_offset = 0;
    int                i;
    int                j;
    int                k;
    char               nbuf[64];

    static const int CF_DIR_MAX_CHUNKS[CF_Direction_NUM][CF_NUM_CHANNELS] = {CF_CHANNEL_NUM_RX_CHUNKS_PER_TRANSACTION,
                                                                             CF_CHANNEL_NUM_TX_CHUNKS_PER_TRANSACTION};

    memset(&CF_AppData.engine, 0, sizeof(CF_AppData.engine));

    for (i = 0; i < CF_NUM_CHANNELS; ++i)
    {
        snprintf(nbuf, sizeof(nbuf) - 1, "%s%d", CF_CHANNEL_PIPE_PREFIX, i);
        ret = CFE_SB_CreatePipe(&CF_AppData.engine.channels[i].pipe, CF_AppData.config_table->chan[i].pipe_depth_input,
                                nbuf);
        if (ret != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(CF_CR_CHANNEL_PIPE_ERR_EID, CFE_EVS_EventType_ERROR,
                              "CF: failed to create pipe %s, returned 0x%08lx", nbuf, (unsigned long)ret);
            break;
        }

        ret = CFE_SB_SubscribeLocal(CFE_SB_ValueToMsgId(CF_AppData.config_table->chan[i].mid_input),
                                    CF_AppData.engine.channels[i].pipe,
                                    CF_AppData.config_table->chan[i].pipe_depth_input);
        if (ret != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(CF_INIT_SUB_ERR_EID, CFE_EVS_EventType_ERROR,
                              "CF: failed to subscribe to MID 0x%lx, returned 0x%08lx",
                              (unsigned long)CF_AppData.config_table->chan[i].mid_input, (unsigned long)ret);
            break;
        }

        if (CF_AppData.config_table->chan[i].sem_name[0])
        {
            /*
             * There is a start up race condition because CFE starts all apps at the same time,
             * and if this sem is instantiated by another app, it may not be created yet.
             *
             * Therefore if OSAL returns OS_ERR_NAME_NOT_FOUND, assume this is what is going
             * on, delay a bit and try again.
             */
            ret = OS_ERR_NAME_NOT_FOUND;
            for (j = 0; j < CF_STARTUP_SEM_MAX_RETRIES; ++j)
            {
                ret = OS_CountSemGetIdByName(&CF_AppData.engine.channels[i].sem_id,
                                             CF_AppData.config_table->chan[i].sem_name);

                if (ret != OS_ERR_NAME_NOT_FOUND)
                {
                    break;
                }

                OS_TaskDelay(CF_STARTUP_SEM_TASK_DELAY);
            }

            if (ret != OS_SUCCESS)
            {
                CFE_EVS_SendEvent(CF_INIT_SEM_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "CF: failed to get sem id for name %s, error=%ld",
                                  CF_AppData.config_table->chan[i].sem_name, (long)ret);
                break;
            }
        }

        for (j = 0; j < CF_NUM_TRANSACTIONS_PER_CHANNEL; ++j, ++txn)
        {
            /* Initially put this on the free list for this channel */
            CF_FreeTransaction(txn, i);

            for (k = 0; k < CF_Direction_NUM; ++k, ++cw)
            {
                list_head = CF_GetChunkListHead(&CF_AppData.engine.channels[i], k);

                CF_Assert((chunk_mem_offset + CF_DIR_MAX_CHUNKS[k][i]) <= CF_NUM_CHUNKS_ALL_CHANNELS);
                CF_ChunkListInit(&cw->chunks, CF_DIR_MAX_CHUNKS[k][i], &CF_AppData.engine.chunk_mem[chunk_mem_offset]);
                chunk_mem_offset += CF_DIR_MAX_CHUNKS[k][i];
                CF_CList_InitNode(&cw->cl_node);
                CF_CList_InsertBack(list_head, &cw->cl_node);
            }
        }

        for (j = 0; j < CF_NUM_HISTORIES_PER_CHANNEL; ++j)
        {
            history = &CF_AppData.engine.histories[(i * CF_NUM_HISTORIES_PER_CHANNEL) + j];
            CF_CList_InitNode(&history->cl_node);
            CF_CList_InsertBack_Ex(&CF_AppData.engine.channels[i], CF_QueueIdx_HIST_FREE, &history->cl_node);
        }
    }

    if (ret == CFE_SUCCESS)
    {
        CF_AppData.engine.enabled = true;
    }

    return ret;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CF_CListTraverse_Status_t CF_CFDP_CycleTxFirstActive(CF_CListNode_t *node, void *context)
{
    CF_CFDP_CycleTx_args_t *  args = (CF_CFDP_CycleTx_args_t *)context;
    CF_Transaction_t *        txn  = container_of(node, CF_Transaction_t, cl_node);
    CF_CListTraverse_Status_t ret  = CF_CLIST_EXIT; /* default option is exit traversal */

    if (txn->flags.com.suspended)
    {
        ret = CF_CLIST_CONT; /* suspended, so move on to next */
    }
    else
    {
        CF_Assert(txn->flags.com.q_index == CF_QueueIdx_TXA); /* huh? */

        /* if no more messages, then chan->cur will be set.
         * If the transaction sent the last filedata PDU and EOF, it will move itself
         * off the active queue. Run until either of these occur. */
        while (!args->chan->cur && txn->flags.com.q_index == CF_QueueIdx_TXA)
        {
            CFE_ES_PerfLogEntry(CF_PERF_ID_PDUSENT(txn->chan_num));
            CF_CFDP_DispatchTx(txn);
            CFE_ES_PerfLogExit(CF_PERF_ID_PDUSENT(txn->chan_num));
        }

        args->ran_one = 1;
    }

    return ret;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_CycleTx(CF_Channel_t *chan)
{
    CF_Transaction_t *     txn;
    CF_CFDP_CycleTx_args_t args;
    uint8                  chan_num = (chan - CF_AppData.engine.channels);

    if (CF_AppData.config_table->chan[chan_num].dequeue_enabled)
    {
        args = (CF_CFDP_CycleTx_args_t) {chan, 0};

        /* loop through as long as there are pending transactions, and a message buffer to send their PDUs on */

        /* NOTE: tick processing is higher priority than sending new filedata PDUs, so only send however many
         * PDUs that can be sent once we get to here */
        if (!chan->cur)
        { /* don't enter if cur is set, since we need to pick up where we left off on tick processing next wakeup */

            while (true)
            {
                /* Attempt to run something on TXA */
                CF_CList_Traverse(chan->qs[CF_QueueIdx_TXA], CF_CFDP_CycleTxFirstActive, &args);

                /* Keep going until CF_QueueIdx_PEND is empty or something is run */
                if (args.ran_one || chan->qs[CF_QueueIdx_PEND] == NULL)
                {
                    break;
                }

                txn = container_of(chan->qs[CF_QueueIdx_PEND], CF_Transaction_t, cl_node);

                /* to be processed this needs a chunklist, get one now */
                if (txn->chunks == NULL)
                {
                    txn->chunks = CF_CFDP_FindUnusedChunks(chan, CF_Direction_TX);
                }
                if (txn->chunks == NULL)
                {
                    /* leave it pending, come back later */
                    /* it needs to wait until a chunklist is freed */
                    break;
                }

                CF_CFDP_ArmInactTimer(txn);
                CF_MoveTransaction(txn, CF_QueueIdx_TXA);
            }
        }

        /* in case the loop exited due to no message buffers, clear it and start from the top next time */
        chan->cur = NULL;
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CF_CListTraverse_Status_t CF_CFDP_DoTick(CF_CListNode_t *node, void *context)
{
    CF_CListTraverse_Status_t ret  = CF_CLIST_CONT; /* CF_CLIST_CONT means don't tick one, keep looking for cur */
    CF_CFDP_Tick_args_t *     args = (CF_CFDP_Tick_args_t *)context;
    CF_Transaction_t *        txn  = container_of(node, CF_Transaction_t, cl_node);
    if (!args->chan->cur || (args->chan->cur == txn))
    {
        /* found where we left off, so clear that and move on */
        args->chan->cur = NULL;
        if (!txn->flags.com.suspended)
        {
            args->fn(txn, &args->cont);
        }

        /* if args->chan->cur was set to not-NULL above, then exit early */
        /* NOTE: if channel is frozen, then tick processing won't have been entered.
         *     so there is no need to check it here */
        if (args->chan->cur)
        {
            ret              = CF_CLIST_EXIT;
            args->early_exit = true;
        }
    }

    return ret; /* don't tick one, keep looking for cur */
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_TickTransactions(CF_Channel_t *chan)
{
    bool reset = true;

    void (*fns[CF_TickType_NUM_TYPES])(CF_Transaction_t *, int *) = {CF_CFDP_R_Tick, CF_CFDP_S_Tick,
                                                                     CF_CFDP_S_Tick_Nak};
    int qs[CF_TickType_NUM_TYPES]                                 = {CF_QueueIdx_RX, CF_QueueIdx_TXW, CF_QueueIdx_TXW};

    CF_Assert(chan->tick_type < CF_TickType_NUM_TYPES);

    for (; chan->tick_type < CF_TickType_NUM_TYPES; ++chan->tick_type)
    {
        CF_CFDP_Tick_args_t args = {chan, fns[chan->tick_type], 0, 0};

        do
        {
            args.cont = 0;
            CF_CList_Traverse(chan->qs[qs[chan->tick_type]], CF_CFDP_DoTick, &args);
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
                if (chan->tick_type != CF_TickType_TXW_NAK)
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
        chan->tick_type = CF_TickType_RX; /* reset tick type */
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_InitTxnTxFile(CF_Transaction_t *txn, CF_CFDP_Class_t cfdp_class, uint8 keep, uint8 chan, uint8 priority)
{
    txn->chan_num = chan;
    txn->priority = priority;
    txn->keep     = keep;
    txn->state    = cfdp_class ? CF_TxnState_S2 : CF_TxnState_S1;
}

/*----------------------------------------------------------------
 *
 * Internal helper routine only, not part of API.
 *
 *-----------------------------------------------------------------*/
static void CF_CFDP_TxFile_Initiate(CF_Transaction_t *txn, CF_CFDP_Class_t cfdp_class, uint8 keep, uint8 chan,
                                    uint8 priority, CF_EntityId_t dest_id)
{
    CFE_EVS_SendEvent(CF_CFDP_S_START_SEND_INF_EID, CFE_EVS_EventType_INFORMATION,
                      "CF: start class %d tx of file %lu:%.*s -> %lu:%.*s", cfdp_class + 1,
                      (unsigned long)CF_AppData.config_table->local_eid, CF_FILENAME_MAX_LEN,
                      txn->history->fnames.src_filename, (unsigned long)dest_id, CF_FILENAME_MAX_LEN,
                      txn->history->fnames.dst_filename);

    CF_CFDP_InitTxnTxFile(txn, cfdp_class, keep, chan, priority);

    /* Increment sequence number for new transaction */
    ++CF_AppData.engine.seq_num;

    /* Capture info for history */
    txn->history->seq_num  = CF_AppData.engine.seq_num;
    txn->history->src_eid  = CF_AppData.config_table->local_eid;
    txn->history->peer_eid = dest_id;

    CF_InsertSortPrio(txn, CF_QueueIdx_PEND);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_CFDP_TxFile(const char *src_filename, const char *dst_filename, CF_CFDP_Class_t cfdp_class, uint8 keep,
                            uint8 chan_num, uint8 priority, CF_EntityId_t dest_id)
{
    CF_Transaction_t *txn;
    CF_Channel_t *    chan = &CF_AppData.engine.channels[chan_num];
    CF_Assert(chan_num < CF_NUM_CHANNELS);

    CFE_Status_t ret = CFE_SUCCESS;

    if (chan->num_cmd_tx < CF_MAX_COMMANDED_PLAYBACK_FILES_PER_CHAN)
    {
        txn = CF_FindUnusedTransaction(&CF_AppData.engine.channels[chan_num], CF_Direction_TX);
    }
    else
    {
        txn = NULL;
    }

    if (txn == NULL)
    {
        CFE_EVS_SendEvent(CF_CFDP_MAX_CMD_TX_ERR_EID, CFE_EVS_EventType_ERROR,
                          "CF: max number of commanded files reached");
        ret = CF_ERROR;
    }
    else
    {
        /* NOTE: the caller of this function ensures the provided src and dst filenames are NULL terminated */
        strncpy(txn->history->fnames.src_filename, src_filename, sizeof(txn->history->fnames.src_filename) - 1);
        txn->history->fnames.src_filename[sizeof(txn->history->fnames.src_filename) - 1] = 0;
        strncpy(txn->history->fnames.dst_filename, dst_filename, sizeof(txn->history->fnames.dst_filename) - 1);
        txn->history->fnames.dst_filename[sizeof(txn->history->fnames.dst_filename) - 1] = 0;
        CF_CFDP_TxFile_Initiate(txn, cfdp_class, keep, chan_num, priority, dest_id);

        ++chan->num_cmd_tx;
        txn->flags.tx.cmd_tx = true;
    }

    return ret;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CF_Transaction_t *CF_CFDP_StartRxTransaction(uint8 chan_num)
{
    CF_Channel_t *    chan = &CF_AppData.engine.channels[chan_num];
    CF_Transaction_t *txn;

    if (CF_AppData.hk.Payload.channel_hk[chan_num].q_size[CF_QueueIdx_RX] < CF_MAX_SIMULTANEOUS_RX)
    {
        txn = CF_FindUnusedTransaction(chan, CF_Direction_RX);
    }
    else
    {
        txn = NULL;
    }

    if (txn != NULL)
    {
        /* set default FIN status */
        txn->state_data.receive.r2.dc = CF_CFDP_FinDeliveryCode_INCOMPLETE;
        txn->state_data.receive.r2.fs = CF_CFDP_FinFileStatus_DISCARDED;

        txn->flags.com.q_index = CF_QueueIdx_RX;
        CF_CList_InsertBack_Ex(chan, txn->flags.com.q_index, &txn->cl_node);
    }

    return txn;
}

/*----------------------------------------------------------------
 *
 * Internal helper routine only, not part of API.
 *
 *-----------------------------------------------------------------*/
static CFE_Status_t CF_CFDP_PlaybackDir_Initiate(CF_Playback_t *pb, const char *src_filename, const char *dst_filename,
                                                 CF_CFDP_Class_t cfdp_class, uint8 keep, uint8 chan, uint8 priority,
                                                 CF_EntityId_t dest_id)
{
    CFE_Status_t ret;

    /* make sure the directory can be open */
    ret = OS_DirectoryOpen(&pb->dir_id, src_filename);
    if (ret != OS_SUCCESS)
    {
        CFE_EVS_SendEvent(CF_CFDP_OPENDIR_ERR_EID, CFE_EVS_EventType_ERROR,
                          "CF: failed to open playback directory %s, error=%ld", src_filename, (long)ret);
        ++CF_AppData.hk.Payload.channel_hk[chan].counters.fault.directory_read;
    }
    else
    {
        pb->diropen    = true;
        pb->busy       = true;
        pb->keep       = keep;
        pb->priority   = priority;
        pb->dest_id    = dest_id;
        pb->cfdp_class = cfdp_class;

        /* NOTE: the caller of this function ensures the provided src and dst filenames are NULL terminated */
        strncpy(pb->fnames.src_filename, src_filename, sizeof(pb->fnames.src_filename) - 1);
        pb->fnames.src_filename[sizeof(pb->fnames.src_filename) - 1] = 0;
        strncpy(pb->fnames.dst_filename, dst_filename, sizeof(pb->fnames.dst_filename) - 1);
        pb->fnames.dst_filename[sizeof(pb->fnames.dst_filename) - 1] = 0;
    }

    /* the executor will start the transfer next cycle */
    return ret;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_CFDP_PlaybackDir(const char *src_filename, const char *dst_filename, CF_CFDP_Class_t cfdp_class,
                                 uint8 keep, uint8 chan, uint8 priority, uint16 dest_id)
{
    int            i;
    CF_Playback_t *pb;

    for (i = 0; i < CF_MAX_COMMANDED_PLAYBACK_DIRECTORIES_PER_CHAN; ++i)
    {
        pb = &CF_AppData.engine.channels[chan].playback[i];
        if (!pb->busy)
        {
            break;
        }
    }

    if (i == CF_MAX_COMMANDED_PLAYBACK_DIRECTORIES_PER_CHAN)
    {
        CFE_EVS_SendEvent(CF_CFDP_DIR_SLOT_ERR_EID, CFE_EVS_EventType_ERROR, "CF: no playback dir slot available");
        return CF_ERROR;
    }

    return CF_CFDP_PlaybackDir_Initiate(pb, src_filename, dst_filename, cfdp_class, keep, chan, priority, dest_id);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_ProcessPlaybackDirectory(CF_Channel_t *chan, CF_Playback_t *pb)
{
    CF_Transaction_t *txn;
    os_dirent_t       dirent;
    int32             status;

    /* either there's no transaction (first one) or the last one was finished, so check for a new one */

    memset(&dirent, 0, sizeof(dirent));

    while (pb->diropen && (pb->num_ts < CF_NUM_TRANSACTIONS_PER_PLAYBACK))
    {
        if (pb->pending_file[0] == 0)
        {
            CFE_ES_PerfLogEntry(CF_PERF_ID_DIRREAD);
            status = OS_DirectoryRead(pb->dir_id, &dirent);
            CFE_ES_PerfLogExit(CF_PERF_ID_DIRREAD);

            if (status != OS_SUCCESS)
            {
                /* PFTO: can we figure out the difference between "end of dir" and an error? */
                OS_DirectoryClose(pb->dir_id);
                pb->diropen = false;
                break;
            }

            if (!strcmp(dirent.FileName, ".") || !strcmp(dirent.FileName, ".."))
            {
                continue;
            }

            strncpy(pb->pending_file, OS_DIRENTRY_NAME(dirent), sizeof(pb->pending_file) - 1);
            pb->pending_file[sizeof(pb->pending_file) - 1] = 0;
        }
        else
        {
            txn = CF_FindUnusedTransaction(chan, CF_Direction_TX);
            if (txn == NULL)
            {
                /* while not expected this can certainly happen, because
                 * rx transactions consume in these as well. */
                /* should not need to do anything special, will come back next tick */
                break;
            }

            snprintf(txn->history->fnames.src_filename, sizeof(txn->history->fnames.src_filename), "%.*s/%.*s",
                     CF_FILENAME_MAX_PATH - 1, pb->fnames.src_filename, CF_FILENAME_MAX_NAME - 1, pb->pending_file);
            snprintf(txn->history->fnames.dst_filename, sizeof(txn->history->fnames.dst_filename), "%.*s/%.*s",
                     CF_FILENAME_MAX_PATH - 1, pb->fnames.dst_filename, CF_FILENAME_MAX_NAME - 1, pb->pending_file);

            CF_CFDP_TxFile_Initiate(txn, pb->cfdp_class, pb->keep, (chan - CF_AppData.engine.channels), pb->priority,
                                    pb->dest_id);

            txn->pb = pb;
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

/*----------------------------------------------------------------
 *
 * Internal helper routine only, not part of API.
 *
 *-----------------------------------------------------------------*/
static void CF_CFDP_UpdatePollPbCounted(CF_Playback_t *pb, int up, uint8 *counter)
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
            CF_Assert(*counter); /* sanity check it isn't zero */
            --*counter;
        }
    }
}

/*----------------------------------------------------------------
 *
 * Internal helper routine only, not part of API.
 *
 *-----------------------------------------------------------------*/
static void CF_CFDP_ProcessPlaybackDirectories(CF_Channel_t *chan)
{
    int       i;
    const int chan_index = (chan - CF_AppData.engine.channels);

    for (i = 0; i < CF_MAX_COMMANDED_PLAYBACK_DIRECTORIES_PER_CHAN; ++i)
    {
        CF_CFDP_ProcessPlaybackDirectory(chan, &chan->playback[i]);
        CF_CFDP_UpdatePollPbCounted(&chan->playback[i], chan->playback[i].busy,
                                    &CF_AppData.hk.Payload.channel_hk[chan_index].playback_counter);
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_ProcessPollingDirectories(CF_Channel_t *chan)
{
    CF_Poll_t *         poll;
    CF_ChannelConfig_t *cc;
    CF_PollDir_t *      pd;
    int                 i;
    int                 chan_index;
    int                 count_check;
    int                 ret;

    for (i = 0; i < CF_MAX_POLLING_DIR_PER_CHAN; ++i)
    {
        poll        = &chan->poll[i];
        chan_index  = (chan - CF_AppData.engine.channels);
        cc          = &CF_AppData.config_table->chan[chan_index];
        pd          = &cc->polldir[i];
        count_check = 0;

        if (pd->enabled)
        {
            if (!poll->pb.busy && !poll->pb.num_ts)
            {
                if (!poll->timer_set && pd->interval_sec)
                {
                    /* timer was not set, so set it now */
                    CF_Timer_InitRelSec(&poll->interval_timer, pd->interval_sec);
                    poll->timer_set = true;
                }
                else if (CF_Timer_Expired(&poll->interval_timer))
                {
                    /* the timer has expired */
                    ret = CF_CFDP_PlaybackDir_Initiate(&poll->pb, pd->src_dir, pd->dst_dir, pd->cfdp_class, 0,
                                                       chan_index, pd->priority, pd->dest_eid);
                    if (!ret)
                    {
                        poll->timer_set = false;
                    }
                    else
                    {
                        /* error occurred in playback directory, so reset the timer */
                        /* an event is sent in CF_CFDP_PlaybackDir_Initiate so there is no reason to
                         * to have another here */
                        CF_Timer_InitRelSec(&poll->interval_timer, pd->interval_sec);
                    }
                }
                else
                {
                    CF_Timer_Tick(&poll->interval_timer);
                }
            }
            else
            {
                /* playback is active, so step it */
                CF_CFDP_ProcessPlaybackDirectory(chan, &poll->pb);
            }

            count_check = 1;
        }

        CF_CFDP_UpdatePollPbCounted(&poll->pb, count_check, &CF_AppData.hk.Payload.channel_hk[chan_index].poll_counter);
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_CycleEngine(void)
{
    CF_Channel_t *chan;
    int           i;

    if (CF_AppData.engine.enabled)
    {
        for (i = 0; i < CF_NUM_CHANNELS; ++i)
        {
            chan                               = &CF_AppData.engine.channels[i];
            CF_AppData.engine.outgoing_counter = 0;

            /* consume all received messages, even if channel is frozen */
            CF_CFDP_ReceiveMessage(chan);

            if (!CF_AppData.hk.Payload.channel_hk[i].frozen)
            {
                /* handle ticks before tx cycle. Do this because there may be a limited number of TX messages available
                 * this cycle, and it's important to respond to class 2 ACK/NAK more than it is to send new filedata
                 * PDUs. */

                /* cycle all transactions (tick) */
                CF_CFDP_TickTransactions(chan);

                /* cycle the current tx transaction */
                CF_CFDP_CycleTx(chan);

                CF_CFDP_ProcessPlaybackDirectories(chan);
                CF_CFDP_ProcessPollingDirectories(chan);
            }
        }
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_FinishTransaction(CF_Transaction_t *txn, bool keep_history)
{
    CF_Channel_t *chan;

    if (txn->flags.com.q_index == CF_QueueIdx_FREE)
    {
        CFE_EVS_SendEvent(CF_RESET_FREED_XACT_DBG_EID, CFE_EVS_EventType_DEBUG,
                          "CF: attempt to reset a transaction that has already been freed");
        return;
    }

    chan = CF_GetChannelFromTxn(txn);

    /* this should always be */
    CF_Assert(chan != NULL);

    /* If this was on the TXA queue (transmit side) then we need to move it out
     * so the tick processor will stop trying to actively transmit something -
     * it should move on to the next transaction.
     *
     * RX transactions can stay on the RX queue, that does not hurt anything
     * because they are only triggered when a PDU comes in matching that seq_num
     * (RX queue is not separated into A/W parts) */
    if (txn->flags.com.q_index == CF_QueueIdx_TXA)
    {
        CF_DequeueTransaction(txn);
        CF_InsertSortPrio(txn, CF_QueueIdx_TXW);
    }

    if (OS_ObjectIdDefined(txn->fd))
    {
        CF_WrappedClose(txn->fd);

        if (!txn->keep)
        {
            CF_CFDP_HandleNotKeepFile(txn);
        }

        txn->fd = OS_OBJECT_ID_UNDEFINED;
    }

    if (txn->history != NULL)
    {
        CF_CFDP_SendEotPkt(txn);

        /* extra bookkeeping for tx direction only */
        if (txn->history->dir == CF_Direction_TX && txn->flags.tx.cmd_tx)
        {
            CF_Assert(chan->num_cmd_tx); /* sanity check */

            --chan->num_cmd_tx;
        }

        txn->flags.com.keep_history = keep_history;
    }

    if (txn->pb)
    {
        /* a playback's transaction is now done, decrement the playback counter */
        CF_Assert(txn->pb->num_ts);
        --txn->pb->num_ts;
    }

    /* no need to come back to this txn */
    if (chan->cur == txn)
    {
        chan->cur = NULL;
    }

    /* Put this transaction into the holdover state, inactivity timer will recycle it */
    txn->state = CF_TxnState_HOLD;
    CF_CFDP_ArmInactTimer(txn);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_RecycleTransaction(CF_Transaction_t *txn)
{
    CF_Channel_t *   chan;
    CF_CListNode_t **chunklist_head;
    CF_QueueIdx_t    hist_destq;

    /* File should have been closed by the state machine, but if
     * it still hanging open at this point, close it now so its not leaked.
     * This is not normal/expected so log it if this happens. */
    if (OS_ObjectIdDefined(txn->fd))
    {
        CFE_ES_WriteToSysLog("%s(): Closing dangling file handle: %lu\n", __func__, OS_ObjectIdToInteger(txn->fd));
        OS_close(txn->fd);
        txn->fd = OS_OBJECT_ID_UNDEFINED;
    }

    CF_DequeueTransaction(txn); /* this makes it "float" (not in any queue) */

    chan = CF_GetChannelFromTxn(txn);

    /* this should always be */
    if (chan != NULL && txn->history != NULL)
    {
        if (txn->chunks != NULL)
        {
            chunklist_head = CF_GetChunkListHead(chan, txn->history->dir);
            if (chunklist_head != NULL)
            {
                CF_CList_InsertBack(chunklist_head, &txn->chunks->cl_node);
                txn->chunks = NULL;
            }
        }

        if (txn->flags.com.keep_history)
        {
            /* move transaction history to history queue */
            hist_destq = CF_QueueIdx_HIST;
        }
        else
        {
            hist_destq = CF_QueueIdx_HIST_FREE;
        }
        CF_CList_InsertBack_Ex(chan, hist_destq, &txn->history->cl_node);
        txn->history = NULL;
    }

    /* this wipes it and puts it back onto the list to be found by
     * CF_FindUnusedTransaction().  Need to preserve the chan_num
     * and keep it associated with this channel, though. */
    CF_FreeTransaction(txn, txn->chan_num);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_r.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_SetTxnStatus(CF_Transaction_t *txn, CF_TxnStatus_t txn_stat)
{
    if (!CF_TxnStatus_IsError(txn->history->txn_stat))
    {
        txn->history->txn_stat = txn_stat;
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_SendEotPkt(CF_Transaction_t *txn)
{
    CF_EotPacket_t * EotPktPtr;
    CFE_SB_Buffer_t *BufPtr;

    /*
    ** Get a Message block of memory and initialize it
    */
    BufPtr = CFE_SB_AllocateMessageBuffer(sizeof(*EotPktPtr));

    if (BufPtr != NULL)
    {
        EotPktPtr = (void *)BufPtr;

        CFE_MSG_Init(CFE_MSG_PTR(EotPktPtr->TelemetryHeader), CFE_SB_ValueToMsgId(CF_EOT_TLM_MID), sizeof(*EotPktPtr));

        EotPktPtr->Payload.channel    = txn->chan_num;
        EotPktPtr->Payload.direction  = txn->history->dir;
        EotPktPtr->Payload.fnames     = txn->history->fnames;
        EotPktPtr->Payload.state      = txn->state;
        EotPktPtr->Payload.txn_stat   = txn->history->txn_stat;
        EotPktPtr->Payload.src_eid    = txn->history->src_eid;
        EotPktPtr->Payload.peer_eid   = txn->history->peer_eid;
        EotPktPtr->Payload.seq_num    = txn->history->seq_num;
        EotPktPtr->Payload.fsize      = txn->fsize;
        EotPktPtr->Payload.crc_result = txn->crc.result;

        /*
        ** Timestamp and send eod of transaction telemetry
        */
        CFE_SB_TimeStampMsg(CFE_MSG_PTR(EotPktPtr->TelemetryHeader));
        CFE_SB_TransmitBuffer(BufPtr, true);
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
int CF_CFDP_CopyStringFromLV(char *buf, size_t buf_maxsz, const CF_Logical_Lv_t *src_lv)
{
    if (src_lv->length < buf_maxsz)
    {
        memcpy(buf, src_lv->data_ptr, src_lv->length);
        buf[src_lv->length] = 0;
        return src_lv->length;
    }

    /* ensure output is empty */
    buf[0] = 0;
    return CF_ERROR; /* invalid len in lv? */
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_CancelTransaction(CF_Transaction_t *txn)
{
    void (*fns[CF_Direction_NUM])(CF_Transaction_t *) = {
        [CF_Direction_RX] = CF_CFDP_R_Cancel, [CF_Direction_TX] = CF_CFDP_S_Cancel};

    if (!txn->flags.com.canceled)
    {
        txn->flags.com.canceled = true;
        CF_CFDP_SetTxnStatus(txn, CF_TxnStatus_CANCEL_REQUEST_RECEIVED);

        /* this should always be true, just confirming before indexing into array */
        if (txn->history->dir < CF_Direction_NUM)
        {
            fns[txn->history->dir](txn);
        }
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CF_CListTraverse_Status_t CF_CFDP_CloseFiles(CF_CListNode_t *node, void *context)
{
    CF_Transaction_t *txn = container_of(node, CF_Transaction_t, cl_node);
    if (OS_ObjectIdDefined(txn->fd))
    {
        CF_WrappedClose(txn->fd);
    }
    return CF_CLIST_CONT;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_DisableEngine(void)
{
    int                        i;
    int                        j;
    static const CF_QueueIdx_t CLOSE_QUEUES[] = {CF_QueueIdx_RX, CF_QueueIdx_TXA, CF_QueueIdx_TXW};
    CF_Channel_t *             chan;

    CF_AppData.engine.enabled = false;

    for (i = 0; i < CF_NUM_CHANNELS; ++i)
    {
        chan = &CF_AppData.engine.channels[i];

        /* first, close all active files */
        for (j = 0; j < (sizeof(CLOSE_QUEUES) / sizeof(CLOSE_QUEUES[0])); ++j)
        {
            CF_CList_Traverse(chan->qs[CLOSE_QUEUES[j]], CF_CFDP_CloseFiles, NULL);
        }

        /* any playback directories need to have their directory ids closed */
        for (j = 0; j < CF_MAX_COMMANDED_PLAYBACK_DIRECTORIES_PER_CHAN; ++j)
        {
            if (chan->playback[j].busy)
            {
                OS_DirectoryClose(chan->playback[j].dir_id);
            }
        }

        for (j = 0; j < CF_MAX_POLLING_DIR_PER_CHAN; ++j)
        {
            if (chan->poll[j].pb.busy)
            {
                OS_DirectoryClose(chan->poll[j].pb.dir_id);
            }
        }

        /* finally all queue counters must be reset */
        memset(&CF_AppData.hk.Payload.channel_hk[i].q_size, 0, sizeof(CF_AppData.hk.Payload.channel_hk[i].q_size));

        CFE_SB_DeletePipe(chan->pipe);
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
bool CF_CFDP_IsPollingDir(const char *src_file, uint8 chan_num)
{
    bool                return_code                  = false;
    char                src_dir[CF_FILENAME_MAX_LEN] = "\0";
    CF_ChannelConfig_t *cc;
    CF_PollDir_t *      pd;
    int                 i;

    char *last_slash = strrchr(src_file, '/');
    if (last_slash != NULL)
    {
        strncpy(src_dir, src_file, last_slash - src_file);
    }

    cc = &CF_AppData.config_table->chan[chan_num];
    for (i = 0; i < CF_MAX_POLLING_DIR_PER_CHAN; ++i)
    {
        pd = &cc->polldir[i];
        if (strcmp(src_dir, pd->src_dir) == 0)
        {
            return_code = true;
            break;
        }
    }

    return return_code;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_HandleNotKeepFile(CF_Transaction_t *txn)
{
    /* Sender */
    if (CF_CFDP_IsSender(txn))
    {
        if (!CF_TxnStatus_IsError(txn->history->txn_stat))
        {
            /* If move directory is defined attempt move */
            CF_CFDP_MoveFile(txn->history->fnames.src_filename, CF_AppData.config_table->chan[txn->chan_num].move_dir);
        }
        else
        {
            /* file inside an polling directory */
            if (CF_CFDP_IsPollingDir(txn->history->fnames.src_filename, txn->chan_num))
            {
                /* If fail directory is defined attempt move */
                CF_CFDP_MoveFile(txn->history->fnames.src_filename, CF_AppData.config_table->fail_dir);
            }
        }
    }
    /* Not Sender */
    else
    {
        OS_remove(txn->history->fnames.dst_filename);
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_MoveFile(const char *src, const char *dest_dir)
{
    osal_status_t status = OS_ERROR;
    char *        filename;
    char          destination[OS_MAX_PATH_LEN];
    int           dest_path_len;

    if (dest_dir[0] != 0)
    {
        filename = strrchr(src, '/');
        if (filename != NULL)
        {
            dest_path_len = snprintf(destination, sizeof(destination), "%s%s", dest_dir, filename);
            if (dest_path_len >= (int)sizeof(destination))
            {
                /* Mark character before zero terminator to indicate truncation */
                destination[sizeof(destination) - 2] = CF_FILENAME_TRUNCATED;

                /* Send event describing that the path would be truncated */
                CFE_EVS_SendEvent(CF_EID_INF_CFDP_BUF_EXCEED, CFE_EVS_EventType_INFORMATION,
                                  "CF: destination has been truncated to %s", destination);
            }
            status = OS_mv(src, destination);
        }
    }

    if (status != OS_SUCCESS)
    {
        OS_remove(src);
    }
}

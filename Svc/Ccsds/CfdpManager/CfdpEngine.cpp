// ======================================================================
// \title  CfdpEngine.cpp
// \brief  CFDP Engine implementation
//
// This file is a port of the cf_cfdp.c file from the
// NASA Core Flight System (cFS) CFDP (CF) Application,
// version 3.0.0, adapted for use within the F-Prime (F') framework.
//
// This file contains two sets of functions. The first is what is needed
// to deal with CFDP PDUs. Specifically validating them for correctness
// and ensuring the byte-order is correct for the target. The second
// is incoming and outgoing CFDP PDUs pass through here. All receive
// CFDP PDU logic is performed here and the data is passed to the
// R (rx) and S (tx) logic.
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

#include <string.h>

#include <Os/FileSystem.hpp>

#include <Svc/Ccsds/CfdpManager/CfdpEngine.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpChannel.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpManager.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpRx.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpTx.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpUtils.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpDispatch.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpLogicalPdu.hpp>

namespace Svc {
namespace Ccsds {

void CF_CFDP_EncodeStart(CF_EncoderState_t *penc, U8 *msgbuf, CF_Logical_PduBuffer_t *ph, size_t total_size)
{
    // TODO BPC: Current thought is to rework the encore to include a buffer reference
    /* Clear the PDU buffer structure to start */
    memset(ph, 0, sizeof(*ph));

    /* attach encoder object to PDU buffer which is inside in an Fw::Buffer */
    // TODO BPC: msgbuf should be passed in as the Fw::Buffer
    penc->base = msgbuf;
    ph->penc   = penc;

    CF_CFDP_CodecReset(&penc->codec_state, total_size);
}

void CF_CFDP_DecodeStart(CF_DecoderState_t *pdec, const U8 *msgbuf, CF_Logical_PduBuffer_t *ph, size_t total_size)
{
    /* Clear the PDU buffer structure to start */
    memset(ph, 0, sizeof(*ph));

    /* attach decoder object to PDU buffer which is inside in an Fw::Buffer */
    // TODO BPC: msgbuf should be passed in as the Fw::Buffer
    pdec->base = msgbuf;
    ph->pdec   = pdec;

    CF_CFDP_CodecReset(&pdec->codec_state, total_size);
}

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

CfdpEngine::CfdpEngine(CfdpManager* manager) :
    m_manager(manager),
    m_seqNum(0)
{
    // TODO BPC: Should we intialize CfdpEngine here or wait for the init() function?
    for (U8 i = 0; i < CF_NUM_CHANNELS; ++i)
    {
        m_channels[i] = nullptr;
    }
}

CfdpEngine::~CfdpEngine()
{
    for (U8 i = 0; i < CF_NUM_CHANNELS; ++i)
    {
        if (m_channels[i] != nullptr)
        {
            delete m_channels[i];
            m_channels[i] = nullptr;
        }
    }
}

// ----------------------------------------------------------------------
// Public interface methods
// ----------------------------------------------------------------------

CfdpStatus::T CfdpEngine::init()
{
    /* initialize all transaction nodes */
    CF_History_t * history;
    CF_Transaction_t * txn = this->m_transactions;
    CF_ChunkWrapper_t *cw  = this->m_chunks;
    CF_CListNode_t **  list_head;
    CfdpStatus::T ret = CfdpStatus::SUCCESS;
    U32 chunk_mem_offset = 0;
    U8 i;
    U32 j;
    U8 k;

    static const int CF_DIR_MAX_CHUNKS[CF_Direction_NUM][CF_NUM_CHANNELS] = {CF_CHANNEL_NUM_RX_CHUNKS_PER_TRANSACTION,
                                                                             CF_CHANNEL_NUM_TX_CHUNKS_PER_TRANSACTION};

    for (i = 0; i < CF_NUM_CHANNELS; ++i)
    {
        m_channels[i] = new CfdpChannel(this, i, this->m_manager);
        FW_ASSERT(m_channels[i] != nullptr);

        for (j = 0; j < CF_NUM_TRANSACTIONS_PER_CHANNEL; ++j, ++txn)
        {
            // TODO BPC: Add pointer to component in order to send output buffers
            txn->cfdpManager = this->m_manager;

            /* Initially put this on the free list for this channel */
            m_channels[i]->freeTransaction(txn);

            for (k = 0; k < CF_Direction_NUM; ++k, ++cw)
            {
                list_head = m_channels[i]->getChunkListHead(k);

                FW_ASSERT((chunk_mem_offset + CF_DIR_MAX_CHUNKS[k][i]) <= CF_NUM_CHUNKS_ALL_CHANNELS,
                          chunk_mem_offset, CF_DIR_MAX_CHUNKS[k][i], CF_NUM_CHUNKS_ALL_CHANNELS);
                CF_ChunkListInit(&cw->chunks, CF_DIR_MAX_CHUNKS[k][i], &this->m_chunkMem[chunk_mem_offset]);
                chunk_mem_offset += CF_DIR_MAX_CHUNKS[k][i];
                CF_CList_InitNode(&cw->cl_node);
                CF_CList_InsertBack(list_head, &cw->cl_node);
            }
        }

        for (j = 0; j < CF_NUM_HISTORIES_PER_CHANNEL; ++j)
        {
            history = &this->m_histories[(i * CF_NUM_HISTORIES_PER_CHANNEL) + j];
            CF_CList_InitNode(&history->cl_node);
            m_channels[i]->insertBackInQueue(CfdpQueueId::HIST_FREE, &history->cl_node);
        }
    }

    return ret;
}


void CfdpEngine::armAckTimer(CF_Transaction_t *txn)
{
    txn->ack_timer.setTimer(txn->cfdpManager->getAckTimerParam(txn->chan_num));
    txn->flags.com.ack_timer_armed = true;
}

inline CfdpClass::T CF_CFDP_GetClass(const CF_Transaction_t *txn)
{
    FW_ASSERT(txn->flags.com.q_index != CfdpQueueId::FREE, txn->flags.com.q_index);
    return txn->txn_class;
}

inline bool CF_CFDP_IsSender(CF_Transaction_t *txn)
{
    FW_ASSERT(txn->history);

    return (txn->history->dir == CF_Direction_TX);
}

void CfdpEngine::armInactTimer(CF_Transaction_t *txn)
{
    U32 timerDuration = 0;

    /* select timeout based on the state */
    if (CF_CFDP_GetTxnStatus(txn) == CF_CFDP_AckTxnStatus_ACTIVE)
    {
        /* in an active transaction, we expect traffic so use the normal inactivity timer */
        timerDuration = txn->cfdpManager->getInactivityTimerParam(txn->chan_num);
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
        timerDuration = txn->cfdpManager->getAckTimerParam(txn->chan_num) * 2;
    }

    txn->inactivity_timer.setTimer(timerDuration);
}

void CfdpEngine::dispatchRecv(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    // Dispatch based on transaction state
    switch (txn->state)
    {
        case CF_TxnState_INIT:
            this->recvInit(txn, ph);
            break;
        case CF_TxnState_R1:
            CF_CFDP_R1_Recv(txn, ph);
            break;
        case CF_TxnState_S1:
            CF_CFDP_S1_Recv(txn, ph);
            break;
        case CF_TxnState_R2:
            CF_CFDP_R2_Recv(txn, ph);
            break;
        case CF_TxnState_S2:
            CF_CFDP_S2_Recv(txn, ph);
            break;
        case CF_TxnState_DROP:
            this->recvDrop(txn, ph);
            break;
        case CF_TxnState_HOLD:
            this->recvHold(txn, ph);
            break;
        default:
            // Invalid or undefined state
            break;
    }

    this->armInactTimer(txn); /* whenever a packet was received by the other size, always arm its inactivity timer */
}

void CfdpEngine::dispatchTx(CF_Transaction_t *txn)
{
    static const CF_CFDP_TxnSendDispatchTable_t state_fns = {
        {
            nullptr, // CF_TxnState_UNDEF
            nullptr, // CF_TxnState_INIT
            nullptr, // CF_TxnState_R1
            CF_CFDP_S1_Tx, // CF_TxnState_S1
            nullptr, // CF_TxnState_R2
            CF_CFDP_S2_Tx, // CF_TxnState_S2
            nullptr, // CF_TxnState_DROP
            nullptr // CF_TxnState_HOLD
        }
    };

    CF_CFDP_TxStateDispatch(txn, &state_fns);
}

CF_ChunkWrapper_t *CF_CFDP_FindUnusedChunks(CF_Channel_t *chan, CF_Direction_t dir)
{
    CF_ChunkWrapper_t *ret = NULL;
    CF_CListNode_t* node;
    CF_CListNode_t ** chunklist_head;

    chunklist_head = CF_GetChunkListHead(chan, dir);

    /* this should never be null */
    FW_ASSERT(chunklist_head);

    if (*chunklist_head != NULL)
    {
        node = CF_CList_Pop(chunklist_head);
        if(node != NULL)
        {
            ret = container_of_cpp(node, &CF_ChunkWrapper_t::cl_node);
        }
    }

    return ret;
}

void CfdpEngine::setPduLength(CF_Logical_PduBuffer_t *ph)
{
    U16 final_pos;

    /* final position of the encoder state should reflect the entire PDU length */
    final_pos = static_cast<U16>(CF_CODEC_GET_POSITION(ph->penc));

    if (final_pos >= ph->pdu_header.header_encoded_length)
    {
        /* the value that goes into the packet is length _after_ header */
        ph->pdu_header.data_encoded_length = final_pos - ph->pdu_header.header_encoded_length;
    }

    CF_CFDP_EncodeHeaderFinalSize(ph->penc, &ph->pdu_header);
}

CF_Logical_PduBuffer_t * CfdpEngine::constructPduHeader(const CF_Transaction_t *txn, CF_CFDP_FileDirective_t directive_code,
                                                   CfdpEntityId src_eid, CfdpEntityId dst_eid, bool towards_sender,
                                                   CfdpTransactionSeq tsn, bool silent)
{
    /* directive_code == 0 if file data */
    CF_Logical_PduBuffer_t *ph = NULL;
    CF_Logical_PduHeader_t *hdr;
    U8* msgPtr = NULL;
    U8 eid_len;
    CfdpStatus::T status;
    CF_EncoderState *encoder = NULL;;

    FW_ASSERT(txn != NULL);
    FW_ASSERT(txn->chan != NULL);

    // This is where a message buffer is requested
    status = txn->cfdpManager->getPduBuffer(ph, msgPtr, encoder, *txn->chan, sizeof(CF_Logical_PduBuffer_t));
    if (status == CfdpStatus::SUCCESS)
    {
        FW_ASSERT(ph != NULL);
        FW_ASSERT(msgPtr != NULL);
        FW_ASSERT(encoder != NULL);

        // TODO BPC: This was previously called as part of CF_CFDP_MsgOutGet()
        // Call it here to attach the storage returned by cfdpGetMessageBuffer() to the encoder
        ph->penc = encoder;
        CF_CFDP_EncodeStart(ph->penc, msgPtr, ph, CF_MAX_PDU_SIZE);

        hdr = &ph->pdu_header;

        hdr->version   = 1;
        hdr->pdu_type  = (directive_code == 0);     /* set to '1' for file data PDU, '0' for a directive PDU */
        hdr->direction = (towards_sender != false); /* set to '1' for toward sender, '0' for toward receiver */
        hdr->txm_mode  = (CF_CFDP_GetClass(txn) == CfdpClass::CLASS_1); /* set to '1' for class 1 data, '0' for class 2 */

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
    else if (status == CfdpStatus::SEND_PDU_NO_BUF_AVAIL_ERROR)
    {
        if (!silent)
        {
            // TODO BPC: send event here
        }
    }

    return ph;
}

CfdpStatus::T CfdpEngine::sendMd(CF_Transaction_t *txn)
{
    CF_Logical_PduBuffer_t *ph =
        this->constructPduHeader(txn, CF_CFDP_FileDirective_METADATA, txn->cfdpManager->getLocalEidParam(),
                                   txn->history->peer_eid, 0, txn->history->seq_num, false);
    CF_Logical_PduMd_t *md;
    CfdpStatus::T sret = CfdpStatus::SUCCESS;

    if (!ph)
    {
        sret = CfdpStatus::SEND_PDU_NO_BUF_AVAIL_ERROR;
    }
    else
    {
        md = &ph->int_header.md;

        FW_ASSERT((txn->state == CF_TxnState_S1) || (txn->state == CF_TxnState_S2), txn->state);

        md->size = txn->fsize;

        /* Set closure requested flag based on transaction class */
        /* Class 1: closure not requested (0), Class 2: closure requested (1) */
        md->close_req = (txn->state == CF_TxnState_S2) ? 1 : 0;

        // Set checksum type
        // TODO BPC: Probably need to set this based on a config
        md->checksum_type = 0;

        /* at this point, need to append filenames into md packet */
        /* this does not actually copy here - that is done during encode */
        // TODO BPC: Convert these to Fw::String
        md->source_filename.length = static_cast<U8>(txn->history->fnames.src_filename.length());
        md->source_filename.data_ptr = txn->history->fnames.src_filename.toChar();
        md->dest_filename.length = static_cast<U8>(txn->history->fnames.dst_filename.length());
        md->dest_filename.data_ptr = txn->history->fnames.dst_filename.toChar();

        CF_CFDP_EncodeMd(ph->penc, md);
        this->setPduLength(ph);
        txn->cfdpManager->sendPduBuffer(txn->chan_num, ph, ph->penc->base);
    }

    return sret;
}

CfdpStatus::T CfdpEngine::sendFd(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    /* NOTE: SendFd does not need a call to CF_CFDP_MsgOutGet, as the caller already has it */
    CfdpStatus::T ret = CfdpStatus::SUCCESS;

    /* this should check if any encoding error occurred */

    /* update PDU length */
    this->setPduLength(ph);
    txn->cfdpManager->sendPduBuffer(txn->chan_num, ph, ph->penc->base);

    return ret;
}

void CfdpEngine::appendTlv(CF_Logical_TlvList_t *ptlv_list, CF_CFDP_TlvType_t tlv_type, CfdpEntityId local_eid)
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
            ptlv->data.eid = local_eid;
            ptlv->length   = CF_CFDP_GetValueEncodedSize(ptlv->data.eid);
        }
        else
        {
            ptlv->data.data_ptr = NULL;
            ptlv->length        = 0;
        }
    }
}

CfdpStatus::T CfdpEngine::sendEof(CF_Transaction_t *txn)
{
    CF_Logical_PduBuffer_t *ph =
        this->constructPduHeader(txn, CF_CFDP_FileDirective_EOF, txn->cfdpManager->getLocalEidParam(),
                                   txn->history->peer_eid, 0, txn->history->seq_num, false);
    CF_Logical_PduEof_t *eof;
    CfdpStatus::T         ret = CfdpStatus::SUCCESS;

    if (!ph)
    {
        ret = CfdpStatus::SEND_PDU_NO_BUF_AVAIL_ERROR;
    }
    else
    {
        eof = &ph->int_header.eof;

        eof->cc   = CF_TxnStatus_To_ConditionCode(txn->history->txn_stat);
        eof->crc  = txn->crc.getValue();
        eof->size = txn->fsize;

        if (eof->cc != CF_CFDP_ConditionCode_NO_ERROR)
        {
            this->appendTlv(&eof->tlv_list, CF_CFDP_TLV_TYPE_ENTITY_ID,  txn->cfdpManager->getLocalEidParam());
        }

        CF_CFDP_EncodeEof(ph->penc, eof);
        this->setPduLength(ph);
        txn->cfdpManager->sendPduBuffer(txn->chan_num, ph, ph->penc->base);
    }

    return ret;
}

CfdpStatus::T CfdpEngine::sendAck(CF_Transaction_t *txn, CF_CFDP_AckTxnStatus_t ts, CF_CFDP_FileDirective_t dir_code,
                             CF_CFDP_ConditionCode_t cc, CfdpEntityId peer_eid, CfdpTransactionSeq tsn)
{
    CF_Logical_PduBuffer_t *ph;
    CF_Logical_PduAck_t *   ack;
    CfdpStatus::T            ret = CfdpStatus::SUCCESS;
    CfdpEntityId           src_eid;
    CfdpEntityId           dst_eid;

    FW_ASSERT((dir_code == CF_CFDP_FileDirective_EOF) || (dir_code == CF_CFDP_FileDirective_FIN), dir_code);

    if (CF_CFDP_IsSender(txn))
    {
        src_eid = txn->cfdpManager->getLocalEidParam();
        dst_eid = peer_eid;
    }
    else
    {
        src_eid = peer_eid;
        dst_eid = txn->cfdpManager->getLocalEidParam();
    }

    ph = this->constructPduHeader(txn, CF_CFDP_FileDirective_ACK, src_eid, dst_eid,
                                    (dir_code == CF_CFDP_FileDirective_EOF), tsn, false);
    if (!ph)
    {
        ret = CfdpStatus::SEND_PDU_NO_BUF_AVAIL_ERROR;
    }
    else
    {
        ack = &ph->int_header.ack;

        ack->ack_directive_code = dir_code;
        ack->ack_subtype_code   = 1; /* looks like always 1 if not extended features */
        ack->cc                 = cc;
        ack->txn_status         = ts;

        CF_CFDP_EncodeAck(ph->penc, ack);
        this->setPduLength(ph);
        txn->cfdpManager->sendPduBuffer(txn->chan_num, ph, ph->penc->base);
    }

    return ret;
}

CfdpStatus::T CfdpEngine::sendFin(CF_Transaction_t *txn, CF_CFDP_FinDeliveryCode_t dc, CF_CFDP_FinFileStatus_t fs,
                             CF_CFDP_ConditionCode_t cc)
{
    CF_Logical_PduBuffer_t *ph =
        this->constructPduHeader(txn, CF_CFDP_FileDirective_FIN, txn->history->peer_eid,
                                   txn->cfdpManager->getLocalEidParam(), 1, txn->history->seq_num, false);
    CF_Logical_PduFin_t *fin;
    CfdpStatus::T         ret = CfdpStatus::SUCCESS;

    if (!ph)
    {
        ret = CfdpStatus::SEND_PDU_NO_BUF_AVAIL_ERROR;
    }
    else
    {
        fin = &ph->int_header.fin;

        fin->cc            = cc;
        fin->delivery_code = dc;
        fin->file_status   = fs;

        if (cc != CF_CFDP_ConditionCode_NO_ERROR)
        {
            this->appendTlv(&fin->tlv_list, CF_CFDP_TLV_TYPE_ENTITY_ID, txn->cfdpManager->getLocalEidParam());
        }

        CF_CFDP_EncodeFin(ph->penc, fin);
        this->setPduLength(ph);
        txn->cfdpManager->sendPduBuffer(txn->chan_num, ph, ph->penc->base);
    }

    return ret;
}

CfdpStatus::T CfdpEngine::sendNak(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    CF_Logical_PduNak_t *nak;
    CfdpStatus::T         ret = CfdpStatus::SUCCESS;

    if (!ph)
    {
        ret = CfdpStatus::SEND_PDU_NO_BUF_AVAIL_ERROR;
    }
    else
    {
        CfdpClass::T tx_class = CF_CFDP_GetClass(txn);
        FW_ASSERT(tx_class == CfdpClass::CLASS_2, tx_class);

        nak = &ph->int_header.nak;

        /*
         * NOTE: the caller should have already initialized all the fields.
         * This does not need to add anything more to the NAK here
         */

        CF_CFDP_EncodeNak(ph->penc, nak);
        this->setPduLength(ph);
        txn->cfdpManager->sendPduBuffer(txn->chan_num, ph, ph->penc->base);
    }

    return ret;
}

CfdpStatus::T CfdpEngine::recvPh(U8 chan_num, CF_Logical_PduBuffer_t *ph)
{
    CfdpStatus::T ret = CfdpStatus::SUCCESS;

    FW_ASSERT(chan_num < CF_NUM_CHANNELS, chan_num, CF_NUM_CHANNELS);
    /*
     * If the source eid, destination eid, or sequence number fields
     * are larger than the sizes configured in the cf platform config
     * file, then reject the PDU.
     */
    if (CF_CFDP_DecodeHeader(ph->pdec, &ph->pdu_header) != CfdpStatus::SUCCESS)
    {
        // CFE_EVS_SendEvent(CF_PDU_TRUNCATION_ERR_EID, CFE_EVS_EventType_ERROR,
        //                   "CF: PDU rejected due to EID/seq number field truncation");
        // ++CF_AppData.hk.Payload.channel_hk[chan_num].counters.recv.error;
        ret = CfdpStatus::ERROR;
    }
    /*
     * The "large file" flag is not supported by this implementation yet.
     * This means file sizes and offsets will be 64 bits, so codec routines
     * will need to be updated to understand this.  OSAL also doesn't support
     * 64-bit file access yet.
     */
    else if (CF_CODEC_IS_OK(ph->pdec) && ph->pdu_header.large_flag)
    {
        // CFE_EVS_SendEvent(CF_PDU_LARGE_FILE_ERR_EID, CFE_EVS_EventType_ERROR,
        //                   "CF: PDU with large file bit received (unsupported)");
        // ++CF_AppData.hk.Payload.channel_hk[chan_num].counters.recv.error;
        ret = CfdpStatus::ERROR;
    }
    else
    {
        if (CF_CODEC_IS_OK(ph->pdec) && ph->pdu_header.pdu_type == 0)
        {
            CF_CFDP_DecodeFileDirectiveHeader(ph->pdec, &ph->fdirective);
        }

        if (!CF_CODEC_IS_OK(ph->pdec))
        {
            // CFE_EVS_SendEvent(CF_PDU_SHORT_HEADER_ERR_EID, CFE_EVS_EventType_ERROR, "CF: PDU too short (%lu received)",
            //                   (unsigned long)CF_CODEC_GET_SIZE(ph->pdec));
            // ++CF_AppData.hk.Payload.channel_hk[chan_num].counters.recv.error;
            ret = CfdpStatus::SHORT_PDU_ERROR;
        }
        else
        {
            /* PDU is ok, so continue processing */
            // ++CF_AppData.hk.Payload.channel_hk[chan_num].counters.recv.pdu;
        }
    }

    return ret;
}

CfdpStatus::T CfdpEngine::recvMd(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    const CF_Logical_PduMd_t *md = &ph->int_header.md;
    CfdpStatus::T lvRet;
    CfdpStatus::T ret = CfdpStatus::SUCCESS;

    CF_CFDP_DecodeMd(ph->pdec, &ph->int_header.md);
    if (!CF_CODEC_IS_OK(ph->pdec))
    {
        // CFE_EVS_SendEvent(CF_PDU_MD_SHORT_ERR_EID, CFE_EVS_EventType_ERROR,
        //                   "CF: metadata packet too short: %lu bytes received",
        //                   (unsigned long)CF_CODEC_GET_SIZE(ph->pdec));
        // ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.recv.error;
        ret = CfdpStatus::PDU_METADATA_ERROR;
    }
    else
    {
        /* store the expected file size in transaction */
        txn->fsize = md->size;

        /*
         * store the filenames in transaction.
         *
         * NOTE: The "copyStringFromLV()" now knows that the data is supposed to be a C string,
         * and ensures that the output content is properly terminated, so this only needs to check that
         * it worked.
         */
        lvRet = this->copyStringFromLV(txn->history->fnames.src_filename, &md->source_filename);
        if (lvRet != CfdpStatus::SUCCESS)
        {
            // CFE_EVS_SendEvent(CF_PDU_INVALID_SRC_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
            //                   "CF: metadata PDU rejected due to invalid length in source filename of 0x%02x",
            //                   md->source_filename.length);
            // ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.recv.error;
            ret = CfdpStatus::PDU_METADATA_ERROR;
        }
        else
        {
            lvRet = this->copyStringFromLV(txn->history->fnames.dst_filename, &md->dest_filename);
            if (lvRet != CfdpStatus::SUCCESS)
            {
                // CFE_EVS_SendEvent(CF_PDU_INVALID_DST_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
                //                   "CF: metadata PDU rejected due to invalid length in dest filename of 0x%02x",
                //                   md->dest_filename.length);
                // ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.recv.error;
                ret = CfdpStatus::PDU_METADATA_ERROR;
            }
            else
            {
                // CFE_EVS_SendEvent(CF_PDU_MD_RECVD_INF_EID, CFE_EVS_EventType_INFORMATION,
                //                   "CF: md received for source: %s, dest: %s", txn->history->fnames.src_filename,
                //                   txn->history->fnames.dst_filename);
            }
        }
    }

    return ret;
}

CfdpStatus::T CfdpEngine::recvFd(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    CfdpStatus::T ret = CfdpStatus::SUCCESS;

    CF_CFDP_DecodeFileDataHeader(ph->pdec, ph->pdu_header.segment_meta_flag, &ph->int_header.fd);

    /* if the CRC flag is set, need to deduct the size of the CRC from the data - always 32 bits */
    if (CF_CODEC_IS_OK(ph->pdec) && ph->pdu_header.crc_flag)
    {
        if (ph->int_header.fd.data_len < sizeof(CF_CFDP_U32_t))
        {
            CF_CODEC_SET_DONE(ph->pdec);
        }
        else
        {
            ph->int_header.fd.data_len -= sizeof(CF_CFDP_U32_t);
        }
    }

    if (!CF_CODEC_IS_OK(ph->pdec))
    {
        // CFE_EVS_SendEvent(CF_PDU_FD_SHORT_ERR_EID, CFE_EVS_EventType_ERROR,
        //                   "CF: filedata PDU too short: %lu bytes received", (unsigned long)CF_CODEC_GET_SIZE(ph->pdec));
        this->setTxnStatus(txn, CF_TxnStatus_PROTOCOL_ERROR);
        // ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.recv.error;
        ret = CfdpStatus::SHORT_PDU_ERROR;
    }
    else if (ph->pdu_header.segment_meta_flag)
    {
        /* If recv PDU has the "segment_meta_flag" set, this is not currently handled in CF. */
        // CFE_EVS_SendEvent(CF_PDU_FD_UNSUPPORTED_ERR_EID, CFE_EVS_EventType_ERROR,
        //                   "CF: filedata PDU with segment metadata received");
        this->setTxnStatus(txn, CF_TxnStatus_PROTOCOL_ERROR);
        // ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.recv.error;
        ret = CfdpStatus::ERROR;
    }

    return ret;
}

CfdpStatus::T CfdpEngine::recvEof(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    CfdpStatus::T ret = CfdpStatus::SUCCESS;

    CF_CFDP_DecodeEof(ph->pdec, &ph->int_header.eof);

    if (!CF_CODEC_IS_OK(ph->pdec))
    {
        // CFE_EVS_SendEvent(CF_PDU_EOF_SHORT_ERR_EID, CFE_EVS_EventType_ERROR,
        //                   "CF: EOF PDU too short: %lu bytes received", (unsigned long)CF_CODEC_GET_SIZE(ph->pdec));
        ret = CfdpStatus::SHORT_PDU_ERROR;
    }

    return ret;
}

CfdpStatus::T CfdpEngine::recvAck(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    CfdpStatus::T ret = CfdpStatus::SUCCESS;

    CF_CFDP_DecodeAck(ph->pdec, &ph->int_header.ack);

    if (!CF_CODEC_IS_OK(ph->pdec))
    {
        // CFE_EVS_SendEvent(CF_PDU_ACK_SHORT_ERR_EID, CFE_EVS_EventType_ERROR,
        //                   "CF: ACK PDU too short: %lu bytes received", (unsigned long)CF_CODEC_GET_SIZE(ph->pdec));
        ret = CfdpStatus::SHORT_PDU_ERROR;
    }

    /* nothing to do for this one, as all fields are bytes */
    return ret;
}

CfdpStatus::T CfdpEngine::recvFin(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    CfdpStatus::T ret = CfdpStatus::SUCCESS;

    CF_CFDP_DecodeFin(ph->pdec, &ph->int_header.fin);

    if (!CF_CODEC_IS_OK(ph->pdec))
    {
        // CFE_EVS_SendEvent(CF_PDU_FIN_SHORT_ERR_EID, CFE_EVS_EventType_ERROR,
        //                   "CF: FIN PDU too short: %lu bytes received", (unsigned long)CF_CODEC_GET_SIZE(ph->pdec));
        ret = CfdpStatus::SHORT_PDU_ERROR;
    }

    /* NOTE: right now we don't care about the fault location */
    /* nothing to do for this one. All fields are bytes */
    return ret;
}

CfdpStatus::T CfdpEngine::recvNak(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    CfdpStatus::T ret = CfdpStatus::SUCCESS;

    CF_CFDP_DecodeNak(ph->pdec, &ph->int_header.nak);

    if (!CF_CODEC_IS_OK(ph->pdec))
    {
        // CFE_EVS_SendEvent(CF_PDU_NAK_SHORT_ERR_EID, CFE_EVS_EventType_ERROR,
        //                   "CF: NAK PDU too short: %lu bytes received", (unsigned long)CF_CODEC_GET_SIZE(ph->pdec));
        ret = CfdpStatus::SHORT_PDU_ERROR;
    }

    return ret;
}

void CfdpEngine::recvDrop(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    // ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.recv.dropped;
}

void CfdpEngine::recvHold(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    /* anything received in this state is considered spurious */
    // ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.recv.spurious;

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
        if (!this->recvFin(txn, ph))
        {
            this->sendAck(txn, CF_CFDP_AckTxnStatus_TERMINATED, CF_CFDP_FileDirective_FIN, ph->int_header.fin.cc,
                            ph->pdu_header.destination_eid, ph->pdu_header.sequence_num);
        }
    }
}

void CfdpEngine::recvInit(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
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
        txn->chunks = txn->chan->findUnusedChunks(CF_Direction_RX);
    }
    if (txn->chunks == NULL)
    {
        // CFE_EVS_SendEvent(CF_CFDP_NO_CHUNKLIST_AVAIL_EID, CFE_EVS_EventType_ERROR,
        //                   "CF: cannot get chunklist -- abandoning transaction %u\n",
        //                  (unsigned int)ph->pdu_header.sequence_num);
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
            txn->txn_class = CfdpClass::CLASS_2;
            CF_CFDP_R_Init(txn);
            this->dispatchRecv(txn, ph); /* re-dispatch to enter r2 */
        }
    }
    else
    {
        fdh = &ph->fdirective;

        /* file directive PDU, but we are in an idle state. It only makes sense right now to accept metadata PDU. */
        switch (fdh->directive_code)
        {
            case CF_CFDP_FileDirective_METADATA:
                status = this->recvMd(txn, ph);
                if (!status)
                {
                    /* NOTE: whether or not class 1 or 2, get a free chunks. It's cheap, and simplifies cleanup path */
                    txn->state            = ph->pdu_header.txm_mode ? CF_TxnState_R1 : CF_TxnState_R2;
                    txn->txn_class        = ph->pdu_header.txm_mode ? CfdpClass::CLASS_1 : CfdpClass::CLASS_2;
                    txn->flags.rx.md_recv = true;
                    CF_CFDP_R_Init(txn); /* initialize R */
                }
                else
                {
                    // CFE_EVS_SendEvent(CF_CFDP_IDLE_MD_ERR_EID, CFE_EVS_EventType_ERROR,
                    //                   "CF: got invalid md PDU -- abandoning transaction");
                    // ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.recv.error;
                    /* leave state as idle, which will reset below */
                }
                break;
            default:
                // CFE_EVS_SendEvent(CF_CFDP_FD_UNHANDLED_ERR_EID, CFE_EVS_EventType_ERROR,
                //                   "CF: unhandled file directive code 0x%02x in idle state", fdh->directive_code);
                // ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.recv.error;
                break;
        }
    }

    if (txn->state == CF_TxnState_INIT)
    {
        /* state was not changed, so free the transaction */
        this->finishTransaction(txn, false);
    }
}

void CfdpEngine::receivePdu(U8 chan_id, CF_Logical_PduBuffer_t *ph)
{
    CF_Transaction_t *txn = NULL;
    CfdpChannel *chan = NULL;

    FW_ASSERT(chan_id < CF_NUM_CHANNELS, chan_id, CF_NUM_CHANNELS);
    FW_ASSERT(ph != NULL);

    chan = m_channels[chan_id];
    FW_ASSERT(chan != NULL);

    CfdpStatus::T recv_status = this->recvPh(chan_id, ph);
    if (recv_status == CfdpStatus::SUCCESS)
    {
        /* got a valid PDU -- look it up by sequence number */
        txn = chan->findTransactionBySequenceNumber(ph->pdu_header.sequence_num, ph->pdu_header.source_eid);

        if (txn == NULL)
        {
            /* if no match found, then it must be the case that we would be the destination entity id, so verify it
                */
            if (ph->pdu_header.destination_eid == this->m_manager->getLocalEidParam())
            {
                /* we didn't find a match, so assign it to a transaction */
                /* assume this is initiating an RX transaction, as TX transactions are only commanded */
                txn = this->startRxTransaction(chan->getChannelId());
                if (txn == NULL)
                {
                    // CFE_EVS_SendEvent(
                    //     CF_CFDP_RX_DROPPED_ERR_EID, CFE_EVS_EventType_ERROR,
                    //     "CF: dropping packet from %lu transaction number 0x%08lx due max RX transactions reached",
                    //     (unsigned long)ph->pdu_header.source_eid, (unsigned long)ph->pdu_header.sequence_num);
                }
            }
            else
            {
                // CFE_EVS_SendEvent(CF_CFDP_INVALID_DST_ERR_EID, CFE_EVS_EventType_ERROR,
                //                   "CF: dropping packet for invalid destination eid 0x%lx",
                //                   (unsigned long)ph->pdu_header.destination_eid);
            }
        }

        if (txn != NULL)
        {
            /* found one! Send it to the transaction state processor */
            this->dispatchRecv(txn, ph);
        }
        else
        {
            // TODO BPC: Add throttled EVR
        }
    }
}


void CfdpEngine::setChannelFlowState(U8 channelId, CfdpFlow::T flowState)
{
    FW_ASSERT(channelId <= CF_NUM_CHANNELS, channelId, CF_NUM_CHANNELS);
    m_channels[channelId]->setFlowState(flowState);
}

void CfdpEngine::initTxnTxFile(CF_Transaction_t *txn, CfdpClass::T cfdp_class, CfdpKeep::T keep, U8 chan, U8 priority)
{
    txn->chan_num = chan;
    txn->priority = priority;
    txn->keep = keep;
    txn->txn_class = cfdp_class;
    txn->state = (cfdp_class == CfdpClass::CLASS_2) ? CF_TxnState_S2 : CF_TxnState_S1;
    txn->state_data.send.sub_state = CF_TxSubState_METADATA;
}

void CfdpEngine::txFileInitiate(CF_Transaction_t *txn, CfdpClass::T cfdp_class, CfdpKeep::T keep, U8 chan,
                             U8 priority, CfdpEntityId dest_id)
{
    // CFE_EVS_SendEvent(CF_CFDP_S_START_SEND_INF_EID, CFE_EVS_EventType_INFORMATION,
    //                   "CF: start class %d tx of file %lu:%.*s -> %lu:%.*s", cfdp_class + 1,
    //                   (unsigned long)txn->cfdpManager->getLocalEidParam(), CF_FILENAME_MAX_LEN,
    //                   txn->history->fnames.src_filename, (unsigned long)dest_id, CF_FILENAME_MAX_LEN,
    //                   txn->history->fnames.dst_filename);

    this->initTxnTxFile(txn, cfdp_class, keep, chan, priority);

    /* Increment sequence number for new transaction */
    ++this->m_seqNum;

    /* Capture info for history */
    txn->history->seq_num  = this->m_seqNum;
    txn->history->src_eid  = txn->cfdpManager->getLocalEidParam();
    txn->history->peer_eid = dest_id;

    txn->chan->insertSortPrio(txn, CfdpQueueId::PEND);
}

CfdpStatus::T CfdpEngine::txFile(const Fw::String& src_filename, const Fw::String& dst_filename,
                             CfdpClass::T cfdp_class, CfdpKeep::T keep, U8 chan_num,
                             U8 priority, CfdpEntityId dest_id)
{
    CF_Transaction_t *txn;
    CfdpChannel* chan = nullptr;

    FW_ASSERT(chan_num < CF_NUM_CHANNELS, chan_num, CF_NUM_CHANNELS);
    chan = m_channels[chan_num];

    CfdpStatus::T ret = CfdpStatus::SUCCESS;

    if (chan->getNumCmdTx() < CF_MAX_COMMANDED_PLAYBACK_FILES_PER_CHAN)
    {
        txn = chan->findUnusedTransaction(CF_Direction_TX);
    }
    else
    {
        txn = NULL;
    }

    if (txn == NULL)
    {
        // CFE_EVS_SendEvent(CF_CFDP_MAX_CMD_TX_ERR_EID, CFE_EVS_EventType_ERROR,
        //                   "CF: max number of commanded files reached");
        ret = CfdpStatus::ERROR;
    }
    else
    {
        /* NOTE: the caller of this function ensures the provided src and dst filenames are NULL terminated */

        txn->history->fnames.src_filename = src_filename;
        txn->history->fnames.dst_filename = dst_filename;
        this->txFileInitiate(txn, cfdp_class, keep, chan_num, priority, dest_id);

        chan->incrementCmdTxCounter();
        txn->flags.tx.cmd_tx = true;
    }

    return ret;
}

CF_Transaction_t *CfdpEngine::startRxTransaction(U8 chan_num)
{
    CfdpChannel *chan = nullptr;
    CF_Transaction_t *txn;
    
    FW_ASSERT(chan_num < CF_NUM_CHANNELS, chan_num, CF_NUM_CHANNELS);
    chan = m_channels[chan_num];

    // if (CF_AppData.hk.Payload.channel_hk[chan_num].q_size[CfdpQueueId::RX] < CF_MAX_SIMULTANEOUS_RX)
    // {
    //     txn = chan->findUnusedTransaction(CF_Direction_RX);
    // }
    // else
    // {
    //     txn = NULL;
    // }
    // TODO BPC: Do I need to limit receive transactions?
    txn = chan->findUnusedTransaction(CF_Direction_RX);

    if (txn != NULL)
    {
        /* set default FIN status */
        txn->state_data.receive.r2.dc = CF_CFDP_FinDeliveryCode_INCOMPLETE;
        txn->state_data.receive.r2.fs = CF_CFDP_FinFileStatus_DISCARDED;

        txn->flags.com.q_index = CfdpQueueId::RX;
        chan->insertBackInQueue(static_cast<CfdpQueueId::T>(txn->flags.com.q_index), &txn->cl_node);
    }

    return txn;
}

CfdpStatus::T CfdpEngine::playbackDirInitiate(CF_Playback_t *pb, const Fw::String& src_filename, const Fw::String& dst_filename,
                                           CfdpClass::T cfdp_class, CfdpKeep::T keep, U8 chan, U8 priority,
                                           CfdpEntityId dest_id)
{
    CfdpStatus::T status = CfdpStatus::SUCCESS;
    Os::Directory::Status dirStatus;

    /* make sure the directory can be open */
    dirStatus = pb->dir.open(src_filename.toChar(), Os::Directory::READ);
    if (dirStatus != Os::Directory::OP_OK)
    {
        // CFE_EVS_SendEvent(CF_CFDP_OPENDIR_ERR_EID, CFE_EVS_EventType_ERROR,
        //                   "CF: failed to open playback directory %s, error=%ld", src_filename, (long)ret);
        // ++CF_AppData.hk.Payload.channel_hk[chan].counters.fault.directory_read;
        status = CfdpStatus::ERROR;
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
        pb->fnames.src_filename = src_filename;
        pb->fnames.dst_filename = dst_filename;
    }

    /* the executor will start the transfer next cycle */
    return status;
}

CfdpStatus::T CfdpEngine::playbackDir(const Fw::String& src_filename, const Fw::String& dst_filename, CfdpClass::T cfdp_class,
                                  CfdpKeep::T keep, U8 chan, U8 priority, CfdpEntityId dest_id)
{
    int i;
    CF_Playback_t *pb;

    // Loop through the channel's playback directories to find an open slot
    for (i = 0; i < CF_MAX_COMMANDED_PLAYBACK_DIRECTORIES_PER_CHAN; ++i)
    {
        pb = m_channels[chan]->getPlayback(i);
        if (!pb->busy)
        {
            break;
        }
    }

    if (i == CF_MAX_COMMANDED_PLAYBACK_DIRECTORIES_PER_CHAN)
    {
        // CFE_EVS_SendEvent(CF_CFDP_DIR_SLOT_ERR_EID, CFE_EVS_EventType_ERROR, "CF: no playback dir slot available");
        return CfdpStatus::ERROR;
    }
    else
    {
        return this->playbackDirInitiate(pb, src_filename, dst_filename, cfdp_class, keep, chan, priority, dest_id);
    }

}

CfdpStatus::T CfdpEngine::startPollDir(U8 chanId, U8 pollId, const Fw::String& srcDir, const Fw::String& dstDir,
                                     CfdpClass::T cfdp_class, U8 priority, CfdpEntityId destEid,
                                     U32 intervalSec)
{
    CfdpStatus::T status = CfdpStatus::SUCCESS;
    CF_PollDir_t* pd = NULL;

    FW_ASSERT(chanId < CF_NUM_CHANNELS, chanId, CF_NUM_CHANNELS);
    FW_ASSERT(pollId < CF_MAX_POLLING_DIR_PER_CHAN, pollId, CF_MAX_POLLING_DIR_PER_CHAN);

    // First check if the poll directory is already in use
    pd = m_channels[chanId]->getPollDir(pollId);
    if(pd->enabled == Fw::Enabled::DISABLED)
    {
        // Populate arguments
        pd->intervalSec = intervalSec;
        pd->priority = priority;
        pd->cfdpClass = cfdp_class;
        pd->destEid = destEid;
        pd->srcDir = srcDir;
        pd->dstDir = dstDir;
        
        // Set timer and enable polling
        pd->intervalTimer.setTimer(pd->intervalSec);
        pd->enabled = Fw::Enabled::ENABLED;
    }
    else
    {
        // TODO BPC: emit EVR here
        status = CfdpStatus::ERROR;
    }

    return status;
}

CfdpStatus::T CfdpEngine::stopPollDir(U8 chanId, U8 pollId)
{
    CfdpStatus::T status = CfdpStatus::SUCCESS;
    CF_PollDir_t* pd = NULL;

    FW_ASSERT(chanId < CF_NUM_CHANNELS, chanId, CF_NUM_CHANNELS);
    FW_ASSERT(pollId < CF_MAX_POLLING_DIR_PER_CHAN, pollId, CF_MAX_POLLING_DIR_PER_CHAN);

    // Check if the poll directory is in use
    pd = m_channels[chanId]->getPollDir(pollId);
    if(pd->enabled == Fw::Enabled::DISABLED)
    {
        // Clear poll directory arguments
        pd->intervalSec = 0;
        pd->priority = 0;
        pd->cfdpClass = static_cast<CfdpClass::T>(0);
        pd->destEid = static_cast<CfdpEntityId>(0);
        pd->srcDir = "";
        pd->dstDir = "";
        
        // Disable timer and polling
        pd->intervalTimer.disableTimer();
        pd->enabled = Fw::Enabled::DISABLED;
    }
    else
    {
        // TODO BPC: emit EVR here
        status = CfdpStatus::ERROR;
    }

    return status;
}

void CfdpEngine::cycle(void)
{
    int i;

    for (i = 0; i < CF_NUM_CHANNELS; ++i)
    {
        CfdpChannel* chan = m_channels[i];
        FW_ASSERT(chan != nullptr);

        chan->resetOutgoingCounter();

        if (chan->getFlowState() == CfdpFlow::NOT_FROZEN)
        {
            /* handle ticks before tx cycle. Do this because there may be a limited number of TX messages available
             * this cycle, and it's important to respond to class 2 ACK/NAK more than it is to send new filedata
             * PDUs. */

            /* cycle all transactions (tick) */
            chan->tickTransactions();

            /* cycle the current tx transaction */
            chan->cycleTx();

            chan->processPlaybackDirectories();
            chan->processPollingDirectories();
        }
    }
}

void CfdpEngine::finishTransaction(CF_Transaction_t *txn, bool keep_history)
{
    if (txn->flags.com.q_index == CfdpQueueId::FREE)
    {
        // CFE_EVS_SendEvent(CF_RESET_FREED_XACT_DBG_EID, CFE_EVS_EventType_DEBUG,
        //                   "CF: attempt to reset a transaction that has already been freed");
        return;
    }

    /* this should always be */
    FW_ASSERT(txn->chan != NULL);

    /* If this was on the TXA queue (transmit side) then we need to move it out
     * so the tick processor will stop trying to actively transmit something -
     * it should move on to the next transaction.
     *
     * RX transactions can stay on the RX queue, that does not hurt anything
     * because they are only triggered when a PDU comes in matching that seq_num
     * (RX queue is not separated into A/W parts) */
    if (txn->flags.com.q_index == CfdpQueueId::TXA)
    {
        txn->chan->dequeueTransaction(txn);
        txn->chan->insertSortPrio(txn, CfdpQueueId::TXW);
    }

    if (true == txn->fd.isOpen())
    {
        txn->fd.close();

        if (!txn->keep)
        {
            this->handleNotKeepFile(txn);
        }
    }

    if (txn->history != NULL)
    {
        this->sendEotPkt(txn);

        /* extra bookkeeping for tx direction only */
        if (txn->history->dir == CF_Direction_TX && txn->flags.tx.cmd_tx)
        {
            txn->chan->decrementCmdTxCounter();
        }

        txn->flags.com.keep_history = keep_history;
    }

    if (txn->pb)
    {
        /* a playback's transaction is now done, decrement the playback counter */
        FW_ASSERT(txn->pb->num_ts);
        --txn->pb->num_ts;
    }

    txn->chan->clearCurrentIfMatch(txn);

    /* Put this transaction into the holdover state, inactivity timer will recycle it */
    txn->state = CF_TxnState_HOLD;
    this->armInactTimer(txn);
}

void CfdpEngine::setTxnStatus(CF_Transaction_t *txn, CF_TxnStatus_t txn_stat)
{
    if (!CF_TxnStatus_IsError(txn->history->txn_stat))
    {
        txn->history->txn_stat = txn_stat;
    }
}

void CfdpEngine::sendEotPkt(CF_Transaction_t *txn)
{
    // TODO BPC: This is sending a telemetry packet at the end of a completed transaction
    // How do we want to handle this in F' telemetry?

    // CF_EotPacket_t * EotPktPtr;
    // CFE_SB_Buffer_t *BufPtr;

    // /*
    // ** Get a Message block of memory and initialize it
    // */
    // BufPtr = CFE_SB_AllocateMessageBuffer(sizeof(*EotPktPtr));

    // if (BufPtr != NULL)
    // {
    //     EotPktPtr = (void *)BufPtr;

    //     CFE_MSG_Init(CFE_MSG_PTR(EotPktPtr->TelemetryHeader), CFE_SB_ValueToMsgId(CF_EOT_TLM_MID), sizeof(*EotPktPtr));

    //     EotPktPtr->Payload.channel    = txn->chan_num;
    //     EotPktPtr->Payload.direction  = txn->history->dir;
    //     EotPktPtr->Payload.fnames     = txn->history->fnames;
    //     EotPktPtr->Payload.state      = txn->state;
    //     EotPktPtr->Payload.txn_stat   = txn->history->txn_stat;
    //     EotPktPtr->Payload.src_eid    = txn->history->src_eid;
    //     EotPktPtr->Payload.peer_eid   = txn->history->peer_eid;
    //     EotPktPtr->Payload.seq_num    = txn->history->seq_num;
    //     EotPktPtr->Payload.fsize      = txn->fsize;
    //     EotPktPtr->Payload.crc_result = txn->crc.getValue();

    //     /*
    //     ** Timestamp and send eod of transaction telemetry
    //     */
    //     CFE_SB_TimeStampMsg(CFE_MSG_PTR(EotPktPtr->TelemetryHeader));
    //     CFE_SB_TransmitBuffer(BufPtr, true);
    // }
}

CfdpStatus::T CfdpEngine::copyStringFromLV(Fw::String& out, const CF_Logical_Lv_t* src_lv)
{
    if (src_lv->length > 0)
    {
        // Determine max copy length based on string capacity
        const FwSizeType maxCopy =
            (src_lv->length < out.getCapacity() - 1) ? src_lv->length : out.getCapacity() - 1;

        char tmp[FileNameStringSize]; // Max size for CFDP file names
        std::memcpy(tmp, src_lv->data_ptr, maxCopy);
        tmp[maxCopy] = '\0';

        out = tmp;
        return CfdpStatus::SUCCESS;
    }

    // LV length is zero or invalid: clear the output
    out = "";
    return CfdpStatus::ERROR;
}

void CfdpEngine::cancelTransaction(CF_Transaction_t *txn)
{
    void (*fns[CF_Direction_NUM])(CF_Transaction_t*) = {nullptr};

    fns[CF_Direction_RX] = CF_CFDP_R_Cancel;
    fns[CF_Direction_TX] = CF_CFDP_S_Cancel;

    if (!txn->flags.com.canceled)
    {
        txn->flags.com.canceled = true;
        this->setTxnStatus(txn, CF_TxnStatus_CANCEL_REQUEST_RECEIVED);

        /* this should always be true, just confirming before indexing into array */
        if (txn->history->dir < CF_Direction_NUM)
        {
            fns[txn->history->dir](txn);
        }
    }
}

bool CfdpEngine::isPollingDir(const char *src_file, U8 chan_num)
{
    bool return_code = false;
    char src_dir[CF_FILENAME_MAX_LEN] = "\0";
    CF_PollDir_t * pd;
    int i;

    const char* last_slash = strrchr(src_file, '/');
    if (last_slash != NULL)
    {
        strncpy(src_dir, src_file, last_slash - src_file);
    }

    for (i = 0; i < CF_MAX_POLLING_DIR_PER_CHAN; ++i)
    {
        pd = m_channels[chan_num]->getPollDir(i);
        if (strcmp(src_dir, pd->srcDir.toChar()) == 0)
        {
            return_code = true;
            break;
        }
    }

    return return_code;
}

void CfdpEngine::handleNotKeepFile(CF_Transaction_t *txn)
{
    Os::FileSystem::Status fileStatus;
    Fw::String failDir;
    Fw::String moveDir;

    /* Sender */
    if (CF_CFDP_IsSender(txn))
    {
        if (!CF_TxnStatus_IsError(txn->history->txn_stat))
        {
            /* If move directory is defined attempt move */
            moveDir = txn->cfdpManager->getMoveDirParam(txn->chan_num);
            if(moveDir.length() > 0)
            {
                fileStatus = Os::FileSystem::moveFile(txn->history->fnames.src_filename.toChar(), moveDir.toChar());
                if(fileStatus != Os::FileSystem::OP_OK)
                {
                    // TODO BPC: event interfaces are protected
                    // txn->cfdpManager->log_WARNING_LO_FailKeepFileMove(txn->history->fnames.src_filename,
                    //                                                   moveDir, fileStatus);
                }
            }
        }
        else
        {
            /* file inside an polling directory */
            if (this->isPollingDir(txn->history->fnames.src_filename.toChar(), txn->chan_num))
            {
                /* If fail directory is defined attempt move */
                failDir = txn->cfdpManager->getFailDirParam();
                if(failDir.length() > 0)
                {
                    fileStatus = Os::FileSystem::moveFile(txn->history->fnames.src_filename.toChar(), failDir.toChar());
                    if(fileStatus != Os::FileSystem::OP_OK)
                    {
                        // TODO BPC: event interfaces are protected
                        // txn->cfdpManager->log_WARNING_LO_FailPollFileMove(txn->history->fnames.src_filename,
                        //                                                   failDir, fileStatus);
                    }
                }
            }
        }
    }
    /* Not Sender */
    else
    {
        fileStatus = Os::FileSystem::removeFile(txn->history->fnames.dst_filename.toChar());
        // TODO BPC: emit failure EVR
        (void) fileStatus;
    }
}

}  // namespace Ccsds
}  // namespace Svc
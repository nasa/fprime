// ======================================================================
// \title  CfdpRx.cpp
// \brief  CFDP receive logic source file
//
// This file is a port of the cf_cfdp_r.c file from the 
// NASA Core Flight System (cFS) CFDP (CF) Application,
// version 3.0.0, adapted for use within the F-Prime (F') framework.
//
// Handles all CFDP engine functionality specific to RX transactions.
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

#include <Svc/Ccsds/CfdpManager/CfdpEngine.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpChannel.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpRx.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpDispatch.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpUtils.hpp>

namespace Svc {
namespace Ccsds {

void CF_CFDP_R2_SetFinTxnStatus(CF_Transaction_t *txn, CF_TxnStatus_t txn_stat)
{
    CF_CFDP_SetTxnStatus(txn, txn_stat);
    txn->flags.rx.send_fin = true;
}

void CF_CFDP_R1_Reset(CF_Transaction_t *txn)
{
    CF_CFDP_FinishTransaction(txn, true);
}

void CF_CFDP_R2_Reset(CF_Transaction_t *txn)
{
    if ((txn->state_data.receive.sub_state == CF_RxSubState_CLOSEOUT_SYNC) ||
        (txn->state_data.receive.r2.eof_cc != CF_CFDP_ConditionCode_NO_ERROR) ||
        CF_TxnStatus_IsError(txn->history->txn_stat) || txn->flags.com.canceled)
    {
        CF_CFDP_R1_Reset(txn); /* it's done */
    }
    else
    {
        /* not waiting for FIN ACK, so trigger send FIN */
        txn->flags.rx.send_fin = true;
    }
}

CfdpStatus::T CF_CFDP_R_CheckCrc(CF_Transaction_t *txn, U32 expected_crc)
{
    CfdpStatus::T ret = CfdpStatus::SUCCESS;
    U32 crc_result;

    // The F' version does not have an equivelent finalize call as it
    // - Never stores a partial word internally
    // - Never needs to "flush" anything
    // - Always accounts for padding at update time
    // CF_CRC_Finalize(&txn->crc);
    crc_result = txn->crc.getValue();
    if (crc_result != expected_crc)
    {
        // CFE_EVS_SendEvent(CF_CFDP_R_CRC_ERR_EID, CFE_EVS_EventType_ERROR,
        //                   "CF R%d(%lu:%lu): CRC mismatch for R trans. got 0x%08lx expected 0x%08lx",
        //                   (txn->state == CF_TxnState_R2), (unsigned long)txn->history->src_eid,
        //                   (unsigned long)txn->history->seq_num, (unsigned long)crc_result,
        //                   (unsigned long)expected_crc);
        // ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.fault.crc_mismatch;
        ret = CfdpStatus::ERROR;
    }

    return ret;
}

void CF_CFDP_R2_Complete(CF_Transaction_t *txn, bool ok_to_send_nak)
{
    U32 ret;
    bool send_nak = false;
    bool send_fin = false;
    U8 nack_limit = 0;
    /* checking if r2 is complete. Check NAK list, and send NAK if appropriate */
    /* if all data is present, then there will be no gaps in the chunk */

    if (!CF_TxnStatus_IsError(txn->history->txn_stat))
    {
        /* first, check if md is received. If not, send specialized NAK */
        if (!txn->flags.rx.md_recv)
        {
            send_nak = true;
        }
        else
        {
            /* only look for 1 gap, since the goal here is just to know that there are gaps */
            ret = CF_ChunkList_ComputeGaps(&txn->chunks->chunks, 1, txn->fsize, 0, NULL, NULL);

            if (ret)
            {
                /* there is at least 1 gap, so send a NAK */
                send_nak = true;
            }
            else if (txn->flags.rx.eof_recv)
            {
                /* the EOF was received, and there are no NAKs -- process completion in send FIN state */
                send_fin = true;
            }
        }

        if (send_nak && ok_to_send_nak)
        {
            /* Increment the acknak counter */
            ++txn->state_data.receive.r2.acknak_count;

            /* Check limit and handle if needed */
            nack_limit = txn->cfdpManager->getNackLimitParam(txn->chan_num);
            if (txn->state_data.receive.r2.acknak_count >= nack_limit)
            {
                // CFE_EVS_SendEvent(CF_CFDP_R_NAK_LIMIT_ERR_EID, CFE_EVS_EventType_ERROR,
                //                   "CF R%d(%lu:%lu): NAK limited reach", (txn->state == CF_TxnState_R2),
                //                   (unsigned long)txn->history->src_eid, (unsigned long)txn->history->seq_num);
                send_fin = true;
                // ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.fault.nak_limit;
                /* don't use CF_CFDP_R2_SetFinTxnStatus because many places in this function set send_fin */
                CF_CFDP_SetTxnStatus(txn, CF_TxnStatus_NAK_LIMIT_REACHED);
                txn->state_data.receive.r2.acknak_count = 0; /* reset for fin/ack */
            }
            else
            {
                txn->flags.rx.send_nak = true;
            }
        }

        if (send_fin)
        {
            txn->flags.rx.complete = true; /* latch completeness, since send_fin is cleared later */

            /* the transaction is now considered complete, but this will not overwrite an
             * error status code if there was one set */
            CF_CFDP_R2_SetFinTxnStatus(txn, CF_TxnStatus_NO_ERROR);
        }

        /* always go to CF_RxSubState_FILEDATA, and let tick change state */
        txn->state_data.receive.sub_state = CF_RxSubState_FILEDATA;
    }
}

CfdpStatus::T CF_CFDP_R_ProcessFd(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    const CF_Logical_PduFileDataHeader_t *pdu;
    Os::File::Status status;
    CfdpStatus::T ret;

    /* this function is only entered for data PDUs */
    pdu = &ph->int_header.fd;
    ret = CfdpStatus::SUCCESS;

    /*
     * NOTE: The decode routine should have left a direct pointer to the data and actual data length
     * within the PDU.  The length has already been verified, too.  Should not need to make any
     * adjustments here, just write it.
     */

    // TODO BPC: get rid of pdu->offset in favor of Os::File::position()
    if (txn->state_data.receive.cached_pos != pdu->offset)
    {
        status = txn->fd.seek(pdu->offset, Os::File::SeekType::ABSOLUTE);
        if (status != Os::File::OP_OK)
        {
            // CFE_EVS_SendEvent(CF_CFDP_R_SEEK_FD_ERR_EID, CFE_EVS_EventType_ERROR,
            //                   "CF R%d(%lu:%lu): failed to seek offset %ld, got %ld", (txn->state == CF_TxnState_R2),
            //                   (unsigned long)txn->history->src_eid, (unsigned long)txn->history->seq_num,
            //                   (long)pdu->offset, (long)fret);
            CF_CFDP_SetTxnStatus(txn, CF_TxnStatus_FILE_SIZE_ERROR);
            // ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.fault.file_seek;
            ret = CfdpStatus::ERROR; /* connection will reset in caller */
        }
    }

    if (ret != CfdpStatus::ERROR)
    {
        FwSizeType write_size = pdu->data_len;
        status = txn->fd.write(pdu->data_ptr, write_size, Os::File::WaitType::WAIT);
        if (status != Os::File::OP_OK)
        {
            // CFE_EVS_SendEvent(CF_CFDP_R_WRITE_ERR_EID, CFE_EVS_EventType_ERROR,
            //                   "CF R%d(%lu:%lu): OS_write expected %ld, got %ld", (txn->state == CF_TxnState_R2),
            //                   (unsigned long)txn->history->src_eid, (unsigned long)txn->history->seq_num,
            //                   (long)pdu->data_len, (long)fret);
            CF_CFDP_SetTxnStatus(txn, CF_TxnStatus_FILESTORE_REJECTION);
            // ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.fault.file_write;
            ret = CfdpStatus::ERROR; /* connection will reset in caller */
        }
        else
        {
            txn->state_data.receive.cached_pos = static_cast<U32>(pdu->data_len) + pdu->offset;
            // CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.recv.file_data_bytes += pdu->data_len;
        }
    }

    return ret;
}

CfdpStatus::T CF_CFDP_R_SubstateRecvEof(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    CfdpStatus::T               ret = CfdpStatus::SUCCESS;
    const CF_Logical_PduEof_t *eof;

    if (!CF_CFDP_RecvEof(txn, ph))
    {
        /* this function is only entered for PDUs identified as EOF type */
        eof = &ph->int_header.eof;

        /* only check size if MD received, otherwise it's still OK */
        if (txn->flags.rx.md_recv && (eof->size != txn->fsize))
        {
            // CFE_EVS_SendEvent(CF_CFDP_R_SIZE_MISMATCH_ERR_EID, CFE_EVS_EventType_ERROR,
            //                   "CF R%d(%lu:%lu): EOF file size mismatch: got %lu expected %lu",
            //                   (txn->state == CF_TxnState_R2), (unsigned long)txn->history->src_eid,
            //                   (unsigned long)txn->history->seq_num, (unsigned long)eof->size,
            //                   (unsigned long)txn->fsize);
            // ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.fault.file_size_mismatch;
            ret = CfdpStatus::REC_PDU_FSIZE_MISMATCH_ERROR;
        }
    }
    else
    {
        // CFE_EVS_SendEvent(CF_CFDP_R_PDU_EOF_ERR_EID, CFE_EVS_EventType_ERROR, "CF R%d(%lu:%lu): invalid EOF packet",
        //                   (txn->state == CF_TxnState_R2), (unsigned long)txn->history->src_eid,
        //                   (unsigned long)txn->history->seq_num);
        // ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.recv.error;
        ret = CfdpStatus::REC_PDU_BAD_EOF_ERROR;
    }

    return ret;
}

void CF_CFDP_R1_SubstateRecvEof(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    int                        ret = CF_CFDP_R_SubstateRecvEof(txn, ph);
    U32                     crc;
    const CF_Logical_PduEof_t *eof;

    /* this function is only entered for PDUs identified as EOF type */
    eof = &ph->int_header.eof;
    crc = eof->crc;

    if (ret == CfdpStatus::SUCCESS)
    {
        /* Verify CRC */
        if (CF_CFDP_R_CheckCrc(txn, crc) == CfdpStatus::SUCCESS)
        {
            /* successfully processed the file */
            txn->keep = CfdpKeep::KEEP; /* save the file */
        }
        /* if file failed to process, there's nothing to do. CF_CFDP_R_CheckCrc() generates an event on failure */
    }

    /* after exit, always reset since we are done */
    /* reset even if the EOF failed -- class 1, so it won't come again! */
    CF_CFDP_R1_Reset(txn);
}

void CF_CFDP_R2_SubstateRecvEof(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    const CF_Logical_PduEof_t *eof;
    int                        ret;

    if (!txn->flags.rx.eof_recv)
    {
        ret = CF_CFDP_R_SubstateRecvEof(txn, ph);

        /* did receiving EOF succeed? */
        if (ret == CfdpStatus::SUCCESS)
        {
            eof = &ph->int_header.eof;

            txn->flags.rx.eof_recv = true;

            /* need to remember the EOF CRC for later */
            txn->state_data.receive.r2.eof_crc  = eof->crc;
            txn->state_data.receive.r2.eof_size = eof->size;

            /* always ACK the EOF, even if we're not done */
            txn->state_data.receive.r2.eof_cc = eof->cc;
            txn->flags.rx.send_eof_ack        = true; /* defer sending ACK to tick handling */

            /* only check for complete if EOF with no errors */
            if (txn->state_data.receive.r2.eof_cc == CF_CFDP_ConditionCode_NO_ERROR)
            {
                CF_CFDP_R2_Complete(txn, true); /* CF_CFDP_R2_Complete() will change state */
            }
            else
            {
                /* All CFDP CC values directly correspond to a Transaction Status of the same numeric value */
                CF_CFDP_SetTxnStatus(txn, static_cast<CF_TxnStatus_t>(txn->state_data.receive.r2.eof_cc));
                CF_CFDP_R2_Reset(txn);
            }
        }
        else
        {
            /* bad EOF sent? */
            if (ret == CfdpStatus::REC_PDU_FSIZE_MISMATCH_ERROR)
            {
                CF_CFDP_R2_SetFinTxnStatus(txn, CF_TxnStatus_FILE_SIZE_ERROR);
            }
            else
            {
                /* can't do anything with this bad EOF, so return to FILEDATA */
                txn->state_data.receive.sub_state = CF_RxSubState_FILEDATA;
            }
        }
    }
}

void CF_CFDP_R1_SubstateRecvFileData(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    int ret;

    /* got file data PDU? */
    ret = CF_CFDP_RecvFd(txn, ph);
    if (ret == CfdpStatus::SUCCESS)
    {
        ret = CF_CFDP_R_ProcessFd(txn, ph);
    }

    if (ret == CfdpStatus::SUCCESS)
    {
        /* class 1 digests CRC */
        txn->crc.update(ph->int_header.fd.data_ptr, ph->int_header.fd.offset,
                        static_cast<U32>(ph->int_header.fd.data_len));
    }
    else
    {
        /* Reset transaction on failure */
        CF_CFDP_R1_Reset(txn);
    }
}

void CF_CFDP_R2_SubstateRecvFileData(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    const CF_Logical_PduFileDataHeader_t *fd;
    int                                   ret;

    /* this function is only entered for data PDUs */
    fd = &ph->int_header.fd;

    // If CRC calculation has started (file reopened in READ mode), ignore late FileData PDUs.
    // This can happen if retransmitted FileData arrives after EOF was received and CRC began.
    if (txn->state_data.receive.r2.rx_crc_calc_bytes > 0)
    {
        // Silently ignore - file is complete and we're calculating CRC
        // TODO BPC: do we want a throttled EVR here?
        return;
    }

    /* got file data PDU? */
    ret = CF_CFDP_RecvFd(txn, ph);
    if (ret == CfdpStatus::SUCCESS)
    {
        ret = CF_CFDP_R_ProcessFd(txn, ph);
    }

    if (ret == CfdpStatus::SUCCESS)
    {
        /* class 2 does CRC at FIN, but track gaps */
        CF_ChunkListAdd(&txn->chunks->chunks, fd->offset, static_cast<CF_ChunkSize_t>(fd->data_len));

        if (txn->flags.rx.fd_nak_sent)
        {
            CF_CFDP_R2_Complete(txn, false); /* once nak-retransmit received, start checking for completion at each fd */
        }

        if (!txn->flags.rx.complete)
        {
            CF_CFDP_ArmAckTimer(txn); /* re-arm ACK timer, since we got data */
        }

        txn->state_data.receive.r2.acknak_count = 0;
    }
    else
    {
        /* Reset transaction on failure */
        CF_CFDP_R2_Reset(txn);
    }
}

void CF_CFDP_R2_GapCompute(const CF_ChunkList_t *chunks, const CF_Chunk_t *chunk, void *opaque)
{
    CF_GapComputeArgs_t *        args = static_cast<CF_GapComputeArgs_t *>(opaque);
    CF_Logical_SegmentRequest_t *pseg;
    CF_Logical_SegmentList_t *   pseglist;
    CF_Logical_PduNak_t *        nak;

    /* This function is only invoked for NAK types */
    nak      = args->nak;
    pseglist = &nak->segment_list;
    FW_ASSERT(chunk->size > 0, chunk->size);

    /* it seems that scope in the old engine is not used the way I read it in the spec, so
     * leave this code here for now for future reference */

    if (pseglist->num_segments < CF_PDU_MAX_SEGMENTS)
    {
        pseg = &pseglist->segments[pseglist->num_segments];

        pseg->offset_start = chunk->offset - nak->scope_start;
        pseg->offset_end   = pseg->offset_start + chunk->size;

        ++pseglist->num_segments;
    }
}

CfdpStatus::T CF_CFDP_R_SubstateSendNak(CF_Transaction_t *txn)
{
    CF_Logical_PduBuffer_t *ph =
        CF_CFDP_ConstructPduHeader(txn, CF_CFDP_FileDirective_NAK, txn->history->peer_eid,
                                   txn->cfdpManager->getLocalEidParam(), 1, txn->history->seq_num, true);
    CF_Logical_PduNak_t *nak;
    CfdpStatus::T sret;
    U32 cret;
    CfdpStatus::T ret = CfdpStatus::ERROR;

    if (ph)
    {
        nak = &ph->int_header.nak;

        if (txn->flags.rx.md_recv)
        {
            /* we have metadata, so send valid NAK */
            CF_GapComputeArgs_t args = {txn, nak};

            nak->scope_start = 0;
            cret             = CF_ChunkList_ComputeGaps(&txn->chunks->chunks,
                                            (txn->chunks->chunks.count < txn->chunks->chunks.max_chunks)
                                                            ? txn->chunks->chunks.max_chunks
                                                            : (txn->chunks->chunks.max_chunks - 1),
                                            txn->fsize, 0, CF_CFDP_R2_GapCompute, &args);

            if (!cret)
            {
                /* no gaps left, so go ahead and check for completion */
                txn->flags.rx.complete = true; /* we know md was received, and there's no gaps -- it's complete */
                ret                    = CfdpStatus::SUCCESS;
            }
            else
            {
                /* gaps are present, so let's send the NAK PDU */
                nak->scope_end            = 0;
                sret                      = CF_CFDP_SendNak(txn, ph);
                txn->flags.rx.fd_nak_sent = true; /* latch that at least one NAK has been sent requesting filedata */
                /* NOTE: this assert is here because CF_CFDP_SendNak() does not return SEND_PDU_ERROR,
                   so if it's ever added to that function we need to test handling it here */
                FW_ASSERT(sret != CfdpStatus::SEND_PDU_ERROR); 
                if (sret == CfdpStatus::SUCCESS)
                {
                    // CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.sent.nak_segment_requests += cret;
                    ret = CfdpStatus::SUCCESS;
                }
            }
        }
        else
        {
            /* need to send simple NAK packet to request metadata PDU again */
            /* after doing so, transition to recv md state */
            // CFE_EVS_SendEvent(CF_CFDP_R_REQUEST_MD_INF_EID, CFE_EVS_EventType_INFORMATION,
            //                   "CF R%d(%lu:%lu): requesting MD", (txn->state == CF_TxnState_R2),
            //                   (unsigned long)txn->history->src_eid, (unsigned long)txn->history->seq_num);
            /* scope start/end, and sr[0] start/end == 0 special value to request metadata */
            nak->scope_start                           = 0;
            nak->scope_end                             = 0;
            nak->segment_list.segments[0].offset_start = 0;
            nak->segment_list.segments[0].offset_end   = 0;
            nak->segment_list.num_segments             = 1;

            sret = CF_CFDP_SendNak(txn, ph);
            // this assert is here because CF_CFDP_SendNak() does not return SEND_PDU_ERROR */
            FW_ASSERT(sret != CfdpStatus::SEND_PDU_ERROR);
            if (sret == CfdpStatus::SUCCESS)
            {
                ret = CfdpStatus::SUCCESS;
            }
        }
    }

    return ret;
}

void CF_CFDP_R_Init(CF_Transaction_t *txn)
{
    Os::File::Status status;
    Fw::String tmpDir;
    Fw::String dst;

    if (txn->state == CF_TxnState_R2)
    {
        if (!txn->flags.rx.md_recv)
        {
            tmpDir = txn->cfdpManager->getTmpDirParam();
            /* we need to make a temp file and then do a NAK for md PDU */
            /* the transaction already has a history, and that has a buffer that we can use to
             * hold the temp filename which is defined by the sequence number and the source entity ID */

            // Create destination filepath with format: <tmpDir>/<src_eid>:<seq_num>.tmp
            dst.format("%s/%" CF_PRI_ENTITY_ID ":%" CF_PRI_TRANSACTION_SEQ ".tmp",
                       tmpDir.toChar(),
                       txn->history->src_eid,
                       txn->history->seq_num);

            txn->history->fnames.dst_filename = dst;

            // CFE_EVS_SendEvent(CF_CFDP_R_TEMP_FILE_INF_EID, CFE_EVS_EventType_INFORMATION,
            //                   "CF R%d(%lu:%lu): making temp file %s for transaction without MD",
            //                   (txn->state == CF_TxnState_R2), (unsigned long)txn->history->src_eid,
            //                   (unsigned long)txn->history->seq_num, txn->history->fnames.dst_filename);
        }

        CF_CFDP_ArmAckTimer(txn);
    }

    status = txn->fd.open(txn->history->fnames.dst_filename.toChar(), Os::File::OPEN_CREATE, Os::File::OVERWRITE);
    if (status != Os::File::OP_OK)
    {
        // CFE_EVS_SendEvent(CF_CFDP_R_CREAT_ERR_EID, CFE_EVS_EventType_ERROR,
        //                   "CF R%d(%lu:%lu): failed to create file %s for writing, error=%ld",
        //                   (txn->state == CF_TxnState_R2), (unsigned long)txn->history->src_eid,
        //                   (unsigned long)txn->history->seq_num, txn->history->fnames.dst_filename, (long)ret);
        // ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.fault.file_open;
        // txn->fd = OS_OBJECT_ID_UNDEFINED; /* just in case */
        if (txn->state == CF_TxnState_R2)
        {
            CF_CFDP_R2_SetFinTxnStatus(txn, CF_TxnStatus_FILESTORE_REJECTION);
        }
        else
        {
            CF_CFDP_R1_Reset(txn);
        }
    }
    else
    {
        txn->state_data.receive.sub_state = CF_RxSubState_FILEDATA;
    }
}

CfdpStatus::T CF_CFDP_R2_CalcCrcChunk(CF_Transaction_t *txn)
{
    U8 buf[CF_R2_CRC_CHUNK_SIZE];
    size_t count_bytes;
    size_t want_offs_size;
    FwSizeType read_size;
    Os::File::Status fileStatus;
    CfdpStatus::T ret;
    bool success = true;
    U32 rx_crc_calc_bytes_per_wakeup = 0;

    memset(buf, 0, sizeof(buf));

    count_bytes = 0;
    ret         = CfdpStatus::ERROR;

    if (txn->state_data.receive.r2.rx_crc_calc_bytes == 0)
    {
        txn->crc = CFDP::Checksum(0);

        // For Class 2 RX, the file was opened in WRITE mode for receiving FileData PDUs.
        // Now we need to READ it for CRC calculation. Close and reopen in READ mode.
        if (txn->fd.isOpen())
        {
            txn->fd.close();
        }

        fileStatus = txn->fd.open(txn->history->fnames.dst_filename.toChar(), Os::File::OPEN_READ);
        if (fileStatus != Os::File::OP_OK)
        {
            CF_CFDP_SetTxnStatus(txn, CF_TxnStatus_FILE_SIZE_ERROR);
            return CfdpStatus::ERROR;
        }

        // Reset cached position since we just reopened the file
        txn->state_data.receive.cached_pos = 0;
    }

    rx_crc_calc_bytes_per_wakeup = txn->cfdpManager->getRxCrcCalcBytesPerWakeupParam();

    while ((count_bytes < rx_crc_calc_bytes_per_wakeup) &&
           (txn->state_data.receive.r2.rx_crc_calc_bytes < txn->fsize))
    {
        want_offs_size = txn->state_data.receive.r2.rx_crc_calc_bytes + sizeof(buf);

        if (want_offs_size > txn->fsize)
        {
            read_size = txn->fsize - txn->state_data.receive.r2.rx_crc_calc_bytes;
        }
        else
        {
            read_size = sizeof(buf);
        }

        if (txn->state_data.receive.cached_pos != txn->state_data.receive.r2.rx_crc_calc_bytes)
        {
            fileStatus = txn->fd.seek(txn->state_data.receive.r2.rx_crc_calc_bytes, Os::File::SeekType::ABSOLUTE);
            if (fileStatus != Os::File::OP_OK)
            {
                // CFE_EVS_SendEvent(CF_CFDP_R_SEEK_CRC_ERR_EID, CFE_EVS_EventType_ERROR,
                //                   "CF R%d(%lu:%lu): failed to seek offset %lu, got %ld", (txn->state == CF_TxnState_R2),
                //                   (unsigned long)txn->history->src_eid, (unsigned long)txn->history->seq_num,
                //                   (unsigned long)txn->state_data.receive.r2.rx_crc_calc_bytes, (long)fret);
                // CF_CFDP_SetTxnStatus(txn, CF_TxnStatus_FILE_SIZE_ERROR);
                // ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.fault.file_seek;
                success = false;
                break;
            }
        }

        fileStatus = txn->fd.read(buf, read_size, Os::File::WaitType::WAIT);
        if (fileStatus != Os::File::OP_OK)
        {
            // CFE_EVS_SendEvent(CF_CFDP_R_READ_ERR_EID, CFE_EVS_EventType_ERROR,
            //                   "CF R%d(%lu:%lu): failed to read file expected %lu, got %ld",
            //                   (txn->state == CF_TxnState_R2), (unsigned long)txn->history->src_eid,
            //                   (unsigned long)txn->history->seq_num, (unsigned long)read_size, (long)fret);
            CF_CFDP_SetTxnStatus(txn, CF_TxnStatus_FILE_SIZE_ERROR);
            // ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.fault.file_read;
            success = false;
            break;
        }

        txn->crc.update(buf, txn->state_data.receive.r2.rx_crc_calc_bytes, static_cast<U32>(read_size));
        txn->state_data.receive.r2.rx_crc_calc_bytes += static_cast<U32>(read_size);
        txn->state_data.receive.cached_pos = txn->state_data.receive.r2.rx_crc_calc_bytes;
        count_bytes += read_size;
    }

    if (success && txn->state_data.receive.r2.rx_crc_calc_bytes == txn->fsize)
    {
        /* all bytes calculated, so now check */
        if (CF_CFDP_R_CheckCrc(txn, txn->state_data.receive.r2.eof_crc) == CfdpStatus::SUCCESS)
        {
            /* CRC matched! We are happy */
            txn->keep = CfdpKeep::KEEP; /* save the file */

            /* set FIN PDU status */
            txn->state_data.receive.r2.dc = CF_CFDP_FinDeliveryCode_COMPLETE;
            txn->state_data.receive.r2.fs = CF_CFDP_FinFileStatus_RETAINED;
        }
        else
        {
            CF_CFDP_R2_SetFinTxnStatus(txn, CF_TxnStatus_FILE_CHECKSUM_FAILURE);
        }

        txn->flags.com.crc_calc = true;

        ret = CfdpStatus::SUCCESS;
    }

    return ret;
}

CfdpStatus::T CF_CFDP_R2_SubstateSendFin(CF_Transaction_t *txn)
{
    CfdpStatus::T sret;
    CfdpStatus::T ret = CfdpStatus::SUCCESS;

    if (!CF_TxnStatus_IsError(txn->history->txn_stat) && !txn->flags.com.crc_calc)
    {
        /* no error, and haven't checked CRC -- so start checking it */
        if (CF_CFDP_R2_CalcCrcChunk(txn))
        {
            ret = CfdpStatus::ERROR; /* signal to caller to re-enter next tick */
        }
    }

    if (ret != CfdpStatus::ERROR)
    {
        sret = CF_CFDP_SendFin(txn, txn->state_data.receive.r2.dc, txn->state_data.receive.r2.fs,
                               CF_TxnStatus_To_ConditionCode(txn->history->txn_stat));
        /* CF_CFDP_SendFin does not return SEND_PDU_ERROR */
        FW_ASSERT(sret != CfdpStatus::SEND_PDU_ERROR); 
        txn->state_data.receive.sub_state =
            CF_RxSubState_CLOSEOUT_SYNC; /* whether or not FIN send successful, ok to transition state */
        if (sret != CfdpStatus::SUCCESS)
        {
            ret = CfdpStatus::ERROR;
        }
    }

    /* if no message, then try again next time */
    return ret;
}

void CF_CFDP_R2_Recv_fin_ack(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    if (!CF_CFDP_RecvAck(txn, ph))
    {
        /* got fin-ack, so time to close the state */
        CF_CFDP_R2_Reset(txn);
    }
    else
    {
        // CFE_EVS_SendEvent(CF_CFDP_R_PDU_FINACK_ERR_EID, CFE_EVS_EventType_ERROR, "CF R%d(%lu:%lu): invalid fin-ack",
        //                   (txn->state == CF_TxnState_R2), (unsigned long)txn->history->src_eid,
        //                   (unsigned long)txn->history->seq_num);
        // ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.recv.error;
    }
}

void CF_CFDP_R2_RecvMd(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    Fw::String fname;
    CfdpStatus::T status;
    Os::File::Status fileStatus;
    Os::FileSystem::Status fileSysStatus;
    bool success = true;

    /* it isn't an error to get another MD PDU, right? */
    if (!txn->flags.rx.md_recv)
    {
        /* NOTE: txn->flags.rx.md_recv always 1 in R1, so this is R2 only */
        /* parse the md PDU. this will overwrite the transaction's history, which contains our filename. so let's
         * save the filename in a local buffer so it can be used with moveFile upon successful parsing of
         * the md PDU */
        fname = txn->history->fnames.dst_filename;

        status = CF_CFDP_RecvMd(txn, ph);
        if (status == CfdpStatus::SUCCESS)
        {
            /* successfully obtained md PDU */
            if (txn->flags.rx.eof_recv)
            {
                /* EOF was received, so check that md and EOF sizes match */
                if (txn->state_data.receive.r2.eof_size != txn->fsize)
                {
                    // CFE_EVS_SendEvent(CF_CFDP_R_EOF_MD_SIZE_ERR_EID, CFE_EVS_EventType_ERROR,
                    //                   "CF R%d(%lu:%lu): EOF/md size mismatch md: %lu, EOF: %lu",
                    //                   (txn->state == CF_TxnState_R2), (unsigned long)txn->history->src_eid,
                    //                   (unsigned long)txn->history->seq_num, (unsigned long)txn->fsize,
                    //                   (unsigned long)txn->state_data.receive.r2.eof_size);
                    // ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.fault.file_size_mismatch;
                    CF_CFDP_R2_SetFinTxnStatus(txn, CF_TxnStatus_FILE_SIZE_ERROR);
                    success = false;
                }
            }

            if (success)
            {
                /* close and rename file */
                txn->fd.close();

                fileSysStatus = Os::FileSystem::moveFile(fname.toChar(),
                                                         txn->history->fnames.dst_filename.toChar());
                if (fileSysStatus != Os::FileSystem::OP_OK)
                {
                    // CFE_EVS_SendEvent(CF_CFDP_R_RENAME_ERR_EID, CFE_EVS_EventType_ERROR,
                    //                   "CF R%d(%lu:%lu): failed to rename file in R2, error=%ld",
                    //                   (txn->state == CF_TxnState_R2), (unsigned long)txn->history->src_eid,
                    //                   (unsigned long)txn->history->seq_num, (long)status);
                    // txn->fd = OS_OBJECT_ID_UNDEFINED;
                    CF_CFDP_R2_SetFinTxnStatus(txn, CF_TxnStatus_FILESTORE_REJECTION);
                    // ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.fault.file_rename;
                    success = false;
                }
                else
                {
                     // TODO BPC: flags = OS_FILE_FLAG_NONE, access = OS_READ_WRITE
                     // File was succesfully renamed, open for writing
                    fileStatus = txn->fd.open(txn->history->fnames.dst_filename.toChar(), Os::File::OPEN_WRITE);
                    if (fileStatus != Os::File::OP_OK)
                    {
                        // CFE_EVS_SendEvent(CF_CFDP_R_OPEN_ERR_EID, CFE_EVS_EventType_ERROR,
                        //                   "CF R%d(%lu:%lu): failed to open renamed file in R2, error=%ld",
                        //                   (txn->state == CF_TxnState_R2), (unsigned long)txn->history->src_eid,
                        //                   (unsigned long)txn->history->seq_num, (long)ret);
                        CF_CFDP_R2_SetFinTxnStatus(txn, CF_TxnStatus_FILESTORE_REJECTION);
                        // ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.fault.file_open;
                        // txn->fd = OS_OBJECT_ID_UNDEFINED; /* just in case */
                        success = false;
                    }
                }

                if (success)
                {
                    txn->state_data.receive.cached_pos      = 0; /* reset psn due to open */
                    txn->flags.rx.md_recv                   = true;
                    txn->state_data.receive.r2.acknak_count = 0; /* in case part of NAK */
                    CF_CFDP_R2_Complete(txn, true);                 /* check for completion now that md is received */
                }
            }
        }
        else
        {
            // CFE_EVS_SendEvent(CF_CFDP_R_PDU_MD_ERR_EID, CFE_EVS_EventType_ERROR, "CF R%d(%lu:%lu): invalid md received",
            //                   (txn->state == CF_TxnState_R2), (unsigned long)txn->history->src_eid,
            //                   (unsigned long)txn->history->seq_num);
            // ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.recv.error;
            /* do nothing here, since it will be NAK'd again later */
        }
    }
}

void CF_CFDP_R1_Recv(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    static const CF_CFDP_FileDirectiveDispatchTable_t r1_fdir_handlers = {
        {
            nullptr, /* CF_CFDP_FileDirective_INVALID_MIN */
            nullptr, /* 1 is unused in the CF_CFDP_FileDirective_t enum */
            nullptr, /* 2 is unused in the CF_CFDP_FileDirective_t enum */
            nullptr, /* 3 is unused in the CF_CFDP_FileDirective_t enum */
            CF_CFDP_R1_SubstateRecvEof, /* CF_CFDP_FileDirective_EOF */
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

    CF_CFDP_R_DispatchRecv(txn, ph, &substate_fns, CF_CFDP_R1_SubstateRecvFileData);
}

void CF_CFDP_R2_Recv(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    static const CF_CFDP_FileDirectiveDispatchTable_t r2_fdir_handlers_normal = {
        {
            nullptr, /* CF_CFDP_FileDirective_INVALID_MIN */
            nullptr, /* 1 is unused in the CF_CFDP_FileDirective_t enum */
            nullptr, /* 2 is unused in the CF_CFDP_FileDirective_t enum */
            nullptr, /* 3 is unused in the CF_CFDP_FileDirective_t enum */
            CF_CFDP_R2_SubstateRecvEof, /* CF_CFDP_FileDirective_EOF */
            nullptr, /* CF_CFDP_FileDirective_FIN */
            nullptr, /* CF_CFDP_FileDirective_ACK */
            CF_CFDP_R2_RecvMd, /* CF_CFDP_FileDirective_METADATA */
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
            CF_CFDP_R2_SubstateRecvEof, /* CF_CFDP_FileDirective_EOF */
            nullptr, /* CF_CFDP_FileDirective_FIN */
            CF_CFDP_R2_Recv_fin_ack, /* CF_CFDP_FileDirective_ACK */
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

    CF_CFDP_R_DispatchRecv(txn, ph, &substate_fns, CF_CFDP_R2_SubstateRecvFileData);
}

void CF_CFDP_R_Cancel(CF_Transaction_t *txn)
{
    /* for cancel, only need to send FIN if R2 */
    if ((txn->state == CF_TxnState_R2) && (txn->state_data.receive.sub_state < CF_RxSubState_CLOSEOUT_SYNC))
    {
        txn->flags.rx.send_fin = true;
    }
    else
    {
        CF_CFDP_R1_Reset(txn); /* if R1, just call it quits */
    }
}

void CF_CFDP_R_SendInactivityEvent(CF_Transaction_t *txn)
{
    (void) txn;
    // CFE_EVS_SendEvent(CF_CFDP_R_INACT_TIMER_ERR_EID, CFE_EVS_EventType_ERROR,
    //                   "CF R%d(%lu:%lu): inactivity timer expired", (txn->state == CF_TxnState_R2),
    //                   (unsigned long)txn->history->src_eid, (unsigned long)txn->history->seq_num);
    // ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.fault.inactivity_timer;
}

void CF_CFDP_R_AckTimerTick(CF_Transaction_t *txn)
{
    U8 ack_limit = 0;

    /* note: the ack timer is only ever armed on class 2 */
    if (txn->state != CF_TxnState_R2 || !txn->flags.com.ack_timer_armed)
    {
        /* nothing to do */
        return;
    }

    if (txn->ack_timer.getStatus() == CfdpTimer::Status::RUNNING)
    {
        txn->ack_timer.run();
    }
    else
    {
        /* ACK timer expired, so check for completion */
        if (!txn->flags.rx.complete)
        {
            CF_CFDP_R2_Complete(txn, true);
        }
        else if (txn->state_data.receive.sub_state == CF_RxSubState_CLOSEOUT_SYNC)
        {
            /* Increment acknak counter */
            ++txn->state_data.receive.r2.acknak_count;

            /* Check limit and handle if needed */
            ack_limit = txn->cfdpManager->getAckLimitParam(txn->chan_num);
            if (txn->state_data.receive.r2.acknak_count >= ack_limit)
            {
                // CFE_EVS_SendEvent(CF_CFDP_R_ACK_LIMIT_ERR_EID, CFE_EVS_EventType_ERROR,
                //                   "CF R2(%lu:%lu): ACK limit reached, no fin-ack", (unsigned long)txn->history->src_eid,
                //                   (unsigned long)txn->history->seq_num);
                CF_CFDP_SetTxnStatus(txn, CF_TxnStatus_ACK_LIMIT_NO_FIN);
                // ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.fault.ack_limit;

                /* give up on this */
                CF_CFDP_FinishTransaction(txn, true);
                txn->flags.com.ack_timer_armed = false;
            }
            else
            {
                txn->flags.rx.send_fin = true;
            }
        }

        /* re-arm the timer if it is still pending */
        if (txn->flags.com.ack_timer_armed)
        {
            /* whether sending FIN or waiting for more filedata, need ACK timer armed */
            CF_CFDP_ArmAckTimer(txn);
        }
    }
}

void CF_CFDP_R_Tick(CF_Transaction_t *txn, int *cont /* unused */)
{
    /* Steven is not real happy with this function. There should be a better way to separate out
     * the logic by state so that it isn't a bunch of if statements for different flags
     */

    CfdpStatus::T sret;
    bool         pending_send;

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
            if (txn->state != CF_TxnState_HOLD)
            {
                CF_CFDP_R_SendInactivityEvent(txn);

                /* in class 2 this also triggers sending an early FIN response */
                if (txn->state == CF_TxnState_R2)
                {
                    CF_CFDP_R2_SetFinTxnStatus(txn, CF_TxnStatus_INACTIVITY_DETECTED);
                }
            }
        }
    }

    pending_send = true; /* maybe; tbd */

    /* rx maintenance: possibly process send_eof_ack, send_nak or send_fin */
    if (txn->flags.rx.send_eof_ack)
    {
        sret = CF_CFDP_SendAck(txn, CF_CFDP_AckTxnStatus_ACTIVE, CF_CFDP_FileDirective_EOF,
                               static_cast<CF_CFDP_ConditionCode_t>(txn->state_data.receive.r2.eof_cc),
                               txn->history->peer_eid, txn->history->seq_num);
        FW_ASSERT(sret != CfdpStatus::SEND_PDU_ERROR);

        /* if CfdpStatus::SUCCESS, then move on in the state machine. CF_CFDP_SendAck does not return
         * SEND_PDU_ERROR */
        if (sret != CfdpStatus::SEND_PDU_NO_BUF_AVAIL_ERROR)
        {
            txn->flags.rx.send_eof_ack = false;
        }
    }
    else if (txn->flags.rx.send_nak)
    {
        if (!CF_CFDP_R_SubstateSendNak(txn))
        {
            txn->flags.rx.send_nak = false; /* will re-enter on error */
        }
    }
    else if (txn->flags.rx.send_fin)
    {
        if (!CF_CFDP_R2_SubstateSendFin(txn))
        {
            txn->flags.rx.send_fin = false; /* will re-enter on error */
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
        CF_CFDP_R_AckTimerTick(txn);
    }
}

}  // namespace Ccsds
}  // namespace Svc
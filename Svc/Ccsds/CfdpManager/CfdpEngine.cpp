// ======================================================================
// \title  CfdpEngine.cpp
// \brief  CFDP Engine implementation
//
// This file is a port of CFDP engine operations from the following files
// from the NASA Core Flight System (cFS) CFDP (CF) Application, version 3.0.0,
// adapted for use within the F-Prime (F') framework:
// - cf_cfdp.c (CFDP PDU validation, processing, and engine operations)
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
#include <Fw/Types/StringUtils.hpp>

#include <Svc/Ccsds/CfdpManager/CfdpEngine.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpChannel.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpManager.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpTransaction.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpUtils.hpp>
#include <Svc/Ccsds/CfdpManager/Types/PduBase.hpp>

namespace Svc {
namespace Ccsds {
namespace Cfdp {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

CfdpEngine::CfdpEngine(CfdpManager* manager) :
    m_manager(manager),
    m_seqNum(0)
{
    // TODO BPC: Should we intialize CfdpEngine here or wait for the init() function?
    for (U8 i = 0; i < CFDP_NUM_CHANNELS; ++i)
    {
        m_channels[i] = nullptr;
    }
}

CfdpEngine::~CfdpEngine()
{
    for (U8 i = 0; i < CFDP_NUM_CHANNELS; ++i)
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

void CfdpEngine::init()
{
    // Create all channels
    for (U8 i = 0; i < CFDP_NUM_CHANNELS; ++i)
    {
        m_channels[i] = new CfdpChannel(this, i, this->m_manager);
        FW_ASSERT(m_channels[i] != nullptr);
    }
}


void CfdpEngine::armAckTimer(CfdpTransaction *txn)
{
    txn->m_ack_timer.setTimer(txn->m_cfdpManager->getAckTimerParam(txn->m_chan_num));
    txn->m_flags.com.ack_timer_armed = true;
}


void CfdpEngine::armInactTimer(CfdpTransaction *txn)
{
    U32 timerDuration = 0;

    /* select timeout based on the state */
    if (CfdpGetTxnStatus(txn) == ACK_TXN_STATUS_ACTIVE)
    {
        /* in an active transaction, we expect traffic so use the normal inactivity timer */
        timerDuration = txn->m_cfdpManager->getInactivityTimerParam(txn->m_chan_num);
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
        timerDuration = txn->m_cfdpManager->getAckTimerParam(txn->m_chan_num) * 2;
    }

    txn->m_inactivity_timer.setTimer(timerDuration);
}

void CfdpEngine::dispatchRecv(CfdpTransaction *txn, const Fw::Buffer& buffer)
{
    // Dispatch based on transaction state
    switch (txn->m_state)
    {
        case TXN_STATE_INIT:
            this->recvInit(txn, buffer);
            break;
        case TXN_STATE_R1:
            txn->r1Recv(buffer);
            break;
        case TXN_STATE_S1:
            txn->s1Recv(buffer);
            break;
        case TXN_STATE_R2:
            txn->r2Recv(buffer);
            break;
        case TXN_STATE_S2:
            txn->s2Recv(buffer);
            break;
        case TXN_STATE_DROP:
            this->recvDrop(txn, buffer);
            break;
        case TXN_STATE_HOLD:
            this->recvHold(txn, buffer);
            break;
        default:
            // Invalid or undefined state
            break;
    }

    this->armInactTimer(txn); /* whenever a packet was received by the other size, always arm its inactivity timer */
}

void CfdpEngine::dispatchTx(CfdpTransaction *txn)
{
    static const TxnSendDispatchTable state_fns = {
        {
            nullptr, // TXN_STATE_UNDEF
            nullptr, // TXN_STATE_INIT
            nullptr, // TXN_STATE_R1
            &CfdpTransaction::s1Tx, // TXN_STATE_S1
            nullptr, // TXN_STATE_R2
            &CfdpTransaction::s2Tx, // TXN_STATE_S2
            nullptr, // TXN_STATE_DROP
            nullptr // TXN_STATE_HOLD
        }
    };

    txn->txStateDispatch(&state_fns);
}

Status::T CfdpEngine::sendMd(CfdpTransaction *txn)
{
    Fw::Buffer buffer;
    Status::T status = Cfdp::Status::SUCCESS;

    FW_ASSERT((txn->m_state == TXN_STATE_S1) || (txn->m_state == TXN_STATE_S2), txn->m_state);
    FW_ASSERT(txn->m_chan != NULL);

    // Create and initialize Metadata PDU
    MetadataPdu md;

    // Set closure requested flag based on transaction class
    // Class 1: closure not requested (0), Class 2: closure requested (1)
    U8 closureRequested = (txn->m_state == TXN_STATE_S2) ? 1 : 0;

    // Direction is toward receiver for metadata PDU sent by sender
    Cfdp::PduDirection direction = DIRECTION_TOWARD_RECEIVER;

    md.initialize(
        direction,
        txn->getClass(),  // transmission mode (Class 1 or 2)
        m_manager->getLocalEidParam(),  // source EID
        txn->m_history->seq_num,  // transaction sequence number
        txn->m_history->peer_eid,  // destination EID
        txn->m_fsize,  // file size
        txn->m_history->fnames.src_filename,  // source filename
        txn->m_history->fnames.dst_filename,  // destination filename
        CHECKSUM_TYPE_MODULAR,  // checksum type
        closureRequested  // closure requested flag
    );

    // Allocate buffer
    status = m_manager->getPduBuffer(buffer, *txn->m_chan, md.getBufferSize());
    if (status == Cfdp::Status::SUCCESS) {
        // Serialize to buffer
        Fw::SerialBuffer sb(buffer.getData(), buffer.getSize());
        Fw::SerializeStatus serStatus = md.serializeTo(sb);
        if (serStatus != Fw::FW_SERIALIZE_OK) {
            // Failed to serialize, return the buffer
            m_manager->log_WARNING_LO_FailMetadataPduSerialization(txn->getChannelId(), static_cast<I32>(serStatus));
            m_manager->returnPduBuffer(*txn->m_chan, buffer);
            status = Cfdp::Status::ERROR;
        } else {
            // Update buffer size to actual serialized size
            buffer.setSize(sb.getSize());
            // Send the PDU
            m_manager->sendPduBuffer(*txn->m_chan, buffer);
        }
    }

    return status;
}

Status::T CfdpEngine::sendFd(CfdpTransaction *txn, FileDataPdu& fdPdu)
{
    Fw::Buffer buffer;
    Status::T status = Cfdp::Status::SUCCESS;

    status = m_manager->getPduBuffer(buffer, *txn->m_chan, fdPdu.getBufferSize());
    if (status == Cfdp::Status::SUCCESS) {
        // Serialize to buffer
        Fw::SerialBuffer sb(buffer.getData(), buffer.getSize());
        Fw::SerializeStatus serStatus = fdPdu.serializeTo(sb);
        if (serStatus != Fw::FW_SERIALIZE_OK) {
            m_manager->log_WARNING_LO_FailFileDataPduSerialization(txn->getChannelId(), static_cast<I32>(serStatus));
            m_manager->returnPduBuffer(*txn->m_chan, buffer);
            status = Cfdp::Status::ERROR;
        } else {
            // Update buffer size to actual serialized size
            buffer.setSize(sb.getSize());
            m_manager->sendPduBuffer(*txn->m_chan, buffer);
        }
    }

    return status;
}

Status::T CfdpEngine::sendEof(CfdpTransaction *txn)
{
    Fw::Buffer buffer;
    Status::T status = Cfdp::Status::SUCCESS;

    // Create and initialize EOF PDU
    EofPdu eof;

    // Direction is toward receiver for EOF sent by sender
    Cfdp::PduDirection direction = DIRECTION_TOWARD_RECEIVER;
    ConditionCode conditionCode = static_cast<ConditionCode>(CfdpTxnStatusToConditionCode(txn->m_history->txn_stat));

    eof.initialize(
        direction,
        txn->getClass(),  // transmission mode
        m_manager->getLocalEidParam(),  // source EID
        txn->m_history->seq_num,  // transaction sequence number
        txn->m_history->peer_eid,  // destination EID
        conditionCode,  // condition code
        txn->m_crc.getValue(),  // checksum
        txn->m_fsize  // file size
    );

    // Add entity ID TLV on error conditions (optional per CCSDS spec)
    if (conditionCode != CONDITION_CODE_NO_ERROR) {
        Cfdp::Tlv tlv;
        tlv.initialize(m_manager->getLocalEidParam());  // Local entity ID
        eof.appendTlv(tlv);
    }

    // Allocate buffer
    status = m_manager->getPduBuffer(buffer, *txn->m_chan, eof.getBufferSize());
    if (status == Cfdp::Status::SUCCESS) {
        // Serialize to buffer
        Fw::SerialBuffer sb(buffer.getData(), buffer.getSize());
        Fw::SerializeStatus serStatus = eof.serializeTo(sb);
        if (serStatus != Fw::FW_SERIALIZE_OK) {
            // Failed to serialize, return the buffer
            m_manager->log_WARNING_LO_FailEofPduSerialization(txn->getChannelId(), static_cast<I32>(serStatus));
            m_manager->returnPduBuffer(*txn->m_chan, buffer);
            status = Cfdp::Status::ERROR;
        } else {
            // Update buffer size to actual serialized size
            buffer.setSize(sb.getSize());
            // Send the PDU
            m_manager->sendPduBuffer(*txn->m_chan, buffer);
        }
    }

    return status;
}

Status::T CfdpEngine::sendAck(CfdpTransaction *txn, AckTxnStatus ts, FileDirective dir_code,
                             ConditionCode cc, EntityId peer_eid, TransactionSeq tsn)
{
    Fw::Buffer buffer;
    Status::T status = Cfdp::Status::SUCCESS;

    FW_ASSERT((dir_code == FILE_DIRECTIVE_END_OF_FILE) || (dir_code == FILE_DIRECTIVE_FIN), dir_code);

    // Determine source and destination EIDs based on transaction direction
    EntityId src_eid;
    EntityId dst_eid;
    if (txn->getHistory()->dir == DIRECTION_TX)
    {
        src_eid = m_manager->getLocalEidParam();
        dst_eid = peer_eid;
    }
    else
    {
        src_eid = peer_eid;
        dst_eid = m_manager->getLocalEidParam();
    }

    // Create and initialize ACK PDU
    AckPdu ack;

    // Direction: toward sender for EOF ACK, toward receiver for FIN ACK
    Cfdp::PduDirection direction = (dir_code == FILE_DIRECTIVE_END_OF_FILE) ?
                                 Cfdp::DIRECTION_TOWARD_SENDER : Cfdp::DIRECTION_TOWARD_RECEIVER;

    ack.initialize(
        direction,
        txn->getClass(),  // transmission mode
        src_eid,  // source EID
        tsn,  // transaction sequence number
        dst_eid,  // destination EID
        dir_code,  // directive being acknowledged
        1,  // directive subtype code (always 1)
        cc,  // condition code
        ts  // transaction status
    );

    // Allocate buffer
    status = m_manager->getPduBuffer(buffer, *txn->m_chan, ack.getBufferSize());
    if (status == Cfdp::Status::SUCCESS) {
        // Serialize to buffer
        Fw::SerialBuffer sb(buffer.getData(), buffer.getSize());
        Fw::SerializeStatus serStatus = ack.serializeTo(sb);
        if (serStatus != Fw::FW_SERIALIZE_OK) {
            // Failed to serialize, return the buffer
            m_manager->log_WARNING_LO_FailAckPduSerialization(txn->getChannelId(), static_cast<I32>(serStatus));
            m_manager->returnPduBuffer(*txn->m_chan, buffer);
            status = Cfdp::Status::ERROR;
        } else {
            // Update buffer size to actual serialized size
            buffer.setSize(sb.getSize());
            // Send the PDU
            m_manager->sendPduBuffer(*txn->m_chan, buffer);
        }
    }

    return status;
}

Status::T CfdpEngine::sendFin(CfdpTransaction *txn, FinDeliveryCode dc, FinFileStatus fs,
                             ConditionCode cc)
{
    Fw::Buffer buffer;
    Status::T status = Cfdp::Status::SUCCESS;

    // Create and initialize FIN PDU
    FinPdu fin;

    // Direction is toward sender for FIN sent by receiver
    Cfdp::PduDirection direction = DIRECTION_TOWARD_SENDER;

    fin.initialize(
        direction,
        txn->getClass(),  // transmission mode
        txn->m_history->peer_eid,  // source EID (receiver)
        txn->m_history->seq_num,  // transaction sequence number
        m_manager->getLocalEidParam(),  // destination EID (sender)
        cc,  // condition code
        static_cast<FinDeliveryCode>(dc),  // delivery code
        static_cast<FinFileStatus>(fs)  // file status
    );

    // Add entity ID TLV on error conditions (optional per CCSDS spec)
    if (cc != CONDITION_CODE_NO_ERROR) {
        Cfdp::Tlv tlv;
        tlv.initialize(m_manager->getLocalEidParam());  // Local entity ID
        fin.appendTlv(tlv);
    }

    // Allocate buffer
    status = m_manager->getPduBuffer(buffer, *txn->m_chan, fin.getBufferSize());
    if (status == Cfdp::Status::SUCCESS) {
        // Serialize to buffer
        Fw::SerialBuffer sb(buffer.getData(), buffer.getSize());
        Fw::SerializeStatus serStatus = fin.serializeTo(sb);
        if (serStatus != Fw::FW_SERIALIZE_OK) {
            // Failed to serialize, return the buffer
            m_manager->log_WARNING_LO_FailFinPduSerialization(txn->getChannelId(), static_cast<I32>(serStatus));
            m_manager->returnPduBuffer(*txn->m_chan, buffer);
            status = Cfdp::Status::ERROR;
        } else {
            // Update buffer size to actual serialized size
            buffer.setSize(sb.getSize());
            // Send the PDU
            m_manager->sendPduBuffer(*txn->m_chan, buffer);
        }
    }

    return status;
}

Status::T CfdpEngine::sendNak(CfdpTransaction *txn, NakPdu& nakPdu)
{
    Fw::Buffer buffer;
    Status::T status = Cfdp::Status::SUCCESS;

    // Verify this is a Class 2 transaction (NAK only used in Class 2)
    Class::T tx_class = txn->getClass();
    FW_ASSERT(tx_class == Cfdp::Class::CLASS_2, tx_class);

    status = m_manager->getPduBuffer(buffer, *txn->m_chan, nakPdu.getBufferSize());
    if (status == Cfdp::Status::SUCCESS) {
        // Serialize to buffer
        Fw::SerialBuffer sb(buffer.getData(), buffer.getSize());
        Fw::SerializeStatus serStatus = nakPdu.serializeTo(sb);
        if (serStatus != Fw::FW_SERIALIZE_OK) {
            m_manager->log_WARNING_LO_FailNakPduSerialization(txn->getChannelId(), static_cast<I32>(serStatus));
            m_manager->returnPduBuffer(*txn->m_chan, buffer);
            status = Cfdp::Status::ERROR;
        } else {
            // Update buffer size to actual serialized size
            buffer.setSize(sb.getSize());
            m_manager->sendPduBuffer(*txn->m_chan, buffer);
        }
    }

    return status;
}

void CfdpEngine::recvMd(CfdpTransaction *txn, const MetadataPdu& md)
{
    /* store the expected file size in transaction */
    txn->m_fsize = md.getFileSize();

    /* store the filenames in transaction - validation already done during deserialization */
    txn->m_history->fnames.src_filename = md.getSourceFilename();
    txn->m_history->fnames.dst_filename = md.getDestFilename();

    // CFE_EVS_SendEvent(CF_PDU_MD_RECVD_INF_EID, CFE_EVS_EventType_INFORMATION,
    //                   "CF: md received for source: %s, dest: %s", txn->m_history->fnames.src_filename.toChar(),
    //                   txn->m_history->fnames.dst_filename.toChar());
}

Status::T CfdpEngine::recvFd(CfdpTransaction *txn, const FileDataPdu& fd)
{
    Status::T ret = Cfdp::Status::SUCCESS;

    // Extract header
    const Cfdp::PduHeader& header = fd.asHeader();

    // Check for segment metadata flag (not currently supported)
    if (header.hasSegmentMetadata())
    {
        /* If recv PDU has the "segment_meta_flag" set, this is not currently handled in CF. */
        // CFE_EVS_SendEvent(CF_PDU_FD_UNSUPPORTED_ERR_EID, CFE_EVS_EventType_ERROR,
        //                   "CF: filedata PDU with segment metadata received");
        this->setTxnStatus(txn, TXN_STATUS_PROTOCOL_ERROR);
        // ++CF_AppData.hk.Payload.channel_hk[txn->getChannelId()].counters.recv.error;
        ret = Cfdp::Status::ERROR;
    }

    return ret;
}

Status::T CfdpEngine::recvEof(CfdpTransaction *txn, const EofPdu& eofPdu)
{
    // EOF PDU has been validated during fromBuffer()

    // Process TLVs if present
    const Cfdp::TlvList& tlvList = eofPdu.getTlvList();
    for (U8 i = 0; i < tlvList.getNumTlv(); i++) {
        const Cfdp::Tlv& tlv = tlvList.getTlv(i);
        if (tlv.getType() == Cfdp::TLV_TYPE_ENTITY_ID) {
            // Entity ID TLV present - could validate entity ID matches expected
            // Future enhancement: Add validation or logging
            // TODO BPC: What does GSW what to do with these if anything
        }
        // Other TLV types can be processed here in the future
    }

    return Cfdp::Status::SUCCESS;
}

Status::T CfdpEngine::recvAck(CfdpTransaction *txn, const AckPdu& pdu)
{
    // ACK PDU has been validated during fromBuffer()
    return Cfdp::Status::SUCCESS;
}

Status::T CfdpEngine::recvFin(CfdpTransaction *txn, const FinPdu& finPdu)
{
    // FIN PDU has been validated during fromBuffer()

    // Process TLVs if present
    const Cfdp::TlvList& tlvList = finPdu.getTlvList();
    for (U8 i = 0; i < tlvList.getNumTlv(); i++) {
        const Cfdp::Tlv& tlv = tlvList.getTlv(i);
        if (tlv.getType() == Cfdp::TLV_TYPE_ENTITY_ID) {
            // Entity ID TLV present - could validate entity ID matches expected
            // Future enhancement: Add validation or logging
            // TODO BPC: What does GSW what to do with these if anything
        }
        // Other TLV types can be processed here in the future
    }

    return Cfdp::Status::SUCCESS;
}

Status::T CfdpEngine::recvNak(CfdpTransaction *txn, const NakPdu& pdu)
{
    // NAK PDU has been validated during fromBuffer()
    return Cfdp::Status::SUCCESS;
}

void CfdpEngine::recvDrop(CfdpTransaction *txn, const Fw::Buffer& buffer)
{
    // ++CF_AppData.hk.Payload.channel_hk[txn->getChannelId()].counters.recv.dropped;
    (void)buffer;  // Unused - we're just dropping the PDU
}

void CfdpEngine::recvHold(CfdpTransaction *txn, const Fw::Buffer& buffer)
{
    /* anything received in this state is considered spurious */
    // ++CF_AppData.hk.Payload.channel_hk[txn->getChannelId()].counters.recv.spurious;

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

    // Use peekPduType to determine the PDU type
    Cfdp::PduTypeEnum pduType = Cfdp::peekPduType(buffer);

    // Check if this is a FIN PDU for a Class 2 transaction
    if (pduType == Cfdp::T_FIN &&
        txn->getClass() == Cfdp::Class::CLASS_2) {

        // Deserialize FIN PDU
        FinPdu fin;
        Fw::SerialBuffer sb2(const_cast<U8*>(buffer.getData()), buffer.getSize());
        sb2.setBuffLen(buffer.getSize());

        Fw::SerializeStatus deserStatus = fin.deserializeFrom(sb2);
        if (deserStatus == Fw::FW_SERIALIZE_OK) {
            // Re-send the FIN-ACK
            this->sendAck(txn, ACK_TXN_STATUS_TERMINATED,
                         FILE_DIRECTIVE_FIN,
                         fin.getConditionCode(),
                         txn->m_history->peer_eid,
                         txn->m_history->seq_num);
        }
        // Note: Deserialization errors are silently ignored in hold state
        // as we're just trying to be helpful by re-acking FIN if we can
    }
}

void CfdpEngine::recvInit(CfdpTransaction *txn, const Fw::Buffer& buffer)
{
    // Use peekPduType to determine the PDU type before deserializing
    Cfdp::PduTypeEnum pduType = Cfdp::peekPduType(buffer);

    // First parse header to get transaction information
    Fw::SerialBuffer sb(const_cast<U8*>(buffer.getData()), buffer.getSize());
    sb.setBuffLen(buffer.getSize());

    Cfdp::PduHeader header;
    Fw::SerializeStatus status = header.fromSerialBuffer(sb);

    if (status == Fw::FW_SERIALIZE_OK) {
        TransactionSeq transactionSeq = header.getTransactionSeq();
        EntityId sourceEid = header.getSourceEid();
        Class::T txmMode = header.getTxmMode();

        /* only RX transactions dare tread here */
        txn->m_history->seq_num = transactionSeq;

        /* peer_eid is always the remote partner. src_eid is always the transaction source.
         * in this case, they are the same */
        txn->m_history->peer_eid = sourceEid;
        txn->m_history->src_eid  = sourceEid;

        /* all RX transactions will need a chunk list to track file segments */
        if (txn->m_chunks == NULL)
        {
            txn->m_chunks = txn->m_chan->findUnusedChunks(DIRECTION_RX);
        }
        if (txn->m_chunks == NULL)
        {
            // CFE_EVS_SendEvent(CF_CFDP_NO_CHUNKLIST_AVAIL_EID, CFE_EVS_EventType_ERROR,
            //                   "CF: cannot get chunklist -- abandoning transaction %u\n",
            //                  (unsigned int)transactionSeq);
        }
        else
        {
            if (pduType == Cfdp::T_FILE_DATA)
            {
                /* file data PDU */
                /* being idle and receiving a file data PDU means that no active transaction knew
                 * about the transaction in progress, so most likely PDUs were missed. */

                if (txmMode == Cfdp::Class::CLASS_1)
                {
                    /* R1, can't do anything without metadata first */
                    txn->m_state = TXN_STATE_DROP; /* drop all incoming */
                    /* use inactivity timer to ultimately free the state */
                }
                else
                {
                    /* R2 can handle missing metadata, so go ahead and create a temp file */
                    txn->m_state = TXN_STATE_R2;
                    txn->m_txn_class = Cfdp::Class::CLASS_2;
                    txn->rInit();
                    this->dispatchRecv(txn, buffer); /* re-dispatch to enter r2 */
                }
            }
            else if (pduType == Cfdp::T_METADATA)
            {
                /* file directive PDU with metadata - this is the expected case for starting a new RX transaction */
                MetadataPdu md;
                Fw::SerialBuffer sb2(const_cast<U8*>(buffer.getData()), buffer.getSize());
                sb2.setBuffLen(buffer.getSize());

                Fw::SerializeStatus deserStatus = md.deserializeFrom(sb2);
                if (deserStatus == Fw::FW_SERIALIZE_OK)
                {
                    this->recvMd(txn, md);

                    /* NOTE: whether or not class 1 or 2, get a free chunks. It's cheap, and simplifies cleanup path */
                    txn->m_state = txmMode == Cfdp::Class::CLASS_1 ? TXN_STATE_R1 : TXN_STATE_R2;
                    txn->m_txn_class = txmMode;
                    txn->m_flags.rx.md_recv = true;
                    txn->rInit(); /* initialize R */
                }
                else
                {
                    m_manager->log_WARNING_LO_FailMetadataPduDeserialization(txn->getChannelId(), static_cast<I32>(deserStatus));
                }
            }
            else
            {
                // Unexpected PDU type in init state
                // CFE_EVS_SendEvent(CF_CFDP_FD_UNHANDLED_ERR_EID, CFE_EVS_EventType_ERROR,
                //                   "CF: unhandled PDU type in idle state");
                // ++CF_AppData.hk.Payload.channel_hk[txn->getChannelId()].counters.recv.error;
            }
        }

        if (txn->m_state == TXN_STATE_INIT)
        {
            /* state was not changed, so free the transaction */
            this->finishTransaction(txn, false);
        }
    } else {
        m_manager->log_WARNING_LO_FailPduHeaderDeserialization(txn->getChannelId(), status);
    }
}

void CfdpEngine::receivePdu(U8 chan_id, const Fw::Buffer& buffer)
{
    CfdpTransaction *txn = NULL;
    CfdpChannel *chan = NULL;

    FW_ASSERT(chan_id < CFDP_NUM_CHANNELS, chan_id, CFDP_NUM_CHANNELS);

    chan = m_channels[chan_id];
    FW_ASSERT(chan != NULL);

    // Parse the header to get transaction routing info
    // Avoid full PDU deserialization here to defer it until the appropriate handler
    Fw::SerialBuffer sb(const_cast<U8*>(buffer.getData()), buffer.getSize());
    sb.setBuffLen(buffer.getSize());

    Cfdp::PduHeader header;
    Fw::SerializeStatus status = header.fromSerialBuffer(sb);

    if (status == Fw::FW_SERIALIZE_OK) {
        TransactionSeq transactionSeq = header.getTransactionSeq();
        EntityId sourceEid = header.getSourceEid();
        EntityId destEid = header.getDestEid();

        // Look up transaction by sequence number
        txn = chan->findTransactionBySequenceNumber(transactionSeq, sourceEid);

        if (txn == NULL)
        {
            /* if no match found, then it must be the case that we would be the destination entity id, so verify it
                */
            if (destEid == this->m_manager->getLocalEidParam())
            {
                /* we didn't find a match, so assign it to a transaction */
                /* assume this is initiating an RX transaction, as TX transactions are only commanded */
                txn = this->startRxTransaction(chan->getChannelId());
                if (txn == NULL)
                {
                    // CFE_EVS_SendEvent(
                    //     CF_CFDP_RX_DROPPED_ERR_EID, CFE_EVS_EventType_ERROR,
                    //     "CF: dropping packet from %lu transaction number 0x%08lx due max RX transactions reached",
                    //     (unsigned long)sourceEid, (unsigned long)transactionSeq);
                }
            }
            else
            {
                // CFE_EVS_SendEvent(CF_CFDP_INVALID_DST_ERR_EID, CFE_EVS_EventType_ERROR,
                //                   "CF: dropping packet for invalid destination eid 0x%lx",
                //                   (unsigned long)destEid);
            }
        }

        if (txn != NULL)
        {
            /* found one! Send it to the transaction state processor */
            this->dispatchRecv(txn, buffer);
        }
        else
        {
            // TODO BPC: Add throttled EVR
        }
    } else {
        // Invalid PDU header, drop packet
        m_manager->log_WARNING_LO_FailPduHeaderDeserialization(chan_id, static_cast<I32>(status));
    }
}


void CfdpEngine::setChannelFlowState(U8 channelId, Flow::T flowState)
{
    FW_ASSERT(channelId <= CFDP_NUM_CHANNELS, channelId, CFDP_NUM_CHANNELS);
    m_channels[channelId]->setFlowState(flowState);
}

void CfdpEngine::txFileInitiate(CfdpTransaction *txn, Class::T cfdp_class, Keep::T keep, U8 chan,
                             U8 priority, EntityId dest_id)
{
    // CFE_EVS_SendEvent(CF_CFDP_S_START_SEND_INF_EID, CFE_EVS_EventType_INFORMATION,
    //                   "CF: start class %d tx of file %lu:%.*s -> %lu:%.*s", cfdp_class + 1,
    //                   (unsigned long)m_manager->getLocalEidParam(), CFDP_FILENAME_MAX_LEN,
    //                   txn->m_history->fnames.src_filename, (unsigned long)dest_id, CFDP_FILENAME_MAX_LEN,
    //                   txn->m_history->fnames.dst_filename);

    txn->initTxFile(cfdp_class, keep, chan, priority);

    /* Increment sequence number for new transaction */
    ++this->m_seqNum;

    /* Capture info for history */
    txn->m_history->seq_num  = this->m_seqNum;
    txn->m_history->src_eid  = m_manager->getLocalEidParam();
    txn->m_history->peer_eid = dest_id;

    txn->m_chan->insertSortPrio(txn, QueueId::PEND);
}

Status::T CfdpEngine::txFile(const Fw::String& src_filename, const Fw::String& dst_filename,
                             Class::T cfdp_class, Keep::T keep, U8 chan_num,
                             U8 priority, EntityId dest_id)
{
    CfdpTransaction *txn;
    CfdpChannel* chan = nullptr;

    FW_ASSERT(chan_num < CFDP_NUM_CHANNELS, chan_num, CFDP_NUM_CHANNELS);
    chan = m_channels[chan_num];

    Status::T ret = Cfdp::Status::SUCCESS;

    if (chan->getNumCmdTx() < CFDP_MAX_COMMANDED_PLAYBACK_FILES_PER_CHAN)
    {
        txn = chan->findUnusedTransaction(DIRECTION_TX);
    }
    else
    {
        txn = NULL;
    }

    if (txn == NULL)
    {
        // CFE_EVS_SendEvent(CF_CFDP_MAX_CMD_TX_ERR_EID, CFE_EVS_EventType_ERROR,
        //                   "CF: max number of commanded files reached");
        ret = Cfdp::Status::ERROR;
    }
    else
    {
        /* NOTE: the caller of this function ensures the provided src and dst filenames are NULL terminated */

        txn->m_history->fnames.src_filename = src_filename;
        txn->m_history->fnames.dst_filename = dst_filename;
        this->txFileInitiate(txn, cfdp_class, keep, chan_num, priority, dest_id);

        chan->incrementCmdTxCounter();
        txn->m_flags.tx.cmd_tx = true;
    }

    return ret;
}

CfdpTransaction *CfdpEngine::startRxTransaction(U8 chan_num)
{
    CfdpChannel *chan = nullptr;
    CfdpTransaction *txn;
    
    FW_ASSERT(chan_num < CFDP_NUM_CHANNELS, chan_num, CFDP_NUM_CHANNELS);
    chan = m_channels[chan_num];

    // if (CF_AppData.hk.Payload.channel_hk[chan_num].q_size[QueueId::RX] < CF_MAX_SIMULTANEOUS_RX)
    // {
    //     txn = chan->findUnusedTransaction(DIRECTION_RX);
    // }
    // else
    // {
    //     txn = NULL;
    // }
    // TODO BPC: Do I need to limit receive transactions?
    txn = chan->findUnusedTransaction(DIRECTION_RX);

    if (txn != NULL)
    {
        /* set default FIN status */
        txn->m_state_data.receive.r2.dc = FIN_DELIVERY_CODE_INCOMPLETE;
        txn->m_state_data.receive.r2.fs = FIN_FILE_STATUS_DISCARDED;

        txn->m_flags.com.q_index = QueueId::RX;
        chan->insertBackInQueue(static_cast<QueueId::T>(txn->m_flags.com.q_index), &txn->m_cl_node);
    }

    return txn;
}

Status::T CfdpEngine::playbackDirInitiate(Playback *pb, const Fw::String& src_filename, const Fw::String& dst_filename,
                                           Class::T cfdp_class, Keep::T keep, U8 chan, U8 priority,
                                           EntityId dest_id)
{
    Status::T status = Cfdp::Status::SUCCESS;
    Os::Directory::Status dirStatus;

    /* make sure the directory can be open */
    dirStatus = pb->dir.open(src_filename.toChar(), Os::Directory::READ);
    if (dirStatus != Os::Directory::OP_OK)
    {
        // CFE_EVS_SendEvent(CF_CFDP_OPENDIR_ERR_EID, CFE_EVS_EventType_ERROR,
        //                   "CF: failed to open playback directory %s, error=%ld", src_filename, (long)ret);
        // ++CF_AppData.hk.Payload.channel_hk[chan].counters.fault.directory_read;
        status = Cfdp::Status::ERROR;
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

Status::T CfdpEngine::playbackDir(const Fw::String& src_filename, const Fw::String& dst_filename, Class::T cfdp_class,
                                  Keep::T keep, U8 chan, U8 priority, EntityId dest_id)
{
    int i;
    Playback *pb;
    Status::T status;

    // Loop through the channel's playback directories to find an open slot
    for (i = 0; i < CFDP_MAX_COMMANDED_PLAYBACK_DIRECTORIES_PER_CHAN; ++i)
    {
        pb = m_channels[chan]->getPlayback(i);
        if (!pb->busy)
        {
            break;
        }
    }

    if (i == CFDP_MAX_COMMANDED_PLAYBACK_DIRECTORIES_PER_CHAN)
    {
        // CFE_EVS_SendEvent(CF_CFDP_DIR_SLOT_ERR_EID, CFE_EVS_EventType_ERROR, "CF: no playback dir slot available");
        status = Cfdp::Status::ERROR;
    }
    else
    {
        status = this->playbackDirInitiate(pb, src_filename, dst_filename, cfdp_class, keep, chan, priority, dest_id);
    }

    return status;
}

Status::T CfdpEngine::startPollDir(U8 chanId, U8 pollId, const Fw::String& srcDir, const Fw::String& dstDir,
                                     Class::T cfdp_class, U8 priority, EntityId destEid,
                                     U32 intervalSec)
{
    Status::T status = Cfdp::Status::SUCCESS;
    CfdpPollDir* pd = NULL;

    FW_ASSERT(chanId < CFDP_NUM_CHANNELS, chanId, CFDP_NUM_CHANNELS);
    FW_ASSERT(pollId < CFDP_MAX_POLLING_DIR_PER_CHAN, pollId, CFDP_MAX_POLLING_DIR_PER_CHAN);

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
        status = Cfdp::Status::ERROR;
    }

    return status;
}

Status::T CfdpEngine::stopPollDir(U8 chanId, U8 pollId)
{
    Status::T status = Cfdp::Status::SUCCESS;
    CfdpPollDir* pd = NULL;

    FW_ASSERT(chanId < CFDP_NUM_CHANNELS, chanId, CFDP_NUM_CHANNELS);
    FW_ASSERT(pollId < CFDP_MAX_POLLING_DIR_PER_CHAN, pollId, CFDP_MAX_POLLING_DIR_PER_CHAN);

    // Check if the poll directory is in use
    pd = m_channels[chanId]->getPollDir(pollId);
    if(pd->enabled == Fw::Enabled::DISABLED)
    {
        // Clear poll directory arguments
        pd->intervalSec = 0;
        pd->priority = 0;
        pd->cfdpClass = static_cast<Class::T>(0);
        pd->destEid = static_cast<EntityId>(0);
        pd->srcDir = "";
        pd->dstDir = "";
        
        // Disable timer and polling
        pd->intervalTimer.disableTimer();
        pd->enabled = Fw::Enabled::DISABLED;
    }
    else
    {
        // TODO BPC: emit EVR here
        status = Cfdp::Status::ERROR;
    }

    return status;
}

void CfdpEngine::cycle(void)
{
    int i;

    for (i = 0; i < CFDP_NUM_CHANNELS; ++i)
    {
        CfdpChannel* chan = m_channels[i];
        FW_ASSERT(chan != nullptr);

        chan->resetOutgoingCounter();

        if (chan->getFlowState() == Cfdp::Flow::NOT_FROZEN)
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

void CfdpEngine::finishTransaction(CfdpTransaction *txn, bool keep_history)
{
    if (txn->m_flags.com.q_index == QueueId::FREE)
    {
        // CFE_EVS_SendEvent(CF_RESET_FREED_XACT_DBG_EID, CFE_EVS_EventType_DEBUG,
        //                   "CF: attempt to reset a transaction that has already been freed");
        return;
    }

    /* this should always be */
    FW_ASSERT(txn->m_chan != NULL);

    /* If this was on the TXA queue (transmit side) then we need to move it out
     * so the tick processor will stop trying to actively transmit something -
     * it should move on to the next transaction.
     *
     * RX transactions can stay on the RX queue, that does not hurt anything
     * because they are only triggered when a PDU comes in matching that seq_num
     * (RX queue is not separated into A/W parts) */
    if (txn->m_flags.com.q_index == QueueId::TXA)
    {
        txn->m_chan->dequeueTransaction(txn);
        txn->m_chan->insertSortPrio(txn, QueueId::TXW);
    }

    if (true == txn->m_fd.isOpen())
    {
        txn->m_fd.close();

        if (!txn->m_keep)
        {
            this->handleNotKeepFile(txn);
        }
    }

    if (txn->m_history != NULL)
    {
        this->sendEotPkt(txn);

        /* extra bookkeeping for tx direction only */
        if (txn->m_history->dir == DIRECTION_TX && txn->m_flags.tx.cmd_tx)
        {
            txn->m_chan->decrementCmdTxCounter();
        }

        txn->m_flags.com.keep_history = keep_history;
    }

    if (txn->m_pb)
    {
        /* a playback's transaction is now done, decrement the playback counter */
        FW_ASSERT(txn->m_pb->num_ts);
        --txn->m_pb->num_ts;
    }

    txn->m_chan->clearCurrentIfMatch(txn);

    /* Put this transaction into the holdover state, inactivity timer will recycle it */
    txn->m_state = TXN_STATE_HOLD;
    this->armInactTimer(txn);
}

void CfdpEngine::setTxnStatus(CfdpTransaction *txn, TxnStatus txn_stat)
{
    if (!CfdpTxnStatusIsError(txn->m_history->txn_stat))
    {
        txn->m_history->txn_stat = txn_stat;
    }
}

void CfdpEngine::sendEotPkt(CfdpTransaction *txn)
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

    //     EotPktPtr->Payload.channel    = txn->getChannelId();
    //     EotPktPtr->Payload.direction  = txn->m_history->dir;
    //     EotPktPtr->Payload.fnames     = txn->m_history->fnames;
    //     EotPktPtr->Payload.state      = txn->m_state;
    //     EotPktPtr->Payload.txn_stat   = txn->m_history->txn_stat;
    //     EotPktPtr->Payload.src_eid    = txn->m_history->src_eid;
    //     EotPktPtr->Payload.peer_eid   = txn->m_history->peer_eid;
    //     EotPktPtr->Payload.seq_num    = txn->m_history->seq_num;
    //     EotPktPtr->Payload.fsize      = txn->m_fsize;
    //     EotPktPtr->Payload.crc_result = txn->m_crc.getValue();

    //     /*
    //     ** Timestamp and send eod of transaction telemetry
    //     */
    //     CFE_SB_TimeStampMsg(CFE_MSG_PTR(EotPktPtr->TelemetryHeader));
    //     CFE_SB_TransmitBuffer(BufPtr, true);
    // }
}

void CfdpEngine::cancelTransaction(CfdpTransaction *txn)
{
    void (CfdpTransaction::*fns[DIRECTION_NUM])() = {nullptr};

    fns[DIRECTION_RX] = &CfdpTransaction::rCancel;
    fns[DIRECTION_TX] = &CfdpTransaction::sCancel;

    if (!txn->m_flags.com.canceled)
    {
        txn->m_flags.com.canceled = true;
        this->setTxnStatus(txn, TXN_STATUS_CANCEL_REQUEST_RECEIVED);

        /* this should always be true, just confirming before indexing into array */
        if (txn->m_history->dir < DIRECTION_NUM)
        {
            (txn->*fns[txn->m_history->dir])();
        }
    }
}

bool CfdpEngine::isPollingDir(const char *src_file, U8 chan_num)
{
    bool return_code = false;
    char src_dir[CFDP_FILENAME_MAX_LEN] = "\0";
    CfdpPollDir * pd;
    int i;

    const char* last_slash = strrchr(src_file, '/');
    if (last_slash != NULL)
    {
        strncpy(src_dir, src_file, last_slash - src_file);
    }

    for (i = 0; i < CFDP_MAX_POLLING_DIR_PER_CHAN; ++i)
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

void CfdpEngine::handleNotKeepFile(CfdpTransaction *txn)
{
    Os::FileSystem::Status fileStatus;
    Fw::String failDir;
    Fw::String moveDir;

    /* Sender */
    if (txn->getHistory()->dir == DIRECTION_TX)
    {
        if (!CfdpTxnStatusIsError(txn->getHistory()->txn_stat))
        {
            /* If move directory is defined attempt move */
            moveDir = m_manager->getMoveDirParam(txn->getChannelId());
            if(moveDir.length() > 0)
            {
                fileStatus = Os::FileSystem::moveFile(txn->m_history->fnames.src_filename.toChar(), moveDir.toChar());
                if(fileStatus != Os::FileSystem::OP_OK)
                {
                    // TODO BPC: event interfaces are protected
                    // m_manager->log_WARNING_LO_FailKeepFileMove(txn->m_history->fnames.src_filename,
                    //                                                   moveDir, fileStatus);
                }
            }
        }
        else
        {
            /* file inside an polling directory */
            if (this->isPollingDir(txn->m_history->fnames.src_filename.toChar(), txn->getChannelId()))
            {
                /* If fail directory is defined attempt move */
                failDir = m_manager->getFailDirParam();
                if(failDir.length() > 0)
                {
                    fileStatus = Os::FileSystem::moveFile(txn->m_history->fnames.src_filename.toChar(), failDir.toChar());
                    if(fileStatus != Os::FileSystem::OP_OK)
                    {
                        // TODO BPC: event interfaces are protected
                        // m_manager->log_WARNING_LO_FailPollFileMove(txn->m_history->fnames.src_filename,
                        //                                                   failDir, fileStatus);
                    }
                }
            }
        }
    }
    /* Not Sender */
    else
    {
        fileStatus = Os::FileSystem::removeFile(txn->m_history->fnames.dst_filename.toChar());
        // TODO BPC: emit failure EVR
        (void) fileStatus;
    }
}

}  // namespace Cfdp
}  // namespace Ccsds
}  // namespace Svc
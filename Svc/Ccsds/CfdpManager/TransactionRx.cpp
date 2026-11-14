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

#include <limits>

#include <Fw/Types/SuccessEnumAc.hpp>
#include <Os/FileSystem.hpp>

#include <Svc/Ccsds/CfdpManager/CfdpManager.hpp>
#include <Svc/Ccsds/CfdpManager/Channel.hpp>
#include <Svc/Ccsds/CfdpManager/Chunk.hpp>
#include <Svc/Ccsds/CfdpManager/Engine.hpp>
#include <Svc/Ccsds/CfdpManager/Transaction.hpp>
#include <Svc/Ccsds/CfdpManager/Utils.hpp>

namespace Svc {
namespace Ccsds {
namespace Cfdp {

// ======================================================================
// Construction and Destruction
// ======================================================================

namespace {

//! True for an ASCII letter or digit, independent of the C locale
bool isAsciiAlphanumeric(char c) {
    return ((c >= '0') && (c <= '9')) || ((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z'));
}

//! Lower-case form of an ASCII upper-case letter; any other character is returned unchanged
char asciiToLower(char c) {
    return ((c >= 'A') && (c <= 'Z')) ? static_cast<char>(c - 'A' + 'a') : c;
}

//! Moves pos back over the ASCII letters and digits that end at name[pos - 1]; true if there was at
//! least one. Letters are accepted because CFDP_PRI_ENTITY_ID and CFDP_PRI_TRANSACTION_SEQ are
//! configurable (e.g. hex). pos never goes below 0.
bool skipIdBack(const char* name, FwSizeType& pos) {
    const FwSizeType end = pos;
    for (; (pos > 0) && isAsciiAlphanumeric(name[pos - 1]); --pos) {
        // the loop header moves pos back
    }
    return pos < end;
}

//! True if name[pos - 1] is separator, in which case pos is moved back over it; pos never goes below 0
bool skipSeparatorBack(const char* name, FwSizeType& pos, char separator) {
    const bool found = (pos > 0) && (name[pos - 1] == separator);
    if (found) {
        --pos;
    }
    return found;
}

//! True if the length characters at text equal the lower-case ASCII word, ignoring ASCII case, so that
//! the check also holds on case-insensitive file systems
bool equalsIgnoringCase(const char* text, const char* word, FwSizeType length) {
    bool equal = true;
    for (FwSizeType i = 0; equal && (i < length); ++i) {
        equal = (asciiToLower(text[i]) == word[i]);
    }
    return equal;
}

//! True if the last component of path has the form receive transactions use for their own files:
//! "<name>.cfdp-<id>-<id>-<id>.part" (staging, see Transaction::rFormatStagingPath) or "<id>:<id>.tmp" (temp),
//! compared ignoring ASCII case. All positions are offsets into the last component and stay within it.
bool isReservedRxFileName(const Fw::StringBase& path) {
    static constexpr char STAGING_SUFFIX[] = ".part";
    static constexpr char STAGING_MARKER[] = ".cfdp";
    static constexpr char TEMP_SUFFIX[] = ".tmp";
    static constexpr FwSizeType STAGING_SUFFIX_LENGTH = sizeof(STAGING_SUFFIX) - 1;
    static constexpr FwSizeType STAGING_MARKER_LENGTH = sizeof(STAGING_MARKER) - 1;
    static constexpr FwSizeType TEMP_SUFFIX_LENGTH = sizeof(TEMP_SUFFIX) - 1;
    static constexpr U32 STAGING_ID_FIELDS = 3;

    const char* const text = path.toChar();
    const FwSizeType length = path.length();
    FwSizeType baseStart = 0;
    for (FwSizeType i = 0; i < length; ++i) {
        if (text[i] == '/') {
            baseStart = i + 1;
        }
    }
    const char* const name = &text[baseStart];
    const FwSizeType nameLength = length - baseStart;

    bool reserved = false;
    if ((nameLength > STAGING_SUFFIX_LENGTH) &&
        equalsIgnoringCase(&name[nameLength - STAGING_SUFFIX_LENGTH], STAGING_SUFFIX, STAGING_SUFFIX_LENGTH)) {
        // ".cfdp-<id>-<id>-<id>" immediately before ".part"
        FwSizeType pos = nameLength - STAGING_SUFFIX_LENGTH;
        reserved = true;
        for (U32 field = 0; reserved && (field < STAGING_ID_FIELDS); ++field) {
            reserved = skipIdBack(name, pos) && skipSeparatorBack(name, pos, '-');
        }
        reserved = reserved && (pos >= STAGING_MARKER_LENGTH) &&
                   equalsIgnoringCase(&name[pos - STAGING_MARKER_LENGTH], STAGING_MARKER, STAGING_MARKER_LENGTH);
    } else if ((nameLength > TEMP_SUFFIX_LENGTH) &&
               equalsIgnoringCase(&name[nameLength - TEMP_SUFFIX_LENGTH], TEMP_SUFFIX, TEMP_SUFFIX_LENGTH)) {
        // the whole name is "<id>:<id>" before ".tmp": an id, the ':', an id, then the start of the name
        FwSizeType pos = nameLength - TEMP_SUFFIX_LENGTH;
        reserved = skipIdBack(name, pos) && skipSeparatorBack(name, pos, ':') && skipIdBack(name, pos) && (pos == 0);
    }

    return reserved;
}

}  // namespace

Transaction::Transaction(Channel* channel, U8 channelId, Engine* engine, CfdpManager* manager)
    : m_state(TxnState::TXN_STATE_UNDEF),
      m_txn_class(Cfdp::Class::CLASS_1),
      m_history(nullptr),
      m_chunks(nullptr),
      m_inactivity_timer(),
      m_ack_timer(),
      m_fsize(0),
      m_foffs(0),
      m_fd(),
      m_crc(),
      m_keep(Cfdp::Keep::DELETE),
      m_chan_num(channelId),  // Initialize from parameter
      m_priority(0),
      m_initType(TransactionInitType::INIT_BY_COMMAND),
      m_cl_node{},
      m_pb(nullptr),
      m_state_data{},
      m_flags{},
      m_cfdpManager(manager),  // Initialize from parameter
      m_chan(channel),         // Initialize from parameter
      m_engine(engine)         // Initialize from parameter
{
    // Fully zero the union storage
    memset(&this->m_state_data, 0, sizeof(this->m_state_data));
    memset(&this->m_flags, 0, sizeof(this->m_flags));
}

Transaction::~Transaction() {}

void Transaction::reset() {
    // Reset transaction state to default values
    this->m_state = TxnState::TXN_STATE_UNDEF;
    this->m_txn_class = Cfdp::Class::CLASS_1;
    this->m_fsize = 0;
    this->m_foffs = 0;
    // A received file is kept only once its checksum has been verified; transmit transactions set
    // their own keep value in initTxFile().
    this->m_keep = Cfdp::Keep::DELETE;
    this->m_priority = 0;
    this->m_initType = TransactionInitType::INIT_BY_COMMAND;
    this->m_crc = CFDP::Checksum(0);
    this->m_pb = nullptr;

    // Fully zero the union storage
    memset(&this->m_state_data, 0, sizeof(this->m_state_data));
    memset(&this->m_flags, 0, sizeof(this->m_flags));

    // Close the file if it is open
    if (this->m_fd.isOpen()) {
        this->m_fd.close();
    }

    // Disable timers to ensure clean state for next transaction
    // This prevents stale timers from a previous transaction firing in a new context
    this->m_inactivity_timer.disableTimer();
    this->m_ack_timer.disableTimer();

    // The following state information is PRESERVED across reset (NOT modified):
    // - this->m_cfdpManager  // Channel binding
    // - this->m_chan         // Channel binding
    // - this->m_engine       // Channel binding
    // - this->m_chan_num     // Channel binding
    // - this->m_history      // Assigned when transaction is activated
    // - this->m_chunks       // Assigned when transaction is activated
    // - this->m_cl_node      // Managed by queue operations in freeTransaction()
}

// ======================================================================
// RX State Machine - Public Methods
// ======================================================================

void Transaction::r1Recv(const Fw::Buffer& buffer) {
    static const FileDirectiveDispatchTable r1_fdir_handlers = {{
        nullptr,                         /* CFDP_FileDirective_INVALID_MIN */
        nullptr,                         /* 1 is unused in the CFDP_FileDirective_t enum */
        nullptr,                         /* 2 is unused in the CFDP_FileDirective_t enum */
        nullptr,                         /* 3 is unused in the CFDP_FileDirective_t enum */
        &Transaction::r1SubstateRecvEof, /* CFDP_FileDirective_EOF */
        nullptr,                         /* CFDP_FileDirective_FIN */
        nullptr,                         /* CFDP_FileDirective_ACK */
        nullptr,                         /* CFDP_FileDirective_METADATA */
        nullptr,                         /* CFDP_FileDirective_NAK */
        nullptr,                         /* CFDP_FileDirective_PROMPT */
        nullptr,                         /* 10 is unused in the CFDP_FileDirective_t enum */
        nullptr,                         /* 11 is unused in the CFDP_FileDirective_t enum */
        nullptr,                         /* CFDP_FileDirective_KEEP_ALIVE */
    }};

    static const RSubstateDispatchTable substate_fns = {{
        &r1_fdir_handlers, /* RxSubState::RX_SUB_STATE_FILEDATA */
        &r1_fdir_handlers, /* RxSubState::RX_SUB_STATE_EOF */
        &r1_fdir_handlers, /* RxSubState::RX_SUB_STATE_CLOSEOUT_SYNC */
    }};

    this->rDispatchRecv(buffer, &substate_fns, &Transaction::r1SubstateRecvFileData);
}

void Transaction::r2Recv(const Fw::Buffer& buffer) {
    static const FileDirectiveDispatchTable r2_fdir_handlers_normal = {{
        nullptr,                         /* CFDP_FileDirective_INVALID_MIN */
        nullptr,                         /* 1 is unused in the CFDP_FileDirective_t enum */
        nullptr,                         /* 2 is unused in the CFDP_FileDirective_t enum */
        nullptr,                         /* 3 is unused in the CFDP_FileDirective_t enum */
        &Transaction::r2SubstateRecvEof, /* CFDP_FileDirective_EOF */
        nullptr,                         /* CFDP_FileDirective_FIN */
        nullptr,                         /* CFDP_FileDirective_ACK */
        &Transaction::r2RecvMd,          /* CFDP_FileDirective_METADATA */
        nullptr,                         /* CFDP_FileDirective_NAK */
        nullptr,                         /* CFDP_FileDirective_PROMPT */
        nullptr,                         /* 10 is unused in the CFDP_FileDirective_t enum */
        nullptr,                         /* 11 is unused in the CFDP_FileDirective_t enum */
        nullptr,                         /* CFDP_FileDirective_KEEP_ALIVE */
    }};
    static const FileDirectiveDispatchTable r2_fdir_handlers_finack = {{
        nullptr,                         /* CFDP_FileDirective_INVALID_MIN */
        nullptr,                         /* 1 is unused in the CFDP_FileDirective_t enum */
        nullptr,                         /* 2 is unused in the CFDP_FileDirective_t enum */
        nullptr,                         /* 3 is unused in the CFDP_FileDirective_t enum */
        &Transaction::r2SubstateRecvEof, /* CFDP_FileDirective_EOF */
        nullptr,                         /* CFDP_FileDirective_FIN */
        &Transaction::r2RecvFinAck,      /* CFDP_FileDirective_ACK */
        nullptr,                         /* CFDP_FileDirective_METADATA */
        nullptr,                         /* CFDP_FileDirective_NAK */
        nullptr,                         /* CFDP_FileDirective_PROMPT */
        nullptr,                         /* 10 is unused in the CFDP_FileDirective_t enum */
        nullptr,                         /* 11 is unused in the CFDP_FileDirective_t enum */
        nullptr,                         /* CFDP_FileDirective_KEEP_ALIVE */
    }};

    static const RSubstateDispatchTable substate_fns = {{
        &r2_fdir_handlers_normal, /* RxSubState::RX_SUB_STATE_FILEDATA */
        &r2_fdir_handlers_normal, /* RxSubState::RX_SUB_STATE_EOF */
        &r2_fdir_handlers_finack, /* RxSubState::RX_SUB_STATE_CLOSEOUT_SYNC */
    }};

    this->rDispatchRecv(buffer, &substate_fns, &Transaction::r2SubstateRecvFileData);
}

void Transaction::rAckTimerTick() {
    U8 ack_limit = 0;

    /* note: the ack timer is only ever armed on class 2 */
    if (this->m_state != TxnState::TXN_STATE_R2 || !this->m_flags.com.ack_timer_armed) {
        /* nothing to do */
        return;
    }

    if (this->m_ack_timer.getStatus() == Timer::Status::RUNNING) {
        this->m_ack_timer.run();
    } else {
        /* ACK timer expired, so check for completion */
        if (!this->m_flags.rx.complete) {
            this->r2Complete(true);
        } else if (this->m_state_data.receive.sub_state == RxSubState::RX_SUB_STATE_CLOSEOUT_SYNC) {
            /* Increment acknak counter */
            ++this->m_state_data.receive.r2.acknak_count;

            /* Check limit and handle if needed */
            ack_limit = this->m_cfdpManager->getAckLimitParam(this->m_chan_num);
            if (this->m_state_data.receive.r2.acknak_count >= ack_limit) {
                this->m_cfdpManager->log_WARNING_LO_RxAckLimitReached(this->getClass(), this->m_history->src_eid,
                                                                      this->m_history->seq_num);
                this->m_engine->setTxnStatus(this, TxnStatus::TXN_STATUS_ACK_LIMIT_NO_FIN);
                this->m_cfdpManager->incrementFaultAckLimit(this->m_chan_num);

                /* give up on this */
                this->m_engine->finishTransaction(this, true);
                this->m_flags.com.ack_timer_armed = false;
            } else {
                this->m_flags.rx.send_fin = true;
            }
        }

        /* re-arm the timer if it is still pending */
        if (this->m_flags.com.ack_timer_armed) {
            /* whether sending FIN or waiting for more filedata, need ACK timer armed */
            this->m_engine->armAckTimer(this);
        }
    }
}

void Transaction::rTick(I32* cont /* unused */) {
    /* Steven is not real happy with this function. There should be a better way to separate out
     * the logic by state so that it isn't a bunch of if statements for different flags
     */

    Status::T sret;
    bool pending_send;

    if (!this->m_flags.com.inactivity_fired) {
        if (this->m_inactivity_timer.getStatus() == Timer::Status::RUNNING) {
            this->m_inactivity_timer.run();
            // Check if timer just expired naturally (after run())
            if (this->m_inactivity_timer.getStatus() == Timer::Status::EXPIRED) {
                this->m_flags.com.inactivity_fired = true;

                /* HOLD state is the normal path to recycle transaction objects, not an error */
                /* Canceled transactions timing out is also normal */
                /* inactivity is abnormal in any other state */
                if (this->m_state != TxnState::TXN_STATE_HOLD && !this->m_flags.com.canceled) {
                    this->rSendInactivityEvent();

                    /* in class 2 this also triggers sending an early FIN response */
                    if (this->m_state == TxnState::TXN_STATE_R2) {
                        this->r2SetFinTxnStatus(TxnStatus::TXN_STATUS_INACTIVITY_DETECTED);
                    }
                }
            }
        }
    }

    pending_send = true; /* maybe; tbd */

    /* rx maintenance: possibly process send_eof_ack, send_nak or send_fin */
    if (this->m_flags.rx.send_eof_ack) {
        sret = this->m_engine->sendAck(this, AckTxnStatus::ACK_TXN_STATUS_ACTIVE,
                                       FileDirective::FILE_DIRECTIVE_END_OF_FILE,
                                       static_cast<ConditionCode>(this->m_state_data.receive.r2.eof_cc),
                                       this->m_history->peer_eid, this->m_history->seq_num);

        /* if SUCCESS, move on. If NO_BUF_AVAIL, retry later. If ERROR, stop retrying. */
        if (sret == Cfdp::Status::SUCCESS) {
            this->m_flags.rx.send_eof_ack = false;
        } else if (sret == Cfdp::Status::ERROR) {
            /* Serialization failed - error already logged in serializeAndSendPdu */
            /* Clear flag to avoid infinite retry loop */
            this->m_flags.rx.send_eof_ack = false;
            pending_send = false;
        }
        /* else NO_BUF_AVAIL: leave flag set to retry next tick */
    } else if (this->m_flags.rx.send_nak) {
        if (!this->rSubstateSendNak()) {
            this->m_flags.rx.send_nak = false; /* will re-enter on error */
        }
    } else if (this->m_flags.rx.send_fin) {
        if (!this->r2SubstateSendFin()) {
            this->m_flags.rx.send_fin = false; /* will re-enter on error */
        }
    } else {
        /* no pending responses to the sender */
        pending_send = false;
    }

    /* if the inactivity timer ran out, then there is no sense
     * pending for responses for anything.  Send out anything
     * that we need to send (i.e. the FIN) just in case the sender
     * is still listening to us but do not expect any future ACKs.
     *
     * Bound the deferral like the TX path: after a small retry budget, recycle regardless. */
    bool retries_exhausted = false;
    if (this->m_flags.com.inactivity_fired && pending_send) {
        if (this->m_flags.com.post_inactivity_send_retries >=
            this->m_cfdpManager->getPostInactivitySendRetriesParam()) {
            retries_exhausted = true;
        } else {
            this->m_flags.com.post_inactivity_send_retries++;
        }
    }
    if (this->m_flags.com.inactivity_fired && (!pending_send || retries_exhausted)) {
        /* the transaction is now recyclable - this means we will
         * no longer have a record of this transaction seq.  If the sender
         * wakes up or if the network delivers severely delayed PDUs at
         * some future point, then they will be seen as spurious.  They
         * will no longer be associable with this transaction at all */
        if ((this->m_state == TxnState::TXN_STATE_R1) || (this->m_state == TxnState::TXN_STATE_R2)) {
            /* the reception never finished: report it as failed and dispose of its file (removed
             * unless its checksum was already verified) before the slot is recycled */
            this->m_engine->setTxnStatus(this, TxnStatus::TXN_STATUS_INACTIVITY_DETECTED);
            this->m_engine->finishTransaction(this, true);
        }
        this->m_chan->recycleTransaction(this);

        /* NOTE: this must be the last thing in here.  Do not use txn after this */
    } else {
        /* transaction still valid so process the ACK timer, if relevant */
        this->rAckTimerTick();
    }
}

void Transaction::rCancel() {
    /* for cancel, only need to send FIN if R2 */
    if ((this->m_state == TxnState::TXN_STATE_R2) &&
        (this->m_state_data.receive.sub_state < RxSubState::RX_SUB_STATE_CLOSEOUT_SYNC)) {
        this->m_flags.rx.send_fin = true;
    } else {
        this->r1Reset(); /* if R1, just call it quits */
    }
}

void Transaction::rInit() {
    Os::File::Status status = Os::File::OTHER_ERROR;
    Fw::String tmpDir;
    Fw::String path;
    RxFileOwned owned = RxFileOwned::RX_FILE_NONE;

    if (this->m_state == TxnState::TXN_STATE_R2) {
        if (!this->m_flags.rx.md_recv) {
            tmpDir = this->m_cfdpManager->getTmpDirParam(this->m_chan_num);
            /* we need to make a temp file and then do a NAK for md PDU */
            /* the transaction already has a history, and that has a buffer that we can use to
             * hold the temp filename which is defined by the sequence number and the source entity ID */

            // Create destination filepath with format: <tmpDir>/<src_eid>:<seq_num>.tmp
            if (path.format("%s/%" CFDP_PRI_ENTITY_ID ":%" CFDP_PRI_TRANSACTION_SEQ ".tmp", tmpDir.toChar(),
                            this->m_history->src_eid, this->m_history->seq_num) == Fw::FormatStatus::SUCCESS) {
                this->m_history->fnames.dst_filename = path;
                owned = RxFileOwned::RX_FILE_TMP;

                // Event text keeps the first MaxFilePathSize characters of a longer temp path (display only)
                this->m_cfdpManager->log_ACTIVITY_LO_RxTempFileCreated(this->getClass(), this->m_history->src_eid,
                                                                       this->m_history->seq_num,
                                                                       this->m_history->fnames.dst_filename);
            }
        }

        this->m_engine->armAckTimer(this);
    }

    Os::File::Status refusal = (owned == RxFileOwned::RX_FILE_TMP) ? Os::File::OP_OK : Os::File::OTHER_ERROR;

    // With metadata the data is staged beside the destination, which is never opened here
    if (this->m_flags.rx.md_recv) {
        owned = RxFileOwned::RX_FILE_STAGING;
        refusal = this->rStagingPathFor(this->m_history->fnames.dst_filename, path);
    }

    status = this->rCreateFile(this->m_fd, path, refusal);
    if (status != Os::File::OP_OK) {
        if (this->m_state == TxnState::TXN_STATE_R2) {
            this->r2SetFinTxnStatus(TxnStatus::TXN_STATUS_FILESTORE_REJECTION);
        } else {
            this->m_engine->setTxnStatus(this, TxnStatus::TXN_STATUS_FILESTORE_REJECTION);
            this->r1Reset();
        }
    } else {
        this->m_flags.rx.file_owned = owned;
        this->m_state_data.receive.sub_state = RxSubState::RX_SUB_STATE_FILEDATA;
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
    if ((this->m_state_data.receive.sub_state == RxSubState::RX_SUB_STATE_CLOSEOUT_SYNC) ||
        (static_cast<U8>(this->m_state_data.receive.r2.eof_cc) !=
         static_cast<U8>(ConditionCode::CONDITION_CODE_NO_ERROR)) ||
        TxnStatusIsError(this->m_history->txn_stat) || this->m_flags.com.canceled) {
        this->r1Reset(); /* it's done */
    } else {
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
    if (crc_result != expected_crc) {
        this->m_cfdpManager->log_WARNING_LO_RxCrcMismatch(this->getClass(), this->m_history->src_eid,
                                                          this->m_history->seq_num, expected_crc, crc_result);
        this->m_cfdpManager->incrementFaultCrcMismatch(this->m_chan_num);
        ret = Cfdp::Status::ERROR;
    }

    return ret;
}

Status::T Transaction::rFormatStagingPath(const Fw::StringBase& dst, Fw::StringBase& staging) const {
    // The channel, source and sequence number identify a live receive transaction uniquely, so no two
    // transactions share a staging path. A path that does not fit is refused rather than truncated.
    const Fw::FormatStatus status =
        staging.format("%s.cfdp-%" PRIu32 "-%" CFDP_PRI_ENTITY_ID "-%" CFDP_PRI_TRANSACTION_SEQ ".part", dst.toChar(),
                       static_cast<U32>(this->m_chan_num), this->m_history->src_eid, this->m_history->seq_num);
    return (status == Fw::FormatStatus::SUCCESS) ? Cfdp::Status::SUCCESS : Cfdp::Status::ERROR;
}

Status::T Transaction::rCommitFile() {
    Status::T ret = Cfdp::Status::ERROR;
    Fw::String stagingName;
    Os::FileSystem::Status refusal = Os::FileSystem::OTHER_ERROR;
    bool flushed = true;

    // The data must reach stable storage before the rename makes it the destination, or a reset just
    // after the commit could leave an empty or partial destination. A Class 2 receive flushed its data
    // before its checksum pass and holds a read-only handle here, which reports INVALID_MODE.
    if (this->m_fd.isOpen()) {
        const Os::File::Status flushStatus = this->m_fd.flush();
        flushed = (flushStatus == Os::File::OP_OK) ||
                  ((flushStatus == Os::File::INVALID_MODE) && (this->getClass() == Cfdp::Class::CLASS_2));
        this->m_fd.close();
    }

    // The staging file is beside the destination, so this is a single rename on the same file system. Its
    // path is formatted first so that a refused commit still names it in RxFileRenameFailed.
    const bool staged =
        (this->m_flags.rx.file_owned == RxFileOwned::RX_FILE_STAGING) &&
        (this->rFormatStagingPath(this->m_history->fnames.dst_filename, stagingName) == Cfdp::Status::SUCCESS);
    if (flushed && staged) {
        refusal = Os::FileSystem::OP_OK;
    }
    const Os::FileSystem::Status fileSysStatus =
        this->rMoveFile(stagingName, this->m_history->fnames.dst_filename, refusal, true);

    if (fileSysStatus == Os::FileSystem::OP_OK) {
        this->m_flags.rx.file_owned = RxFileOwned::RX_FILE_NONE;
        ret = Cfdp::Status::SUCCESS;
    }

    return ret;
}

void Transaction::rDiscardOwnedFile() {
    Fw::String path;
    bool owned = false;

    // m_flags is shared with transmit transactions, so file_owned is only meaningful for a receive
    if ((this->m_history != nullptr) && (this->m_history->dir == Direction::DIRECTION_RX)) {
        if (this->m_flags.rx.file_owned == RxFileOwned::RX_FILE_TMP) {
            path = this->m_history->fnames.dst_filename;
            owned = true;
        } else if (this->m_flags.rx.file_owned == RxFileOwned::RX_FILE_STAGING) {
            owned = (this->rFormatStagingPath(this->m_history->fnames.dst_filename, path) == Cfdp::Status::SUCCESS);
        }
        this->m_flags.rx.file_owned = RxFileOwned::RX_FILE_NONE;
    }

    if (owned) {
        if (this->m_fd.isOpen()) {
            this->m_fd.close();
        }
        this->rRemoveFile(path);
    }
}

Os::File::Status Transaction::rStagingPathFor(const Fw::StringBase& dst, Fw::StringBase& staging) const {
    Os::File::Status refusal = Os::File::OP_OK;

    // A destination named like a receive's own staging or temp file could replace another
    // transaction's file when committed, so it is refused
    if (isReservedRxFileName(dst)) {
        staging = dst;
        refusal = Os::File::INVALID_ARGUMENT;
    } else if (this->rFormatStagingPath(dst, staging) != Cfdp::Status::SUCCESS) {
        refusal = Os::File::OTHER_ERROR;
    }

    return refusal;
}

Os::File::Status Transaction::rCreateFile(Os::File& file, const Fw::StringBase& path, Os::File::Status refusal) {
    Os::File::Status status = refusal;

    // Created exclusively: an existing file of that name is not this transaction's to replace
    if (refusal == Os::File::OP_OK) {
        status = file.open(path.toChar(), Os::File::OPEN_CREATE, Os::File::NO_OVERWRITE);
    }
    if (status != Os::File::OP_OK) {
        // Event text keeps the first MaxFilePathSize characters of a longer staging path (display only)
        this->m_cfdpManager->log_WARNING_LO_RxFileCreateFailed(this->getClass(), this->m_history->src_eid,
                                                               this->m_history->seq_num, path, status);
        this->m_cfdpManager->incrementFaultFileOpen(this->m_chan_num);
    }

    return status;
}

Os::FileSystem::Status Transaction::rMoveFile(const Fw::StringBase& from,
                                              const Fw::StringBase& to,
                                              Os::FileSystem::Status refusal,
                                              bool sameFileSystem) {
    Os::FileSystem::Status status = refusal;

    if (refusal == Os::FileSystem::OP_OK) {
        // moveFile copies when the paths are on different file systems; a rename never does
        status = sameFileSystem ? Os::FileSystem::rename(from.toChar(), to.toChar())
                                : Os::FileSystem::moveFile(from.toChar(), to.toChar());
    }
    if (status != Os::FileSystem::OP_OK) {
        // Event text keeps the first MaxFilePathSize characters of a longer path (display only)
        this->m_cfdpManager->log_WARNING_LO_RxFileRenameFailed(this->getClass(), this->m_history->src_eid,
                                                               this->m_history->seq_num, from, to, status);
        this->m_cfdpManager->incrementFaultFileRename(this->m_chan_num);
    }

    return status;
}

void Transaction::rRemoveFile(const Fw::StringBase& path) {
    // Best effort, not reported. FileRemoveFailed already has other emission sites with the same
    // arguments, so another one would make it ambiguous (CPP-36); a dedicated receive event would be an
    // interface change. A file left behind is this transaction's own staging or temp file, never the
    // destination, and a later transaction that needs the same name reports it with RxFileCreateFailed.
    (void)Os::FileSystem::removeFile(path.toChar());
}

void Transaction::r2Complete(I32 ok_to_send_nak) {
    U32 ret;
    bool send_nak = false;
    bool send_fin = false;
    U8 nack_limit = 0;
    /* checking if r2 is complete. Check NAK list, and send NAK if appropriate */
    /* if all data is present, then there will be no gaps in the chunk */

    if (!TxnStatusIsError(this->m_history->txn_stat)) {
        /* first, check if md is received. If not, send specialized NAK */
        if (!this->m_flags.rx.md_recv) {
            send_nak = true;
        } else {
            /* only look for 1 gap, since the goal here is just to know that there are gaps */
            ret = this->m_chunks->chunks.computeGaps(1, this->m_fsize, 0, nullptr, nullptr);

            if (ret) {
                /* there is at least 1 gap, so send a NAK */
                send_nak = true;
            } else if (this->m_flags.rx.eof_recv) {
                /* the EOF was received, and there are no NAKs -- process completion in send FIN state */
                send_fin = true;
            }
        }

        if (send_nak && ok_to_send_nak) {
            /* Increment the acknak counter */
            ++this->m_state_data.receive.r2.acknak_count;

            /* Check limit and handle if needed */
            nack_limit = this->m_cfdpManager->getNackLimitParam(this->m_chan_num);
            if (this->m_state_data.receive.r2.acknak_count >= nack_limit) {
                this->m_cfdpManager->log_WARNING_LO_RxNakLimitReached(this->getClass(), this->m_history->src_eid,
                                                                      this->m_history->seq_num);
                send_fin = true;
                this->m_cfdpManager->incrementFaultNakLimit(this->m_chan_num);
                /* don't use CFDP_R2_SetFinTxnStatus because many places in this function set send_fin */
                this->m_engine->setTxnStatus(this, TxnStatus::TXN_STATUS_NAK_LIMIT_REACHED);
                this->m_state_data.receive.r2.acknak_count = 0; /* reset for fin/ack */
            } else {
                this->m_flags.rx.send_nak = true;
            }
        }

        if (send_fin) {
            this->m_flags.rx.complete = true; /* latch completeness, since send_fin is cleared later */

            /* the transaction is now considered complete, but this will not overwrite an
             * error status code if there was one set */
            this->r2SetFinTxnStatus(TxnStatus::TXN_STATUS_NO_ERROR);
        }

        /* always go to RxSubState::RX_SUB_STATE_FILEDATA, and let tick change state */
        this->m_state_data.receive.sub_state = RxSubState::RX_SUB_STATE_FILEDATA;
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
    // const_cast: Fw::SerialBuffer requires non-const U8* even for deserialization (read-only)
    Fw::SerialBuffer sb(const_cast<U8*>(buffer.getData()), buffer.getSize());
    sb.setBuffLen(buffer.getSize());

    Fw::SerializeStatus deserStatus = fd.deserializeFrom(sb);
    if (deserStatus != Fw::FW_SERIALIZE_OK) {
        this->m_cfdpManager->log_WARNING_LO_FailFileDataPduDeserialization(this->getChannelId(),
                                                                           static_cast<I32>(deserStatus));
        this->m_engine->setTxnStatus(this, TxnStatus::TXN_STATUS_PROTOCOL_ERROR);
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

    // Reject file data past the declared file size, or past the addressable offset space when no
    // metadata has been received yet (subtraction form avoids offset + dataSize overflow).
    if (ret == Cfdp::Status::SUCCESS) {
        const FileSize bound = this->m_flags.rx.md_recv ? this->m_fsize : std::numeric_limits<FileSize>::max();
        if ((offset > bound) || ((bound - offset) < dataSize)) {
            this->m_cfdpManager->log_WARNING_LO_RxFileDataOutOfBounds(
                this->getClass(), this->m_history->src_eid, this->m_history->seq_num, offset, dataSize, bound);
            this->m_engine->setTxnStatus(this, TxnStatus::TXN_STATUS_FILE_SIZE_ERROR);
            this->m_cfdpManager->incrementFaultFileSizeMismatch(this->m_chan_num);
            ret = Cfdp::Status::ERROR;
        }
    }

    // A zero-length FileData segment is syntactically valid (payload length equals the encoded
    // offset length) but carries no bytes. There is nothing to seek to, write, or track as a gap,
    // so treat it as a successful no-op. This avoids a pointless file I/O round trip and prevents
    // a zero-size interval from ever reaching the chunk tracker.
    if ((ret == Cfdp::Status::SUCCESS) && (dataSize == 0)) {
        return ret;
    }

    // Seek to file offset if needed
    if (ret == Cfdp::Status::SUCCESS) {
        if (this->m_state_data.receive.cached_pos != offset) {
            Os::File::Status status = this->m_fd.seek(offset, Os::File::SeekType::ABSOLUTE);
            if (status != Os::File::OP_OK) {
                this->m_cfdpManager->log_WARNING_LO_RxSeekFailed(this->getClass(), this->m_history->src_eid,
                                                                 this->m_history->seq_num, offset, status);
                this->m_engine->setTxnStatus(this, TxnStatus::TXN_STATUS_FILE_SIZE_ERROR);
                this->m_cfdpManager->incrementFaultFileSeek(this->m_chan_num);
                ret = Cfdp::Status::ERROR;
            }
        }
    }

    // Write file data
    if (ret == Cfdp::Status::SUCCESS) {
        FwSizeType write_size = dataSize;
        Os::File::Status status = this->m_fd.write(dataPtr, write_size, Os::File::WaitType::WAIT);
        if (status != Os::File::OP_OK) {
            this->m_cfdpManager->log_WARNING_LO_RxWriteFailed(this->getClass(), this->m_history->src_eid,
                                                              this->m_history->seq_num, dataSize,
                                                              static_cast<I32>(write_size));
            this->m_engine->setTxnStatus(this, TxnStatus::TXN_STATUS_FILESTORE_REJECTION);
            this->m_cfdpManager->incrementFaultFileWrite(this->m_chan_num);
            ret = Cfdp::Status::ERROR;
        } else {
            this->m_state_data.receive.cached_pos = static_cast<FileSize>(dataSize) + offset;
            this->m_cfdpManager->addRecvFileDataBytes(this->m_chan_num, dataSize);
        }
    }

    return ret;
}

Status::T Transaction::rSubstateRecvEof(const Fw::Buffer& buffer) {
    Status::T ret = Cfdp::Status::SUCCESS;

    // Deserialize EOF PDU from buffer
    EofPdu eof;
    // const_cast: Fw::SerialBuffer requires non-const U8* even for deserialization (read-only)
    Fw::SerialBuffer sb(const_cast<U8*>(buffer.getData()), buffer.getSize());
    sb.setBuffLen(buffer.getSize());

    Fw::SerializeStatus deserStatus = eof.deserializeFrom(sb);
    if (deserStatus != Fw::FW_SERIALIZE_OK) {
        this->m_cfdpManager->log_WARNING_LO_FailEofPduDeserialization(this->getChannelId(),
                                                                      static_cast<I32>(deserStatus));
        ret = Cfdp::Status::REC_PDU_BAD_EOF_ERROR;
    }

    if (ret == Cfdp::Status::SUCCESS) {
        // NOTE: Engine::recvEof() currently always returns SUCCESS, so the failure branch is
        // omitted here. Once recvEof() performs real EOF-level validation (see the TLV
        // "Future enhancement" TODO in Engine::recvEof) and can return an error status, add an
        // else branch that emits log_WARNING_LO_RxInvalidEofPdu, increments recvErrors, and sets
        // Cfdp::Status::REC_PDU_BAD_EOF_ERROR for the failure case.
        if (!this->m_engine->recvEof(this, eof)) {
            /* this function is only entered for PDUs identified as EOF type */
            ConditionCode cc = eof.getConditionCode();

            /* Only check size if MD received and EOF doesn't have a non-zero condition code (e.g., don't check size for
             * canceled transactions) */
            if (this->m_flags.rx.md_recv && (cc == ConditionCode::CONDITION_CODE_NO_ERROR) &&
                (eof.getFileSize() != this->m_fsize)) {
                this->m_cfdpManager->log_WARNING_LO_RxFileSizeMismatch(this->getClass(), this->m_history->src_eid,
                                                                       this->m_history->seq_num, this->m_fsize,
                                                                       eof.getFileSize());
                this->m_cfdpManager->incrementFaultFileSizeMismatch(this->m_chan_num);
                ret = Cfdp::Status::REC_PDU_FSIZE_MISMATCH_ERROR;
            }

            /* Log condition code if non-zero (cancel or error) - applies to both Class 1 and Class 2 */
            if (cc != ConditionCode::CONDITION_CODE_NO_ERROR) {
                /* Set transaction status from condition code to prevent completion event */
                this->m_engine->setTxnStatus(this, static_cast<TxnStatus>(static_cast<I32>(cc)));

                if (cc == ConditionCode::CONDITION_CODE_CANCEL_REQUEST_RECEIVED) {
                    /* Increment receive EOF cancellation counter (normal operation) */
                    this->m_cfdpManager->incrementRecvEofCanceled(this->m_chan_num);

                    this->m_cfdpManager->log_ACTIVITY_HI_RxEofCancelReceived(this->getClass(), this->m_history->src_eid,
                                                                             this->m_history->seq_num);
                } else {
                    /* Increment RX EOF error counter (protocol error) */
                    this->m_cfdpManager->incrementFaultRxEofError(this->m_chan_num);

                    this->m_cfdpManager->log_WARNING_LO_RxEofWithError(this->getClass(), this->m_history->src_eid,
                                                                       this->m_history->seq_num, static_cast<U8>(cc));
                }
            }
        }
    }

    return ret;
}

void Transaction::r1SubstateRecvEof(const Fw::Buffer& buffer) {
    // Deserialize EOF PDU from buffer
    EofPdu eof;
    // const_cast: Fw::SerialBuffer requires non-const U8* even for deserialization (read-only)
    Fw::SerialBuffer sb(const_cast<U8*>(buffer.getData()), buffer.getSize());
    sb.setBuffLen(buffer.getSize());

    Fw::SerializeStatus deserStatus = eof.deserializeFrom(sb);
    if (deserStatus != Fw::FW_SERIALIZE_OK) {
        // Bad EOF, reset transaction
        this->m_cfdpManager->log_WARNING_LO_FailEofPduDeserialization(this->getChannelId(),
                                                                      static_cast<I32>(deserStatus));
        this->m_engine->setTxnStatus(this, TxnStatus::TXN_STATUS_PROTOCOL_ERROR);
        this->r1Reset();
        return;
    }

    Status::T ret = this->rSubstateRecvEof(buffer);
    U32 crc = eof.getChecksum();
    ConditionCode cc = eof.getConditionCode();

    if (ret == Cfdp::Status::SUCCESS) {
        /* Only check CRC if no error condition code; otherwise rSubstateRecvEof() set the status */
        if (cc == ConditionCode::CONDITION_CODE_NO_ERROR) {
            /* Every byte must have arrived, since missing zero bytes do not change the checksum;
             * then verify the CRC. CFDP_R_CheckCrc() generates an event on failure */
            if ((this->m_chunks == nullptr) ||
                (this->m_chunks->chunks.computeGaps(1, this->m_fsize, 0, nullptr, nullptr) != 0)) {
                this->m_engine->setTxnStatus(this, TxnStatus::TXN_STATUS_FILE_SIZE_ERROR);
                this->m_cfdpManager->incrementFaultFileSizeMismatch(this->m_chan_num);
            } else if (this->rCheckCrc(crc) == Cfdp::Status::SUCCESS) {
                /* successfully processed the file */
                this->m_keep = Cfdp::Keep::KEEP; /* save the file */
                if (this->rCommitFile() != Cfdp::Status::SUCCESS) {
                    this->m_engine->setTxnStatus(this, TxnStatus::TXN_STATUS_FILESTORE_REJECTION);
                }
            } else {
                /* rCheckCrc() generates the event; fail the transaction so the file is removed */
                this->m_engine->setTxnStatus(this, TxnStatus::TXN_STATUS_FILE_CHECKSUM_FAILURE);
            }
        }
    } else if (ret == Cfdp::Status::REC_PDU_FSIZE_MISMATCH_ERROR) {
        /* rSubstateRecvEof() generates the event */
        this->m_engine->setTxnStatus(this, TxnStatus::TXN_STATUS_FILE_SIZE_ERROR);
    } else {
        this->m_engine->setTxnStatus(this, TxnStatus::TXN_STATUS_PROTOCOL_ERROR);
    }

    /* after exit, always reset since we are done */
    /* reset even if the EOF failed -- class 1, so it won't come again! */
    this->r1Reset();
}

void Transaction::r2SubstateRecvEof(const Fw::Buffer& buffer) {
    Status::T ret;

    if (!this->m_flags.rx.eof_recv) {
        // Deserialize EOF PDU from buffer
        EofPdu eof;
        // const_cast: Fw::SerialBuffer requires non-const U8* even for deserialization (read-only)
        Fw::SerialBuffer sb(const_cast<U8*>(buffer.getData()), buffer.getSize());
        sb.setBuffLen(buffer.getSize());

        Fw::SerializeStatus deserStatus = eof.deserializeFrom(sb);
        if (deserStatus != Fw::FW_SERIALIZE_OK) {
            // Bad EOF, return to FILEDATA substate
            this->m_cfdpManager->log_WARNING_LO_FailEofPduDeserialization(this->getChannelId(),
                                                                          static_cast<I32>(deserStatus));
            this->m_state_data.receive.sub_state = RxSubState::RX_SUB_STATE_FILEDATA;
            return;
        }

        ret = this->rSubstateRecvEof(buffer);

        /* did receiving EOF succeed? */
        if (ret == Cfdp::Status::SUCCESS) {
            this->m_flags.rx.eof_recv = true;

            /* need to remember the EOF CRC for later */
            this->m_state_data.receive.r2.eof_crc = eof.getChecksum();
            this->m_state_data.receive.r2.eof_size = eof.getFileSize();

            /* always ACK the EOF, even if we're not done */
            this->m_state_data.receive.r2.eof_cc = static_cast<U8>(eof.getConditionCode());
            this->m_flags.rx.send_eof_ack = true; /* defer sending ACK to tick handling */

            /* only check for complete if EOF with no errors */
            if (static_cast<U8>(this->m_state_data.receive.r2.eof_cc) ==
                static_cast<U8>(ConditionCode::CONDITION_CODE_NO_ERROR)) {
                this->r2Complete(true); /* CFDP_R2_Complete() will change state */
            } else {
                /* All CFDP CC values directly correspond to a Transaction Status of the same numeric value */
                this->m_engine->setTxnStatus(
                    this, static_cast<TxnStatus>(static_cast<I32>(this->m_state_data.receive.r2.eof_cc)));
                this->r2Reset();
            }
        } else {
            /* bad EOF sent? */
            if (ret == Cfdp::Status::REC_PDU_FSIZE_MISMATCH_ERROR) {
                this->r2SetFinTxnStatus(TxnStatus::TXN_STATUS_FILE_SIZE_ERROR);
            } else {
                /* can't do anything with this bad EOF, so return to FILEDATA */
                this->m_state_data.receive.sub_state = RxSubState::RX_SUB_STATE_FILEDATA;
            }
        }
    }
}

void Transaction::r1SubstateRecvFileData(const Fw::Buffer& buffer) {
    Status::T ret;

    // Deserialize FileData PDU from buffer
    FileDataPdu fd;
    // const_cast: Fw::SerialBuffer requires non-const U8* even for deserialization (read-only)
    Fw::SerialBuffer sb(const_cast<U8*>(buffer.getData()), buffer.getSize());
    sb.setBuffLen(buffer.getSize());

    Fw::SerializeStatus deserStatus = fd.deserializeFrom(sb);
    if (deserStatus != Fw::FW_SERIALIZE_OK) {
        // Bad file data PDU, reset transaction
        this->m_cfdpManager->log_WARNING_LO_FailFileDataPduDeserialization(this->getChannelId(),
                                                                           static_cast<I32>(deserStatus));
        this->m_engine->setTxnStatus(this, TxnStatus::TXN_STATUS_PROTOCOL_ERROR);
        this->r1Reset();
        return;
    }

    /* got file data PDU? */
    ret = this->m_engine->recvFd(this, fd);

    if (ret == Cfdp::Status::SUCCESS) {
        ret = this->rProcessFd(buffer);
    }

    if (ret == Cfdp::Status::SUCCESS) {
        /* class 1 digests CRC */
        this->m_crc.update(fd.getData(), fd.getOffset(), static_cast<U32>(fd.getDataSize()));
        /* and records which bytes arrived, so a missing segment is caught at EOF */
        if (this->m_chunks != nullptr) {
            this->m_chunks->chunks.add(fd.getOffset(), static_cast<FileSize>(fd.getDataSize()));
        }
    } else {
        /* Reset transaction on failure */
        this->r1Reset();
    }
}

void Transaction::r2SubstateRecvFileData(const Fw::Buffer& buffer) {
    Status::T ret;

    // If CRC calculation has started (file reopened in READ mode), ignore late FileData PDUs.
    // This can happen if retransmitted FileData arrives after EOF was received and CRC began.
    if (this->m_state_data.receive.r2.rx_crc_calc_bytes > 0) {
        // Silently ignore - file is complete and we're calculating CRC
        // No EVR needed - late retransmissions are expected in CFDP Class 2
        return;
    }

    // Deserialize FileData PDU from buffer
    FileDataPdu fd;
    // const_cast: Fw::SerialBuffer requires non-const U8* even for deserialization (read-only)
    Fw::SerialBuffer sb(const_cast<U8*>(buffer.getData()), buffer.getSize());
    sb.setBuffLen(buffer.getSize());

    Fw::SerializeStatus deserStatus = fd.deserializeFrom(sb);
    if (deserStatus != Fw::FW_SERIALIZE_OK) {
        // Bad file data PDU, reset transaction
        this->m_cfdpManager->log_WARNING_LO_FailFileDataPduDeserialization(this->getChannelId(),
                                                                           static_cast<I32>(deserStatus));
        this->m_engine->setTxnStatus(this, TxnStatus::TXN_STATUS_PROTOCOL_ERROR);
        this->r2Reset();
        return;
    }

    /* got file data PDU? */
    ret = this->m_engine->recvFd(this, fd);
    if (ret == Cfdp::Status::SUCCESS) {
        ret = this->rProcessFd(buffer);
    }

    if (ret == Cfdp::Status::SUCCESS) {
        /* class 2 does CRC at FIN, but track gaps */
        this->m_chunks->chunks.add(fd.getOffset(), static_cast<FileSize>(fd.getDataSize()));

        if (this->m_flags.rx.fd_nak_sent) {
            this->r2Complete(false); /* once nak-retransmit received, start checking for completion at each fd */
        }

        if (!this->m_flags.rx.complete) {
            this->m_engine->armAckTimer(this); /* re-arm ACK timer, since we got data */
        }

        this->m_state_data.receive.r2.acknak_count = 0;
    } else {
        /* Reset transaction on failure */
        this->r2Reset();
    }
}

void Transaction::r2GapCompute(const Chunk* chunk, NakPdu& nak) {
    FW_ASSERT(chunk->size > 0, static_cast<FwAssertArgType>(chunk->size));

    // Calculate segment offsets relative to scope start
    FileSize offsetStart = chunk->offset - nak.getScopeStart();
    FileSize offsetEnd = offsetStart + chunk->size;

    // Add segment to NAK PDU (returns false if array is full)
    nak.addSegment(offsetStart, offsetEnd);
}

void Transaction::r2GapComputeWrapper(const Chunk* chunk, void* opaque) {
    struct GapComputeContext {
        Transaction* txn;
        NakPdu* nak;
    };
    GapComputeContext* ctx = static_cast<GapComputeContext*>(opaque);
    ctx->txn->r2GapCompute(chunk, *ctx->nak);
}

Status::T Transaction::rSubstateSendNak() {
    Status::T status = Cfdp::Status::SUCCESS;

    // Create and initialize NAK PDU
    NakPdu nakPdu;
    Cfdp::PduDirection direction = PduDirection::DIRECTION_TOWARD_SENDER;

    if (this->m_flags.rx.md_recv) {
        // We have metadata, so send NAK with file data gaps
        nakPdu.initialize(direction,
                          this->getClass(),                         // transmission mode
                          this->m_history->peer_eid,                // source EID (receiver)
                          this->m_history->seq_num,                 // transaction sequence number
                          this->m_cfdpManager->getLocalEidParam(),  // destination EID (sender)
                          0,                                        // scope start
                          0                                         // scope end
        );

        // Compute gaps and add segments to NAK PDU
        U32 chunkCount = this->m_chunks->chunks.getCount();
        U32 maxChunks = this->m_chunks->chunks.getMaxChunks();
        U32 gapLimit = (chunkCount < maxChunks) ? maxChunks : (maxChunks - 1);

        // For each gap found, add it as a segment to the NAK PDU via callback
        struct GapComputeContext {
            Transaction* txn;
            NakPdu* nak;
        } gapCtx = {this, &nakPdu};

        U32 gapCount = this->m_chunks->chunks.computeGaps(static_cast<ChunkIdx>(gapLimit), this->m_fsize, 0,
                                                          &Transaction::r2GapComputeWrapper, &gapCtx);

        if (!gapCount) {
            // No gaps left, file reception is complete
            this->m_flags.rx.complete = true;
            status = Cfdp::Status::SUCCESS;
        } else {
            // Gaps are present, send the NAK PDU
            status = this->m_engine->sendNak(this, nakPdu);
            if (status == Cfdp::Status::SUCCESS) {
                this->m_flags.rx.fd_nak_sent = true;
                this->m_cfdpManager->addSentNakSegmentRequests(this->m_chan_num, gapCount);
            }
        }
    } else {
        // Need to send NAK to request metadata PDU again
        // Special case: scope start/end and segment[0] all zeros requests metadata
        nakPdu.initialize(direction,
                          this->getClass(),                         // transmission mode
                          this->m_history->peer_eid,                // source EID (receiver)
                          this->m_history->seq_num,                 // transaction sequence number
                          this->m_cfdpManager->getLocalEidParam(),  // destination EID (sender)
                          0,                                        // scope start (special value)
                          0                                         // scope end (special value)
        );

        // Add special segment [0,0] to request metadata
        nakPdu.addSegment(0, 0);

        status = this->m_engine->sendNak(this, nakPdu);
    }

    return status;
}

Status::T Transaction::r2CalcCrcChunk() {
    U8 buf[R2CrcChunkSize];
    FileSize count_bytes;
    FileSize want_offs_size;
    FwSizeType read_size;
    Os::File::Status fileStatus;
    Status::T ret = Cfdp::Status::SUCCESS;
    FileSize rx_crc_calc_bytes_per_cycle = 0;

    memset(buf, 0, sizeof(buf));

    count_bytes = 0;

    // Open file for CRC calculation if needed
    if (ret == Cfdp::Status::SUCCESS) {
        if (this->m_state_data.receive.r2.rx_crc_calc_bytes == 0) {
            this->m_crc = CFDP::Checksum(0);

            // For Class 2 RX, the file was opened in WRITE mode for receiving FileData PDUs.
            // Now we need to READ it for CRC calculation. Close and reopen in READ mode. The data is
            // flushed to stable storage first, since the commit after the checksum pass renames it.
            bool flushed = true;
            if (this->m_fd.isOpen()) {
                // INVALID_MODE: an earlier pass that read nothing already flushed and reopened it read-only
                const Os::File::Status flushStatus = this->m_fd.flush();
                flushed = (flushStatus == Os::File::OP_OK) || (flushStatus == Os::File::INVALID_MODE);
                this->m_fd.close();
            }

            Fw::String stagingName;
            fileStatus = Os::File::OTHER_ERROR;
            if (!flushed) {
                this->m_engine->setTxnStatus(this, TxnStatus::TXN_STATUS_FILESTORE_REJECTION);
                this->m_cfdpManager->incrementFaultFileWrite(this->m_chan_num);
                ret = Cfdp::Status::ERROR;
            } else {
                if ((this->m_flags.rx.file_owned == RxFileOwned::RX_FILE_STAGING) &&
                    (this->rFormatStagingPath(this->m_history->fnames.dst_filename, stagingName) ==
                     Cfdp::Status::SUCCESS)) {
                    fileStatus = this->m_fd.open(stagingName.toChar(), Os::File::OPEN_READ);
                }
                if (fileStatus != Os::File::OP_OK) {
                    this->m_engine->setTxnStatus(this, TxnStatus::TXN_STATUS_FILE_SIZE_ERROR);
                    ret = Cfdp::Status::ERROR;
                } else {
                    // Reset cached position since we just reopened the file
                    this->m_state_data.receive.cached_pos = 0;
                }
            }
        }
    }

    // Process file in chunks
    if (ret == Cfdp::Status::SUCCESS) {
        rx_crc_calc_bytes_per_cycle = this->m_cfdpManager->getRxCrcCalcBytesPerCycleParam();

        while ((ret == Cfdp::Status::SUCCESS) && (count_bytes < rx_crc_calc_bytes_per_cycle) &&
               (this->m_state_data.receive.r2.rx_crc_calc_bytes < this->m_fsize)) {
            want_offs_size = this->m_state_data.receive.r2.rx_crc_calc_bytes + static_cast<FileSize>(sizeof(buf));

            if (want_offs_size > this->m_fsize) {
                read_size = this->m_fsize - this->m_state_data.receive.r2.rx_crc_calc_bytes;
            } else {
                read_size = sizeof(buf);
            }

            if (this->m_state_data.receive.cached_pos != this->m_state_data.receive.r2.rx_crc_calc_bytes) {
                fileStatus =
                    this->m_fd.seek(this->m_state_data.receive.r2.rx_crc_calc_bytes, Os::File::SeekType::ABSOLUTE);
                if (fileStatus != Os::File::OP_OK) {
                    this->m_cfdpManager->log_WARNING_LO_RxSeekCrcFailed(
                        this->getClass(), this->m_history->src_eid, this->m_history->seq_num,
                        this->m_state_data.receive.r2.rx_crc_calc_bytes, fileStatus);
                    // this->m_engine->setTxnStatus(this, TxnStatus::TXN_STATUS_FILE_SIZE_ERROR);
                    this->m_cfdpManager->incrementFaultFileSeek(this->m_chan_num);
                    ret = Cfdp::Status::ERROR;
                }
            }

            if (ret == Cfdp::Status::SUCCESS) {
                FwSizeType expected_read_size = read_size;
                fileStatus = this->m_fd.read(buf, read_size, Os::File::WaitType::WAIT);
                if ((fileStatus != Os::File::OP_OK) || (read_size != expected_read_size)) {
                    this->m_cfdpManager->log_WARNING_LO_RxReadCrcFailed(
                        this->getClass(), this->m_history->src_eid, this->m_history->seq_num,
                        static_cast<U32>(expected_read_size), static_cast<I32>(read_size));
                    this->m_engine->setTxnStatus(this, TxnStatus::TXN_STATUS_FILE_SIZE_ERROR);
                    this->m_cfdpManager->incrementFaultFileRead(this->m_chan_num);
                    ret = Cfdp::Status::ERROR;
                } else {
                    this->m_crc.update(buf, this->m_state_data.receive.r2.rx_crc_calc_bytes,
                                       static_cast<U32>(read_size));
                    this->m_state_data.receive.r2.rx_crc_calc_bytes += static_cast<FileSize>(read_size);
                    this->m_state_data.receive.cached_pos = this->m_state_data.receive.r2.rx_crc_calc_bytes;
                    count_bytes += static_cast<FileSize>(read_size);

                    // Reset inactivity timer to indicate transaction is actively processing
                    this->m_engine->armInactTimer(this);
                }
            }
        }
    }

    // Check final CRC if all bytes processed
    if (ret == Cfdp::Status::SUCCESS) {
        if (this->m_state_data.receive.r2.rx_crc_calc_bytes == this->m_fsize) {
            /* all bytes calculated, so now check */
            if (this->rCheckCrc(this->m_state_data.receive.r2.eof_crc) == Cfdp::Status::SUCCESS) {
                /* CRC matched! We are happy */
                this->m_keep = Cfdp::Keep::KEEP; /* save the file */

                /* the file is retained only once it is at its destination */
                if (this->rCommitFile() == Cfdp::Status::SUCCESS) {
                    /* set FIN PDU status */
                    this->m_state_data.receive.r2.dc = FinDeliveryCode::FIN_DELIVERY_CODE_COMPLETE;
                    this->m_state_data.receive.r2.fs = FinFileStatus::FIN_FILE_STATUS_RETAINED;
                } else {
                    /* the data was complete and verified, but the filestore refused to keep it */
                    this->m_state_data.receive.r2.dc = FinDeliveryCode::FIN_DELIVERY_CODE_COMPLETE;
                    this->m_state_data.receive.r2.fs = FinFileStatus::FIN_FILE_STATUS_DISCARDED_FILESTORE;
                    this->r2SetFinTxnStatus(TxnStatus::TXN_STATUS_FILESTORE_REJECTION);
                }
            } else {
                this->r2SetFinTxnStatus(TxnStatus::TXN_STATUS_FILE_CHECKSUM_FAILURE);
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

    if (!TxnStatusIsError(this->m_history->txn_stat) && !this->m_flags.com.crc_calc) {
        /* no error, and haven't checked CRC -- so start checking it */
        if (this->r2CalcCrcChunk()) {
            ret = Cfdp::Status::ERROR; /* signal to caller to re-enter next tick */
        }
    }

    if (ret != Cfdp::Status::ERROR) {
        sret = this->m_engine->sendFin(this, this->m_state_data.receive.r2.dc, this->m_state_data.receive.r2.fs,
                                       static_cast<ConditionCode>(TxnStatusToConditionCode(this->m_history->txn_stat)));

        /* Serialization error already logged in serializeAndSendPdu if ERROR returned */
        this->m_state_data.receive.sub_state =
            RxSubState::RX_SUB_STATE_CLOSEOUT_SYNC; /* whether or not FIN send successful, ok to transition state */
        if (sret != Cfdp::Status::SUCCESS) {
            ret = Cfdp::Status::ERROR;
        }
    }

    /* if no message, then try again next time */
    return ret;
}

void Transaction::r2RecvFinAck(const Fw::Buffer& buffer) {
    // Deserialize ACK PDU from buffer
    AckPdu ack;
    // const_cast: Fw::SerialBuffer requires non-const U8* even for deserialization (read-only)
    Fw::SerialBuffer sb(const_cast<U8*>(buffer.getData()), buffer.getSize());
    sb.setBuffLen(buffer.getSize());

    Fw::SerializeStatus deserStatus = ack.deserializeFrom(sb);
    if (deserStatus != Fw::FW_SERIALIZE_OK) {
        // Bad ACK PDU
        this->m_cfdpManager->log_WARNING_LO_FailAckPduDeserialization(this->getChannelId(),
                                                                      static_cast<I32>(deserStatus));
        this->m_cfdpManager->incrementRecvErrors(this->m_chan_num);
        return;
    }

    // ACK PDU has been validated during deserialization
    // Got fin-ack, so time to close the state
    this->r2Reset();
}

void Transaction::r2RecvMd(const Fw::Buffer& buffer) {
    Fw::String fname;
    Fw::String dstName;
    Fw::String stagingName;
    Os::File claim;
    Os::File::Status fileStatus;
    Os::FileSystem::Status fileSysStatus;
    bool success = true;

    /* it isn't an error to get another MD PDU, right? Only a transaction that has not failed and still
     * holds its temp file acts on one, so a retransmitted MD PDU can never move a file a second time */
    if (!this->m_flags.rx.md_recv && (this->m_flags.rx.file_owned == RxFileOwned::RX_FILE_TMP) &&
        !TxnStatusIsError(this->m_history->txn_stat)) {
        /* NOTE: this->m_flags.rx.md_recv always 1 in R1, so this is R2 only */
        /* parse the md PDU. this will overwrite the transaction's history, which contains our filename. so let's
         * save the filename in a local buffer so it can be used with moveFile upon successful parsing of
         * the md PDU */
        fname = this->m_history->fnames.dst_filename;

        // Deserialize Metadata PDU from buffer
        MetadataPdu md;
        // const_cast: Fw::SerialBuffer requires non-const U8* even for deserialization (read-only)
        Fw::SerialBuffer sb(const_cast<U8*>(buffer.getData()), buffer.getSize());
        sb.setBuffLen(buffer.getSize());

        Fw::SerializeStatus deserStatus = md.deserializeFrom(sb);
        if (deserStatus != Fw::FW_SERIALIZE_OK) {
            // Bad metadata PDU
            this->m_cfdpManager->log_WARNING_LO_FailMetadataPduDeserialization(this->getChannelId(),
                                                                               static_cast<I32>(deserStatus));
            return;
        }

        // Structural PDU validation already done during deserialization; recvMd validates the
        // destination path against the channel rx_dir before committing anything, so on rejection
        // dst_filename still names the temp file.
        if (this->m_engine->recvMd(this, md) != Status::SUCCESS) {
            // Destination rejected: keep writing/cleaning up against the temp file only and fail the
            // transaction with a filestore rejection so the sender is told in the FIN.
            this->r2SetFinTxnStatus(TxnStatus::TXN_STATUS_FILESTORE_REJECTION);
            return;
        }

        /* successfully obtained md PDU. recvMd() put the destination in dst_filename; until the temp file
         * has been moved, dst_filename keeps naming the temp file so cleanup can only remove that file */
        dstName = this->m_history->fnames.dst_filename;
        this->m_history->fnames.dst_filename = fname;

        if (this->m_flags.rx.eof_recv) {
            /* EOF was received, so check that md and EOF sizes match */
            if (this->m_state_data.receive.r2.eof_size != this->m_fsize) {
                this->m_cfdpManager->log_WARNING_LO_RxEofMdSizeMismatch(this->getClass(), this->m_history->src_eid,
                                                                        this->m_history->seq_num, this->m_fsize,
                                                                        this->m_state_data.receive.r2.eof_size);
                this->m_cfdpManager->incrementFaultFileSizeMismatch(this->m_chan_num);
                this->r2SetFinTxnStatus(TxnStatus::TXN_STATUS_FILE_SIZE_ERROR);
                success = false;
            }
        }

        if (success) {
            /* File Data that arrived before the metadata was not bounded by the declared size. Data past it
             * would make the committed file longer than the bytes the checksum covers, so it fails here */
            FwSizeType tempSize = 0;
            if ((this->m_fd.size(tempSize) != Os::File::OP_OK) || (tempSize > static_cast<FwSizeType>(this->m_fsize))) {
                this->m_cfdpManager->incrementFaultFileSizeMismatch(this->m_chan_num);
                this->r2SetFinTxnStatus(TxnStatus::TXN_STATUS_FILE_SIZE_ERROR);
                success = false;
            }
        }

        if (success) {
            /* claim the staging file before moving the temp file into it */
            const Os::File::Status refusal = this->rStagingPathFor(dstName, stagingName);
            fileStatus = this->rCreateFile(claim, stagingName, refusal);
            if (fileStatus != Os::File::OP_OK) {
                this->r2SetFinTxnStatus(TxnStatus::TXN_STATUS_FILESTORE_REJECTION);
                success = false;
            } else {
                claim.close();
                this->m_fd.close();

                // A rename replaces only the claimed file; across file systems moveFile copies into it
                fileSysStatus = this->rMoveFile(fname, stagingName, Os::FileSystem::OP_OK, false);
                if (fileSysStatus != Os::FileSystem::OP_OK) {
                    this->r2SetFinTxnStatus(TxnStatus::TXN_STATUS_FILESTORE_REJECTION);
                    success = false;

                    // The staging file was created above, so it is this transaction's to remove; the temp
                    // file is still owned and is removed when the transaction ends
                    this->rRemoveFile(stagingName);
                }
            }
        }

        if (success) {
            /* the temp file is gone and its data is in the staging file */
            this->m_history->fnames.dst_filename = dstName;
            this->m_flags.rx.file_owned = RxFileOwned::RX_FILE_STAGING;

            fileStatus = this->m_fd.open(stagingName.toChar(), Os::File::OPEN_WRITE);
            if (fileStatus != Os::File::OP_OK) {
                // Event text keeps the first MaxFilePathSize characters of a longer staging path (display only)
                this->m_cfdpManager->log_WARNING_LO_RxFileReopenFailed(
                    this->getClass(), this->m_history->src_eid, this->m_history->seq_num, stagingName, fileStatus);
                this->r2SetFinTxnStatus(TxnStatus::TXN_STATUS_FILESTORE_REJECTION);
                this->m_cfdpManager->incrementFaultFileOpen(this->m_chan_num);
                success = false;
            }

            if (success) {
                this->m_state_data.receive.cached_pos = 0; /* reset psn due to open */
                this->m_flags.rx.md_recv = true;
                this->m_state_data.receive.r2.acknak_count = 0; /* in case part of NAK */
                this->r2Complete(true);                         /* check for completion now that md is received */
            }
        }
    }
}

void Transaction::rSendInactivityEvent() {
    this->m_cfdpManager->log_WARNING_LO_RxInactivityTimeout(this->getClass(), this->m_history->src_eid,
                                                            this->m_history->seq_num);
    this->m_cfdpManager->incrementFaultInactivityTimer(this->m_chan_num);
}

// ======================================================================
// Dispatch Methods
// ======================================================================

void Transaction::rDispatchRecv(const Fw::Buffer& buffer, const RSubstateDispatchTable* dispatch, StateRecvFunc fd_fn) {
    StateRecvFunc selected_handler;

    FW_ASSERT(this->m_state_data.receive.sub_state < RxSubState::RX_SUB_STATE_NUM_STATES,
              static_cast<U8>(this->m_state_data.receive.sub_state),
              static_cast<U8>(RxSubState::RX_SUB_STATE_NUM_STATES));

    selected_handler = nullptr;

    // Peek at PDU type from buffer
    Cfdp::PduTypeEnum::T pduType = Cfdp::peekPduType(buffer);

    // Special handling for file data PDU
    if (pduType == Cfdp::PduTypeEnum::FILE_DATA) {
        /* For file data PDU, use the provided fd_fn */
        if (!TxnStatusIsError(this->m_history->txn_stat)) {
            selected_handler = fd_fn;
        }
    } else {
        // Not a file-data PDU - parse as a directive PDU to get the directive code.
        // const_cast: Fw::SerialBuffer requires non-const U8* even for deserialization (read-only)
        Fw::SerialBuffer sb(const_cast<U8*>(buffer.getData()), buffer.getSize());
        sb.setBuffLen(buffer.getSize());

        Cfdp::PduHeader header;
        if (header.fromSerialBuffer(sb) == Fw::FW_SERIALIZE_OK) {
            // Read directive code (first byte after header for directive PDUs)
            U8 directiveCodeByte;
            if (sb.deserializeTo(directiveCodeByte) == Fw::FW_SERIALIZE_OK) {
                FileDirective directiveCode = static_cast<FileDirective>(directiveCodeByte);

                if (directiveCode < FileDirective::FILE_DIRECTIVE_INVALID_MAX) {
                    /* The CFDP_R_SubstateDispatchTable_t is only used with file directive PDU */
                    if (dispatch->state[static_cast<U32>(this->m_state_data.receive.sub_state)] != nullptr) {
                        selected_handler = dispatch->state[static_cast<U32>(this->m_state_data.receive.sub_state)]
                                               ->fdirective[static_cast<U32>(directiveCode)];
                    }
                } else {
                    this->m_cfdpManager->incrementRecvSpurious(this->m_chan_num);
                    this->m_cfdpManager->log_WARNING_LO_RxInvalidDirectiveCode(
                        this->getClass(), this->m_history->src_eid, this->m_history->seq_num, directiveCodeByte,
                        static_cast<U8>(this->m_state_data.receive.sub_state));
                }
            }
        }
    }

    /*
     * NOTE: if no handler is selected, this will drop packets on the floor here.
     */
    if (selected_handler != nullptr) {
        (this->*selected_handler)(buffer);
    } else {
        this->m_cfdpManager->incrementRecvDropped(this->m_chan_num);
    }
}

}  // namespace Cfdp
}  // namespace Ccsds
}  // namespace Svc

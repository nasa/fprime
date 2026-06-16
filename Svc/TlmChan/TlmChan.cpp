/**
 * \file
 * \author T. Canham
 * \brief Implementation file for channelized telemetry storage component
 *
 * \copyright
 * Copyright 2009-2015, by the California Institute of Technology.
 * ALL RIGHTS RESERVED.  United States Government Sponsorship
 * acknowledged.
 * <br /><br />
 */
#include <Fw/Com/ComBuffer.hpp>
#include <Fw/FPrimeBasicTypes.hpp>
#include <Fw/Time/Time.hpp>
#include <Fw/Types/Assert.hpp>
#include <Os/RawTime.hpp>
#include <Svc/TlmChan/TlmChan.hpp>

namespace Svc {

// Definition of TLMCHAN_HASH_BUCKETS is >= number of telemetry ids
static_assert(std::numeric_limits<FwChanIdType>::max() >= TLMCHAN_HASH_BUCKETS,
              "Cannot have more hash buckets than maximum telemetry ids in the system");
// TLMCHAN_HASH_BUCKETS >= TLMCHAN_NUM_TLM_HASH_SLOTS >= 0
static_assert(std::numeric_limits<FwChanIdType>::max() >= TLMCHAN_NUM_TLM_HASH_SLOTS,
              "Cannot have more hash slots than maximum telemetry ids in the system");

// TLMCHAN_MAX_ENTRIES_PER_RUN must be defined in TlmChanImplCfg.hpp.
// It caps the number of updated telemetry entries that Run_handler will
// serialize and downlink in a single invocation.  Any entries beyond this
// limit are skipped (deferred) for the current cycle and will be cleared
// by the next buffer swap, so they are effectively dropped rather than
// queued.  Choose a value that keeps Run_handler's worst-case execution
// time within its rate-group budget.
static_assert(TLMCHAN_MAX_ENTRIES_PER_RUN > 0, "TLMCHAN_MAX_ENTRIES_PER_RUN must be greater than zero");
static_assert(TLMCHAN_MAX_ENTRIES_PER_RUN <= TLMCHAN_HASH_BUCKETS,
              "TLMCHAN_MAX_ENTRIES_PER_RUN cannot exceed TLMCHAN_HASH_BUCKETS");

TlmChan::TlmChan(const char* name)
    : TlmChanComponentBase(name), m_procCapCount(0), m_activeBuffer(ActiveBuffer::Buffer_0) {
    FW_ASSERT(name != nullptr);

    // clear slot pointers
    for (FwChanIdType entry = 0; entry < TLMCHAN_NUM_TLM_HASH_SLOTS; entry++) {
        this->m_tlmEntries[0].slots[entry] = nullptr;
        this->m_tlmEntries[1].slots[entry] = nullptr;
    }
    // clear buckets
    for (FwChanIdType entry = 0; entry < TLMCHAN_HASH_BUCKETS; entry++) {
        this->m_tlmEntries[0].buckets[entry].used = false;
        this->m_tlmEntries[0].buckets[entry].updated = false;
        this->m_tlmEntries[0].buckets[entry].bucketNo = entry;
        this->m_tlmEntries[0].buckets[entry].next = nullptr;
        this->m_tlmEntries[0].buckets[entry].id = 0;
        this->m_tlmEntries[1].buckets[entry].used = false;
        this->m_tlmEntries[1].buckets[entry].updated = false;
        this->m_tlmEntries[1].buckets[entry].bucketNo = entry;
        this->m_tlmEntries[1].buckets[entry].next = nullptr;
        this->m_tlmEntries[1].buckets[entry].id = 0;
    }
    // clear free index
    this->m_tlmEntries[0].free = 0;
    this->m_tlmEntries[1].free = 0;

    // determine deployed channel size
    this->m_chanIdSize = static_cast<U32>(sizeof(FwChanIdType));

    // ------- Set random telemetry hash seed -------
    U32 seed = 0;

    // get current time and use as non-deterministic source for seed
    Os::RawTime rawTime;
    rawTime.now();
    U8 timeBuf[FW_RAW_TIME_SERIALIZATION_MAX_SIZE] = {};
    Fw::ExternalSerializeBuffer serBuf(timeBuf, sizeof(timeBuf));
    (void)rawTime.serializeTo(serBuf);

    U32 foldedTime = 0;
    for (U32 i = 0; i < static_cast<U32>(serBuf.getSize()); i++) {
        // Rotate-and-XOR each byte to avoid cancellation when bytes are equal
        foldedTime = (foldedTime << 8) | (foldedTime >> 24);
        foldedTime ^= static_cast<U32>(timeBuf[i]);
    }

    // read stack-address - address varies per boot
    const U64 raw = reinterpret_cast<U64>(&seed);
    const U32 foldedStack = static_cast<U32>(raw ^ (raw >> 32));

    seed = foldedTime ^ foldedStack;

    // Force a non-zero seed. Of the three hash paths, only the narrow
    // (<16-bit) path actually loses its keying at seed == 0: it reverts to the
    // original linear (id % MOD) % SLOTS reduction, re-exposing the predictable
    // collision pattern this change removes. The Murmur3 and Wang paths still
    // diffuse a zero seed correctly, so this guard is conservative for them.
    // Substituting a known non-zero constant keeps every branch keyed and the
    // seed uniform to reason about across platforms.
    if (seed == 0) {
        seed = 0xDEADBEEFU;
    }

    this->m_hashSeed = seed;
}

TlmChan::~TlmChan() {}

FwChanIdType TlmChan::doHash(FwChanIdType id) const {
    // Validate input before use.
    static_assert(std::is_unsigned<FwChanIdType>::value, "FwChanIdType must be unsigned");
    static_assert(sizeof(FwChanIdType) <= sizeof(U32), "FwChanIdType must fit within U32 for safe hash cast");
    static_assert(TLMCHAN_NUM_TLM_HASH_SLOTS > 0, "TLMCHAN_NUM_TLM_HASH_SLOTS must be greater than zero");

    FwChanIdType result;

    if (this->m_chanIdSize >= 4) {
        // Verify id fits in U16 before narrowing cast
        FW_ASSERT(id <= static_cast<FwChanIdType>(std::numeric_limits<U32>::max()), static_cast<FwAssertArgType>(id));

        U32 h = static_cast<U32>(id) ^ static_cast<U32>(this->m_hashSeed);

        // Murmur3 32-bit
        h ^= (h >> 16);
        h *= MURMUR3_C1;
        h ^= (h >> 13);
        h *= MURMUR3_C2;
        h ^= (h >> 16);

        result = static_cast<FwChanIdType>(h % TLMCHAN_NUM_TLM_HASH_SLOTS);
    } else if (this->m_chanIdSize == 2) {
        // Verify id fits in U16 before narrowing cast
        FW_ASSERT(id <= static_cast<FwChanIdType>(std::numeric_limits<U16>::max()), static_cast<FwAssertArgType>(id));

        U16 h = (static_cast<U16>(id)) ^ (static_cast<U16>(this->m_hashSeed) & static_cast<U16>(0xFFFFU));

        // Wang 16-bit
        h = static_cast<U16>(h ^ (h >> 7));
        h = static_cast<U16>(h * WANG16_C1);
        h = static_cast<U16>(h ^ (h >> 5));
        h = static_cast<U16>(h * WANG16_C2);
        h = static_cast<U16>(h ^ (h >> 3));

        result = static_cast<FwChanIdType>(h % TLMCHAN_NUM_TLM_HASH_SLOTS);
    } else {
        // Verify id fits in U8 before narrowing cast
        FW_ASSERT(id <= static_cast<FwChanIdType>(std::numeric_limits<U8>::max()), static_cast<FwAssertArgType>(id));

        // FwChanIdType is smaller than 16 bits (at most 255 distinct channel IDs).
        // XOR with the low byte of the seed before reduction to maintain consistency
        const U8 h = static_cast<U8>(id) ^ (static_cast<U8>(this->m_hashSeed) & static_cast<U8>(0xFFU));
        result = static_cast<FwChanIdType>((h % TLMCHAN_HASH_MOD_VALUE) % TLMCHAN_NUM_TLM_HASH_SLOTS);
    }
    return result;
}

void TlmChan::pingIn_handler(const FwIndexType portNum, U32 key) {
    static_assert(NUM_PINGIN_INPUT_PORTS == 1, "pingIn_handler expects exactly one input port");
    // return key
    this->pingOut_out(0, key);
}

Fw::TlmValid TlmChan::TlmGet_handler(FwIndexType portNum, FwChanIdType id, Fw::Time& timeTag, Fw::TlmBuffer& val) {
    static_assert(NUM_TLMGET_INPUT_PORTS == 1, "TlmGet_handler expects exactly one input port");
    FwChanIdType index = this->doHash(id);

    // Search to see if channel has been stored
    // check both buffers
    // don't need to lock because this port is guarded
    TlmEntry* activeEntry = this->m_tlmEntries[static_cast<U8>(this->m_activeBuffer)].slots[index];
    for (FwChanIdType bucket = 0; bucket < TLMCHAN_HASH_BUCKETS; bucket++) {
        if (activeEntry) {
            if (activeEntry->id == id) {
                break;
            } else {
                activeEntry = activeEntry->next;
            }
        } else {
            break;
        }
    }

    TlmEntry* inactiveEntry = this->m_tlmEntries[1 - static_cast<U8>(this->m_activeBuffer)].slots[index];
    for (FwChanIdType bucket = 0; bucket < TLMCHAN_HASH_BUCKETS; bucket++) {
        if (inactiveEntry) {
            if (inactiveEntry->id == id) {
                break;
            } else {
                inactiveEntry = inactiveEntry->next;
            }
        } else {
            break;
        }
    }

    if (activeEntry && inactiveEntry) {
        Fw::TimeComparison cmp = Fw::Time::compare(inactiveEntry->lastUpdate, activeEntry->lastUpdate);
        if (cmp == Fw::TimeComparison::GT) {
            val = inactiveEntry->buffer;
            timeTag = inactiveEntry->lastUpdate;
            return Fw::TlmValid::VALID;
        } else if (cmp != Fw::TimeComparison::INCOMPARABLE) {
            val = activeEntry->buffer;
            timeTag = activeEntry->lastUpdate;
            return Fw::TlmValid::VALID;
        } else {
            if (inactiveEntry->updated) {
                val = inactiveEntry->buffer;
                timeTag = inactiveEntry->lastUpdate;
                return Fw::TlmValid::VALID;
            } else {
                val = activeEntry->buffer;
                timeTag = activeEntry->lastUpdate;
                return Fw::TlmValid::VALID;
            }
        }
    } else if (activeEntry) {
        val = activeEntry->buffer;
        timeTag = activeEntry->lastUpdate;
        return Fw::TlmValid::VALID;
    } else if (inactiveEntry) {
        val = inactiveEntry->buffer;
        timeTag = inactiveEntry->lastUpdate;
        return Fw::TlmValid::VALID;
    } else {
        val.resetSer();
    }
    return Fw::TlmValid::INVALID;
}

void TlmChan::TlmRecv_handler(FwIndexType portNum, FwChanIdType id, Fw::Time& timeTag, Fw::TlmBuffer& val) {
    static_assert(NUM_TLMRECV_INPUT_PORTS == 1, "TlmRecv_handler expects exactly one input port");
    FwChanIdType index = this->doHash(id);
    TlmEntry* entryToUse = nullptr;
    TlmEntry* prevEntry = nullptr;

    // Search to see if channel has already been stored or a bucket needs to be added
    if (this->m_tlmEntries[static_cast<U8>(this->m_activeBuffer)].slots[index]) {
        entryToUse = this->m_tlmEntries[static_cast<U8>(this->m_activeBuffer)].slots[index];
        // Loop one extra time so that we don't inadvertently fall through the end of the loop early.
        for (FwChanIdType bucket = 0; bucket < TLMCHAN_HASH_BUCKETS + 1; bucket++) {
            if (entryToUse) {
                if (entryToUse->id == id) {
                    break;
                } else {
                    prevEntry = entryToUse;
                    entryToUse = entryToUse->next;
                }
            } else {
                // Make sure that we haven't run out of buckets
                FW_ASSERT(this->m_tlmEntries[static_cast<U8>(this->m_activeBuffer)].free < TLMCHAN_HASH_BUCKETS);
                // add new bucket from free list
                entryToUse = &this->m_tlmEntries[static_cast<U8>(this->m_activeBuffer)]
                                  .buckets[this->m_tlmEntries[static_cast<U8>(this->m_activeBuffer)].free++];
                FW_ASSERT(prevEntry);
                prevEntry->next = entryToUse;
                entryToUse->next = nullptr;
                break;
            }
        }
    } else {
        FW_ASSERT(this->m_tlmEntries[static_cast<U8>(this->m_activeBuffer)].free < TLMCHAN_HASH_BUCKETS);
        this->m_tlmEntries[static_cast<U8>(this->m_activeBuffer)].slots[index] =
            &this->m_tlmEntries[static_cast<U8>(this->m_activeBuffer)]
                 .buckets[this->m_tlmEntries[static_cast<U8>(this->m_activeBuffer)].free++];
        entryToUse = this->m_tlmEntries[static_cast<U8>(this->m_activeBuffer)].slots[index];
        entryToUse->next = nullptr;
    }

    FW_ASSERT(entryToUse);
    entryToUse->used = true;
    entryToUse->id = id;
    entryToUse->updated = true;
    entryToUse->lastUpdate = timeTag;
    entryToUse->buffer = val;
}

void TlmChan::Run_handler(FwIndexType portNum, U32 context) {
    static_assert(NUM_RUN_INPUT_PORTS == 1, "Run_handler expects exactly one input port");
    // Only write packets if connected
    if (not this->isConnected_PktSend_OutputPort(0)) {
        return;
    }

    // Lock mutex long enough to swap the active buffer so the inactive buffer
    // can be read without worrying about concurrent updates.
    this->lock();
    this->m_activeBuffer =
        (this->m_activeBuffer == ActiveBuffer::Buffer_0) ? ActiveBuffer::Buffer_1 : ActiveBuffer::Buffer_0;
    // Clear the new active buffer's updated flags so it is clean for incoming
    // writes.  Any entries that were deferred (skipped) in the previous cycle
    // and still carry updated=true in this buffer are also cleared here.
    // This is intentional: deferred entries are dropped rather than re-queued,
    // which preserves Run_handler's bounded execution-time guarantee.
    for (U32 entry = 0; entry < TLMCHAN_HASH_BUCKETS; entry++) {
        this->m_tlmEntries[static_cast<U8>(this->m_activeBuffer)].buckets[entry].updated = false;
    }
    this->unLock();

    // -----------------------------------------------------------------------
    // CPU processing guard
    //
    // entriesProcessed — updated entries serialized into downlink packets this
    //                    invocation.  Hard-capped at TLMCHAN_MAX_ENTRIES_PER_RUN.
    // entriesDeferred  — updated entries skipped because the cap was already
    //                    reached.  These samples are dropped for this cycle.
    //                    A non-zero value means the system is producing
    //                    telemetry faster than Run_handler can drain it.
    // -----------------------------------------------------------------------
    U32 entriesProcessed = 0;
    U32 entriesDeferred = 0;

    Fw::TlmPacket pkt;
    pkt.resetPktSer();

    for (U32 entry = 0; entry < TLMCHAN_HASH_BUCKETS; entry++) {
        TlmEntry* p_entry = &this->m_tlmEntries[1 - static_cast<U8>(this->m_activeBuffer)].buckets[entry];
        if ((p_entry->updated) && (p_entry->used)) {
            // ------------------------------------------------------------------
            // CPU guard check: once the per-run cap is reached, count this entry
            // as deferred and skip serialization.  The entry's updated flag will
            // be cleared by the next buffer swap (see lock section above), so
            // the sample is intentionally dropped for this cycle.  This bounds
            // Run_handler's worst-case execution time and prevents it from
            // starving higher-priority tasks during a telemetry burst caused by
            // a hardware anomaly, runaway component, software fault, or
            // cyber-attack.
            // ------------------------------------------------------------------
            if (entriesProcessed >= TLMCHAN_MAX_ENTRIES_PER_RUN) {
                entriesDeferred++;
                continue;
            }

            Fw::SerializeStatus stat = pkt.addValue(p_entry->id, p_entry->lastUpdate, p_entry->buffer);

            if (Fw::FW_SERIALIZE_NO_ROOM_LEFT == stat) {
                this->PktSend_out(0, pkt.getBuffer(), 0);
                pkt.resetPktSer();
                stat = pkt.addValue(p_entry->id, p_entry->lastUpdate, p_entry->buffer);
                // If a single channel doesn't fit in an empty packet the packet
                // is misconfigured; assert so the error is visible immediately.
                FW_ASSERT(Fw::FW_SERIALIZE_OK == stat, static_cast<FwAssertArgType>(stat));
            } else if (Fw::FW_SERIALIZE_OK == stat) {
                // room available, continue filling packet
            } else {
                FW_ASSERT(0, static_cast<FwAssertArgType>(stat));
            }

            p_entry->updated = false;
            entriesProcessed++;
        }
    }

    // send remnant entries
    if (pkt.getNumEntries() > 0) {
        this->PktSend_out(0, pkt.getBuffer(), 0);
    }

    // Emit a WARNING_HI event when the processing cap was reached this cycle.
    // Using an event rather than injecting a reserved telemetry channel into
    // the downlink stream is the correct F-Prime anomaly reporting mechanism.
    if (entriesDeferred > 0) {
        this->m_procCapCount++;
        this->log_WARNING_HI_TlmChanEpochProcessingCapReached(entriesDeferred, this->m_procCapCount);
    }
}

}  // namespace Svc

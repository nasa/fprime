// ======================================================================
// \title  TlmPacketizerImpl.cpp
// \author tcanham
// \brief  cpp file for TlmPacketizer component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

#include <Fw/Com/ComPacket.hpp>
#include <Fw/FPrimeBasicTypes.hpp>
#include <Svc/TlmPacketizer/TlmPacketizer.hpp>

namespace Svc {

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

TlmPacketizer ::TlmPacketizer(const char* const compName)
    : TlmPacketizerComponentBase(compName), m_numPackets(0), m_configured(false), m_startLevel(0), m_maxLevel(0) {
    // clear slot pointers
    for (FwChanIdType entry = 0; entry < TLMPACKETIZER_NUM_TLM_HASH_SLOTS; entry++) {
        this->m_tlmEntries.slots[entry] = nullptr;
    }
    // clear buckets
    for (FwChanIdType entry = 0; entry < TLMPACKETIZER_HASH_BUCKETS; entry++) {
        this->m_tlmEntries.buckets[entry].used = false;
        this->m_tlmEntries.buckets[entry].bucketNo = entry;
        this->m_tlmEntries.buckets[entry].next = nullptr;
        this->m_tlmEntries.buckets[entry].id = 0;
    }
    // clear free index
    this->m_tlmEntries.free = 0;
    // clear missing tlm channel check
    for (FwChanIdType entry = 0; entry < TLMPACKETIZER_MAX_MISSING_TLM_CHECK; entry++) {
        this->m_missTlmCheck[entry].checked = false;
        this->m_missTlmCheck[entry].id = 0;
    }

    // clear packet buffers
    for (FwChanIdType buffer = 0; buffer < MAX_PACKETIZER_PACKETS; buffer++) {
        this->m_fillBuffers[buffer].updated = false;
        this->m_fillBuffers[buffer].requested = false;
        this->m_sendBuffers[buffer].updated = false;
    }
}

TlmPacketizer ::~TlmPacketizer() {}

void TlmPacketizer::setPacketList(const TlmPacketizerPacketList& packetList,
                                  const Svc::TlmPacketizerPacket& ignoreList,
                                  const FwChanIdType startLevel) {
    FW_ASSERT(packetList.list);
    FW_ASSERT(ignoreList.list);
    FW_ASSERT(packetList.numEntries <= MAX_PACKETIZER_PACKETS, static_cast<FwAssertArgType>(packetList.numEntries));
    // validate packet sizes against maximum com buffer size and populate hash
    // table
    for (FwChanIdType pktEntry = 0; pktEntry < packetList.numEntries; pktEntry++) {
        // Initial size is packetized telemetry descriptor + size of time tag + sizeof packet ID
        FwSizeType packetLen =
            sizeof(FwPacketDescriptorType) + Fw::Time::SERIALIZED_SIZE + sizeof(FwTlmPacketizeIdType);
        FW_ASSERT(packetList.list[pktEntry]->list, static_cast<FwAssertArgType>(pktEntry));
        // add up entries for each defined packet
        for (FwChanIdType tlmEntry = 0; tlmEntry < packetList.list[pktEntry]->numEntries; tlmEntry++) {
            // get hash value for id
            FwChanIdType id = packetList.list[pktEntry]->list[tlmEntry].id;
            TlmEntry* entryToUse = this->findBucket(id);
            // copy into entry
            FW_ASSERT(entryToUse);
            entryToUse->used = true;
            // not ignored channel
            entryToUse->ignored = false;
            entryToUse->id = id;
            entryToUse->hasValue = false;
            entryToUse->channelSize = packetList.list[pktEntry]->list[tlmEntry].size;
            // the offset into the buffer will be the current packet length
            // the offset must fit within FwSignedSizeType to allow for negative values
            FW_ASSERT(packetLen <= static_cast<FwSizeType>(std::numeric_limits<FwSignedSizeType>::max()),
                      static_cast<FwAssertArgType>(packetLen));
            entryToUse->packetOffset[pktEntry] = static_cast<FwSignedSizeType>(packetLen);

            packetLen += entryToUse->channelSize;

        }  // end channel in packet
        FW_ASSERT(packetLen <= FW_COM_BUFFER_MAX_SIZE, static_cast<FwAssertArgType>(packetLen),
                  static_cast<FwAssertArgType>(pktEntry));
        // clear contents
        memset(this->m_fillBuffers[pktEntry].buffer.getBuffAddr(), 0, static_cast<size_t>(packetLen));
        // serialize packet descriptor and packet ID now since it will always be the same
        Fw::SerializeStatus stat = this->m_fillBuffers[pktEntry].buffer.serialize(
            static_cast<FwPacketDescriptorType>(Fw::ComPacketType::FW_PACKET_PACKETIZED_TLM));
        FW_ASSERT(Fw::FW_SERIALIZE_OK == stat, stat);
        stat = this->m_fillBuffers[pktEntry].buffer.serialize(packetList.list[pktEntry]->id);
        FW_ASSERT(Fw::FW_SERIALIZE_OK == stat, stat);
        // set packet buffer length
        stat = this->m_fillBuffers[pktEntry].buffer.setBuffLen(packetLen);
        FW_ASSERT(Fw::FW_SERIALIZE_OK == stat, stat);
        // save ID
        this->m_fillBuffers[pktEntry].id = packetList.list[pktEntry]->id;
        // save level
        this->m_fillBuffers[pktEntry].level = packetList.list[pktEntry]->level;
        // store max level
        if (packetList.list[pktEntry]->level > this->m_maxLevel) {
            this->m_maxLevel = packetList.list[pktEntry]->level;
        }
        // save start level
        this->m_startLevel = startLevel;

    }  // end packet list

    // populate hash table with ignore list
    for (FwChanIdType channelEntry = 0; channelEntry < ignoreList.numEntries; channelEntry++) {
        // get hash value for id
        FwChanIdType id = ignoreList.list[channelEntry].id;

        TlmEntry* entryToUse = this->findBucket(id);

        // copy into entry
        FW_ASSERT(entryToUse);
        entryToUse->used = true;
        // is ignored channel
        entryToUse->ignored = true;
        entryToUse->id = id;
        entryToUse->hasValue = false;
        entryToUse->channelSize = ignoreList.list[channelEntry].size;
    }  // end ignore list

    // store number of packets
    this->m_numPackets = packetList.numEntries;

    // indicate configured
    this->m_configured = true;
}

TlmPacketizer::TlmEntry* TlmPacketizer::findBucket(FwChanIdType id) {
    FwChanIdType index = this->doHash(id);
    FW_ASSERT(index < TLMPACKETIZER_HASH_BUCKETS);
    TlmEntry* entryToUse = nullptr;
    TlmEntry* prevEntry = nullptr;

    // Search to see if channel has already been stored or a bucket needs to be added
    if (this->m_tlmEntries.slots[index]) {
        entryToUse = this->m_tlmEntries.slots[index];
        for (FwChanIdType bucket = 0; bucket < TLMPACKETIZER_HASH_BUCKETS; bucket++) {
            if (entryToUse) {
                if (entryToUse->id == id) {  // found the matching entry
                    break;
                } else {  // try next entry
                    prevEntry = entryToUse;
                    entryToUse = entryToUse->next;
                }
            } else {
                // Make sure that we haven't run out of buckets
                FW_ASSERT(this->m_tlmEntries.free < TLMPACKETIZER_HASH_BUCKETS,
                          static_cast<FwAssertArgType>(this->m_tlmEntries.free));
                // add new bucket from free list
                entryToUse = &this->m_tlmEntries.buckets[this->m_tlmEntries.free++];
                // Coverity warning about null dereference - see if it happens
                FW_ASSERT(prevEntry);
                prevEntry->next = entryToUse;
                // clear next pointer
                entryToUse->next = nullptr;
                // set all packet offsets to -1 for new entry
                for (FwChanIdType pktOffsetEntry = 0; pktOffsetEntry < MAX_PACKETIZER_PACKETS; pktOffsetEntry++) {
                    entryToUse->packetOffset[pktOffsetEntry] = -1;
                }
                break;
            }
        }
    } else {
        // Make sure that we haven't run out of buckets
        FW_ASSERT(this->m_tlmEntries.free < TLMPACKETIZER_HASH_BUCKETS,
                  static_cast<FwAssertArgType>(this->m_tlmEntries.free));
        // create new entry at slot head
        this->m_tlmEntries.slots[index] = &this->m_tlmEntries.buckets[this->m_tlmEntries.free++];
        entryToUse = this->m_tlmEntries.slots[index];
        entryToUse->next = nullptr;
        // set all packet offsets to -1 for new entry
        for (FwChanIdType pktOffsetEntry = 0; pktOffsetEntry < MAX_PACKETIZER_PACKETS; pktOffsetEntry++) {
            entryToUse->packetOffset[pktOffsetEntry] = -1;
        }
    }

    return entryToUse;
}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void TlmPacketizer ::TlmRecv_handler(const FwIndexType portNum,
                                     FwChanIdType id,
                                     Fw::Time& timeTag,
                                     Fw::TlmBuffer& val) {
    FW_ASSERT(this->m_configured);
    // get hash value for id
    FwChanIdType index = this->doHash(id);
    TlmEntry* entryToUse = nullptr;

    // Search to see if the channel is being sent
    entryToUse = this->m_tlmEntries.slots[index];

    // if no entries at hash, channel not part of a packet or is not ignored
    if (not entryToUse) {
        this->missingChannel(id);
        return;
    }

    for (FwChanIdType bucket = 0; bucket < TLMPACKETIZER_HASH_BUCKETS; bucket++) {
        if (entryToUse) {
            if (entryToUse->id == id) {  // found the matching entry
                // check to see if the channel is ignored. If so, just return.
                if (entryToUse->ignored) {
                    return;
                }
                break;
            } else {  // try next entry
                entryToUse = entryToUse->next;
            }
        } else {
            // telemetry channel not in any packets
            this->missingChannel(id);
            return;
        }
    }

    // copy telemetry value into active buffers
    for (FwChanIdType pkt = 0; pkt < MAX_PACKETIZER_PACKETS; pkt++) {
        // check if current packet has this channel
        if (entryToUse->packetOffset[pkt] != -1) {
            // get destination address
            this->m_lock.lock();
            this->m_fillBuffers[pkt].updated = true;
            this->m_fillBuffers[pkt].latestTime = timeTag;
            U8* ptr = &this->m_fillBuffers[pkt].buffer.getBuffAddr()[entryToUse->packetOffset[pkt]];
            (void)memcpy(ptr, val.getBuffAddr(), static_cast<size_t>(val.getBuffLength()));
            // record that this chan has a value. could do this outside of the loop only once
            // but then we'd need to grab the lock again.
            entryToUse->hasValue = true;
            this->m_lock.unLock();
        }
    }
}

//! Handler for input port TlmGet
Fw::TlmValid TlmPacketizer ::TlmGet_handler(FwIndexType portNum,  //!< The port number
                                            FwChanIdType id,      //!< Telemetry Channel ID
                                            Fw::Time& timeTag,    //!< Time Tag
                                            Fw::TlmBuffer& val    //!< Buffer containing serialized telemetry value.
                                                                  //!< Size set to 0 if channel not found.
) {
    FW_ASSERT(this->m_configured);
    // get hash value for id
    FwChanIdType index = this->doHash(id);
    TlmEntry* entryToUse = nullptr;

    // Search to see if the channel is being sent
    entryToUse = this->m_tlmEntries.slots[index];

    // if no entries at hash, channel not part of a packet or is not ignored
    if (not entryToUse) {
        this->missingChannel(id);
        val.resetSer();
        return Fw::TlmValid::INVALID;
    }

    for (FwChanIdType bucket = 0; bucket < TLMPACKETIZER_HASH_BUCKETS; bucket++) {
        if (entryToUse) {
            if (entryToUse->id == id) {  // found the matching entry
                // check to see if the channel is ignored. If so, just return, as
                // we don't store the bytes of ignored channels
                if (entryToUse->ignored) {
                    val.resetSer();
                    return Fw::TlmValid::INVALID;
                }
                break;
            } else {  // try next entry
                entryToUse = entryToUse->next;
            }
        } else {
            // telemetry channel not in any packets
            this->missingChannel(id);
            val.resetSer();
            return Fw::TlmValid::INVALID;
        }
    }

    if (!entryToUse->hasValue) {
        // haven't received a value yet for this entry.
        val.resetSer();
        return Fw::TlmValid::INVALID;
    }

    // make sure we have enough space to store this entry in our buf
    FW_ASSERT(entryToUse->channelSize <= val.getBuffCapacity(), static_cast<FwAssertArgType>(entryToUse->channelSize),
              static_cast<FwAssertArgType>(val.getBuffCapacity()));

    // okay, we have the matching entry.
    // go over each packet and find the first one which stores this channel

    for (FwChanIdType pkt = 0; pkt < MAX_PACKETIZER_PACKETS; pkt++) {
        // check if current packet has this channel
        if (entryToUse->packetOffset[pkt] != -1) {
            // okay, it has the channel. copy chan val into the tlm buf
            this->m_lock.lock();
            timeTag = this->m_fillBuffers[pkt].latestTime;
            U8* ptr = &this->m_fillBuffers[pkt].buffer.getBuffAddr()[entryToUse->packetOffset[pkt]];
            (void)memcpy(val.getBuffAddr(), ptr, static_cast<size_t>(entryToUse->channelSize));
            // set buf len to the channelSize. keep in mind, this is the MAX serialized size of the channel.
            // so we may actually be filling val with some junk after the value of the channel.
            FW_ASSERT(val.setBuffLen(entryToUse->channelSize) == Fw::SerializeStatus::FW_SERIALIZE_OK);
            this->m_lock.unLock();
            return Fw::TlmValid::VALID;
        }
    }

    // did not find a packet which stores this channel.
    // coding error, this was not an ignored channel so it must be in a packet somewhere
    FW_ASSERT(0, static_cast<FwAssertArgType>(entryToUse->id));
    // TPP (tim paranoia principle)
    val.resetSer();
    return Fw::TlmValid::INVALID;
}

void TlmPacketizer ::Run_handler(const FwIndexType portNum, U32 context) {
    FW_ASSERT(this->m_configured);

    // Only write packets if connected
    if (not this->isConnected_PktSend_OutputPort(0)) {
        return;
    }

    // lock mutex long enough to modify active telemetry buffer
    // so the data can be read without worrying about updates
    this->m_lock.lock();
    // copy buffers from fill side to send side
    for (FwChanIdType pkt = 0; pkt < this->m_numPackets; pkt++) {
        if ((this->m_fillBuffers[pkt].updated) and
            ((this->m_fillBuffers[pkt].level <= this->m_startLevel) or (this->m_fillBuffers[pkt].requested))) {
            this->m_sendBuffers[pkt] = this->m_fillBuffers[pkt];
            if (PACKET_UPDATE_ON_CHANGE == PACKET_UPDATE_MODE) {
                this->m_fillBuffers[pkt].updated = false;
            }
            this->m_fillBuffers[pkt].requested = false;
            // PACKET_UPDATE_AFTER_FIRST_CHANGE will be this case - updated flag will not be cleared
        } else if ((PACKET_UPDATE_ALWAYS == PACKET_UPDATE_MODE) and
                   (this->m_fillBuffers[pkt].level <= this->m_startLevel)) {
            this->m_sendBuffers[pkt] = this->m_fillBuffers[pkt];
            this->m_sendBuffers[pkt].updated = true;
        } else {
            this->m_sendBuffers[pkt].updated = false;
        }
    }
    this->m_lock.unLock();

    // push all updated packet buffers
    for (FwChanIdType pkt = 0; pkt < this->m_numPackets; pkt++) {
        if (this->m_sendBuffers[pkt].updated) {
            // serialize time into time offset in packet
            Fw::ExternalSerializeBuffer buff(
                &this->m_sendBuffers[pkt]
                     .buffer.getBuffAddr()[sizeof(FwPacketDescriptorType) + sizeof(FwTlmPacketizeIdType)],
                Fw::Time::SERIALIZED_SIZE);
            Fw::SerializeStatus stat = buff.serialize(this->m_sendBuffers[pkt].latestTime);
            FW_ASSERT(Fw::FW_SERIALIZE_OK == stat, stat);

            this->PktSend_out(0, this->m_sendBuffers[pkt].buffer, 0);
        }
    }
}

void TlmPacketizer ::pingIn_handler(const FwIndexType portNum, U32 key) {
    // return key
    this->pingOut_out(0, key);
}

// ----------------------------------------------------------------------
// Command handler implementations
// ----------------------------------------------------------------------

void TlmPacketizer ::SET_LEVEL_cmdHandler(const FwOpcodeType opCode, const U32 cmdSeq, FwChanIdType level) {
    this->m_startLevel = level;
    if (level > this->m_maxLevel) {
        this->log_WARNING_LO_MaxLevelExceed(level, this->m_maxLevel);
    }
    this->tlmWrite_SendLevel(level);
    this->log_ACTIVITY_HI_LevelSet(level);
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void TlmPacketizer ::SEND_PKT_cmdHandler(const FwOpcodeType opCode, const U32 cmdSeq, U32 id) {
    FwChanIdType pkt = 0;
    for (pkt = 0; pkt < this->m_numPackets; pkt++) {
        if (this->m_fillBuffers[pkt].id == id) {
            this->m_lock.lock();
            this->m_fillBuffers[pkt].updated = true;
            this->m_fillBuffers[pkt].latestTime = this->getTime();
            this->m_fillBuffers[pkt].requested = true;
            this->m_lock.unLock();

            this->log_ACTIVITY_LO_PacketSent(id);
            break;
        }
    }

    // couldn't find it
    if (pkt == this->m_numPackets) {
        log_WARNING_LO_PacketNotFound(id);
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::VALIDATION_ERROR);
        return;
    }

    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

FwChanIdType TlmPacketizer::doHash(FwChanIdType id) {
    return (id % TLMPACKETIZER_HASH_MOD_VALUE) % TLMPACKETIZER_NUM_TLM_HASH_SLOTS;
}

void TlmPacketizer::missingChannel(FwChanIdType id) {
    // search to see if missing channel has already been sent
    for (FwChanIdType slot = 0; slot < TLMPACKETIZER_MAX_MISSING_TLM_CHECK; slot++) {
        // if it's been checked, return
        if (this->m_missTlmCheck[slot].checked and (this->m_missTlmCheck[slot].id == id)) {
            return;
        } else if (not this->m_missTlmCheck[slot].checked) {
            this->m_missTlmCheck[slot].checked = true;
            this->m_missTlmCheck[slot].id = id;
            this->log_WARNING_LO_NoChan(id);
            return;
        }
    }
}

}  // end namespace Svc

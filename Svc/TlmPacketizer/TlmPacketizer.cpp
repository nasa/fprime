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
#include <cstring>

namespace Svc {

const TlmPacketizer_TelemetrySendPortMap TlmPacketizer::TELEMETRY_SEND_PORT_MAP = {};

static_assert(Svc::TelemetrySection::NUM_SECTIONS >= 1, "At least one telemetry section is required");

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

TlmPacketizer ::TlmPacketizer(const char* const compName)
    : TlmPacketizerComponentBase(compName), m_numPackets(0), m_configured(false) {
    // clear missing tlm channel check
    for (FwChanIdType entry = 0; entry < TLMPACKETIZER_MAX_MISSING_TLM_CHECK; entry++) {
        this->m_missTlmCheck[entry].checked = false;
        this->m_missTlmCheck[entry].id = 0;
    }

    // clear packet buffers
    for (FwChanIdType buffer = 0; buffer < MAX_PACKETIZER_PACKETS; buffer++) {
        this->m_fillBuffers[buffer].updated = false;
    }

    // enable sections
    for (FwIndexType section = 0; section < TelemetrySection::NUM_SECTIONS; section++) {
        (void)(this->m_sectionEnabled[static_cast<FwSizeType>(section)] = Fw::Enabled::ENABLED);
    }

    static_assert(NUM_CONFIGURABLE_TLMPACKETIZER_GROUPS == MAX_CONFIGURABLE_TLMPACKETIZER_GROUP + 1,
                  "NUM_CONFIGURABLE_TLMPACKETIZER_GROUPS MUST BE MAX_CONFIGURABLE_TLMPACKETIZER_GROUP + 1");
}

TlmPacketizer ::~TlmPacketizer() {}

void TlmPacketizer::setPacketList(const TlmPacketizerPacketList& packetList,
                                  const Svc::TlmPacketizerPacket& ignoreList,
                                  const FwChanIdType startLevel,
                                  const TlmPacketizer_GroupConfig& defaultGroupConfig) {
    FW_ASSERT(packetList.list);
    // Ignore list may be nullptr as long as numEntries is 0. Providing an ignore list with numEntries 0 disables
    // functionality for two reasons:
    //     1. There are no ignored channels as configured by FPP.
    //     2. Ignore functionality is intentionally disabled by project where nullptr was intentionally supplied.
    FW_ASSERT(ignoreList.list || ignoreList.numEntries == 0);
    FW_ASSERT(packetList.numEntries <= MAX_PACKETIZER_PACKETS, static_cast<FwAssertArgType>(packetList.numEntries));
    // validate packet sizes against maximum com buffer size and populate hash
    // table
    FwChanIdType maxLevel = 0;
    for (FwChanIdType pktEntry = 0; pktEntry < packetList.numEntries; pktEntry++) {
        // Initial size is packetized telemetry descriptor + size of time tag + sizeof packet ID
        FwSizeType packetLen =
            sizeof(FwPacketDescriptorType) + Fw::Time::SERIALIZED_SIZE + sizeof(FwTlmPacketizeIdType);
        FW_ASSERT(packetList.list[pktEntry]->list, static_cast<FwAssertArgType>(pktEntry));
        // add up entries for each defined packet
        for (FwChanIdType tlmEntry = 0; tlmEntry < packetList.list[pktEntry]->numEntries; tlmEntry++) {
            FwChanIdType id = packetList.list[pktEntry]->list[tlmEntry].id;
            TlmEntry entry;
            if (this->m_channels.find(id, entry) != Fw::Success::SUCCESS) {
                // New channel - initialize offsets to -1 (not in any packet)
                entry.id = id;
                entry.hasValue = false;
                for (FwChanIdType pktOffsetEntry = 0; pktOffsetEntry < MAX_PACKETIZER_PACKETS; pktOffsetEntry++) {
                    entry.packetOffset[pktOffsetEntry] = -1;
                }
            }
            // not ignored channel
            entry.ignored = false;
            entry.channelSize = packetList.list[pktEntry]->list[tlmEntry].size;
            // the offset into the buffer will be the current packet length
            // the offset must fit within FwSignedSizeType to allow for negative values
            FW_ASSERT(packetLen <= static_cast<FwSizeType>(std::numeric_limits<FwSignedSizeType>::max()),
                      static_cast<FwAssertArgType>(packetLen));
            entry.packetOffset[pktEntry] = static_cast<FwSignedSizeType>(packetLen);
            const Fw::Success insertStatus = this->m_channels.insert(id, entry);
            FW_ASSERT(insertStatus == Fw::Success::SUCCESS, static_cast<FwAssertArgType>(insertStatus));

            packetLen += entry.channelSize;

        }  // end channel in packet
        FW_ASSERT(packetLen <= FW_COM_BUFFER_MAX_SIZE, static_cast<FwAssertArgType>(packetLen),
                  static_cast<FwAssertArgType>(pktEntry));
        // clear contents
        memset(this->m_fillBuffers[pktEntry].buffer.getBuffAddr(), 0, static_cast<size_t>(packetLen));
        // serialize packet descriptor and packet ID now since it will always be the same
        Fw::SerializeStatus stat = this->m_fillBuffers[pktEntry].buffer.serializeFrom(
            static_cast<FwPacketDescriptorType>(Fw::ComPacketType::FW_PACKET_PACKETIZED_TLM));
        FW_ASSERT(Fw::FW_SERIALIZE_OK == stat, stat);
        stat = this->m_fillBuffers[pktEntry].buffer.serializeFrom(packetList.list[pktEntry]->id);
        FW_ASSERT(Fw::FW_SERIALIZE_OK == stat, stat);
        // set packet buffer length
        stat = this->m_fillBuffers[pktEntry].buffer.setBuffLen(packetLen);
        FW_ASSERT(Fw::FW_SERIALIZE_OK == stat, stat);
        // save ID
        this->m_fillBuffers[pktEntry].id = packetList.list[pktEntry]->id;
        // save level
        this->m_fillBuffers[pktEntry].level = packetList.list[pktEntry]->level;
        // store max level
        if (packetList.list[pktEntry]->level > maxLevel) {
            maxLevel = packetList.list[pktEntry]->level;
        }

    }  // end packet list
    FW_ASSERT(maxLevel <= MAX_CONFIGURABLE_TLMPACKETIZER_GROUP, static_cast<FwAssertArgType>(maxLevel));

    // Enable and set group configurations
    for (FwIndexType section = 0; section < TelemetrySection::NUM_SECTIONS; section++) {
        for (FwChanIdType group = 0; group < NUM_CONFIGURABLE_TLMPACKETIZER_GROUPS; group++) {
            Fw::Enabled groupEnabled = group <= startLevel ? Fw::Enabled::ENABLED : Fw::Enabled::DISABLED;
            this->m_groupConfigs[static_cast<FwSizeType>(section)][group] = defaultGroupConfig;
            this->m_groupConfigs[static_cast<FwSizeType>(section)][group].set_enabled(groupEnabled);
        }
    }

    // This section adds entries in the map for channels that are intended to be ignored. When the user supplies
    // a list with no length, this loop is skipped. To turn-off ignoring of channels, the user can provide a null
    // list with 0 length.
    for (FwChanIdType channelEntry = 0; channelEntry < ignoreList.numEntries; channelEntry++) {
        FwChanIdType id = ignoreList.list[channelEntry].id;
        TlmEntry entry;
        if (this->m_channels.find(id, entry) != Fw::Success::SUCCESS) {
            // New channel - initialize offsets to -1 (not in any packet)
            entry.id = id;
            entry.hasValue = false;
            for (FwChanIdType pktOffsetEntry = 0; pktOffsetEntry < MAX_PACKETIZER_PACKETS; pktOffsetEntry++) {
                entry.packetOffset[pktOffsetEntry] = -1;
            }
        }
        // is ignored channel
        entry.ignored = true;
        entry.channelSize = ignoreList.list[channelEntry].size;
        const Fw::Success insertStatus = this->m_channels.insert(id, entry);
        FW_ASSERT(insertStatus == Fw::Success::SUCCESS, static_cast<FwAssertArgType>(insertStatus));
    }  // end ignore list

    // store number of packets
    this->m_numPackets = packetList.numEntries;

    // indicate configured
    this->m_configured = true;
}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void TlmPacketizer ::TlmRecv_handler(const FwIndexType portNum,
                                     FwChanIdType id,
                                     Fw::Time& timeTag,
                                     Fw::TlmBuffer& val) {
    FW_ASSERT(this->m_configured);
    TlmEntry entry;

    // Search to see if the channel is being tracked
    if (this->m_channels.find(id, entry) != Fw::Success::SUCCESS) {
        // channel not part of a packet and not ignored
        this->missingChannel(id);
        return;
    }

    // check to see if the channel is ignored. If so, just return.
    if (entry.ignored) {
        return;
    }

    // copy telemetry value into active buffers
    entry.hasValue = true;
    for (FwChanIdType pkt = 0; pkt < MAX_PACKETIZER_PACKETS; pkt++) {
        // check if current packet has this channel
        if (entry.packetOffset[pkt] != -1) {
            // get destination address
            this->m_lock.lock();
            this->m_fillBuffers[pkt].updated = true;
            this->m_fillBuffers[pkt].latestTime = timeTag;
            U8* ptr = &this->m_fillBuffers[pkt].buffer.getBuffAddr()[entry.packetOffset[pkt]];
            (void)memcpy(ptr, val.getBuffAddr(), static_cast<size_t>(val.getSize()));
            this->m_lock.unLock();
        }
    }
    // Update hasValue in the map
    const Fw::Success insertStatus = this->m_channels.insert(id, entry);
    FW_ASSERT(insertStatus == Fw::Success::SUCCESS, static_cast<FwAssertArgType>(insertStatus));
}

void TlmPacketizer ::configureSectionGroupRate_handler(FwIndexType portNum,
                                                       const Svc::TelemetrySection& section,
                                                       FwChanIdType tlmGroup,
                                                       const Svc::RateLogic& rateLogic,
                                                       U32 minDelta,
                                                       U32 maxDelta) {
    this->configureSectionGroupRate(section, tlmGroup, rateLogic, minDelta, maxDelta);
}

//! Handler for input port TlmGet
Fw::TlmValid TlmPacketizer ::TlmGet_handler(FwIndexType portNum,  //!< The port number
                                            FwChanIdType id,      //!< Telemetry Channel ID
                                            Fw::Time& timeTag,    //!< Time Tag
                                            Fw::TlmBuffer& val    //!< Buffer containing serialized telemetry value.
                                                                  //!< Size set to 0 if channel not found.
) {
    FW_ASSERT(this->m_configured);
    TlmEntry entry;

    // Search to see if the channel is being tracked
    if (this->m_channels.find(id, entry) != Fw::Success::SUCCESS) {
        // channel not part of a packet and not ignored
        this->missingChannel(id);
        val.resetSer();
        return Fw::TlmValid::INVALID;
    }

    // check to see if the channel is ignored. If so, just return, as
    // we don't store the bytes of ignored channels
    if (entry.ignored) {
        val.resetSer();
        return Fw::TlmValid::INVALID;
    }

    if (!entry.hasValue) {
        // haven't received a value yet for this entry.
        val.resetSer();
        return Fw::TlmValid::INVALID;
    }

    // make sure we have enough space to store this entry in our buf
    FW_ASSERT(entry.channelSize <= val.getCapacity(), static_cast<FwAssertArgType>(entry.channelSize),
              static_cast<FwAssertArgType>(val.getCapacity()));

    // okay, we have the matching entry.
    // go over each packet and find the first one which stores this channel

    for (FwChanIdType pkt = 0; pkt < MAX_PACKETIZER_PACKETS; pkt++) {
        // check if current packet has this channel
        if (entry.packetOffset[pkt] != -1) {
            // okay, it has the channel. copy chan val into the tlm buf
            this->m_lock.lock();
            timeTag = this->m_fillBuffers[pkt].latestTime;
            U8* ptr = &this->m_fillBuffers[pkt].buffer.getBuffAddr()[entry.packetOffset[pkt]];
            (void)memcpy(val.getBuffAddr(), ptr, static_cast<size_t>(entry.channelSize));
            // set buf len to the channelSize. keep in mind, this is the MAX serialized size of the channel.
            // so we may actually be filling val with some junk after the value of the channel.
            FW_ASSERT(val.setBuffLen(entry.channelSize) == Fw::SerializeStatus::FW_SERIALIZE_OK);
            this->m_lock.unLock();
            return Fw::TlmValid::VALID;
        }
    }

    // did not find a packet which stores this channel.
    // coding error, this was not an ignored channel so it must be in a packet somewhere
    FW_ASSERT(0, static_cast<FwAssertArgType>(entry.id));
    // TPP (tim paranoia principle)
    val.resetSer();
    return Fw::TlmValid::INVALID;
}

void TlmPacketizer ::Run_handler(const FwIndexType portNum, U32 context) {
    FW_ASSERT(this->m_configured);

    // For each packet, send if update, enable, and rate conditions are met
    for (FwChanIdType pkt = 0; pkt < this->m_numPackets; pkt++) {
        this->m_lock.lock();

        // Copy packet data to avoid concurrent updates
        FwChanIdType entryGroup = this->m_fillBuffers[pkt].level;
        Fw::ComBuffer sendBuffer = this->m_fillBuffers[pkt].buffer;
        Fw::Time time = this->m_fillBuffers[pkt].latestTime;
        bool updated = this->m_fillBuffers[pkt].updated;
        this->m_fillBuffers[pkt].updated = false;

        this->m_lock.unLock();

        // Iterate through output sections
        for (FwIndexType section = 0; section < TelemetrySection::NUM_SECTIONS; section++) {
            // Packet is updated and not REQUESTED (Keep REQUESTED marking to bypass disable checks)
            if (updated and this->m_packetFlags[section][pkt].updateFlag != UpdateFlag::REQUESTED) {
                this->m_packetFlags[section][pkt].updateFlag = UpdateFlag::NEW;
            }

            bool sendOutFlag = false;
            const FwIndexType outIndex = this->sectionGroupToPort(section, entryGroup);

            PktSendCounters& pktEntryFlags = this->m_packetFlags[static_cast<FwSizeType>(section)][pkt];
            TlmPacketizer_GroupConfig& entryGroupConfig =
                this->m_groupConfigs[static_cast<FwSizeType>(section)][entryGroup];

            /* Base conditions for sending
            1. Output port is connected
            2. The packet was requested (Override Checks).

            If the packet wasn't requested:
            3. The Section and Group in Section is enabled OR the Group in Section is force enabled
            4. The rate logic is not SILENCED.
            5. The packet has data (marked updated in the past or new)
            */
            if (not this->isConnected_PktSend_OutputPort(outIndex)) {
                continue;
            }
            if (pktEntryFlags.updateFlag == UpdateFlag::REQUESTED) {
                sendOutFlag = true;
            } else {
                if (not((entryGroupConfig.get_enabled() and
                         this->m_sectionEnabled[static_cast<FwSizeType>(section)] == Fw::Enabled::ENABLED) or
                        entryGroupConfig.get_forceEnabled() == Fw::Enabled::ENABLED)) {
                    continue;
                }
                if (entryGroupConfig.get_rateLogic() == Svc::RateLogic::SILENCED) {
                    continue;
                }
                if (pktEntryFlags.updateFlag == UpdateFlag::NEVER_UPDATED) {
                    continue;  // Avoid No Data
                }
            }

            // Update Counter, prevent overflow.
            if (pktEntryFlags.prevSentCounter < std::numeric_limits<U32>::max()) {
                pktEntryFlags.prevSentCounter++;
            }

            /*
            1. Packet has been updated
            2. Group Logic includes checking MIN
            3. Packet sent counter at MIN
            */
            if (pktEntryFlags.updateFlag == UpdateFlag::NEW and
                entryGroupConfig.get_rateLogic() != Svc::RateLogic::EVERY_MAX and
                pktEntryFlags.prevSentCounter >= entryGroupConfig.get_min()) {
                sendOutFlag = true;
            }

            /*
            1. Group Logic includes checking MAX
            2. Packet set counter is at MAX
            */
            if (entryGroupConfig.get_rateLogic() != Svc::RateLogic::ON_CHANGE_MIN and
                pktEntryFlags.prevSentCounter >= entryGroupConfig.get_max()) {
                sendOutFlag = true;
            }

            // Send under the following conditions:
            // 1. Packet received updates and it has been past delta min counts since last packet (min enabled)
            // 2. Packet has passed delta max counts since last packet (max enabled)
            // With the above, the group must be either enabled or force enabled.
            // 3. If the packet was requested.
            if (sendOutFlag) {
                // serialize time into time offset in packet
                Fw::ExternalSerializeBuffer buff(
                    &sendBuffer.getBuffAddr()[sizeof(FwPacketDescriptorType) + sizeof(FwTlmPacketizeIdType)],
                    Fw::Time::SERIALIZED_SIZE);
                Fw::SerializeStatus stat = buff.serializeFrom(time);
                FW_ASSERT(Fw::FW_SERIALIZE_OK == stat, stat);
                this->PktSend_out(outIndex, sendBuffer, pktEntryFlags.prevSentCounter);

                pktEntryFlags.prevSentCounter = 0;
                pktEntryFlags.updateFlag = UpdateFlag::PAST;
            }
        }
    }
}

void TlmPacketizer ::controlIn_handler(FwIndexType portNum,
                                       const Svc::TelemetrySection& section,
                                       const Fw::Enabled& enabled) {
    FW_ASSERT(section.isValid());
    FW_ASSERT(enabled.isValid());
    if (0 <= section && section < TelemetrySection::NUM_SECTIONS) {
        (void)(this->m_sectionEnabled[static_cast<FwSizeType>(section)] = enabled);
    } else {
        this->log_WARNING_LO_SectionUnconfigurable(section, enabled);
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
    if (level > MAX_CONFIGURABLE_TLMPACKETIZER_GROUP) {
        this->log_WARNING_LO_MaxLevelExceed(level, MAX_CONFIGURABLE_TLMPACKETIZER_GROUP);
    }
    for (FwIndexType section = 0; section < TelemetrySection::NUM_SECTIONS; section++) {
        for (FwChanIdType group = 0; group < NUM_CONFIGURABLE_TLMPACKETIZER_GROUPS; group++) {
            this->m_groupConfigs[static_cast<FwSizeType>(section)][group].set_enabled(
                group <= level ? Fw::Enabled::ENABLED : Fw::Enabled::DISABLED);
        }
    }
    this->tlmWrite_GroupConfigs(this->m_groupConfigs);
    this->log_ACTIVITY_HI_LevelSet(level);
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void TlmPacketizer ::SEND_PKT_cmdHandler(const FwOpcodeType opCode,
                                         const U32 cmdSeq,
                                         const U32 id,
                                         const Svc::TelemetrySection section) {
    FW_ASSERT(section.isValid());
    FwChanIdType pkt = 0;
    for (pkt = 0; pkt < this->m_numPackets; pkt++) {
        if (this->m_fillBuffers[pkt].id == id) {
            this->m_lock.lock();
            this->m_fillBuffers[pkt].updated = true;
            this->m_fillBuffers[pkt].latestTime = this->getTime();
            this->m_lock.unLock();

            this->m_packetFlags[section][pkt].updateFlag = UpdateFlag::REQUESTED;

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

void TlmPacketizer ::ENABLE_SECTION_cmdHandler(FwOpcodeType opCode,
                                               U32 cmdSeq,
                                               Svc::TelemetrySection section,
                                               Fw::Enabled enable) {
    FW_ASSERT(section.isValid());
    FW_ASSERT(enable.isValid());
    if (section < 0 or section >= TelemetrySection::NUM_SECTIONS) {
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::VALIDATION_ERROR);
        return;
    }
    (void)(this->m_sectionEnabled[section] = enable);
    this->tlmWrite_SectionEnabled(this->m_sectionEnabled);
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void TlmPacketizer ::ENABLE_GROUP_cmdHandler(FwOpcodeType opCode,
                                             U32 cmdSeq,
                                             Svc::TelemetrySection section,
                                             FwChanIdType tlmGroup,
                                             Fw::Enabled enable) {
    FW_ASSERT(section.isValid());
    FW_ASSERT(enable.isValid());
    if ((0 <= section and section >= TelemetrySection::NUM_SECTIONS) or
        tlmGroup > MAX_CONFIGURABLE_TLMPACKETIZER_GROUP) {
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::VALIDATION_ERROR);
        return;
    }
    this->m_groupConfigs[section][tlmGroup].set_enabled(enable);
    this->tlmWrite_GroupConfigs(this->m_groupConfigs);
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void TlmPacketizer ::FORCE_GROUP_cmdHandler(FwOpcodeType opCode,
                                            U32 cmdSeq,
                                            Svc::TelemetrySection section,
                                            FwChanIdType tlmGroup,
                                            Fw::Enabled enable) {
    FW_ASSERT(section.isValid());
    FW_ASSERT(enable.isValid());
    if (section < 0 or section >= TelemetrySection::NUM_SECTIONS or tlmGroup > MAX_CONFIGURABLE_TLMPACKETIZER_GROUP) {
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::VALIDATION_ERROR);
        return;
    }
    this->m_groupConfigs[section][tlmGroup].set_forceEnabled(enable);
    this->tlmWrite_GroupConfigs(this->m_groupConfigs);
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void TlmPacketizer ::CONFIGURE_GROUP_RATES_cmdHandler(FwOpcodeType opCode,
                                                      U32 cmdSeq,
                                                      Svc::TelemetrySection section,
                                                      FwChanIdType tlmGroup,
                                                      Svc::RateLogic rateLogic,
                                                      U32 minDelta,
                                                      U32 maxDelta) {
    FW_ASSERT(section.isValid());
    FW_ASSERT(rateLogic.isValid());
    if (section < 0 or section >= TelemetrySection::NUM_SECTIONS or tlmGroup > MAX_CONFIGURABLE_TLMPACKETIZER_GROUP) {
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::VALIDATION_ERROR);
        return;
    }
    this->configureSectionGroupRate(section, tlmGroup, rateLogic, minDelta, maxDelta);
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void TlmPacketizer::configureSectionGroupRate(
    const Svc::TelemetrySection& section,  //!< Section grouping
    FwChanIdType tlmGroup,                 //!< Group Identifier
    const Svc::RateLogic& rateLogic,       //!< Rate Logic
    U32 minDelta,  //!< Minimum Sched Ticks to send packets on updates when using ON_CHANGE logic
    U32 maxDelta   //!< Maximum Sched Ticks between packets to send when using EVERY_MAX logic
) {
    FW_ASSERT(section.isValid());
    FW_ASSERT(rateLogic.isValid());
    // These two asserts are an "if" statement in a command so they will no assert on bad user data
    FW_ASSERT(section >= 0 and section < TelemetrySection::NUM_SECTIONS);
    FW_ASSERT(tlmGroup <= MAX_CONFIGURABLE_TLMPACKETIZER_GROUP);

    TlmPacketizer_GroupConfig& groupConfig = this->m_groupConfigs[section][tlmGroup];
    groupConfig.set_rateLogic(rateLogic);
    groupConfig.set_min(minDelta);
    groupConfig.set_max(maxDelta);
    this->tlmWrite_GroupConfigs(this->m_groupConfigs);
}

FwIndexType TlmPacketizer::sectionGroupToPort(const FwIndexType section, const FwSizeType group) {
    // Confirm the indices will not overflow the size of the array
    FW_ASSERT(group < TlmPacketizer_TelemetrySendSection::SIZE, static_cast<FwAssertArgType>(group));
    FW_ASSERT(section < TlmPacketizer_TelemetrySendPortMap::SIZE, static_cast<FwAssertArgType>(section));

    const FwIndexType outIndex = TlmPacketizer::TELEMETRY_SEND_PORT_MAP[static_cast<FwSizeType>(section)][group];

    // Confirm the output port index is within the valid number of telemetry send ports
    FW_ASSERT(outIndex < TELEMETRY_SEND_PORTS, static_cast<FwAssertArgType>(outIndex));
    return outIndex;
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

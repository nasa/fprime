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
#include <Fw/Prm/ParamValid.hpp>
#include <Svc/TlmPacketizer/TlmPacketizer.hpp>
#include <TlmPacketizerConfig/FppConstantsAc.hpp>
#include <cstring>
#include <limits>

namespace Svc {

const TlmPacketizer_TelemetrySendPortMap TlmPacketizer::TELEMETRY_SEND_PORT_MAP = {};

static_assert(Svc::TelemetrySection::NUM_SECTIONS >= 1, "At least one telemetry section is required");

// Keeps every offset a ComBuffer can hold representable in the offset type; setPacketList additionally asserts
// at runtime that no offset equals the NOT_IN_PACKET sentinel (the type's maximum)
static_assert(FW_COM_BUFFER_MAX_SIZE <= std::numeric_limits<FwTlmPacketizeOffsetType>::max(),
              "FwTlmPacketizeOffsetType must span an entire ComBuffer");

static_assert(static_cast<U8>(Fw::FW_DESERIALIZE_BUFFER_EMPTY) == Fw::DeserialStatus::BUFFER_EMPTY and
                  static_cast<U8>(Fw::FW_DESERIALIZE_FORMAT_ERROR) == Fw::DeserialStatus::FORMAT_ERROR and
                  static_cast<U8>(Fw::FW_DESERIALIZE_SIZE_MISMATCH) == Fw::DeserialStatus::SIZE_MISMATCH and
                  static_cast<U8>(Fw::FW_DESERIALIZE_TYPE_MISMATCH) == Fw::DeserialStatus::TYPE_MISMATCH,
              "Fw::DeserialStatus must mirror Fw::SerializeStatus values");

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

TlmPacketizer ::TlmPacketizer(const char* const compName)
    : TlmPacketizerComponentBase(compName), m_numPackets(0), m_configured(false), m_numChannels(0) {
    // Register self as parameter delegate
    this->registerExternalParameters(this);
    // clear missing tlm channel check
    for (FwChanIdType entry = 0; entry < TLMPACKETIZER_MAX_MISSING_TLM_CHECK; entry++) {
        this->m_missTlmCheck[entry].checked = false;
        this->m_missTlmCheck[entry].id = 0;
    }

    // clear packet buffers
    for (FwChanIdType buffer = 0; buffer < MAX_PACKETIZER_PACKETS; buffer++) {
        this->m_fillBuffers[buffer].updated = false;
    }

    static_assert(NUM_CONFIGURABLE_TLMPACKETIZER_GROUPS == MAX_CONFIGURABLE_TLMPACKETIZER_GROUP + 1,
                  "NUM_CONFIGURABLE_TLMPACKETIZER_GROUPS MUST BE MAX_CONFIGURABLE_TLMPACKETIZER_GROUP + 1");
}

TlmPacketizer ::~TlmPacketizer() {}

void TlmPacketizer::setPacketList(const TlmPacketizerPacketList& packetList,
                                  const Svc::TlmPacketizerPacket& ignoreList,
                                  const FwChanIdType startLevel) {
    // Ignore list may be nullptr as long as numEntries is 0. Providing an ignore list with numEntries 0 disables
    // functionality for two reasons:
    //     1. There are no ignored channels as configured by FPP.
    //     2. Ignore functionality is intentionally disabled by project where nullptr was intentionally supplied.
    FW_ASSERT(ignoreList.list || ignoreList.numEntries == 0);
    FW_ASSERT(packetList.numEntries <= MAX_PACKETIZER_PACKETS, static_cast<FwAssertArgType>(packetList.numEntries));

    // Reset key data members incase of reentrant calls
    this->m_numChannels = 0;
    this->m_channelIndices.clear();
    this->m_configured = false;

    // validate packet sizes against maximum com buffer size and populate hash
    // table
    FwChanIdType maxLevel = 0;
    for (FwChanIdType pktEntry = 0; pktEntry < packetList.numEntries; pktEntry++) {
        // Initial size is packetized telemetry descriptor + size of time tag + sizeof packet ID
        FwSizeType packetLen =
            sizeof(FwPacketDescriptorType) + Fw::Time::SERIALIZED_SIZE + sizeof(FwTlmPacketizeIdType);
        // A packet with no channels is autocoded with a nullptr list and is permitted
        FW_ASSERT(packetList.list[pktEntry]->list != nullptr || packetList.list[pktEntry]->numEntries == 0,
                  static_cast<FwAssertArgType>(pktEntry));
        // add up entries for each defined packet
        for (FwChanIdType tlmEntry = 0; tlmEntry < packetList.list[pktEntry]->numEntries; tlmEntry++) {
            FwChanIdType id = packetList.list[pktEntry]->list[tlmEntry].id;
            const FwSizeType channelSize = packetList.list[pktEntry]->list[tlmEntry].size;
            FwSizeType entryIndex = 0;
            if (this->m_channelIndices.find(id, entryIndex) != Fw::Success::SUCCESS) {
                // New channel - allocate a slot and initialize offsets to NOT_IN_PACKET
                entryIndex = this->m_numChannels++;
                this->m_channels[entryIndex].id = id;
                this->m_channels[entryIndex].hasValue = false;
                this->m_channels[entryIndex].channelSize = channelSize;
                for (FwChanIdType pktOffsetEntry = 0; pktOffsetEntry < MAX_PACKETIZER_PACKETS; pktOffsetEntry++) {
                    this->m_channels[entryIndex].packetOffset[pktOffsetEntry] = NOT_IN_PACKET;
                }
                const Fw::Success insertStatus = this->m_channelIndices.insert(id, entryIndex);
                FW_ASSERT(insertStatus == Fw::Success::SUCCESS, static_cast<FwAssertArgType>(insertStatus));
            } else {
                // Existing channel - a channel ID may repeat across packets, but its definition (size) must match.
                // A conflicting size would corrupt the packet offsets computed from the earlier definition and
                // overflow the fill buffer during TlmRecv/TlmGet copies, so reject the misconfiguration here.
                FW_ASSERT(this->m_channels[entryIndex].channelSize == channelSize, static_cast<FwAssertArgType>(id),
                          static_cast<FwAssertArgType>(channelSize),
                          static_cast<FwAssertArgType>(this->m_channels[entryIndex].channelSize));
            }
            // not ignored channel - update entry in place via reference
            TlmEntry& entry = this->m_channels[entryIndex];
            entry.ignored = false;
            entry.channelSize = channelSize;
            // the offset into the buffer will be the current packet length
            // the offset must fit within FwTlmPacketizeOffsetType and must not equal NOT_IN_PACKET
            FW_ASSERT(packetLen < NOT_IN_PACKET, static_cast<FwAssertArgType>(packetLen));
            entry.packetOffset[pktEntry] = static_cast<FwTlmPacketizeOffsetType>(packetLen);

            packetLen += entry.channelSize;

        }  // end channel in packet
        FW_ASSERT(packetLen <= FW_COM_BUFFER_MAX_SIZE, static_cast<FwAssertArgType>(packetLen),
                  static_cast<FwAssertArgType>(pktEntry));
        // clear contents
        (void)memset(this->m_fillBuffers[pktEntry].buffer.getBuffAddr(), 0, static_cast<size_t>(packetLen));
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

    // This section adds entries in the map for channels that are intended to be ignored. When the user supplies
    // a list with no length, this loop is skipped. To turn-off ignoring of channels, the user can provide a null
    // list with 0 length.
    for (FwChanIdType channelEntry = 0; channelEntry < ignoreList.numEntries; channelEntry++) {
        FwChanIdType id = ignoreList.list[channelEntry].id;
        FwSizeType entryIndex = 0;
        if (this->m_channelIndices.find(id, entryIndex) != Fw::Success::SUCCESS) {
            // New channel - allocate a slot and initialize offsets to NOT_IN_PACKET
            entryIndex = this->m_numChannels++;
            this->m_channels[entryIndex].id = id;
            this->m_channels[entryIndex].hasValue = false;
            for (FwChanIdType pktOffsetEntry = 0; pktOffsetEntry < MAX_PACKETIZER_PACKETS; pktOffsetEntry++) {
                this->m_channels[entryIndex].packetOffset[pktOffsetEntry] = NOT_IN_PACKET;
            }
            const Fw::Success insertStatus = this->m_channelIndices.insert(id, entryIndex);
            FW_ASSERT(insertStatus == Fw::Success::SUCCESS, static_cast<FwAssertArgType>(insertStatus));
        } else {
            // Ensure it is a duplicate in the ignore list, not a duplicate of a valid channel
            FW_ASSERT(this->m_channels[entryIndex].ignored, static_cast<FwAssertArgType>(id));
        }
        // is ignored channel - update entry in place via reference
        TlmEntry& entry = this->m_channels[entryIndex];
        entry.ignored = true;
        entry.channelSize = ignoreList.list[channelEntry].size;
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
    FwSizeType entryIndex = 0;

    // Search to see if the channel is being tracked
    if (this->m_channelIndices.find(id, entryIndex) != Fw::Success::SUCCESS) {
        // channel not part of a packet and not ignored
        this->missingChannel(id);
        return;
    }

    TlmEntry& entry = this->m_channels[entryIndex];

    // check to see if the channel is ignored. If so, just return.
    if (entry.ignored) {
        return;
    }

    // An update larger than the configured channel size would overrun the packet
    // fill buffer. Values may arrive from external sources (e.g. a hub bridging
    // another address space), so reject rather than assert.
    if (val.getSize() > entry.channelSize) {
        this->log_WARNING_HI_OversizedChannel(id, val.getSize(), entry.channelSize);
        return;
    }

    // copy telemetry value into active buffers; hasValue written in-place via reference
    for (FwChanIdType pkt = 0; pkt < MAX_PACKETIZER_PACKETS; pkt++) {
        // check if current packet has this channel
        if (entry.packetOffset[pkt] != NOT_IN_PACKET) {
            // get destination address
            Os::ScopeLock lock(this->m_lock);
            this->m_fillBuffers[pkt].updated = true;
            this->m_fillBuffers[pkt].latestTime = timeTag;
            U8* ptr = &this->m_fillBuffers[pkt].buffer.getBuffAddr()[entry.packetOffset[pkt]];

            (void)memcpy(ptr, val.getBuffAddr(), static_cast<size_t>(val.getSize()));
            // set under the lock, after the copy, so TlmGet cannot see a value-less VALID entry
            entry.hasValue = true;
        }
    }
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
    FwSizeType entryIndex = 0;

    // Search to see if the channel is being tracked
    if (this->m_channelIndices.find(id, entryIndex) != Fw::Success::SUCCESS) {
        // channel not part of a packet and not ignored
        this->missingChannel(id);
        val.resetSer();
        return Fw::TlmValid::INVALID;
    }
    const TlmEntry& entry = this->m_channels[entryIndex];

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
        if (entry.packetOffset[pkt] != NOT_IN_PACKET) {
            // okay, it has the channel. copy chan val into the tlm buf
            Os::ScopeLock lock(this->m_lock);
            timeTag = this->m_fillBuffers[pkt].latestTime;
            U8* ptr = &this->m_fillBuffers[pkt].buffer.getBuffAddr()[entry.packetOffset[pkt]];
            (void)memcpy(val.getBuffAddr(), ptr, static_cast<size_t>(entry.channelSize));
            // set buf len to the channelSize. keep in mind, this is the MAX serialized size of the channel.
            // so we may actually be filling val with some junk after the value of the channel.
            const Fw::SerializeStatus setStatus = val.setBuffLen(entry.channelSize);
            FW_ASSERT(setStatus == Fw::SerializeStatus::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(setStatus));
            return Fw::TlmValid::VALID;
        }
    }

    // did not find a packet which stores this channel.
    // coding error, this was not an ignored channel so it must be in a packet somewhere
    FW_ASSERT(false, static_cast<FwAssertArgType>(entry.id));
    // TPP (tim paranoia principle)
    val.resetSer();
    return Fw::TlmValid::INVALID;
}

void TlmPacketizer ::Run_handler(const FwIndexType portNum, U32 context) {
    FW_ASSERT(this->m_configured);

    for (FwChanIdType pkt = 0; pkt < this->m_numPackets; pkt++) {
        // Local flags to track which sections require a packet dispatch
        bool sectionNeedsSend[TelemetrySection::NUM_SECTIONS] = {false};
        bool anySectionNeedsSend = false;

        // Lock only to capture the update status and group, and reset the fill buffer flag.
        bool isNewData = false;
        FwChanIdType entryGroup = 0;
        {
            Os::ScopeLock lock(this->m_lock);
            isNewData = this->m_fillBuffers[pkt].updated;
            entryGroup = this->m_fillBuffers[pkt].level;
            this->m_fillBuffers[pkt].updated = false;
        }

        for (FwIndexType section = 0; section < TelemetrySection::NUM_SECTIONS; section++) {
            PktSendCounters& pktEntryFlags = this->m_packetFlags[static_cast<FwSizeType>(section)][pkt];
            // Group policy with the packet's overridden fields copied in
            const Svc::TlmSendConfig effectiveEntryConfig = this->effectiveConfig(section, pkt);

            // Packet is updated and not REQUESTED (Keep REQUESTED marking to bypass disable checks)
            if (isNewData && pktEntryFlags.updateFlag != UpdateFlag::REQUESTED) {
                pktEntryFlags.updateFlag = UpdateFlag::NEW;
            }

            /* Base conditions for sending
            1. Output port is connected
            2. The packet was requested (Override Checks).

            If the packet wasn't requested:
            3. The effective send state allows sending (packet -> group -> section enable)
            4. The packet has data (marked updated in the past or new)
            */
            if (!this->isConnected_PktSend_OutputPort(this->sectionGroupToPort(section, entryGroup))) {
                continue;
            }

            if (pktEntryFlags.updateFlag == UpdateFlag::REQUESTED) {
                sectionNeedsSend[section] = true;
            } else {
                if (not this->sendAllowed(effectiveEntryConfig.get_sendState(), section)) {
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
                effectiveEntryConfig.get_rates().get_rateLogic() != Svc::RateLogic::EVERY_MAX and
                pktEntryFlags.prevSentCounter >= effectiveEntryConfig.get_rates().get_min()) {
                sectionNeedsSend[section] = true;
            }

            /*
            1. Group Logic includes checking MAX
            2. Packet set counter is at MAX
            */
            if (effectiveEntryConfig.get_rates().get_rateLogic() != Svc::RateLogic::ON_CHANGE_MIN and
                pktEntryFlags.prevSentCounter >= effectiveEntryConfig.get_rates().get_max()) {
                sectionNeedsSend[section] = true;
            }

            if (sectionNeedsSend[section]) {
                anySectionNeedsSend = true;
            }
        }

        // Only perform the buffer copy if at least one section needs to send.
        if (anySectionNeedsSend) {
            // Snapshot only the packet data under the lock
            Fw::ComBuffer sendBuffer;
            Fw::Time sendTime;
            {
                Os::ScopeLock lock(this->m_lock);
                sendBuffer = this->m_fillBuffers[pkt].buffer;
                sendTime = this->m_fillBuffers[pkt].latestTime;
            }

            // serialize time into time offset in packet
            Fw::ExternalSerializeBuffer buff(
                &sendBuffer.getBuffAddr()[sizeof(FwPacketDescriptorType) + sizeof(FwTlmPacketizeIdType)],
                Fw::Time::SERIALIZED_SIZE);
            const Fw::SerializeStatus timeStatus = buff.serializeFrom(sendTime);
            FW_ASSERT(Fw::FW_SERIALIZE_OK == timeStatus, static_cast<FwAssertArgType>(timeStatus));

            for (FwIndexType section = 0; section < TelemetrySection::NUM_SECTIONS; section++) {
                if (sectionNeedsSend[section]) {
                    PktSendCounters& pktEntryFlags = this->m_packetFlags[section][pkt];
                    FwIndexType outIndex = this->sectionGroupToPort(section, entryGroup);

                    this->PktSend_out(outIndex, sendBuffer, pktEntryFlags.prevSentCounter);

                    pktEntryFlags.prevSentCounter = 0;
                    pktEntryFlags.updateFlag = UpdateFlag::PAST;
                }
            }
        }
    }
}

void TlmPacketizer ::controlIn_handler(FwIndexType portNum,
                                       const Svc::TelemetrySection& section,
                                       const Fw::Enabled& enabled) {
    // NUM_SECTIONS is an enum constant (not a standalone constant), so isValid() accepts it.
    // The explicit bounds check prevents an out-of-bounds write to m_sectionEnabled.
    if (section.isValid() && section < TelemetrySection::NUM_SECTIONS && enabled.isValid()) {
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
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::VALIDATION_ERROR);
        return;
    }
    for (FwIndexType section = 0; section < TelemetrySection::NUM_SECTIONS; section++) {
        for (FwChanIdType group = 0; group < NUM_CONFIGURABLE_TLMPACKETIZER_GROUPS; group++) {
            this->m_groupConfigs[static_cast<FwSizeType>(section)][group].set_sendState(
                group <= level ? Svc::TlmSendState::INHERIT_FROM_PARENT : Svc::TlmSendState::SEND_NEVER);
        }
    }
    this->tlmWrite_GroupConfigs(this->m_groupConfigs);
    this->log_ACTIVITY_HI_LevelSet(level);
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void TlmPacketizer ::SEND_PKT_cmdHandler(const FwOpcodeType opCode,
                                         const U32 cmdSeq,
                                         const U32 id,
                                         const Svc::TelemetrySection& section) {
    FwChanIdType pkt = 0;
    if (this->resolvePacket(section, id, pkt) != PacketStatus::OK) {
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::VALIDATION_ERROR);
        return;
    }

    {
        Os::ScopeLock lock(this->m_lock);
        this->m_fillBuffers[pkt].updated = true;
        this->m_fillBuffers[pkt].latestTime = this->getTime();
    }

    this->m_packetFlags[section][pkt].updateFlag = UpdateFlag::REQUESTED;

    this->log_ACTIVITY_HI_PacketSent(id);
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void TlmPacketizer ::ENABLE_SECTION_cmdHandler(FwOpcodeType opCode,
                                               U32 cmdSeq,
                                               const Svc::TelemetrySection& section,
                                               const Fw::Enabled& enable) {
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

void TlmPacketizer ::CONFIGURE_GROUP_SEND_cmdHandler(FwOpcodeType opCode,
                                                     U32 cmdSeq,
                                                     const Svc::TelemetrySection& section,
                                                     FwChanIdType tlmGroup,
                                                     const Svc::TlmSendState& sendState) {
    FW_ASSERT(section.isValid());
    if (section < 0 or section >= TelemetrySection::NUM_SECTIONS or tlmGroup > MAX_CONFIGURABLE_TLMPACKETIZER_GROUP) {
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::VALIDATION_ERROR);
        return;
    }
    this->m_groupConfigs[section][tlmGroup].set_sendState(sendState);
    this->tlmWrite_GroupConfigs(this->m_groupConfigs);
    this->log_ACTIVITY_HI_GroupSendStateSet(section, tlmGroup, sendState);
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void TlmPacketizer ::CONFIGURE_GROUP_RATES_cmdHandler(FwOpcodeType opCode,
                                                      U32 cmdSeq,
                                                      const Svc::TelemetrySection& section,
                                                      FwChanIdType tlmGroup,
                                                      const Svc::RateLogic& rateLogic,
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

void TlmPacketizer ::GET_PACKET_CONFIG_cmdHandler(FwOpcodeType opCode,
                                                  U32 cmdSeq,
                                                  const Svc::TelemetrySection& section,
                                                  U32 packetId) {
    FwChanIdType pkt = 0;
    const PacketStatus status = this->resolvePacket(section, packetId, pkt);
    if (status == PacketStatus::OK) {
        const FwSizeType sectionIndex = static_cast<FwSizeType>(section.e);
        this->log_ACTIVITY_HI_PacketConfigReport(section, packetId, this->m_packetOverrides[sectionIndex][pkt],
                                                 this->effectiveConfig(static_cast<FwIndexType>(section.e), pkt));
    }
    this->cmdResponse_out(opCode, cmdSeq, TlmPacketizer::toCmdResponse(status));
}

void TlmPacketizer ::OVERRIDE_PACKET_SEND_cmdHandler(FwOpcodeType opCode,
                                                     U32 cmdSeq,
                                                     const Svc::TelemetrySection& section,
                                                     U32 packetId,
                                                     const Svc::TlmSendState& sendState) {
    const Svc::PacketOverrideConfig requested(Svc::PacketOverrideType::OVERRIDE_SEND,
                                              Svc::TlmSendConfig(sendState, Svc::RateConfig()));
    this->cmdResponse_out(opCode, cmdSeq,
                          TlmPacketizer::toCmdResponse(this->overridePacket(section, packetId, requested)));
}

void TlmPacketizer ::OVERRIDE_PACKET_RATES_cmdHandler(FwOpcodeType opCode,
                                                      U32 cmdSeq,
                                                      const Svc::TelemetrySection& section,
                                                      U32 packetId,
                                                      const Svc::RateLogic& rateLogic,
                                                      U32 minDelta,
                                                      U32 maxDelta) {
    const Svc::PacketOverrideConfig requested(
        Svc::PacketOverrideType::OVERRIDE_RATES,
        Svc::TlmSendConfig(Svc::TlmSendState::INHERIT_FROM_PARENT, Svc::RateConfig(rateLogic, minDelta, maxDelta)));
    this->cmdResponse_out(opCode, cmdSeq,
                          TlmPacketizer::toCmdResponse(this->overridePacket(section, packetId, requested)));
}

void TlmPacketizer ::CLEAR_PACKET_OVERRIDE_cmdHandler(FwOpcodeType opCode,
                                                      U32 cmdSeq,
                                                      const Svc::TelemetrySection& section,
                                                      U32 packetId) {
    const Svc::PacketOverrideConfig requested(Svc::PacketOverrideType::GROUP_DEFAULT, Svc::TlmSendConfig());
    this->cmdResponse_out(opCode, cmdSeq,
                          TlmPacketizer::toCmdResponse(this->overridePacket(section, packetId, requested)));
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

    this->m_groupConfigs[section][tlmGroup].set_rates(Svc::RateConfig(rateLogic, minDelta, maxDelta));
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

Fw::CmdResponse TlmPacketizer::toCmdResponse(PacketStatus status) {
    switch (status) {
        case PacketStatus::OK:
            return Fw::CmdResponse::OK;
        case PacketStatus::MIRROR_FAILED:
            return Fw::CmdResponse::EXECUTION_ERROR;
        default:
            return Fw::CmdResponse::VALIDATION_ERROR;
    }
}

Svc::TlmSendConfig TlmPacketizer::effectiveConfig(FwIndexType section, FwChanIdType pkt) const {
    const FwSizeType sectionIndex = static_cast<FwSizeType>(section);
    // Start from the group policy, then copy in the fields the packet overrides
    Svc::TlmSendConfig effective = this->m_groupConfigs[sectionIndex][this->m_fillBuffers[pkt].level];
    const Svc::PacketOverrideConfig& packetOverride = this->m_packetOverrides[sectionIndex][pkt];
    const Svc::PacketOverrideType overrideType = packetOverride.get_overrideType();
    const bool overridesSend = (overrideType == Svc::PacketOverrideType::OVERRIDE_SEND) or
                               (overrideType == Svc::PacketOverrideType::OVERRIDE_SEND_AND_RATES);
    const bool overridesRates = (overrideType == Svc::PacketOverrideType::OVERRIDE_RATES) or
                                (overrideType == Svc::PacketOverrideType::OVERRIDE_SEND_AND_RATES);
    // A packet-level INHERIT_FROM_PARENT means "use the group's state", so it is not copied
    if (overridesSend and packetOverride.get_sendConfig().get_sendState() != Svc::TlmSendState::INHERIT_FROM_PARENT) {
        effective.set_sendState(packetOverride.get_sendConfig().get_sendState());
    }
    if (overridesRates) {
        effective.set_rates(packetOverride.get_sendConfig().get_rates());
    }
    return effective;
}

bool TlmPacketizer::sendAllowed(const Svc::TlmSendState& sendState, FwIndexType section) const {
    switch (sendState.e) {
        case Svc::TlmSendState::SEND_ALWAYS:
            return true;
        case Svc::TlmSendState::SEND_NEVER:
            return false;
        // INHERIT_FROM_PARENT here is a group-level state, whose parent is the section. Fall-through intended.
        case Svc::TlmSendState::SEND_WHEN_SECTION_ENABLED:
        case Svc::TlmSendState::INHERIT_FROM_PARENT:
            return this->m_sectionEnabled[static_cast<FwSizeType>(section)] == Fw::Enabled::ENABLED;
        default:
            FW_ASSERT(0, static_cast<FwAssertArgType>(sendState.e));
            return false;
    }
}

Fw::Success TlmPacketizer::findPacketIndexById(U32 packetId, FwChanIdType& pkt) const {
    for (FwChanIdType p = 0; p < this->m_numPackets; p++) {
        if (this->m_fillBuffers[p].id == packetId) {
            pkt = p;
            return Fw::Success::SUCCESS;
        }
    }
    return Fw::Success::FAILURE;
}

TlmPacketizer::PacketStatus TlmPacketizer::resolvePacket(const Svc::TelemetrySection& section,
                                                         U32 packetId,
                                                         FwChanIdType& pkt) {
    if (not section.isValid() or section < 0 or section >= TelemetrySection::NUM_SECTIONS) {
        this->log_WARNING_LO_InvalidPacketConfig(packetId);
        return PacketStatus::INVALID_SECTION;
    }
    if (this->findPacketIndexById(packetId, pkt) != Fw::Success::SUCCESS) {
        this->log_WARNING_LO_PacketNotFound(packetId);
        return PacketStatus::UNKNOWN_PACKET;
    }
    return PacketStatus::OK;
}

Svc::PacketOverrideType TlmPacketizer::mergeOverrideType(const Svc::PacketOverrideType& current,
                                                         const Svc::PacketOverrideType& requested) {
    if (current == Svc::PacketOverrideType::GROUP_DEFAULT or current == requested) {
        return requested;
    }
    // Different fields already overridden: both apply
    return Svc::PacketOverrideType::OVERRIDE_SEND_AND_RATES;
}

TlmPacketizer::PacketStatus TlmPacketizer::applyPacketOverride(const Svc::TelemetrySection& section,
                                                               FwChanIdType pkt,
                                                               const Svc::PacketOverrideConfig& requested) {
    Svc::PacketOverrideConfig& stored = this->m_packetOverrides[static_cast<FwSizeType>(section.e)][pkt];
    // Snapshot so a failed mirror can roll back and the persisted copy never diverges
    const Svc::PacketOverrideConfig previous = stored;
    // Fields not (yet) overridden hold neutral values: INHERIT_FROM_PARENT and default rates
    Svc::TlmSendConfig merged = (previous.get_overrideType() == Svc::PacketOverrideType::GROUP_DEFAULT)
                                    ? Svc::TlmSendConfig()
                                    : stored.get_sendConfig();
    switch (requested.get_overrideType()) {
        case Svc::PacketOverrideType::OVERRIDE_SEND:
            merged.set_sendState(requested.get_sendConfig().get_sendState());
            break;
        case Svc::PacketOverrideType::OVERRIDE_RATES:
            merged.set_rates(requested.get_sendConfig().get_rates());
            break;
        case Svc::PacketOverrideType::OVERRIDE_SEND_AND_RATES:
            merged = requested.get_sendConfig();
            break;
        default:
            // GROUP_DEFAULT: clearing keeps no policy of its own
            merged = Svc::TlmSendConfig();
            break;
    }
    stored.set_sendConfig(merged);
    stored.set_overrideType(
        requested.get_overrideType() == Svc::PacketOverrideType::GROUP_DEFAULT
            ? Svc::PacketOverrideType(Svc::PacketOverrideType::GROUP_DEFAULT)
            : TlmPacketizer::mergeOverrideType(previous.get_overrideType(), requested.get_overrideType()));
    if (this->mirrorOverride(section, pkt) != Fw::Success::SUCCESS) {
        stored = previous;
        return PacketStatus::MIRROR_FAILED;
    }
    return PacketStatus::OK;
}

TlmPacketizer::PacketStatus TlmPacketizer::overridePacket(const Svc::TelemetrySection& section,
                                                          U32 packetId,
                                                          const Svc::PacketOverrideConfig& requested) {
    FwChanIdType pkt = 0;
    PacketStatus status = this->resolvePacket(section, packetId, pkt);
    if (status != PacketStatus::OK) {
        return status;
    }
    status = this->applyPacketOverride(section, pkt, requested);
    if (status != PacketStatus::OK) {
        return status;
    }
    if (requested.get_overrideType() == Svc::PacketOverrideType::GROUP_DEFAULT) {
        this->log_ACTIVITY_HI_PacketOverrideCleared(section, packetId);
    } else {
        this->log_ACTIVITY_HI_PacketOverrideSet(section, packetId,
                                                this->m_packetOverrides[static_cast<FwSizeType>(section.e)][pkt]);
    }
    return PacketStatus::OK;
}

void TlmPacketizer ::packetOverrideIn_handler(FwIndexType portNum, Fw::Buffer& fwBuffer) {
    // Reload per-packet overrides from the persistence component: one PacketConfigEntry record after another
    if (fwBuffer.isValid()) {
        auto deserializer = fwBuffer.getDeserializer();
        const FwSizeType recordSize = static_cast<FwSizeType>(Svc::PacketConfigEntry::SERIALIZED_SIZE);
        const FwSizeType maxRecords = static_cast<FwSizeType>(fwBuffer.getSize()) / recordSize;
        Fw::SerializeStatus status = Fw::FW_SERIALIZE_OK;
        for (FwSizeType record = 0; (record < maxRecords) and (status == Fw::FW_SERIALIZE_OK); record++) {
            Svc::PacketConfigEntry entry;
            status = deserializer.deserializeTo(entry);
            if (status != Fw::FW_SERIALIZE_OK) {
                break;
            }
            const Svc::TelemetrySection section = entry.get_section();
            FwChanIdType pkt = 0;
            // Enum values were validated by deserialization; only the section range remains to check
            if (section < 0 or section >= TelemetrySection::NUM_SECTIONS) {
                this->log_WARNING_LO_ReloadRecordInvalid(entry.get_packetId());
            } else if (this->findPacketIndexById(entry.get_packetId(), pkt) != Fw::Success::SUCCESS) {
                this->log_WARNING_LO_ReloadRecordUnknownPacket(entry.get_packetId());
            } else {
                this->m_packetOverrides[static_cast<FwSizeType>(section.e)][pkt] = entry.get_override();
            }
        }
        // Trailing bytes that do not form a whole record are a malformed buffer
        if ((status == Fw::FW_SERIALIZE_OK) and (deserializer.getDeserializeSizeLeft() != 0)) {
            status = Fw::FW_DESERIALIZE_FORMAT_ERROR;
        }
        if (status != Fw::FW_SERIALIZE_OK) {
            Fw::DeserialStatus reported(static_cast<Fw::DeserialStatus::T>(status));
            if (not reported.isValid()) {
                reported = Fw::DeserialStatus::FORMAT_ERROR;
            }
            this->log_WARNING_HI_ConfigReloadFailed(reported);
        }
    } else {
        this->log_WARNING_HI_ConfigReloadFailed(Fw::DeserialStatus::BUFFER_EMPTY);
    }
    this->packetOverrideReturnOut_out(0, fwBuffer);
}

void TlmPacketizer ::packetOverrideReturnIn_handler(FwIndexType portNum, Fw::Buffer& fwBuffer) {
    // Mirror buffers come from allocate and go back to the same pool; the SDD requires the whole
    // mirror path (packetOverrideOut, allocate, deallocate, packetOverrideReturnIn) wired or none of it
    FW_ASSERT(this->isConnected_deallocate_OutputPort(0));
    this->deallocate_out(0, fwBuffer);
}

Fw::Success TlmPacketizer::mirrorOverride(const Svc::TelemetrySection& section, FwChanIdType pkt) {
    // persistent storage component should mirror this state; skip if not wired
    if (not this->isConnected_packetOverrideOut_OutputPort(0)) {
        return Fw::Success::SUCCESS;
    }
    // A mirror port without an allocator cannot persist anything, and without a deallocator the returned
    // mirror buffers could never be freed; the SDD requires the whole mirror path wired or none of it
    FW_ASSERT(this->isConnected_allocate_OutputPort(0));
    FW_ASSERT(this->isConnected_deallocate_OutputPort(0));
    const FwSizeType sectionIndex = static_cast<FwSizeType>(section.e);
    const Svc::PacketConfigEntry entry(static_cast<U32>(this->m_fillBuffers[pkt].id), section,
                                       this->m_packetOverrides[sectionIndex][pkt]);
    const FwSizeType entrySize = static_cast<FwSizeType>(Svc::PacketConfigEntry::SERIALIZED_SIZE);
    Fw::Buffer buffer = this->allocate_out(0, entrySize);
    if (not buffer.isValid()) {
        this->log_WARNING_HI_ConfigMirrorAllocFailed();
        return Fw::Success::FAILURE;
    }
    // Fw.BufferGet contract: a valid buffer is at least the requested size
    FW_ASSERT(static_cast<FwSizeType>(buffer.getSize()) >= entrySize, static_cast<FwAssertArgType>(buffer.getSize()));
    auto serializer = buffer.getSerializer();
    const Fw::SerializeStatus status = serializer.serializeFrom(entry);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(status));
    buffer.setSize(static_cast<Fw::Buffer::SizeType>(entrySize));
    this->packetOverrideOut_out(0, buffer);
    return Fw::Success::SUCCESS;
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

Fw::SerializeStatus TlmPacketizer::deserializeParam(const FwPrmIdType base_id,
                                                    const FwPrmIdType local_id,
                                                    const Fw::ParamValid prmStat,
                                                    Fw::SerialBufferBase& buff) {
    if (FW_PARAM_OK(prmStat)) {
        switch (local_id) {
            // A stored value of a different shape (e.g. saved by a previous FSW load) must not load as a scrambled
            // table
            case PARAMID_SECTION_ENABLED: {
                Svc::TlmPacketizer_SectionEnabled loaded;
                Fw::SerializeStatus status = buff.deserializeTo(loaded);
                if ((status == Fw::FW_SERIALIZE_OK) and (buff.getDeserializeSizeLeft() != 0)) {
                    status = Fw::FW_DESERIALIZE_SIZE_MISMATCH;
                }
                if (status == Fw::FW_SERIALIZE_OK) {
                    this->m_sectionEnabled = loaded;
                }
                return status;
            }
            case PARAMID_SECTION_CONFIGS: {
                Svc::TlmPacketizer_SectionConfigs loaded;
                Fw::SerializeStatus status = buff.deserializeTo(loaded);
                if ((status == Fw::FW_SERIALIZE_OK) and (buff.getDeserializeSizeLeft() != 0)) {
                    status = Fw::FW_DESERIALIZE_SIZE_MISMATCH;
                }
                if (status == Fw::FW_SERIALIZE_OK) {
                    this->m_groupConfigs = loaded;
                }
                return status;
            }
            default:
                FW_ASSERT(false, static_cast<FwAssertArgType>(local_id));
        }
    }
    return Fw::SerializeStatus::FW_DESERIALIZE_TYPE_MISMATCH;
}

Fw::SerializeStatus TlmPacketizer::serializeParam(const FwPrmIdType base_id,
                                                  const FwPrmIdType local_id,
                                                  Fw::SerialBufferBase& buff) const {
    switch (local_id) {
        case PARAMID_SECTION_ENABLED:
            return buff.serializeFrom(this->m_sectionEnabled);
        case PARAMID_SECTION_CONFIGS:
            return buff.serializeFrom(this->m_groupConfigs);
        default:
            FW_ASSERT(false, static_cast<FwAssertArgType>(local_id));
    }
    return Fw::SerializeStatus::FW_SERIALIZE_FORMAT_ERROR;
}

}  // end namespace Svc

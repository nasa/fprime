// ======================================================================
// \title  TlmPacketizerImpl.hpp
// \author tcanham
// \brief  hpp file for TlmPacketizer component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

#ifndef TlmPacketizer_HPP
#define TlmPacketizer_HPP

#include "Fw/Buffer/Buffer.hpp"
#include "Fw/DataStructures/Array.hpp"
#include "Fw/DataStructures/RedBlackTreeMap.hpp"
#include "Fw/Prm/PrmExternalTypes.hpp"
#include "Fw/Types/EnabledEnumAc.hpp"
#include "Os/Mutex.hpp"
#include "Svc/TlmPacketizer/TlmPacketizerComponentAc.hpp"
#include "Svc/TlmPacketizer/TlmPacketizerTypes.hpp"
#include "Svc/TlmPacketizer/TlmPacketizer_TelemetrySendPortMapArrayAc.hpp"
#include "TlmPacketizerConfig/FppConstantsAc.hpp"
#include "TlmPacketizerConfig/TlmPacketizerCfg.hpp"

namespace Svc {

//! Constant allowing users to ignore the omit list allowing a reduction in required buckets and thus storage
constexpr Svc::TlmPacketizerPacket IGNORE_OMIT_LIST = {nullptr, 0, 0, 0};

class TlmPacketizer final : public TlmPacketizerComponentBase, public Fw::ParamExternalDelegate {
    friend class TlmPacketizerTester;

  public:
    // ----------------------------------------------------------------------
    // Construction, initialization, and destruction
    // ----------------------------------------------------------------------

    //! Construct object TlmPacketizer
    //!
    TlmPacketizer(const char* const compName /*!< The component name*/
    );

    void setPacketList(
        const TlmPacketizerPacketList& packetList,   // channels to packetize
        const Svc::TlmPacketizerPacket& ignoreList,  // channels to ignore (i.e. no warning event if not packetized)
        const FwChanIdType startLevel                // starting level of packets to send
    );

    //! Destroy object TlmPacketizer
    //!
    ~TlmPacketizer();

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for user-defined typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for TlmRecv
    //!
    void TlmRecv_handler(const FwIndexType portNum, /*!< The port number*/
                         FwChanIdType id,           /*!< Telemetry Channel ID*/
                         Fw::Time& timeTag,         /*!< Time Tag*/
                         Fw::TlmBuffer& val         /*!< Buffer containing serialized telemetry value*/
                         ) override;

    //! Handler implementation for configureSectionGroupRate
    //!
    //! Input configuration port
    void configureSectionGroupRate_handler(
        FwIndexType portNum,                   //!< The port number
        const Svc::TelemetrySection& section,  //!< Section grouping
        FwChanIdType tlmGroup,                 //!< Group Identifier
        const Svc::RateLogic& rateLogic,       //!< Rate Logic
        U32 minDelta,  //!< Minimum Sched Ticks to send packets on updates when using ON_CHANGE logic
        U32 maxDelta   //!< Maximum Sched Ticks between packets to send when using EVERY_MAX logic
        ) override;

    //! Handler implementation for Run
    //!
    void Run_handler(const FwIndexType portNum, /*!< The port number*/
                     U32 context                /*!< The call order*/
                     ) override;

    //! Handler implementation for controlIn
    void controlIn_handler(FwIndexType portNum,                   //!< The port number
                           const Svc::TelemetrySection& section,  //!< Section to enable (Primary, Secondary, etc...)
                           const Fw::Enabled& enabled             //!< Enable / Disable Section
                           ) override;

    //! Handler implementation for pingIn
    //!
    void pingIn_handler(const FwIndexType portNum, /*!< The port number*/
                        U32 key                    /*!< Value to return to pinger*/
                        ) override;

    //! Handler for input port packetOverrideIn
    void packetOverrideIn_handler(FwIndexType portNum,  //!< The port number
                                  Fw::Buffer& fwBuffer  //!< Serialized PacketConfigEntry records to apply
                                  ) override;

    //! Handler for input port TlmGet
    Fw::TlmValid TlmGet_handler(FwIndexType portNum,  //!< The port number
                                FwChanIdType id,      //!< Telemetry Channel ID
                                Fw::Time& timeTag,    //!< Time Tag
                                Fw::TlmBuffer& val    //!< Buffer containing serialized telemetry value.
                                                      //!< Size set to 0 if channel not found.
                                ) override;

    //! Implementation for SET_LEVEL command handler
    //! Set telemetry send level
    void SET_LEVEL_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                              U32 cmdSeq,           //!< The command sequence number
                              FwChanIdType level    //!< The I32 command argument
                              ) override;

    //! Implementation for SEND_PKT command handler
    //! Force a packet to be sent
    void SEND_PKT_cmdHandler(FwOpcodeType opCode,                  //!< The opcode
                             U32 cmdSeq,                           //!< The command sequence number
                             U32 id,                               //!< The packet ID
                             const Svc::TelemetrySection& section  //!< Section to emit packet
                             ) override;

    //! Handler implementation for command ENABLE_SECTION
    void ENABLE_SECTION_cmdHandler(FwOpcodeType opCode,                   //!< The opcode
                                   U32 cmdSeq,                            //!< The command sequence number
                                   const Svc::TelemetrySection& section,  //!< Section grouping to configure
                                   const Fw::Enabled& enable              //!< Section enabled or disabled
                                   ) override;

    //! Handler implementation for command ENABLE_GROUP
    //!
    //! Enable / disable telemetry of a group on a section
    void ENABLE_GROUP_cmdHandler(FwOpcodeType opCode,                   //!< The opcode
                                 U32 cmdSeq,                            //!< The command sequence number
                                 const Svc::TelemetrySection& section,  //!< Section grouping to configure
                                 FwChanIdType tlmGroup,                 //!< Group Identifier
                                 const Fw::Enabled& enable              //!< Section enabled or disabled
                                 ) override;

    //! Handler implementation for command FORCE_GROUP
    void FORCE_GROUP_cmdHandler(FwOpcodeType opCode,                   //!< The opcode
                                U32 cmdSeq,                            //!< The command sequence number
                                const Svc::TelemetrySection& section,  //!< Section grouping
                                FwChanIdType tlmGroup,                 //!< Group Identifier
                                const Fw::Enabled& enable              //!< Section enabled or disabled
                                ) override;

    //! Handler implementation for command CONFIGURE_GROUP_RATES
    void CONFIGURE_GROUP_RATES_cmdHandler(
        FwOpcodeType opCode,                   //!< The opcode
        U32 cmdSeq,                            //!< The command sequence number
        const Svc::TelemetrySection& section,  //!< Section grouping
        FwChanIdType tlmGroup,                 //!< Group Identifier
        const Svc::RateLogic& rateLogic,       //!< Rate Logic
        U32 minDelta,  //!< Minimum Sched Ticks to send packets on updates when using ON_CHANGE logic
        U32 maxDelta   //!< Maximum Sched Ticks between packets to send when using EVERY_MAX logic
        ) override;

    //! Handler implementation for command GET_PACKET_CONFIG
    //!
    //! Emits the effective per-packet configuration as the PacketConfigReport event.
    void GET_PACKET_CONFIG_cmdHandler(FwOpcodeType opCode,                   //!< The opcode
                                      U32 cmdSeq,                            //!< The command sequence number
                                      const Svc::TelemetrySection& section,  //!< Section to query
                                      U32 packetId                           //!< Packet identifier
                                      ) override;

    //! Handler implementation for command ENABLE_PACKET
    //!
    //! Enable / disable a single packet as a per-packet override, then mirror to packetOverrideOut.
    void ENABLE_PACKET_cmdHandler(FwOpcodeType opCode,                   //!< The opcode
                                  U32 cmdSeq,                            //!< The command sequence number
                                  const Svc::TelemetrySection& section,  //!< Section to configure
                                  U32 packetId,                          //!< Packet identifier
                                  const Fw::Enabled& enable              //!< Enable / disable this packet
                                  ) override;

    //! Handler implementation for command FORCE_PACKET
    //!
    //! Force / unforce a single packet as a per-packet override, then mirror to packetOverrideOut.
    void FORCE_PACKET_cmdHandler(FwOpcodeType opCode,                   //!< The opcode
                                 U32 cmdSeq,                            //!< The command sequence number
                                 const Svc::TelemetrySection& section,  //!< Section to configure
                                 U32 packetId,                          //!< Packet identifier
                                 const Fw::Enabled& enable              //!< Force enable / disable
                                 ) override;

    //! Handler implementation for command CONFIGURE_PACKET_RATES
    //!
    //! Set the rate logic / thresholds of a single packet as a per-packet override, then
    //! mirror to packetOverrideOut.
    void CONFIGURE_PACKET_RATES_cmdHandler(
        FwOpcodeType opCode,                   //!< The opcode
        U32 cmdSeq,                            //!< The command sequence number
        const Svc::TelemetrySection& section,  //!< Section to configure
        U32 packetId,                          //!< Packet identifier
        const Svc::RateLogic& rateLogic,       //!< Rate logic
        U32 minDelta,                          //!< Minimum Sched ticks between sends when using ON_CHANGE_MIN logic
        U32 maxDelta                           //!< Maximum Sched ticks between sends when using EVERY_MAX logic
        ) override;

    //! Handler implementation for command CLEAR_PACKET_OVERRIDE
    //!
    //! Clear a single packet's per-packet override, reverting it to group-derived behavior,
    //! then mirror the cleared state to packetOverrideOut.
    void CLEAR_PACKET_OVERRIDE_cmdHandler(FwOpcodeType opCode,                   //!< The opcode
                                          U32 cmdSeq,                            //!< The command sequence number
                                          const Svc::TelemetrySection& section,  //!< Section to clear
                                          U32 packetId                           //!< Packet identifier
                                          ) override;

    // number of packets to fill
    FwChanIdType m_numPackets;
    // Array of packet buffers to send
    // Double-buffered to fill one while sending one

    struct BufferEntry {
        Fw::ComBuffer buffer;  //!< buffer for packetized channels
        Fw::Time latestTime;   //!< latest update time
        FwChanIdType id;       //!< channel id
        FwChanIdType level;    //!< channel level
        bool updated;          //!< if packet had any updates during last cycle
    };

    // buffers for filling with telemetry
    BufferEntry m_fillBuffers[MAX_PACKETIZER_PACKETS];

    static constexpr U16 NOT_IN_PACKET = std::numeric_limits<U16>::max();  //!< packetOffset value for no packet

    struct TlmEntry {
        FwChanIdType id;  //!< telemetry id stored in slot
        // Offsets into packet buffers.
        // NOT_IN_PACKET means that channel is not in that packet
        U16 packetOffset[MAX_PACKETIZER_PACKETS];
        FwSizeType channelSize;  //!< max serialized size of the channel in bytes
        bool ignored;            //!< ignored channel id
        bool hasValue;           //!< if the entry has received a value at least once
    };

    Os::Mutex m_lock;  //!< used to lock access to packet buffers

    bool m_configured;  //!< indicates a table has been passed and packets configured

    struct MissingTlmChan {
        FwChanIdType id;
        bool checked;
    } m_missTlmCheck[TLMPACKETIZER_MAX_MISSING_TLM_CHECK];

    void missingChannel(FwChanIdType id);  //!< Helper to check to see if missing channel warning was sent

    TlmPacketizer_SectionEnabled m_sectionEnabled{};

    TlmPacketizer_SectionConfigs m_groupConfigs{};

    //! Per-packet override policy; meaningful only while the matching m_packetOverridden flag is set
    Svc::TlmSendConfig m_packetOverride[TelemetrySection::NUM_SECTIONS][MAX_PACKETIZER_PACKETS]{};
    //! True when (section, packet) has an active override, i.e. effectiveConfig() returns m_packetOverride
    bool m_packetOverridden[TelemetrySection::NUM_SECTIONS][MAX_PACKETIZER_PACKETS]{};

    enum UpdateFlag : U8 {
        NEVER_UPDATED = 0,  //!< Packet has never been updated (NO DATA)
        PAST = 1,           //!< Packet has been sent and has old data
        NEW = 2,            //!< Packet has been updated - use for ON_CHANGE_MIN logic
        REQUESTED = 3,      //!< Packet has been requested - bypass all rate and enabled checks
    };

    struct PktSendCounters {
        U32 prevSentCounter = std::numeric_limits<U32>::max();  // Prevent Start up spam
        UpdateFlag updateFlag = UpdateFlag::NEVER_UPDATED;
    } m_packetFlags[TelemetrySection::NUM_SECTIONS][MAX_PACKETIZER_PACKETS]{};

    //! Mapping of section/group to the output port used to send telemetry
    static const TlmPacketizer_TelemetrySendPortMap TELEMETRY_SEND_PORT_MAP;

  private:
    Fw::SerializeStatus serializeParam(const FwPrmIdType base_id,
                                       const FwPrmIdType local_id,
                                       Fw::SerialBufferBase& buff) const override;

    Fw::SerializeStatus deserializeParam(const FwPrmIdType base_id,
                                         const FwPrmIdType local_id,
                                         const Fw::ParamValid prmStat,
                                         Fw::SerialBufferBase& buff) override;

  private:
    //! Handler implementation for configureSectionGroupRate
    //!
    //! Input configuration port
    void configureSectionGroupRate(
        const Svc::TelemetrySection& section,  //!< Section grouping
        FwChanIdType tlmGroup,                 //!< Group Identifier
        const Svc::RateLogic& rateLogic,       //!< Rate Logic
        U32 minDelta,  //!< Minimum Sched Ticks to send packets on updates when using ON_CHANGE logic
        U32 maxDelta   //!< Maximum Sched Ticks between packets to send when using EVERY_MAX logic
    );

    //! \brief Helper function to get output port index from section and group
    //!
    //! Invokes the mapping defined in TELEMETRY_SEND_PORT_MAP to get the output port index for a given section and
    //! group.
    //! \param section The telemetry section (e.g., PRIMARY, SECONDARY, etc.)
    //! \param group The telemetry group number
    //! \return The output port index to send telemetry for the given section and group
    static FwIndexType sectionGroupToPort(const FwIndexType section, const FwSizeType group);

    //! \brief Compute the effective per-packet policy for a packet/section
    //! \param section Section index
    //! \param pkt Packet index
    Svc::TlmSendConfig effectiveConfig(FwIndexType section, FwChanIdType pkt) const;

    //! \brief Resolve a packet id to its index in the packet list.
    bool findPacketIndexById(U32 packetId, FwChanIdType& pkt) const;

    //! \brief Validate a command's section and resolve packetId to a packet index.
    //! \param section Section the command addresses
    //! \param packetId External packet identifier to resolve
    //! \param pkt Resolved packet index (written on success)
    //! \return true on success; false on a bad section or unknown id
    bool resolvePacket(const Svc::TelemetrySection& section, U32 packetId, FwChanIdType& pkt);

    //! \brief Seed (section, pkt)'s override slot from its current effective config on first use,
    //! so a command touching one field preserves the rest.
    //! \param section Section the override applies to
    //! \param pkt Resolved packet index
    void seedOverride(const Svc::TelemetrySection& section, FwChanIdType pkt);

    //! Which field(s) of the override policy a command updates; selects the mutation in applyPacketOverride.
    enum class OverrideField {
        ENABLE,  //!< ENABLE_PACKET: set the enabled flag
        FORCE,   //!< FORCE_PACKET: set the forceEnabled flag
        RATES    //!< CONFIGURE_PACKET_RATES: set rate logic and MIN/MAX thresholds
    };

    //! \brief Shared body for the override commands: validate, resolve, seed, mutate, mirror, respond.
    //! \param section Section the command addresses
    //! \param packetId External packet identifier
    //! \param argValid Result of the caller's enum-argument validity check
    //! \param field Which field(s) to update
    //! \param enable New value for ENABLE / FORCE (ignored for RATES)
    //! \param rateLogic New rate logic for RATES (ignored otherwise)
    //! \param minDelta New MIN threshold for RATES (ignored otherwise)
    //! \param maxDelta New MAX threshold for RATES (ignored otherwise)
    void applyPacketOverride(FwOpcodeType opCode,
                             U32 cmdSeq,
                             const Svc::TelemetrySection& section,
                             U32 packetId,
                             bool argValid,
                             OverrideField field,
                             const Fw::Enabled& enable,
                             const Svc::RateLogic& rateLogic,
                             U32 minDelta,
                             U32 maxDelta);

    //! \brief Mirror (section, pkt)'s state out packetOverrideOut for an external persistence component.
    //! \param section Section the entry applies to
    //! \param pkt Resolved packet index (packet id is derived from it)
    //! \param source OVERRIDDEN when an override is active, GROUP_DEFAULT when cleared
    //! \return true when the mirror was sent or no persistence component is wired, false on allocation failure
    bool mirrorOverride(const Svc::TelemetrySection& section, FwChanIdType pkt, Svc::PacketConfigSource source);

    //! Apply the command-path argument validity checks to a reloaded packetOverrideIn record
    //! \param entry Deserialized record
    //! \return true when every enumerated field of the record holds a valid value
    bool validPacketConfig(const Svc::PacketConfigEntry& entry) const;

  private:
    FwSizeType m_numChannels;  //!< number of channels being packetized
    Fw::RedBlackTreeMap<FwChanIdType, FwSizeType, MAX_PACKETIZER_CHANNELS> m_channelIndices;
    Fw::Array<TlmEntry, MAX_PACKETIZER_CHANNELS>
        m_channels;  //!< flat storage for channel entries indexed by m_channelIndices
};

}  // end namespace Svc

#endif

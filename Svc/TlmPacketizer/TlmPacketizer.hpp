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

#include "Fw/Types/EnabledEnumAc.hpp"
#include "Os/Mutex.hpp"
#include "Svc/TlmPacketizer/TlmPacketizerComponentAc.hpp"
#include "Svc/TlmPacketizer/TlmPacketizerTypes.hpp"
#include "Svc/TlmPacketizer/TlmPacketizer_TelemetrySendPortMapArrayAc.hpp"
#include "config/TlmPacketizerCfg.hpp"

namespace Svc {

class TlmPacketizer final : public TlmPacketizerComponentBase {
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
        const FwChanIdType startLevel,               // starting level of packets to send
        const TlmPacketizer_GroupConfig& defaultGroupConfig =
            TlmPacketizer_GroupConfig{});  // default group config setting

    //! Destroy object TlmPacketizer
    //!
    ~TlmPacketizer(void);

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
    void SEND_PKT_cmdHandler(FwOpcodeType opCode,           //!< The opcode
                             U32 cmdSeq,                    //!< The command sequence number
                             U32 id,                        //!< The packet ID
                             Svc::TelemetrySection section  //!< Section to emit packet
                             ) override;

    //! Handler implementation for command ENABLE_SECTION
    void ENABLE_SECTION_cmdHandler(FwOpcodeType opCode,            //!< The opcode
                                   U32 cmdSeq,                     //!< The command sequence number
                                   Svc::TelemetrySection section,  //!< Section grouping to configure
                                   Fw::Enabled enable              //!< Section enabled or disabled
                                   ) override;

    //! Handler implementation for command ENABLE_GROUP
    //!
    //! Enable / disable telemetry of a group on a section
    void ENABLE_GROUP_cmdHandler(FwOpcodeType opCode,            //!< The opcode
                                 U32 cmdSeq,                     //!< The command sequence number
                                 Svc::TelemetrySection section,  //!< Section grouping to configure
                                 FwChanIdType tlmGroup,          //!< Group Identifier
                                 Fw::Enabled enable              //!< Section enabled or disabled
                                 ) override;

    //! Handler implementation for command FORCE_GROUP
    void FORCE_GROUP_cmdHandler(FwOpcodeType opCode,            //!< The opcode
                                U32 cmdSeq,                     //!< The command sequence number
                                Svc::TelemetrySection section,  //!< Section grouping
                                FwChanIdType tlmGroup,          //!< Group Identifier
                                Fw::Enabled enable              //!< Section enabled or disabled
                                ) override;

    //! Handler implementation for command CONFIGURE_GROUP_RATES
    void CONFIGURE_GROUP_RATES_cmdHandler(
        FwOpcodeType opCode,            //!< The opcode
        U32 cmdSeq,                     //!< The command sequence number
        Svc::TelemetrySection section,  //!< Section grouping
        FwChanIdType tlmGroup,          //!< Group Identifier
        Svc::RateLogic rateLogic,       //!< Rate Logic
        U32 minDelta,                   //!< Minimum Sched Ticks to send packets on updates when using ON_CHANGE logic
        U32 maxDelta                    //!< Maximum Sched Ticks between packets to send when using EVERY_MAX logic
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
    // buffers for sending - will be copied from fill buffers
    BufferEntry m_sendBuffers[MAX_PACKETIZER_PACKETS];

    struct TlmEntry {
        FwChanIdType id;  //!< telemetry id stored in slot
        // Offsets into packet buffers.
        // -1 means that channel is not in that packet
        FwSignedSizeType packetOffset[MAX_PACKETIZER_PACKETS];
        FwSizeType channelSize;  //!< max serialized size of the channel in bytes
        TlmEntry* next;          //!< pointer to next bucket in table
        bool used;               //!< if entry has been used
        bool ignored;            //!< ignored channel id
        bool hasValue;           //!< if the entry has received a value at least once
        FwChanIdType bucketNo;   //!< for testing
    };

    struct TlmSet {
        TlmEntry* slots[TLMPACKETIZER_NUM_TLM_HASH_SLOTS];  //!< set of hash slots in hash table
        TlmEntry buckets[TLMPACKETIZER_HASH_BUCKETS];       //!< set of buckets used in hash table
        FwChanIdType free;                                  //!< next free bucket
    } m_tlmEntries;

    // hash function for looking up telemetry channel
    FwChanIdType doHash(FwChanIdType id);

    Os::Mutex m_lock;  //!< used to lock access to packet buffers

    bool m_configured;  //!< indicates a table has been passed and packets configured

    struct MissingTlmChan {
        FwChanIdType id;
        bool checked;
    } m_missTlmCheck[TLMPACKETIZER_MAX_MISSING_TLM_CHECK];

    void missingChannel(FwChanIdType id);  //!< Helper to check to see if missing channel warning was sent

    TlmEntry* findBucket(FwChanIdType id);

    TlmPacketizer_SectionEnabled m_sectionEnabled{};

    TlmPacketizer_SectionConfigs m_groupConfigs{};

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
};

}  // end namespace Svc

#endif

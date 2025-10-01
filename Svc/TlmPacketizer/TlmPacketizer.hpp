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

#include "Os/Mutex.hpp"
#include "Svc/TlmPacketizer/TlmPacketizerComponentAc.hpp"
#include "Svc/TlmPacketizer/TlmPacketizerTypes.hpp"
#include "config/TlmPacketizerCfg.hpp"

namespace Svc {

class TlmPacketizer final : public TlmPacketizerComponentBase {
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
        const FwChanIdType startLevel);              // starting level of packets to send

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

    //! Handler implementation for Run
    //!
    void Run_handler(const FwIndexType portNum, /*!< The port number*/
                     U32 context                /*!< The call order*/
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
    //! Set telemetry send leve
    void SET_LEVEL_cmdHandler(const FwOpcodeType opCode, /*!< The opcode*/
                              const U32 cmdSeq,          /*!< The command sequence number*/
                              FwChanIdType level         /*!< The I32 command argument*/
                              ) override;

    //! Implementation for SEND_PKT command handler
    //! Force a packet to be sent
    void SEND_PKT_cmdHandler(const FwOpcodeType opCode, /*!< The opcode*/
                             const U32 cmdSeq,          /*!< The command sequence number*/
                             U32 id                     /*!< The packet ID*/
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
        bool requested;        //!< if the packet was requested with SEND_PKT in the last cycle
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
        bool ignored;            //!< ignored packet id
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

    FwChanIdType m_startLevel;  //!< initial level for sending packets
    FwChanIdType m_maxLevel;    //!< maximum level in all packets
};

}  // end namespace Svc

#endif

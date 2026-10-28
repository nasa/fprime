// ======================================================================
// \title  TlmPacketizer.hpp
// \author tcanham
// \brief  cpp file for TlmPacketizer test harness implementation class
//
// \copyright
// Copyright 2009-2021, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

#include "TlmPacketizerTester.hpp"

#define INSTANCE 0
#define MAX_HISTORY_SIZE 100
#define QUEUE_DEPTH 10

#include <Fw/Com/ComPacket.hpp>
#include <algorithm>
namespace Svc {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

TlmPacketizerTester ::TlmPacketizerTester()
    : TlmPacketizerGTestBase("Tester", MAX_HISTORY_SIZE), component("TlmPacketizer") {
    this->initComponents();
    this->connectPorts();
    this->component.loadParameters();
}

TlmPacketizerTester ::~TlmPacketizerTester() {
    this->component.deinit();
}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

// Some Test tables

TlmPacketizerChannelEntry packet1List[] = {{10, 4}, {100, 2}, {333, 1}};

TlmPacketizerChannelEntry packet2List[] = {{10, 4}, {13, 8}, {250, 2}, {22, 1}};

TlmPacketizerChannelEntry packet3List[] = {{67, 4}};

TlmPacketizerChannelEntry packet4List[] = {{10, 4}, {60, 4}};

TlmPacketizerPacket packet1 = {packet1List, 4, 1, FW_NUM_ARRAY_ELEMENTS(packet1List)};

TlmPacketizerPacket packet2 = {packet2List, 8, 2, FW_NUM_ARRAY_ELEMENTS(packet2List)};

TlmPacketizerPacket packet3 = {packet3List, 12, 2, FW_NUM_ARRAY_ELEMENTS(packet3List)};

TlmPacketizerPacket packet4 = {packet4List, 16, 3, FW_NUM_ARRAY_ELEMENTS(packet4List)};

TlmPacketizerPacketList packetList = {{&packet1, &packet2}, 2};
TlmPacketizerPacketList packetList2 = {{&packet1, &packet2, &packet3, &packet4}, 4};

TlmPacketizerChannelEntry ignoreList[] = {{25, 0}, {50, 0}};

TlmPacketizerPacket ignore = {ignoreList, 0, 0, FW_NUM_ARRAY_ELEMENTS(ignoreList)};

void TlmPacketizerTester ::stockConfiguration() {
    // Stock configuration of telemetry packetizer is all channels and all groups enabled sending out all packets
    for (FwIndexType section = 0; section < Svc::TelemetrySection::NUM_SECTIONS; section++) {
        this->sendCmd_ENABLE_SECTION(0, 0, static_cast<Svc::TelemetrySection::T>(section), Fw::Enabled::ENABLED);
        this->dispatchCurrentMessages(this->component);
        for (FwChanIdType group = 0; group < NUM_CONFIGURABLE_TLMPACKETIZER_GROUPS; group++) {
            this->sendCmd_CONFIGURE_GROUP_RATES(0, 0, static_cast<Svc::TelemetrySection::T>(section), group,
                                                RateLogic::ON_CHANGE_MIN, 0, 0);
            this->sendCmd_CONFIGURE_GROUP_SEND(0, 0, static_cast<Svc::TelemetrySection::T>(section), group,
                                               Svc::TlmSendState::INHERIT_FROM_PARENT);
            this->dispatchCurrentMessages(this->component);
        }
    }
    this->clearHistory();
}

void TlmPacketizerTester ::initTest() {
    this->stockConfiguration();
    this->component.setPacketList(packetList, ignore, 2);
}

void TlmPacketizerTester ::pushTlmTest() {
    this->stockConfiguration();
    this->component.setPacketList(packetList, ignore, 2);
    Fw::Time ts;
    Fw::TlmBuffer buff;

    // first channel
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U32>(20)));
    this->invoke_to_TlmRecv(0, 10, ts, buff);

    buff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U16>(15)));
    this->invoke_to_TlmRecv(0, 100, ts, buff);

    buff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U8>(14)));
    this->invoke_to_TlmRecv(0, 333, ts, buff);

    // second channel
    buff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U32>(50)));
    this->invoke_to_TlmRecv(0, 10, ts, buff);

    buff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U64>(1000000)));
    this->invoke_to_TlmRecv(0, 13, ts, buff);

    buff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U16>(1010)));
    this->invoke_to_TlmRecv(0, 250, ts, buff);

    buff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U8>(15)));
    this->invoke_to_TlmRecv(0, 22, ts, buff);
}

void TlmPacketizerTester ::sendPacketsTest() {
    this->stockConfiguration();
    this->component.setPacketList(packetList, ignore, 2);
    Fw::Time ts;
    Fw::TlmBuffer buff;

    // first channel
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U32>(20)));
    this->invoke_to_TlmRecv(0, 10, ts, buff);

    // second channel
    buff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U16>(15)));
    this->invoke_to_TlmRecv(0, 100, ts, buff);

    // third channel
    buff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U8>(14)));
    this->invoke_to_TlmRecv(0, 333, ts, buff);

    // fifth channel
    buff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U64>(1000000)));
    this->invoke_to_TlmRecv(0, 13, ts, buff);

    // sixth channel
    buff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U16>(1010)));
    this->invoke_to_TlmRecv(0, 250, ts, buff);

    // seventh channel
    buff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U8>(15)));
    this->invoke_to_TlmRecv(0, 22, ts, buff);

    this->setTestTime(this->m_testTime);
    // run scheduler port to send packets
    this->invoke_to_Run(0, 0);
    this->component.doDispatch();

    ASSERT_FROM_PORT_HISTORY_SIZE(2 * Svc::TelemetrySection::NUM_SECTIONS);
    ASSERT_from_PktSend_SIZE(2 * Svc::TelemetrySection::NUM_SECTIONS);

    // construct the packet buffers and make sure they are correct

    Fw::ComBuffer comBuff;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,
              comBuff.serializeFrom(static_cast<FwPacketDescriptorType>(Fw::ComPacketType::FW_PACKET_PACKETIZED_TLM)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<FwTlmPacketizeIdType>(4)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(this->m_testTime));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U32>(20)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U16>(15)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U8>(14)));

    // No recently sent packet 1. Context set to sent counter, so this will be at max value.
    ASSERT_from_PktSend(0 * Svc::TelemetrySection::NUM_SECTIONS, comBuff,
                        static_cast<U32>(std::numeric_limits<U32>::max()));

    comBuff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,
              comBuff.serializeFrom(static_cast<FwPacketDescriptorType>(Fw::ComPacketType::FW_PACKET_PACKETIZED_TLM)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<FwTlmPacketizeIdType>(8)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(this->m_testTime));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U32>(20)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U64>(1000000)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U16>(1010)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U8>(15)));

    // No recently sent packet 2. Context set to sent counter, so this will be at max value.
    ASSERT_from_PktSend(1 * Svc::TelemetrySection::NUM_SECTIONS, comBuff,
                        static_cast<U32>(std::numeric_limits<U32>::max()));
}

void TlmPacketizerTester ::sendPacketLevelsTest() {
    this->stockConfiguration();
    this->component.setPacketList(packetList, ignore, 1);
    Fw::Time ts;
    Fw::TlmBuffer buff;

    // first channel
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U32>(20)));
    this->invoke_to_TlmRecv(0, 10, ts, buff);

    // second channel
    buff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U16>(15)));
    this->invoke_to_TlmRecv(0, 100, ts, buff);

    // third channel
    buff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U8>(14)));
    this->invoke_to_TlmRecv(0, 333, ts, buff);

    // fifth channel
    buff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U64>(1000000)));
    this->invoke_to_TlmRecv(0, 13, ts, buff);

    // sixth channel
    buff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U16>(1010)));
    this->invoke_to_TlmRecv(0, 250, ts, buff);

    // seventh channel
    buff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U8>(15)));
    this->invoke_to_TlmRecv(0, 22, ts, buff);

    this->setTestTime(this->m_testTime);
    // run scheduler port to send packets
    this->invoke_to_Run(0, 0);
    this->component.doDispatch();

    ASSERT_FROM_PORT_HISTORY_SIZE(2);
    ASSERT_from_PktSend_SIZE(2);

    // construct the packet buffers and make sure they are correct

    Fw::ComBuffer comBuff;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,
              comBuff.serializeFrom(static_cast<FwPacketDescriptorType>(Fw::ComPacketType::FW_PACKET_PACKETIZED_TLM)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<FwTlmPacketizeIdType>(4)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(this->m_testTime));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U32>(20)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U16>(15)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U8>(14)));

    // No recently sent packet 1. Context set to sent counter, so this will be at max value.
    ASSERT_from_PktSend(0, comBuff, static_cast<U32>(std::numeric_limits<U32>::max()));

    comBuff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,
              comBuff.serializeFrom(static_cast<FwPacketDescriptorType>(Fw::ComPacketType::FW_PACKET_PACKETIZED_TLM)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<FwTlmPacketizeIdType>(8)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(this->m_testTime));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U32>(20)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U64>(1000000)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U16>(1010)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U8>(15)));

    // No recently sent packet 2. Context set to sent counter, so this will be at max value.
    ASSERT_from_PktSend(1, comBuff, static_cast<U32>(std::numeric_limits<U32>::max()));
}

void TlmPacketizerTester ::updatePacketsTest() {
    this->stockConfiguration();
    this->component.setPacketList(packetList, ignore, 2);
    Fw::Time ts;
    Fw::TlmBuffer buff;

    Fw::ComBuffer comBuff;

    // Initially no packets should be pushed
    this->invoke_to_Run(0, 0);
    this->component.doDispatch();

    // Should be no packets pushed
    ASSERT_from_PktSend_SIZE(0);

    // first channel
    ts.set(100, 1000);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U32>(20)));
    this->invoke_to_TlmRecv(0, 10, ts, buff);

    this->m_testTime.add(1, 0);
    this->setTestTime(this->m_testTime);
    this->clearFromPortHistory();
    this->invoke_to_Run(0, 0);
    this->component.doDispatch();
    ASSERT_from_PktSend_SIZE(2 * Svc::TelemetrySection::NUM_SECTIONS);

    comBuff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,
              comBuff.serializeFrom(static_cast<FwPacketDescriptorType>(Fw::ComPacketType::FW_PACKET_PACKETIZED_TLM)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<FwTlmPacketizeIdType>(4)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(ts));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U32>(20)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U16>(0)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U8>(0)));

    // No recently sent packet 1. Context set to sent counter, so this will be at max value.
    for (FwIndexType section = 0; section < Svc::TelemetrySection::NUM_SECTIONS; section++) {
        ASSERT_from_PktSend((0 * Svc::TelemetrySection::NUM_SECTIONS) + section, comBuff,
                            static_cast<U32>(std::numeric_limits<U32>::max()));
    }

    comBuff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,
              comBuff.serializeFrom(static_cast<FwPacketDescriptorType>(Fw::ComPacketType::FW_PACKET_PACKETIZED_TLM)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<FwTlmPacketizeIdType>(8)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(ts));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U32>(20)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U64>(0)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U16>(0)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U8>(0)));

    // No recently sent packet 1. Context set to sent counter, so this will be at max value.
    for (FwIndexType section = 0; section < Svc::TelemetrySection::NUM_SECTIONS; section++) {
        ASSERT_from_PktSend((1 * Svc::TelemetrySection::NUM_SECTIONS) + section, comBuff,
                            static_cast<U32>(std::numeric_limits<U32>::max()));
    }

    // second channel

    buff.resetSer();
    ts.add(1, 0);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U16>(15)));
    this->invoke_to_TlmRecv(0, 100, ts, buff);

    this->m_testTime.add(1, 0);
    this->setTestTime(this->m_testTime);
    this->clearFromPortHistory();
    this->invoke_to_Run(0, 0);
    this->component.doDispatch();

    // only one should be pushed and is replicated across all sections
    ASSERT_from_PktSend_SIZE(1 * Svc::TelemetrySection::NUM_SECTIONS);

    comBuff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,
              comBuff.serializeFrom(static_cast<FwPacketDescriptorType>(Fw::ComPacketType::FW_PACKET_PACKETIZED_TLM)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<FwTlmPacketizeIdType>(4)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(ts));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U32>(20)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U16>(15)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U8>(0)));

    // Packet 1 sent recently with a delta sched ticks of 1
    for (FwIndexType section = 0; section < Svc::TelemetrySection::NUM_SECTIONS; section++) {
        ASSERT_from_PktSend((0 * Svc::TelemetrySection::NUM_SECTIONS) + section, comBuff, static_cast<U32>(1));
    }

    buff.resetSer();
    ts.add(1, 0);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U8>(14)));
    this->invoke_to_TlmRecv(0, 333, ts, buff);

    this->clearFromPortHistory();
    this->invoke_to_Run(0, 0);
    this->component.doDispatch();
    // only one should be pushed and is replicated across all sections
    ASSERT_from_PktSend_SIZE(1 * Svc::TelemetrySection::NUM_SECTIONS);

    comBuff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,
              comBuff.serializeFrom(static_cast<FwPacketDescriptorType>(Fw::ComPacketType::FW_PACKET_PACKETIZED_TLM)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<FwTlmPacketizeIdType>(4)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(ts));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U32>(20)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U16>(15)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U8>(14)));

    // Packet 1 sent recently with a delta sched ticks of 1
    for (FwIndexType section = 0; section < Svc::TelemetrySection::NUM_SECTIONS; section++) {
        ASSERT_from_PktSend((0 * Svc::TelemetrySection::NUM_SECTIONS) + section, comBuff, static_cast<U32>(1));
    }

    buff.resetSer();
    ts.add(1, 0);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U64>(1000000)));
    this->invoke_to_TlmRecv(0, 13, ts, buff);

    this->clearFromPortHistory();
    this->invoke_to_Run(0, 0);
    this->component.doDispatch();
    ASSERT_from_PktSend_SIZE(1 * Svc::TelemetrySection::NUM_SECTIONS);

    comBuff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,
              comBuff.serializeFrom(static_cast<FwPacketDescriptorType>(Fw::ComPacketType::FW_PACKET_PACKETIZED_TLM)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<FwTlmPacketizeIdType>(8)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(ts));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U32>(20)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U64>(1000000)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U16>(0)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U8>(0)));

    // Packet 2 sent recently with a delta sched ticks of 3
    for (FwIndexType section = 0; section < Svc::TelemetrySection::NUM_SECTIONS; section++) {
        ASSERT_from_PktSend((0 * Svc::TelemetrySection::NUM_SECTIONS) + section, comBuff, static_cast<U32>(3));
    }

    buff.resetSer();
    ts.add(1, 0);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U16>(1010)));
    this->invoke_to_TlmRecv(0, 250, ts, buff);

    this->clearFromPortHistory();
    this->invoke_to_Run(0, 0);
    this->component.doDispatch();
    ASSERT_from_PktSend_SIZE(1 * Svc::TelemetrySection::NUM_SECTIONS);

    comBuff.resetSer();

    comBuff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,
              comBuff.serializeFrom(static_cast<FwPacketDescriptorType>(Fw::ComPacketType::FW_PACKET_PACKETIZED_TLM)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<FwTlmPacketizeIdType>(8)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(ts));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U32>(20)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U64>(1000000)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U16>(1010)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U8>(0)));

    // Packet 2 sent recently with a delta sched ticks of 1
    for (FwIndexType section = 0; section < Svc::TelemetrySection::NUM_SECTIONS; section++) {
        ASSERT_from_PktSend((0 * Svc::TelemetrySection::NUM_SECTIONS) + section, comBuff, static_cast<U32>(1));
    }

    buff.resetSer();
    ts.add(1, 0);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U8>(15)));
    this->invoke_to_TlmRecv(0, 22, ts, buff);

    this->clearFromPortHistory();
    this->invoke_to_Run(0, 0);
    this->component.doDispatch();
    ASSERT_from_PktSend_SIZE(1 * Svc::TelemetrySection::NUM_SECTIONS);

    comBuff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,
              comBuff.serializeFrom(static_cast<FwPacketDescriptorType>(Fw::ComPacketType::FW_PACKET_PACKETIZED_TLM)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<FwTlmPacketizeIdType>(8)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(ts));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U32>(20)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U64>(1000000)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U16>(1010)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U8>(15)));

    // Packet 2 sent recently with a delta sched ticks of 1
    for (FwIndexType section = 0; section < Svc::TelemetrySection::NUM_SECTIONS; section++) {
        ASSERT_from_PktSend((0 * Svc::TelemetrySection::NUM_SECTIONS) + section, comBuff, static_cast<U32>(1));
    }

    //** Update all the packets again with new values

    // first channel
    buff.resetSer();
    ts.add(1, 0);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U32>(1000)));
    this->invoke_to_TlmRecv(0, 10, ts, buff);

    this->clearFromPortHistory();
    this->invoke_to_Run(0, 0);
    this->component.doDispatch();
    ASSERT_from_PktSend_SIZE(2 * Svc::TelemetrySection::NUM_SECTIONS);

    comBuff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,
              comBuff.serializeFrom(static_cast<FwPacketDescriptorType>(Fw::ComPacketType::FW_PACKET_PACKETIZED_TLM)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<FwTlmPacketizeIdType>(4)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(ts));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U32>(1000)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U16>(15)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U8>(14)));

    // Packet 1 sent recently with a delta sched ticks of 4
    for (FwIndexType section = 0; section < Svc::TelemetrySection::NUM_SECTIONS; section++) {
        ASSERT_from_PktSend((0 * Svc::TelemetrySection::NUM_SECTIONS) + section, comBuff, static_cast<U32>(4));
    }

    comBuff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,
              comBuff.serializeFrom(static_cast<FwPacketDescriptorType>(Fw::ComPacketType::FW_PACKET_PACKETIZED_TLM)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<FwTlmPacketizeIdType>(8)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(ts));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U32>(1000)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U64>(1000000)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U16>(1010)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U8>(15)));

    // Packet 2 sent recently with a delta sched ticks of 1
    for (FwIndexType section = 0; section < Svc::TelemetrySection::NUM_SECTIONS; section++) {
        ASSERT_from_PktSend((1 * Svc::TelemetrySection::NUM_SECTIONS) + section, comBuff, static_cast<U32>(1));
    }

    // second channel

    buff.resetSer();
    ts.add(1, 0);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U16>(550)));
    this->invoke_to_TlmRecv(0, 100, ts, buff);

    this->clearFromPortHistory();
    this->invoke_to_Run(0, 0);
    this->component.doDispatch();
    ASSERT_from_PktSend_SIZE(1 * Svc::TelemetrySection::NUM_SECTIONS);

    comBuff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,
              comBuff.serializeFrom(static_cast<FwPacketDescriptorType>(Fw::ComPacketType::FW_PACKET_PACKETIZED_TLM)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<FwTlmPacketizeIdType>(4)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(ts));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U32>(1000)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U16>(550)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U8>(14)));

    // Packet 1 sent recently with a delta sched ticks of 1
    for (FwIndexType section = 0; section < Svc::TelemetrySection::NUM_SECTIONS; section++) {
        ASSERT_from_PktSend((0 * Svc::TelemetrySection::NUM_SECTIONS) + section, comBuff, static_cast<U32>(1));
    }

    buff.resetSer();
    ts.add(1, 0);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U8>(211)));
    this->invoke_to_TlmRecv(0, 333, ts, buff);

    this->clearFromPortHistory();
    this->invoke_to_Run(0, 0);
    this->component.doDispatch();
    ASSERT_from_PktSend_SIZE(1 * Svc::TelemetrySection::NUM_SECTIONS);

    comBuff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,
              comBuff.serializeFrom(static_cast<FwPacketDescriptorType>(Fw::ComPacketType::FW_PACKET_PACKETIZED_TLM)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<FwTlmPacketizeIdType>(4)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(ts));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U32>(1000)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U16>(550)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U8>(211)));

    // Packet 1 sent recently with a delta sched ticks of 1
    for (FwIndexType section = 0; section < Svc::TelemetrySection::NUM_SECTIONS; section++) {
        ASSERT_from_PktSend((0 * Svc::TelemetrySection::NUM_SECTIONS) + section, comBuff, static_cast<U32>(1));
    }

    buff.resetSer();
    ts.add(1, 0);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U64>(34441)));
    this->invoke_to_TlmRecv(0, 13, ts, buff);

    this->clearFromPortHistory();
    this->invoke_to_Run(0, 0);
    this->component.doDispatch();
    ASSERT_from_PktSend_SIZE(1 * Svc::TelemetrySection::NUM_SECTIONS);

    comBuff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,
              comBuff.serializeFrom(static_cast<FwPacketDescriptorType>(Fw::ComPacketType::FW_PACKET_PACKETIZED_TLM)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<FwTlmPacketizeIdType>(8)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(ts));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U32>(1000)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U64>(34441)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U16>(1010)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U8>(15)));

    // Packet 2 sent recently with a delta sched ticks of 3
    for (FwIndexType section = 0; section < Svc::TelemetrySection::NUM_SECTIONS; section++) {
        ASSERT_from_PktSend((0 * Svc::TelemetrySection::NUM_SECTIONS) + section, comBuff, static_cast<U32>(3));
    }

    buff.resetSer();
    ts.add(1, 0);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U16>(8649)));
    this->invoke_to_TlmRecv(0, 250, ts, buff);

    this->clearFromPortHistory();
    this->invoke_to_Run(0, 0);
    this->component.doDispatch();
    ASSERT_from_PktSend_SIZE(1 * Svc::TelemetrySection::NUM_SECTIONS);

    comBuff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,
              comBuff.serializeFrom(static_cast<FwPacketDescriptorType>(Fw::ComPacketType::FW_PACKET_PACKETIZED_TLM)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<FwTlmPacketizeIdType>(8)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(ts));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U32>(1000)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U64>(34441)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U16>(8649)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U8>(15)));

    // Packet 2 sent recently with a delta sched ticks of 1
    for (FwIndexType section = 0; section < Svc::TelemetrySection::NUM_SECTIONS; section++) {
        ASSERT_from_PktSend((0 * Svc::TelemetrySection::NUM_SECTIONS) + section, comBuff, static_cast<U32>(1));
    }

    buff.resetSer();
    ts.add(1, 0);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U8>(65)));
    this->invoke_to_TlmRecv(0, 22, ts, buff);

    this->clearFromPortHistory();
    this->invoke_to_Run(0, 0);
    this->component.doDispatch();
    ASSERT_from_PktSend_SIZE(1 * Svc::TelemetrySection::NUM_SECTIONS);

    comBuff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,
              comBuff.serializeFrom(static_cast<FwPacketDescriptorType>(Fw::ComPacketType::FW_PACKET_PACKETIZED_TLM)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<FwTlmPacketizeIdType>(8)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(ts));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U32>(1000)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U64>(34441)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U16>(8649)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U8>(65)));

    // Packet 2 sent recently with a delta sched ticks of 1
    for (FwIndexType section = 0; section < Svc::TelemetrySection::NUM_SECTIONS; section++) {
        ASSERT_from_PktSend((0 * Svc::TelemetrySection::NUM_SECTIONS) + section, comBuff, static_cast<U32>(1));
    }
}

void TlmPacketizerTester ::ignoreTest() {
    this->stockConfiguration();
    this->component.setPacketList(packetList, ignore, 2);
    Fw::Time ts;
    Fw::TlmBuffer buff;

    Fw::ComBuffer comBuff;

    // Initially no packets should be pushed
    this->invoke_to_Run(0, 0);
    this->component.doDispatch();

    // Should be no packets pushed
    ASSERT_from_PktSend_SIZE(0);

    // first channel
    ts.set(100, 1000);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U32>(20)));
    this->invoke_to_TlmRecv(0, 10, ts, buff);

    this->m_testTime.add(1, 0);
    this->setTestTime(this->m_testTime);
    this->clearFromPortHistory();
    this->invoke_to_Run(0, 0);
    this->component.doDispatch();
    ASSERT_from_PktSend_SIZE(2 * Svc::TelemetrySection::NUM_SECTIONS);

    comBuff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,
              comBuff.serializeFrom(static_cast<FwPacketDescriptorType>(Fw::ComPacketType::FW_PACKET_PACKETIZED_TLM)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<FwTlmPacketizeIdType>(4)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(ts));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U32>(20)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U16>(0)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U8>(0)));

    // First Packet 1 Send. Delta Sched Ticks = Max
    ASSERT_from_PktSend(0, comBuff, static_cast<U32>(std::numeric_limits<U32>::max()));

    comBuff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,
              comBuff.serializeFrom(static_cast<FwPacketDescriptorType>(Fw::ComPacketType::FW_PACKET_PACKETIZED_TLM)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<FwTlmPacketizeIdType>(8)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(ts));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U32>(20)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U64>(0)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U16>(0)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U8>(0)));

    // First Packet 2 Send. Delta Sched Ticks = Max
    for (FwIndexType section = 0; section < Svc::TelemetrySection::NUM_SECTIONS; section++) {
        ASSERT_from_PktSend((1 * Svc::TelemetrySection::NUM_SECTIONS) + section, comBuff,
                            static_cast<U32>(std::numeric_limits<U32>::max()));
    }

    // ignored channel

    buff.resetSer();
    ts.add(1, 0);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U16>(20)));
    this->invoke_to_TlmRecv(0, 25, ts, buff);

    this->m_testTime.add(1, 0);
    this->setTestTime(this->m_testTime);
    this->clearFromPortHistory();
    this->invoke_to_Run(0, 0);
    this->component.doDispatch();

    // no packets should be pushed
    ASSERT_from_PktSend_SIZE(0);
}

void TlmPacketizerTester ::sendManualPacketTest() {
    this->stockConfiguration();
    this->component.setPacketList(packetList, ignore, 2);
    Fw::Time ts;
    Fw::TlmBuffer buff;

    // first channel
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U32>(20)));
    this->invoke_to_TlmRecv(0, 10, ts, buff);

    // second channel
    buff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U16>(15)));
    this->invoke_to_TlmRecv(0, 100, ts, buff);

    // third channel
    buff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U8>(14)));
    this->invoke_to_TlmRecv(0, 333, ts, buff);

    // fifth channel
    buff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U64>(1000000)));
    this->invoke_to_TlmRecv(0, 13, ts, buff);

    // sixth channel
    buff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U16>(1010)));
    this->invoke_to_TlmRecv(0, 250, ts, buff);

    // seventh channel
    buff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U8>(15)));
    this->invoke_to_TlmRecv(0, 22, ts, buff);

    this->setTestTime(this->m_testTime);
    // run scheduler port to send packets
    this->invoke_to_Run(0, 0);
    this->component.doDispatch();

    ASSERT_FROM_PORT_HISTORY_SIZE(2 * Svc::TelemetrySection::NUM_SECTIONS);
    ASSERT_from_PktSend_SIZE(2 * Svc::TelemetrySection::NUM_SECTIONS);

    // construct the packet buffers and make sure they are correct

    Fw::ComBuffer comBuff1;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,
              comBuff1.serializeFrom(static_cast<FwPacketDescriptorType>(Fw::ComPacketType::FW_PACKET_PACKETIZED_TLM)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff1.serializeFrom(static_cast<FwTlmPacketizeIdType>(4)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff1.serializeFrom(this->m_testTime));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff1.serializeFrom(static_cast<U32>(20)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff1.serializeFrom(static_cast<U16>(15)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff1.serializeFrom(static_cast<U8>(14)));

    // First Packet 1 Send. Delta Sched Ticks = Max
    for (FwIndexType section = 0; section < Svc::TelemetrySection::NUM_SECTIONS; section++) {
        ASSERT_from_PktSend((0 * Svc::TelemetrySection::NUM_SECTIONS) + section, comBuff1,
                            static_cast<U32>(std::numeric_limits<U32>::max()));
    }

    Fw::ComBuffer comBuff2;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,
              comBuff2.serializeFrom(static_cast<FwPacketDescriptorType>(Fw::ComPacketType::FW_PACKET_PACKETIZED_TLM)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff2.serializeFrom(static_cast<FwTlmPacketizeIdType>(8)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff2.serializeFrom(this->m_testTime));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff2.serializeFrom(static_cast<U32>(20)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff2.serializeFrom(static_cast<U64>(1000000)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff2.serializeFrom(static_cast<U16>(1010)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff2.serializeFrom(static_cast<U8>(15)));

    // First Packet 2 Send. Delta Sched Ticks = Max
    for (FwIndexType section = 0; section < Svc::TelemetrySection::NUM_SECTIONS; section++) {
        ASSERT_from_PktSend((1 * Svc::TelemetrySection::NUM_SECTIONS) + section, comBuff2,
                            static_cast<U32>(std::numeric_limits<U32>::max()));
    }

    // should not be any new packets
    this->clearHistory();
    this->invoke_to_Run(0, 0);
    this->component.doDispatch();

    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    ASSERT_from_PktSend_SIZE(0);

    // send command to manually send a packet
    this->sendCmd_SEND_PKT(0, 12, 4, static_cast<TelemetrySection::T>(0));
    this->component.doDispatch();
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_PacketSent(0, 4);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, TlmPacketizerComponentBase::OPCODE_SEND_PKT, 12, Fw::CmdResponse::OK);
    // dispatch run call to send packet
    this->invoke_to_Run(0, 0);
    this->component.doDispatch();
    ASSERT_from_PktSend_SIZE(1 * Svc::TelemetrySection::NUM_SECTIONS);
    // Packet 1 Sent recently. Delta Sched Ticks = 2
    for (FwIndexType section = 0; section < Svc::TelemetrySection::NUM_SECTIONS; section++) {
        ASSERT_from_PktSend((0 * Svc::TelemetrySection::NUM_SECTIONS) + section, comBuff1, static_cast<U32>(2));
    }

    // another packet
    this->clearHistory();
    this->invoke_to_Run(0, 0);
    this->component.doDispatch();

    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    ASSERT_from_PktSend_SIZE(0);

    // send command to manually send a packet
    this->clearHistory();
    this->sendCmd_SEND_PKT(0, 12, 8, static_cast<TelemetrySection::T>(0));
    this->component.doDispatch();
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_PacketSent(0, 8);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, TlmPacketizerComponentBase::OPCODE_SEND_PKT, 12, Fw::CmdResponse::OK);
    // dispatch run call to send packet
    this->invoke_to_Run(0, 0);
    this->component.doDispatch();
    ASSERT_from_PktSend_SIZE(1 * Svc::TelemetrySection::NUM_SECTIONS);
    // Packet 2 Sent recently. Delta Sched Ticks = 4
    for (FwIndexType section = 0; section < Svc::TelemetrySection::NUM_SECTIONS; section++) {
        ASSERT_from_PktSend((0 * Svc::TelemetrySection::NUM_SECTIONS) + section, comBuff2, static_cast<U32>(4));
    }

    // Try to send invalid packet
    // send command to manually send a packet
    this->clearHistory();
    this->sendCmd_SEND_PKT(0, 12, 20, static_cast<TelemetrySection::T>(0));
    this->component.doDispatch();
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_PacketNotFound(0, 20);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, TlmPacketizerComponentBase::OPCODE_SEND_PKT, 12, Fw::CmdResponse::VALIDATION_ERROR);
}

void TlmPacketizerTester ::setPacketLevelTest() {
    this->stockConfiguration();
    this->component.setPacketList(packetList, ignore, 0);
    Fw::Time ts;
    Fw::TlmBuffer buff;

    // first channel
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U32>(0x20)));
    this->invoke_to_TlmRecv(0, 10, ts, buff);

    // second channel
    buff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U16>(0x15)));
    this->invoke_to_TlmRecv(0, 100, ts, buff);

    // third channel
    buff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U8>(0x14)));
    this->invoke_to_TlmRecv(0, 333, ts, buff);

    // fifth channel
    buff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U64>(0x1000000)));
    this->invoke_to_TlmRecv(0, 13, ts, buff);

    // sixth channel
    buff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U16>(0x1010)));
    this->invoke_to_TlmRecv(0, 250, ts, buff);

    // seventh channel
    buff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U8>(0x15)));
    this->invoke_to_TlmRecv(0, 22, ts, buff);

    this->setTestTime(this->m_testTime);
    // run scheduler port to send packets
    this->invoke_to_Run(0, 0);
    this->component.doDispatch();

    // should be no packets sent since packet level is 0
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    ASSERT_from_PktSend_SIZE(0);

    // send the command to select packet level 1
    this->clearHistory();
    this->sendCmd_SET_LEVEL(0, 13, 1);
    this->component.doDispatch();
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_LevelSet_SIZE(1);
    ASSERT_EVENTS_LevelSet(0, 1);
    ASSERT_TLM_SIZE(1);
    ASSERT_TLM_GroupConfigs_SIZE(1);

    // send the packets
    // first channel
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U32>(0x20)));
    this->invoke_to_TlmRecv(0, 10, ts, buff);

    // second channel
    buff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U16>(0x15)));
    this->invoke_to_TlmRecv(0, 100, ts, buff);

    // third channel
    buff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U8>(0x14)));
    this->invoke_to_TlmRecv(0, 333, ts, buff);

    // fifth channel
    buff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U64>(0x1000000)));
    this->invoke_to_TlmRecv(0, 13, ts, buff);

    // sixth channel
    buff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U16>(0x1010)));
    this->invoke_to_TlmRecv(0, 250, ts, buff);

    // seventh channel
    buff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U8>(0x15)));
    this->invoke_to_TlmRecv(0, 22, ts, buff);

    this->setTestTime(this->m_testTime);
    // run scheduler port to send packets
    this->invoke_to_Run(0, 0);
    this->component.doDispatch();

    // should be one packet sent since packet level is 1
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    ASSERT_from_PktSend_SIZE(1);

    // Should be packet 4

    Fw::ComBuffer comBuff1;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,
              comBuff1.serializeFrom(static_cast<FwPacketDescriptorType>(Fw::ComPacketType::FW_PACKET_PACKETIZED_TLM)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff1.serializeFrom(static_cast<FwTlmPacketizeIdType>(4)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff1.serializeFrom(this->m_testTime));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff1.serializeFrom(static_cast<U32>(0x20)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff1.serializeFrom(static_cast<U16>(0x15)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff1.serializeFrom(static_cast<U8>(0x14)));

    // First Packet 1 Send. Delta Sched Ticks = Max
    ASSERT_from_PktSend(0, comBuff1, static_cast<U32>(std::numeric_limits<U32>::max()));

    return;
}

void TlmPacketizerTester ::nonPacketizedChannelTest() {
    this->stockConfiguration();
    this->component.setPacketList(packetList, ignore, 2);
    Fw::Time ts;
    Fw::TlmBuffer buff;

    // start at non-used channel
    for (FwChanIdType channel = 1000; channel < 1000 + TLMPACKETIZER_MAX_MISSING_TLM_CHECK; channel++) {
        this->clearEvents();
        this->invoke_to_TlmRecv(0, channel, ts, buff);
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_NoChan_SIZE(1);
        ASSERT_EVENTS_NoChan(0, channel);
    }

    // One more channel should not emit event
    this->clearEvents();
    this->invoke_to_TlmRecv(0, 1000 + TLMPACKETIZER_MAX_MISSING_TLM_CHECK, ts, buff);
    ASSERT_EVENTS_SIZE(0);
    ASSERT_EVENTS_NoChan_SIZE(0);

    // sending the missing channels again should emit no events

    for (FwChanIdType channel = 1000; channel < 1000 + TLMPACKETIZER_MAX_MISSING_TLM_CHECK; channel++) {
        this->clearEvents();
        this->invoke_to_TlmRecv(0, channel, ts, buff);
        ASSERT_EVENTS_SIZE(0);
        ASSERT_EVENTS_NoChan_SIZE(0);
    }
}

void TlmPacketizerTester ::pingTest() {
    this->stockConfiguration();
    this->component.setPacketList(packetList, ignore, 2);
    // ping component
    this->clearFromPortHistory();
    this->invoke_to_pingIn(0, static_cast<U32>(0x1234));
    this->component.doDispatch();
    ASSERT_from_pingOut_SIZE(1);
    ASSERT_from_pingOut(0, static_cast<U32>(0x1234));
}

//! get channel value test
//!
void TlmPacketizerTester ::getChannelValueTest() {
    this->stockConfiguration();
    this->component.setPacketList(packetList, ignore, 2);
    Fw::Time time;
    Fw::TlmBuffer val;
    Fw::TlmValid valid = this->invoke_to_TlmGet(0, 10, time, val);
    // hasn't received a value yet
    ASSERT_EQ(val.getSize(), 0);
    ASSERT_EQ(valid, Fw::TlmValid::INVALID);

    Fw::Time timeIn(123, 456);
    Fw::TlmBuffer valIn;
    valIn.serializeFrom(static_cast<I32>(789));
    this->invoke_to_TlmRecv(0, 10, timeIn, valIn);

    valid = this->invoke_to_TlmGet(0, 10, time, val);
    // should have a value
    ASSERT_EQ(val.getSize(), 4);
    ASSERT_EQ(time, timeIn);
    ASSERT_EQ(valid, Fw::TlmValid::VALID);
    I32 readBack = 0;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, val.deserializeTo(readBack));
    ASSERT_EQ(readBack, 789);

    // channel 250 is only in packet 8 and is not its first channel: the read must come from that packet/offset
    Fw::Time timeIn2(321, 654);
    Fw::TlmBuffer valIn2;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, valIn2.serializeFrom(static_cast<U16>(1010)));
    this->invoke_to_TlmRecv(0, 250, timeIn2, valIn2);

    valid = this->invoke_to_TlmGet(0, 250, time, val);
    ASSERT_EQ(valid, Fw::TlmValid::VALID);
    ASSERT_EQ(val.getSize(), 2);
    ASSERT_EQ(time, timeIn2);
    U16 readBack2 = 0;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, val.deserializeTo(readBack2));
    ASSERT_EQ(readBack2, 1010);

    // grab an ignored chan
    valid = this->invoke_to_TlmGet(0, 25, time, val);
    // should not have a value
    ASSERT_EQ(val.getSize(), 0);
    ASSERT_EQ(valid, Fw::TlmValid::INVALID);

    // grab a nonexistent chan
    // set it to 4 so we can see when it fails
    val.setBuffLen(4);
    valid = this->invoke_to_TlmGet(0, 9123, time, val);
    // should not have a value
    ASSERT_EQ(val.getSize(), 0);
    ASSERT_EQ(valid, Fw::TlmValid::INVALID);
}

//! Configured tlm groups test
//!
void TlmPacketizerTester ::configuredTelemetryGroupsTests() {
    this->stockConfiguration();  // Will be overridden
    if (TelemetrySection::NUM_SECTIONS < 2) {
        GTEST_SKIP() << "This test requires 2 or more telemetry sections to function";
    }
    std::vector<FwIndexType> port_list;
    for (FwIndexType i = 0; i < Svc::TelemetrySection::NUM_SECTIONS; i++) {
        for (FwIndexType j = 0; j < Svc::NUM_CONFIGURABLE_TLMPACKETIZER_GROUPS; j++) {
            FwIndexType port = TlmPacketizer::sectionGroupToPort(i, j);
            if (std::find(port_list.begin(), port_list.end(), port) == port_list.end()) {
                port_list.push_back(port);
            } else {
                GTEST_SKIP() << "This test cannot run with reused ports. Found duplicate port: " << port;
            }
        }
    }

    this->component.setPacketList(packetList2, ignore, 4);
    Fw::Time time;
    Fw::TlmBuffer buffer;

    // Set level to high to enable all levels
    this->sendCmd_SET_LEVEL(0, 0, 10);
    this->component.doDispatch();

    // Group 1
    this->sendCmd_CONFIGURE_GROUP_RATES(0, 0, static_cast<TelemetrySection::T>(0), 1, Svc::RateLogic::ON_CHANGE_MIN, 3,
                                        3);
    this->component.doDispatch();

    // Send every 5 on port 1
    this->sendCmd_CONFIGURE_GROUP_RATES(0, 0, static_cast<TelemetrySection::T>(1), 1, Svc::RateLogic::ON_CHANGE_MIN, 2,
                                        2);
    this->component.doDispatch();

    this->clearHistory();

    // Group 2
    this->invoke_to_configureSectionGroupRate(0, static_cast<TelemetrySection::T>(0), 2,
                                              Svc::RateLogic::ON_CHANGE_MIN_OR_EVERY_MAX, 4, 12);
    this->component.doDispatch();

    this->sendCmd_CONFIGURE_GROUP_SEND(0, 0, static_cast<TelemetrySection::T>(1), 2, Svc::TlmSendState::SEND_NEVER);
    this->component.doDispatch();

    this->clearHistory();

    // Group 3
    this->sendCmd_CONFIGURE_GROUP_RATES(0, 0, static_cast<TelemetrySection::T>(1), 3,
                                        Svc::RateLogic::ON_CHANGE_MIN_OR_EVERY_MAX, 0, 7);
    this->component.doDispatch();

    this->sendCmd_CONFIGURE_GROUP_RATES(0, 0, static_cast<TelemetrySection::T>(0), 3, Svc::RateLogic::EVERY_MAX, 0, 6);
    this->component.doDispatch();

    // Disable output on section 2 via port invocation
    for (FwIndexType section = 2; section < Svc::TelemetrySection::NUM_SECTIONS; section++) {
        this->invoke_to_controlIn(0, static_cast<TelemetrySection::T>(section), Fw::Enabled::DISABLED);
        this->component.doDispatch();
    }

    this->clearHistory();

    /*
    Configuration:
    Section 0 Group 1: 3, 15           MIN 3
    Section 1 Group 1: 2, 14           MIN 2
    Section 0 Group 2: 1, 4, 13, 16.   MIN 4, MAX 12
    Section 1 Group 3: 0, 7, 12, 18.   MIN 0, MAX 7
    Section 0 Group 3: 6, 18.          MAX 6
    Section 1 group 2 Ignored
    */

    /*
    clang-format off
    
    T=0 Tests Updates of packets 1,2, and 4 for Groups 1,2, and 4. Updated Packets are emitted.
    T=1 Tests Updates of packets 1,2, and 3. 
        Packet 3 is emitted, while Packet 2 is not due to < MIN (Each packet has their own counter)
    T=2 Packet 1 emits after passing MIN (configured for port 1, group 1, updated at T=1)
    T=3 Packet 1 emits after passing MIN (configured for port 0, group 1, updated at T=1)
    T=4 Packet 2 emits after passing MIN (Received update at T=1)
    T=4 Test updates packet 2 for group 2. 
        This tests updating a packet when time = MIN, and should be emitted. (Packet 2 and 3 have their own counters)
    T=6 Packet 4 emits on port 1 after passing MAX (configured for port 1, group 3).
    T=7 Packet 4 emits on port 0 after passing MAX, even if it had received no updates.
    
    T=12 Tests updating packets 1, 2, and 4. 
        Packet 4 on is emitted since it is updated after MIN and before MAX. 
        Packets 1 and 2 are updated after MIN and may also be at MAX, which is then emitted.


    Packet Updates     1,2,4   1,2,3                                                                                   1,2,4    
                       V       V                                                                                       V      
                       T=0     T=1     T=2     T=3     T=4     T=5     T=6     T=7     T=8     T=9     T=10    T=11    T=12    
                       
    (Bass Clef)        -|-------------------------------|-------------------------------|-------------------------------|-
    Section 0 Group 1   ●                       ●       |                               |                               ● 
    Section 1 Group 1  -●---------------●---------------|-------------------------------|-------------------------------●-
    Section 0 Group 2   ●       ●                       ●                               |                               ● 
    Section 1 Group 3  -●-------------------------------|-----------------------●-------|-------------------------------●-
    Section 0 Group 3   ●                               |               ●               |                               ● 
    Section 1 Group 2  -|-------------------------------|-------------------------------|-------------------------------|-
                        |                               |                               |                               | 
                       -|-------------------------------|-------------------------------|-------------------------------|-
                                |
                                Note: Packets 2 and 3 are updated and have their own independent counters! 
    
    Expected Output:    5       1       1       1       1       0       1       1       0       0       0       0       5
    
    clang-format on
    */

    // 1st Channel (Pkt 1, 2, 4)
    buffer.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buffer.serializeFrom(static_cast<U32>(1)));
    this->invoke_to_TlmRecv(0, 10, time, buffer);

    // 2nd Channel (Pkt 1)
    buffer.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buffer.serializeFrom(static_cast<U16>(2)));
    this->invoke_to_TlmRecv(0, 100, time, buffer);

    // 3rd Channel (Pkt 1)
    buffer.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buffer.serializeFrom(static_cast<U8>(3)));
    this->invoke_to_TlmRecv(0, 333, time, buffer);

    // 2nd Channel (Pkt 2)
    buffer.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buffer.serializeFrom(static_cast<U64>(2)));
    this->invoke_to_TlmRecv(0, 13, time, buffer);

    // 3rd Channel (Pkt 2)
    buffer.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buffer.serializeFrom(static_cast<U16>(3)));
    this->invoke_to_TlmRecv(0, 250, time, buffer);

    // 4th Channel (Pkt 2)
    buffer.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buffer.serializeFrom(static_cast<U8>(4)));
    this->invoke_to_TlmRecv(0, 22, time, buffer);

    // 2nd Channel (Pkt 4)
    buffer.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buffer.serializeFrom(static_cast<U32>(2)));
    this->invoke_to_TlmRecv(0, 60, time, buffer);

    // run scheduler port to send packets
    // T = 0
    this->invoke_to_Run(0, 0);

    this->component.doDispatch();

    ASSERT_FROM_PORT_HISTORY_SIZE(5);
    ASSERT_from_PktSend_SIZE(5);

    // Packet Location Indices (Checking proper Section, Group)
    ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(0, 1)], 1);
    ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(1, 1)], 1);
    ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(0, 2)], 1);
    ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(1, 3)], 1);
    ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(0, 3)], 1);
    ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(1, 2)], 0);

    // Check that the sections are disabled
    for (FwIndexType section = 2; section < Svc::TelemetrySection::NUM_SECTIONS; section++) {
        for (FwSizeType group = 0; group < Svc::NUM_CONFIGURABLE_TLMPACKETIZER_GROUPS; group++) {
            ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(section, group)], 0);
        }
    }

    // construct the packet buffers and make sure they are correct

    // Pkt 1
    Fw::ComBuffer comBuff;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,
              comBuff.serializeFrom(static_cast<FwPacketDescriptorType>(Fw::ComPacketType::FW_PACKET_PACKETIZED_TLM)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<FwTlmPacketizeIdType>(4)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(this->m_testTime));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U32>(1)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U16>(2)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U8>(3)));

    // First Packet 1 Send. Delta Sched Ticks = Max
    ASSERT_from_PktSend(0, comBuff, static_cast<U32>(std::numeric_limits<U32>::max()));
    ASSERT_from_PktSend(1, comBuff, static_cast<U32>(std::numeric_limits<U32>::max()));

    // Pkt 2
    comBuff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,
              comBuff.serializeFrom(static_cast<FwPacketDescriptorType>(Fw::ComPacketType::FW_PACKET_PACKETIZED_TLM)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<FwTlmPacketizeIdType>(8)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(this->m_testTime));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U32>(1)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U64>(2)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U16>(3)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U8>(4)));

    // First Packet 2 Send. Delta Sched Ticks = Max
    ASSERT_from_PktSend(2, comBuff, static_cast<U32>(std::numeric_limits<U32>::max()));

    // Pkt 4
    comBuff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,
              comBuff.serializeFrom(static_cast<FwPacketDescriptorType>(Fw::ComPacketType::FW_PACKET_PACKETIZED_TLM)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<FwTlmPacketizeIdType>(16)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(this->m_testTime));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U32>(1)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U32>(2)));

    // First Packet 4 Send. Delta Sched Ticks = Max
    ASSERT_from_PktSend(3, comBuff, static_cast<U32>(std::numeric_limits<U32>::max()));
    ASSERT_from_PktSend(4, comBuff, static_cast<U32>(std::numeric_limits<U32>::max()));

    this->clearHistory();

    // 2nd Channel (Pkt 1)
    buffer.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buffer.serializeFrom(static_cast<U16>(22)));
    this->invoke_to_TlmRecv(0, 100, time, buffer);

    // 2nd Channel (Pkt 2)
    buffer.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buffer.serializeFrom(static_cast<U64>(22)));
    this->invoke_to_TlmRecv(0, 13, time, buffer);

    // 1st Channel (Pkt 3)
    buffer.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buffer.serializeFrom(static_cast<U32>(11)));
    this->invoke_to_TlmRecv(0, 67, time, buffer);

    // T = 1
    this->invoke_to_Run(0, 0);
    this->component.doDispatch();

    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    ASSERT_from_PktSend_SIZE(1);

    // Packet Location Indices (Checking proper Section, Group)
    ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(0, 1)], 1);
    ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(1, 1)], 1);
    ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(0, 2)], 2);
    ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(1, 3)], 1);
    ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(0, 3)], 1);
    ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(1, 2)], 0);
    // Check that the sections are disabled
    for (FwIndexType section = 2; section < Svc::TelemetrySection::NUM_SECTIONS; section++) {
        for (FwSizeType group = 0; group < Svc::NUM_CONFIGURABLE_TLMPACKETIZER_GROUPS; group++) {
            ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(section, group)], 0);
        }
    }
    // Pkt 3
    comBuff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,
              comBuff.serializeFrom(static_cast<FwPacketDescriptorType>(Fw::ComPacketType::FW_PACKET_PACKETIZED_TLM)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<FwTlmPacketizeIdType>(12)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(this->m_testTime));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U32>(11)));

    // First Packet 3 Send. Delta Sched Ticks = Max
    ASSERT_from_PktSend(0, comBuff, static_cast<U32>(std::numeric_limits<U32>::max()));

    this->clearHistory();

    // T = 2
    this->invoke_to_Run(0, 0);
    this->component.doDispatch();

    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    ASSERT_from_PktSend_SIZE(1);

    // Packet Location Indices (Checking proper Section, Group)
    ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(0, 1)], 1);
    ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(1, 1)], 2);
    ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(0, 2)], 2);
    ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(1, 3)], 1);
    ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(0, 3)], 1);
    ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(1, 2)], 0);
    // Check that the sections are disabled
    for (FwIndexType section = 2; section < Svc::TelemetrySection::NUM_SECTIONS; section++) {
        for (FwSizeType group = 0; group < Svc::NUM_CONFIGURABLE_TLMPACKETIZER_GROUPS; group++) {
            ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(section, group)], 0);
        }
    }
    // Pkt 1
    comBuff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,
              comBuff.serializeFrom(static_cast<FwPacketDescriptorType>(Fw::ComPacketType::FW_PACKET_PACKETIZED_TLM)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<FwTlmPacketizeIdType>(4)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(this->m_testTime));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U32>(1)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U16>(22)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U8>(3)));

    // Pkt 1 on section 1
    // Pkt 1 on section 1 sent recently with a delta of 2
    ASSERT_from_PktSend(0, comBuff, static_cast<U32>(2));

    this->clearHistory();

    // T = 3
    this->invoke_to_Run(0, 0);
    this->component.doDispatch();

    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    ASSERT_from_PktSend_SIZE(1);

    // Packet Location Indices (Checking proper Section, Group)
    ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(0, 1)], 2);
    ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(1, 1)], 2);
    ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(0, 2)], 2);
    ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(1, 3)], 1);
    ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(0, 3)], 1);
    ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(1, 2)], 0);
    // Check that the sections are disabled
    for (FwIndexType section = 2; section < Svc::TelemetrySection::NUM_SECTIONS; section++) {
        for (FwSizeType group = 0; group < Svc::NUM_CONFIGURABLE_TLMPACKETIZER_GROUPS; group++) {
            ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(section, group)], 0);
        }
    }
    // Pkt 1 on section 0
    // comBuff unchanged since this->m_testTime is the same
    // Pkt 1 on section 0 sent recently with a delta of 3
    ASSERT_from_PktSend(0, comBuff, static_cast<U32>(3));

    this->clearHistory();

    // T = 4
    this->invoke_to_Run(0, 0);
    this->component.doDispatch();

    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    ASSERT_from_PktSend_SIZE(1);

    // Packet Location Indices (Checking proper Section, Group)
    ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(0, 1)], 2);
    ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(1, 1)], 2);
    ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(0, 2)], 3);
    ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(1, 3)], 1);
    ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(0, 3)], 1);
    ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(1, 2)], 0);
    // Check that the sections are disabled
    for (FwIndexType section = 2; section < Svc::TelemetrySection::NUM_SECTIONS; section++) {
        for (FwSizeType group = 0; group < Svc::NUM_CONFIGURABLE_TLMPACKETIZER_GROUPS; group++) {
            ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(section, group)], 0);
        }
    }
    // Pkt 2 on Port 0
    comBuff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,
              comBuff.serializeFrom(static_cast<FwPacketDescriptorType>(Fw::ComPacketType::FW_PACKET_PACKETIZED_TLM)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<FwTlmPacketizeIdType>(8)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(this->m_testTime));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U32>(1)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U64>(22)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U16>(3)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U8>(4)));

    // Pkt 2 on section 0 sent recently with a delta of 4
    ASSERT_from_PktSend(0, comBuff, static_cast<U32>(4));

    this->clearHistory();

    // T = 5
    this->invoke_to_Run(0, 0);
    this->component.doDispatch();

    // Not expecting any packets
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    ASSERT_from_PktSend_SIZE(0);

    // T = 6
    this->invoke_to_Run(0, 0);
    this->component.doDispatch();

    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    ASSERT_from_PktSend_SIZE(1);

    // Packet Location Indices (Checking proper Section, Group)
    ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(0, 1)], 2);
    ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(1, 1)], 2);
    ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(0, 2)], 3);
    ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(1, 3)], 1);
    ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(0, 3)], 2);
    ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(1, 2)], 0);
    // Check that the sections are disabled
    for (FwIndexType section = 2; section < Svc::TelemetrySection::NUM_SECTIONS; section++) {
        for (FwSizeType group = 0; group < Svc::NUM_CONFIGURABLE_TLMPACKETIZER_GROUPS; group++) {
            ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(section, group)], 0);
        }
    }
    // Pkt 4 on section 1 (Unchanged since T = 0)
    comBuff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,
              comBuff.serializeFrom(static_cast<FwPacketDescriptorType>(Fw::ComPacketType::FW_PACKET_PACKETIZED_TLM)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<FwTlmPacketizeIdType>(16)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(this->m_testTime));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U32>(1)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U32>(2)));

    // Pkt 4 on section 0 sent recently with a delta of 6
    ASSERT_from_PktSend(0, comBuff, static_cast<U32>(6));

    this->clearHistory();

    // T = 7
    this->invoke_to_Run(0, 0);
    this->component.doDispatch();

    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    ASSERT_from_PktSend_SIZE(1);

    // Packet Location Indices (Checking proper Section, Group)
    ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(0, 1)], 2);
    ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(1, 1)], 2);
    ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(0, 2)], 3);
    ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(1, 3)], 2);
    ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(0, 3)], 2);
    ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(1, 2)], 0);
    // Check that the sections are disabled
    for (FwIndexType section = 2; section < Svc::TelemetrySection::NUM_SECTIONS; section++) {
        for (FwSizeType group = 0; group < Svc::NUM_CONFIGURABLE_TLMPACKETIZER_GROUPS; group++) {
            ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(section, group)], 0);
        }
    }
    // Pkt 4 on section 1 (Unchanged since T = 0)
    // Pkt 4 on section 1 sent recently with a delta of 7
    ASSERT_from_PktSend(0, comBuff, static_cast<U32>(7));

    this->clearHistory();

    // T = 8-11, Expecting No Updates
    for (U8 trial = 8; trial < 12; trial++) {
        this->invoke_to_Run(0, 0);
        this->component.doDispatch();

        ASSERT_FROM_PORT_HISTORY_SIZE(0);
        ASSERT_from_PktSend_SIZE(0);

        // Packet Location Indices (Checking proper Section, Group)
        ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(0, 1)], 2);
        ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(1, 1)], 2);
        ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(0, 2)], 3);
        ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(1, 3)], 2);
        ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(0, 3)], 2);
        ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(1, 2)], 0);

        // Check that the sections are disabled
        for (FwIndexType section = 2; section < Svc::TelemetrySection::NUM_SECTIONS; section++) {
            for (FwSizeType group = 0; group < Svc::NUM_CONFIGURABLE_TLMPACKETIZER_GROUPS; group++) {
                ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(section, group)], 0);
            }
        }
    }

    buffer.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buffer.serializeFrom(static_cast<U32>(111)));
    this->invoke_to_TlmRecv(0, 10, time, buffer);

    this->clearHistory();

    // T = 12
    this->invoke_to_Run(0, 0);
    this->component.doDispatch();

    ASSERT_FROM_PORT_HISTORY_SIZE(5);
    ASSERT_from_PktSend_SIZE(5);

    // Packet Location Indices (Checking proper Section, Group)
    ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(0, 1)], 3);
    ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(1, 1)], 3);
    ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(0, 2)], 4);
    ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(1, 3)], 3);
    ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(0, 3)], 3);
    ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(1, 2)], 0);

    // Check that the sections are disabled
    for (FwIndexType section = 2; section < Svc::TelemetrySection::NUM_SECTIONS; section++) {
        for (FwSizeType group = 0; group < Svc::NUM_CONFIGURABLE_TLMPACKETIZER_GROUPS; group++) {
            ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(section, group)], 0);
        }
    }

    // Pkt 1
    comBuff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,
              comBuff.serializeFrom(static_cast<FwPacketDescriptorType>(Fw::ComPacketType::FW_PACKET_PACKETIZED_TLM)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<FwTlmPacketizeIdType>(4)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(this->m_testTime));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U32>(111)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U16>(22)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U8>(3)));

    // Pkt 1 on section 0 sent recently with a delta of 9
    // Pkt 1 on section 1 sent recently with a delta of 10
    ASSERT_from_PktSend(0, comBuff, static_cast<U32>(9));   // Section 0
    ASSERT_from_PktSend(1, comBuff, static_cast<U32>(10));  // Section 1

    // Pkt 2
    comBuff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,
              comBuff.serializeFrom(static_cast<FwPacketDescriptorType>(Fw::ComPacketType::FW_PACKET_PACKETIZED_TLM)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<FwTlmPacketizeIdType>(8)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(this->m_testTime));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U32>(111)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U64>(22)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U16>(3)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U8>(4)));

    // Pkt 2 on section 0 sent recently with a delta of 8
    ASSERT_from_PktSend(2, comBuff, static_cast<U32>(8));  // Section 0

    // Pkt 4
    comBuff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,
              comBuff.serializeFrom(static_cast<FwPacketDescriptorType>(Fw::ComPacketType::FW_PACKET_PACKETIZED_TLM)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<FwTlmPacketizeIdType>(16)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(this->m_testTime));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U32>(111)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U32>(2)));

    // Pkt 4 on section 0 sent recently with a delta of 6
    // Pkt 4 on section 1 sent recently with a delta of 5
    ASSERT_from_PktSend(3, comBuff, static_cast<U32>(6));  // Section 0
    ASSERT_from_PktSend(4, comBuff, static_cast<U32>(5));  // Section 1
}

//! Configure telemetry enable logic
//!
void TlmPacketizerTester ::advancedControlGroupTests() {
    this->stockConfiguration();  // Will be overridden by test
    this->component.setPacketList(packetList2, ignore, 4);
    Fw::Time time;
    Fw::TlmBuffer buffer;

    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buffer.serializeFrom(static_cast<U32>(1)));
    this->invoke_to_TlmRecv(0, 10, time, buffer);

    this->sendCmd_SET_LEVEL(0, 0, 1);
    this->component.doDispatch();

    this->invoke_to_Run(0, 0);
    this->component.doDispatch();

    // Default ON_CHANGE Behavior
    ASSERT_FROM_PORT_HISTORY_SIZE(Svc::TelemetrySection::NUM_SECTIONS);
    ASSERT_from_PktSend_SIZE(Svc::TelemetrySection::NUM_SECTIONS);
    this->clearHistory();

    // Send a packet every time the port is invoked.
    this->sendCmd_CONFIGURE_GROUP_RATES(0, 0, static_cast<TelemetrySection::T>(0), 1, Svc::RateLogic::EVERY_MAX, 0, 0);
    this->component.doDispatch();

    this->clearHistory();

    // Expect Packet Default Enabled
    this->invoke_to_Run(0, 0);
    this->component.doDispatch();

    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    ASSERT_from_PktSend_SIZE(1);
    this->clearHistory();

    // Disable this group on section 0 (primary)
    this->sendCmd_CONFIGURE_GROUP_SEND(0, 0, static_cast<TelemetrySection::T>(0), 1, Svc::TlmSendState::SEND_NEVER);
    this->component.doDispatch();
    // Expect No Packets
    this->invoke_to_Run(0, 0);
    this->component.doDispatch();
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    ASSERT_from_PktSend_SIZE(0);

    // Group follows the section again, but disable the section
    this->sendCmd_CONFIGURE_GROUP_SEND(0, 0, static_cast<TelemetrySection::T>(0), 1,
                                       Svc::TlmSendState::INHERIT_FROM_PARENT);
    this->component.doDispatch();
    this->sendCmd_ENABLE_SECTION(0, 0, static_cast<TelemetrySection::T>(0), Fw::Enabled::DISABLED);
    this->component.doDispatch();
    // Expect No Packets
    this->invoke_to_Run(0, 0);
    this->component.doDispatch();
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    ASSERT_from_PktSend_SIZE(0);

    // Silence the group
    this->sendCmd_CONFIGURE_GROUP_SEND(0, 0, static_cast<TelemetrySection::T>(0), 1, Svc::TlmSendState::SEND_NEVER);
    this->component.doDispatch();
    // Re-enable settings using the configuration port and the send command
    this->invoke_to_configureSectionGroupRate(0, static_cast<TelemetrySection::T>(0), 1, Svc::RateLogic::EVERY_MAX, 0,
                                              0);
    this->component.doDispatch();
    this->sendCmd_CONFIGURE_GROUP_SEND(0, 0, static_cast<TelemetrySection::T>(0), 1,
                                       Svc::TlmSendState::INHERIT_FROM_PARENT);
    this->component.doDispatch();

    // Enable Section by Port Invocation
    this->sendCmd_ENABLE_SECTION(0, 0, static_cast<TelemetrySection::T>(0), Fw::Enabled::ENABLED);
    this->component.doDispatch();
    this->invoke_to_controlIn(0, static_cast<TelemetrySection::T>(0), Fw::Enabled::ENABLED);
    this->component.doDispatch();
    // Expect A Packet
    this->invoke_to_Run(0, 0);
    this->component.doDispatch();
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    ASSERT_from_PktSend_SIZE(1);
    this->clearHistory();

    // Disable section by port invocation, but the group sends always
    this->invoke_to_controlIn(0, static_cast<TelemetrySection::T>(0), Fw::Enabled::DISABLED);
    this->component.doDispatch();
    this->sendCmd_CONFIGURE_GROUP_SEND(0, 0, static_cast<TelemetrySection::T>(0), 1, Svc::TlmSendState::SEND_ALWAYS);
    this->component.doDispatch();
    // Expect A Packet
    this->invoke_to_Run(0, 0);
    this->component.doDispatch();
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    ASSERT_from_PktSend_SIZE(1);
    this->clearHistory();

    // Group never sends, with the section still disabled
    this->sendCmd_CONFIGURE_GROUP_SEND(0, 0, static_cast<TelemetrySection::T>(0), 1, Svc::TlmSendState::SEND_NEVER);
    this->component.doDispatch();
    // Expect No Packets
    this->invoke_to_Run(0, 0);
    this->component.doDispatch();
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    ASSERT_from_PktSend_SIZE(0);
    this->clearHistory();
}

void TlmPacketizerTester ::sectionEnabledParameterTest() {
    this->stockConfiguration();  // Will be overridden by test

    // First set up a parameter base that sets all sections to DISABLED so that the "loaded" parameter is a non-default
    // state (e.g. not ENABLED).
    Svc::TlmPacketizer_SectionEnabled param;
    for (FwIndexType section = 0; section < TelemetrySection::NUM_SECTIONS; section++) {
        param[section] = Fw::Enabled::DISABLED;  // Disable all sections on parameter load
    }
    // Publish this non-default parameter and then load it via the "loadParameters" function
    this->paramSet_SECTION_ENABLED(param, Fw::ParamValid::VALID);
    this->component.loadParameters();

    // ENABLE section 0 such that we force an update of the parameter and thus cause the telemetry channel used to
    // verify the loaded parameter to be output.
    this->sendCmd_ENABLE_SECTION(0, 0, static_cast<TelemetrySection::T>(0), Fw::Enabled::ENABLED);
    this->component.doDispatch();

    // Update the local truth and check against it
    param[0] = Fw::Enabled::ENABLED;
    ASSERT_TLM_SectionEnabled(0, param);

    // Set the expected parameter to the updated param. This only updates the expected parameter, not the component.
    this->paramSet_SECTION_ENABLED(param, Fw::ParamValid::VALID);

    // Ask the component to send out its version. This call automatically verifies the stored parameter matches the
    // test version!
    this->paramSave_SECTION_ENABLED(0, 0);
}

void TlmPacketizerTester ::sectionConfigParameterTest() {
    this->stockConfiguration();  // Will be overridden by test

    // First set up a parameter base that sets all sections to DISABLED so that the "loaded" parameter is a non-default
    // state (e.g. not ENABLED).
    Svc::TlmPacketizer_SectionConfigs param;
    for (FwIndexType section = 0; section < TelemetrySection::NUM_SECTIONS; section++) {
        for (FwSizeType group = 0; group < Svc::NUM_CONFIGURABLE_TLMPACKETIZER_GROUPS; group++) {
            param[section][group].set_sendState(Svc::TlmSendState::SEND_NEVER);  // Silence all groups on parameter load
        }
    }
    // Publish this non-default parameter and then load it via the "loadParameters" function
    this->paramSet_SECTION_CONFIGS(param, Fw::ParamValid::VALID);
    this->component.loadParameters();

    // Let group 0 on section 0 follow the section again to force an update of the parameter
    this->sendCmd_CONFIGURE_GROUP_SEND(0, 0, static_cast<TelemetrySection::T>(0), 0,
                                       Svc::TlmSendState::INHERIT_FROM_PARENT);
    this->component.doDispatch();

    // Update the local truth and check against it
    param[0][0].set_sendState(Svc::TlmSendState::INHERIT_FROM_PARENT);
    ASSERT_TLM_GroupConfigs(0, param);

    // Set the expected parameter to the updated param. This only updates the expected parameter, not the component.
    this->paramSet_SECTION_CONFIGS(param, Fw::ParamValid::VALID);

    // Ask the component to send out its version. This call automatically verifies the stored parameter matches the
    // test version!
    this->paramSave_SECTION_CONFIGS(0, 0);
}

void TlmPacketizerTester::pushAllChannels(Fw::Time& ts, Fw::TlmBuffer& buff) {
    // channel 10 — packet1, packet2, packet4
    buff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U32>(0xAB)));
    this->invoke_to_TlmRecv(0, 10, ts, buff);

    // channel 100 — packet1
    buff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U16>(0xCD)));
    this->invoke_to_TlmRecv(0, 100, ts, buff);

    // channel 333 — packet1
    buff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U8>(0xEF)));
    this->invoke_to_TlmRecv(0, 333, ts, buff);

    // channel 13 — packet2
    buff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U64>(0x1122334455667788ULL)));
    this->invoke_to_TlmRecv(0, 13, ts, buff);

    // channel 250 — packet2
    buff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U16>(0x5566)));
    this->invoke_to_TlmRecv(0, 250, ts, buff);

    // channel 22 — packet2
    buff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U8>(0x77)));
    this->invoke_to_TlmRecv(0, 22, ts, buff);

    // channel 67 — packet3
    buff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U32>(0x99)));
    this->invoke_to_TlmRecv(0, 67, ts, buff);

    // channel 60 — packet4
    buff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U32>(0xAA)));
    this->invoke_to_TlmRecv(0, 60, ts, buff);
}

void TlmPacketizerTester::setLevelInvalidTest() {
    this->stockConfiguration();
    // packetList2 contains four packets at levels 1, 2, 2, 3
    // (packet1 id=4, packet2 id=8, packet3 id=12, packet4 id=16)
    this->component.setPacketList(packetList2, IGNORE_OMIT_LIST, 3);

    Fw::Time ts;
    Fw::TlmBuffer buff;

    // ----------------------------------------------------------------
    // Step 1: Establish a known restricted state via SET_LEVEL(1).
    //         Only groups 0 and 1 become enabled; groups 2 and 3 are
    //         disabled.  With the four packets at levels 1, 2, 2, 3,
    //         only packet1 (level 1, group 1) qualifies to send.
    // ----------------------------------------------------------------
    this->sendCmd_SET_LEVEL(0, 1, 1);
    this->component.doDispatch();

    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, TlmPacketizerComponentBase::OPCODE_SET_LEVEL, 1, Fw::CmdResponse::OK);
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_LevelSet_SIZE(1);
    ASSERT_EVENTS_LevelSet(0, static_cast<FwChanIdType>(1));
    this->clearHistory();

    // ----------------------------------------------------------------
    // Step 2 & 3: Push data, run, confirm only the group-1 packet
    //             (packet1, replicated once per enabled section) is
    //             sent.  All other group (2 and 3) packets are silent.
    // ----------------------------------------------------------------
    this->pushAllChannels(ts, buff);
    this->invoke_to_Run(0, 0);
    this->component.doDispatch();

    // Only packet1 at level 1 should be emitted — one copy per section.
    ASSERT_from_PktSend_SIZE(1 * Svc::TelemetrySection::NUM_SECTIONS);
    this->clearHistory();

    // ----------------------------------------------------------------
    // Step 4: Issue SET_LEVEL with an out-of-range value.
    //         MAX_CONFIGURABLE_TLMPACKETIZER_GROUP + 1 is guaranteed
    //         to exceed the valid ceiling.
    // ----------------------------------------------------------------
    const FwChanIdType invalidLevel = static_cast<FwChanIdType>(MAX_CONFIGURABLE_TLMPACKETIZER_GROUP) + 1u;

    this->sendCmd_SET_LEVEL(0, 2, invalidLevel);
    this->component.doDispatch();

    // ----------------------------------------------------------------
    // Step 5: Command must respond with VALIDATION_ERROR, not OK.
    // ----------------------------------------------------------------
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, TlmPacketizerComponentBase::OPCODE_SET_LEVEL, 2, Fw::CmdResponse::VALIDATION_ERROR);

    // ----------------------------------------------------------------
    // Step 6: MaxLevelExceed warning event must be fired.
    // ----------------------------------------------------------------
    ASSERT_EVENTS_MaxLevelExceed_SIZE(1);
    ASSERT_EVENTS_MaxLevelExceed(0, invalidLevel, static_cast<FwChanIdType>(MAX_CONFIGURABLE_TLMPACKETIZER_GROUP));

    // ----------------------------------------------------------------
    // Step 7: LevelSet activity event must NOT be fired — the handler
    //         returned early before the success path.
    // ----------------------------------------------------------------
    ASSERT_EVENTS_LevelSet_SIZE(0);

    this->clearHistory();

    // ----------------------------------------------------------------
    // Steps 8 & 9: Push data again and run.
    //
    //   Fixed code  → group enable state is UNCHANGED from SET_LEVEL(1).
    //                 Only packet1 (group 1) is sent.
    //                 Expected port-send count = 1 * NUM_SECTIONS.
    //
    //   Buggy code  → the invalid SET_LEVEL enabled ALL groups before
    //                 returning, so packets 1–4 would ALL be sent.
    //                 Expected port-send count = 4 * NUM_SECTIONS.
    //
    // The assertion below would FAIL against the old buggy implementation.
    // ----------------------------------------------------------------
    this->pushAllChannels(ts, buff);
    this->invoke_to_Run(0, 0);
    this->component.doDispatch();

    // Only the group-1 packet should be sent — exactly as before the
    // bad SET_LEVEL call.  Any additional packets prove the bug is
    // still present.
    ASSERT_from_PktSend_SIZE(1 * Svc::TelemetrySection::NUM_SECTIONS);
}

// A channel ID may appear in more than one packet as long as the definition
// (serialized size) is identical.  Channel 10 (size 4) is shared by both
// packets below, matching the semantics that identical duplicates are allowed.
namespace {
TlmPacketizerChannelEntry dupMatchPacketAList[] = {{10, 4}, {100, 2}};
TlmPacketizerChannelEntry dupMatchPacketBList[] = {{10, 4}, {200, 2}};
TlmPacketizerPacket dupMatchPacketA = {dupMatchPacketAList, 4, 1, FW_NUM_ARRAY_ELEMENTS(dupMatchPacketAList)};
TlmPacketizerPacket dupMatchPacketB = {dupMatchPacketBList, 8, 1, FW_NUM_ARRAY_ELEMENTS(dupMatchPacketBList)};
TlmPacketizerPacketList dupMatchPacketList = {{&dupMatchPacketA, &dupMatchPacketB}, 2};

// Channel 10 is declared with size 4 in packet A and size 6 in packet B. This
// conflicting definition would corrupt the packet offsets and overflow the
// fill buffer, so setPacketList must assert.
TlmPacketizerChannelEntry dupConflictPacketAList[] = {{10, 4}, {100, 2}};
TlmPacketizerChannelEntry dupConflictPacketBList[] = {{10, 6}, {200, 2}};
TlmPacketizerPacket dupConflictPacketA = {dupConflictPacketAList, 4, 1, FW_NUM_ARRAY_ELEMENTS(dupConflictPacketAList)};
TlmPacketizerPacket dupConflictPacketB = {dupConflictPacketBList, 8, 1, FW_NUM_ARRAY_ELEMENTS(dupConflictPacketBList)};
TlmPacketizerPacketList dupConflictPacketList = {{&dupConflictPacketA, &dupConflictPacketB}, 2};

// A packet with no channels is autocoded by FPP with a nullptr channel list and zero entries.
constexpr TlmPacketizerPacket emptyPacket = {nullptr, 20, 1, 0};
TlmPacketizerPacketList emptyPacketList = {{&packet1, &emptyPacket}, 2};
// A nullptr channel list with a non-zero entry count is still a configuration error.
constexpr TlmPacketizerPacket nullListPacket = {nullptr, 21, 1, 1};
TlmPacketizerPacketList nullListPacketList = {{&packet1, &nullListPacket}, 2};
}  // namespace

void TlmPacketizerTester::duplicateChannelIdMatchingSizeTest() {
    this->stockConfiguration();
    // Identical channel definition across packets is permitted and must not assert.
    this->component.setPacketList(dupMatchPacketList, IGNORE_OMIT_LIST, 1);

    Fw::Time ts;
    Fw::TlmBuffer buff;

    // Push the shared channel and confirm it lands in both packets without overflow.
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U32>(20)));
    this->invoke_to_TlmRecv(0, 10, ts, buff);
    buff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U16>(7)));
    this->invoke_to_TlmRecv(0, 200, ts, buff);

    this->invoke_to_Run(0, 0);
    this->component.doDispatch();

    // Both packets carry channel 10, so both are emitted once per section.
    ASSERT_from_PktSend_SIZE(2 * Svc::TelemetrySection::NUM_SECTIONS);

    // The first packet carries the shared channel at its own offset; channel 100 was never written and stays zero
    Fw::ComBuffer comBuffA;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,
              comBuffA.serializeFrom(static_cast<FwPacketDescriptorType>(Fw::ComPacketType::FW_PACKET_PACKETIZED_TLM)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuffA.serializeFrom(static_cast<FwTlmPacketizeIdType>(4)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuffA.serializeFrom(ts));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuffA.serializeFrom(static_cast<U32>(20)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuffA.serializeFrom(static_cast<U16>(0)));
    ASSERT_from_PktSend(0, comBuffA, static_cast<U32>(std::numeric_limits<U32>::max()));

    // The shared channel must land at its own offset in the second packet, not just the first
    Fw::ComBuffer comBuff;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,
              comBuff.serializeFrom(static_cast<FwPacketDescriptorType>(Fw::ComPacketType::FW_PACKET_PACKETIZED_TLM)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<FwTlmPacketizeIdType>(8)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(ts));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U32>(20)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U16>(7)));
    ASSERT_from_PktSend(1 * Svc::TelemetrySection::NUM_SECTIONS, comBuff,
                        static_cast<U32>(std::numeric_limits<U32>::max()));
}

void TlmPacketizerTester::channelInMaxPacketsTest() {
    this->stockConfiguration();
    // One channel present in every one of MAX_PACKETIZER_PACKETS packets sets every packetOffset slot of its entry
    TlmPacketizerChannelEntry sharedList[] = {{10, 4}};
    TlmPacketizerPacket packets[MAX_PACKETIZER_PACKETS]{};
    TlmPacketizerPacketList maxPacketList{};
    for (FwChanIdType i = 0; i < MAX_PACKETIZER_PACKETS; i++) {
        packets[i] = {sharedList, static_cast<FwTlmPacketizeIdType>(100 + i), 1, FW_NUM_ARRAY_ELEMENTS(sharedList)};
        maxPacketList.list[i] = &packets[i];
    }
    maxPacketList.numEntries = MAX_PACKETIZER_PACKETS;
    this->component.setPacketList(maxPacketList, IGNORE_OMIT_LIST, 1);

    Fw::Time ts(11, 22);
    Fw::TlmBuffer buff;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U32>(20)));
    this->invoke_to_TlmRecv(0, 10, ts, buff);

    this->invoke_to_Run(0, 0);
    this->component.doDispatch();

    // Every packet was updated by the single channel write and is emitted once per section
    static_assert(MAX_PACKETIZER_PACKETS * Svc::TelemetrySection::NUM_SECTIONS <= MAX_HISTORY_SIZE,
                  "channelInMaxPacketsTest needs MAX_HISTORY_SIZE >= MAX_PACKETIZER_PACKETS * NUM_SECTIONS");
    ASSERT_from_PktSend_SIZE(MAX_PACKETIZER_PACKETS * Svc::TelemetrySection::NUM_SECTIONS);
    for (FwChanIdType i = 0; i < MAX_PACKETIZER_PACKETS; i++) {
        Fw::ComBuffer comBuff;
        ASSERT_EQ(
            Fw::FW_SERIALIZE_OK,
            comBuff.serializeFrom(static_cast<FwPacketDescriptorType>(Fw::ComPacketType::FW_PACKET_PACKETIZED_TLM)));
        ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<FwTlmPacketizeIdType>(100 + i)));
        ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(ts));
        ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(static_cast<U32>(20)));
        ASSERT_from_PktSend(i * Svc::TelemetrySection::NUM_SECTIONS, comBuff,
                            static_cast<U32>(std::numeric_limits<U32>::max()));
    }

    // TlmGet reads the value back from the first packet containing the channel
    Fw::Time time;
    Fw::TlmBuffer val;
    ASSERT_EQ(Fw::TlmValid::VALID, this->invoke_to_TlmGet(0, 10, time, val));
    ASSERT_EQ(time, ts);
    U32 readBack = 0;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, val.deserializeTo(readBack));
    ASSERT_EQ(readBack, 20U);
}

void TlmPacketizerTester::fullComBufferPacketTest() {
    this->stockConfiguration();
    constexpr FwSizeType HEADER =
        sizeof(FwPacketDescriptorType) + Fw::Time::SERIALIZED_SIZE + sizeof(FwTlmPacketizeIdType);
    constexpr FwSizeType LAST_OFFSET = FW_COM_BUFFER_MAX_SIZE - 1;
    // Two channels fill the packet to exactly FW_COM_BUFFER_MAX_SIZE; channel 11 sits at the last valid offset
    TlmPacketizerChannelEntry fullList[] = {{10, LAST_OFFSET - HEADER}, {11, 1}};
    TlmPacketizerPacket fullPacket = {fullList, 30, 1, FW_NUM_ARRAY_ELEMENTS(fullList)};
    TlmPacketizerPacketList fullPacketList = {{&fullPacket}, 1};
    this->component.setPacketList(fullPacketList, IGNORE_OMIT_LIST, 1);
    ASSERT_EQ(this->component.m_channels[1].packetOffset[0], LAST_OFFSET);
    // local copy: ASSERT_NE binds its arguments by reference, which would ODR-use the static constexpr member
    constexpr FwTlmPacketizeOffsetType notInPacket = TlmPacketizer::NOT_IN_PACKET;
    ASSERT_NE(this->component.m_channels[1].packetOffset[0], notInPacket);

    Fw::Time ts(5, 6);
    Fw::TlmBuffer buff;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U8>(0xA5)));
    this->invoke_to_TlmRecv(0, 11, ts, buff);

    Fw::Time time;
    Fw::TlmBuffer val;
    ASSERT_EQ(Fw::TlmValid::VALID, this->invoke_to_TlmGet(0, 11, time, val));
    ASSERT_EQ(time, ts);
    U8 readBack = 0;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, val.deserializeTo(readBack));
    ASSERT_EQ(readBack, 0xA5);

    this->invoke_to_Run(0, 0);
    this->component.doDispatch();
    ASSERT_from_PktSend_SIZE(Svc::TelemetrySection::NUM_SECTIONS);
    const Fw::ComBuffer& sent = this->fromPortHistory_PktSend->at(0).data;
    ASSERT_EQ(sent.getSize(), FW_COM_BUFFER_MAX_SIZE);
    ASSERT_EQ(sent.getBuffAddr()[LAST_OFFSET], 0xA5);
}

void TlmPacketizerTester::oversizedPacketTest() {
    this->stockConfiguration();
    constexpr FwSizeType HEADER =
        sizeof(FwPacketDescriptorType) + Fw::Time::SERIALIZED_SIZE + sizeof(FwTlmPacketizeIdType);
    // One byte more than the com buffer can hold must trip the configuration-time assert
    TlmPacketizerChannelEntry overList[] = {{10, FW_COM_BUFFER_MAX_SIZE - HEADER}, {11, 1}};
    TlmPacketizerPacket overPacket = {overList, 31, 1, FW_NUM_ARRAY_ELEMENTS(overList)};
    TlmPacketizerPacketList overPacketList = {{&overPacket}, 1};
    // Match the bound check's own arguments (packetLen, pktEntry) so a later assert in the same file cannot satisfy it
    ASSERT_DEATH_IF_SUPPORTED(this->component.setPacketList(overPacketList, IGNORE_OMIT_LIST, 1),
                              "TlmPacketizer\\.cpp:[0-9]+\" " + std::to_string(FW_COM_BUFFER_MAX_SIZE + 1) + " 0");
}

void TlmPacketizerTester::duplicateChannelIdConflictingSizeTest() {
    this->stockConfiguration();
    // Conflicting size for the same channel ID must trip the configuration-time assert.
    ASSERT_DEATH_IF_SUPPORTED(this->component.setPacketList(dupConflictPacketList, IGNORE_OMIT_LIST, 1),
                              "TlmPacketizer.cpp");
}

void TlmPacketizerTester::emptyPacketTest() {
    this->stockConfiguration();
    // An empty packet specification must be accepted without asserting.
    this->component.setPacketList(emptyPacketList, IGNORE_OMIT_LIST, 1);

    Fw::Time ts;
    Fw::TlmBuffer buff;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U32>(20)));
    this->invoke_to_TlmRecv(0, 10, ts, buff);

    // Only the populated packet is emitted on Run; the empty packet is never marked updated by channel traffic.
    this->invoke_to_Run(0, 0);
    this->component.doDispatch();
    ASSERT_from_PktSend_SIZE(1 * Svc::TelemetrySection::NUM_SECTIONS);
    this->clearHistory();

    // The empty packet can be requested explicitly and is emitted as a header-only packet.
    this->setTestTime(this->m_testTime);
    this->sendCmd_SEND_PKT(0, 12, 20, static_cast<TelemetrySection::T>(0));
    this->component.doDispatch();
    ASSERT_EVENTS_PacketSent_SIZE(1);
    ASSERT_EVENTS_PacketSent(0, 20);
    ASSERT_CMD_RESPONSE(0, TlmPacketizerComponentBase::OPCODE_SEND_PKT, 12, Fw::CmdResponse::OK);

    this->invoke_to_Run(0, 0);
    this->component.doDispatch();
    ASSERT_from_PktSend_SIZE(1 * Svc::TelemetrySection::NUM_SECTIONS);

    Fw::ComBuffer expected;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,
              expected.serializeFrom(static_cast<FwPacketDescriptorType>(Fw::ComPacketType::FW_PACKET_PACKETIZED_TLM)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, expected.serializeFrom(static_cast<FwTlmPacketizeIdType>(20)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, expected.serializeFrom(this->m_testTime));
    for (FwIndexType section = 0; section < Svc::TelemetrySection::NUM_SECTIONS; section++) {
        ASSERT_EQ(this->fromPortHistory_PktSend->at(static_cast<U32>(section)).data, expected);
    }
}

void TlmPacketizerTester::nullChannelListTest() {
    this->stockConfiguration();
    ASSERT_DEATH_IF_SUPPORTED(this->component.setPacketList(nullListPacketList, IGNORE_OMIT_LIST, 1),
                              "TlmPacketizer.cpp");
}

void TlmPacketizerTester::oversizedChannelTest() {
    this->stockConfiguration();
    this->component.setPacketList(packetList, ignore, 2);
    Fw::Time ts;
    Fw::TlmBuffer buff;

    // Channel 100 is configured with size 2; send an 8-byte value
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U64>(0xDEADBEEF12345678ULL)));
    this->invoke_to_TlmRecv(0, 100, ts, buff);

    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_OversizedChannel_SIZE(1);
    ASSERT_EVENTS_OversizedChannel(0, 100, sizeof(U64), 2);

    // A correctly-sized value on the same channel is still accepted
    this->clearEvents();
    buff.resetSer();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<U16>(15)));
    this->invoke_to_TlmRecv(0, 100, ts, buff);
    ASSERT_EVENTS_SIZE(0);
}

// ----------------------------------------------------------------------
// Handlers for typed from ports
// ----------------------------------------------------------------------

void TlmPacketizerTester ::from_PktSend_handler(const FwIndexType portNum, Fw::ComBuffer& data, U32 context) {
    this->m_portOutInvokes[portNum]++;
    this->pushFromPortEntry_PktSend(data, context);
}

void TlmPacketizerTester ::from_pingOut_handler(const FwIndexType portNum, U32 key) {
    this->pushFromPortEntry_pingOut(key);
}

void TlmPacketizerTester ::from_packetOverrideOut_handler(FwIndexType portNum, Fw::Buffer& fwBuffer) {
    this->m_configOutInvokes++;
    (void)fwBuffer.getDeserializer().deserializeTo(this->m_lastConfigEntry);
}

Fw::Buffer TlmPacketizerTester ::from_allocate_handler(FwIndexType portNum, FwSizeType size) {
    // Stand in for a BufferManager; an exhausted pool hands back an invalid (empty) buffer
    if (this->m_allocFail) {
        return Fw::Buffer();
    }
    const FwSizeType n = (size <= CONFIG_BUF_SLOT_SIZE) ? size : CONFIG_BUF_SLOT_SIZE;
    this->m_configBuf.set(this->m_configBufSlot, static_cast<Fw::Buffer::SizeType>(n));
    return this->m_configBuf;
}

// ----------------------------------------------------------------------
// Helper methods
// ----------------------------------------------------------------------

// Populate the six channels that make up packets id 4 and id 8 so a subsequent Run has data.
void TlmPacketizerTester ::fillPacketChannels() {
    Fw::Time ts;
    Fw::TlmBuffer buff;
    buff.resetSer();
    (void)buff.serializeFrom(static_cast<U32>(20));
    this->invoke_to_TlmRecv(0, 10, ts, buff);
    buff.resetSer();
    (void)buff.serializeFrom(static_cast<U16>(15));
    this->invoke_to_TlmRecv(0, 100, ts, buff);
    buff.resetSer();
    (void)buff.serializeFrom(static_cast<U8>(14));
    this->invoke_to_TlmRecv(0, 333, ts, buff);
    buff.resetSer();
    (void)buff.serializeFrom(static_cast<U64>(1000000));
    this->invoke_to_TlmRecv(0, 13, ts, buff);
    buff.resetSer();
    (void)buff.serializeFrom(static_cast<U16>(1010));
    this->invoke_to_TlmRecv(0, 250, ts, buff);
    buff.resetSer();
    (void)buff.serializeFrom(static_cast<U8>(15));
    this->invoke_to_TlmRecv(0, 22, ts, buff);
}

// Fill both packets, run one cycle, and return how many packets left on the REALTIME level-1 port
FwSizeType TlmPacketizerTester ::runAndCountRealtime() {
    this->clearHistory();
    this->fillPacketChannels();
    this->setTestTime(this->m_testTime);
    this->resetCounter();
    this->invoke_to_Run(0, 0);
    this->component.doDispatch();
    return this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(Svc::TelemetrySection::REALTIME, 1)];
}

Svc::PacketOverrideConfig TlmPacketizerTester ::sendOverride(Svc::PacketOverrideType::T type,
                                                             Svc::TlmSendState::T sendState,
                                                             const Svc::RateConfig& rates) {
    return Svc::PacketOverrideConfig(Svc::PacketOverrideType(type),
                                     Svc::TlmSendConfig(Svc::TlmSendState(sendState), rates));
}

void TlmPacketizerTester ::perPacketOverrideTest() {
    this->stockConfiguration();  // all sections/groups enabled
    this->component.setPacketList(packetList, ignore, 2);

    // Override: never send packet id 4 in REALTIME via the per-packet command. Its RECORDED copy and
    // packet id 8 remain group-driven, so exactly one (packet, section) send should drop out.
    this->sendCmd_OVERRIDE_PACKET_SEND(0, 0, Svc::TelemetrySection::REALTIME, 4, Svc::TlmSendState::SEND_NEVER);
    this->component.doDispatch();

    this->fillPacketChannels();
    this->setTestTime(this->m_testTime);
    this->resetCounter();
    this->invoke_to_Run(0, 0);
    this->component.doDispatch();

    // Baseline (all enabled) is 2 packets x NUM_SECTIONS; the override removes exactly one.
    ASSERT_from_PktSend_SIZE(2 * Svc::TelemetrySection::NUM_SECTIONS - 1);
    ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(Svc::TelemetrySection::REALTIME, 1)], 1u);
    ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(Svc::TelemetrySection::RECORDED, 1)], 2u);
}

void TlmPacketizerTester ::perPacketCommandsTest() {
    this->stockConfiguration();
    this->component.setPacketList(packetList, ignore, 2);
    const Svc::RateConfig noRates;
    const Svc::RateConfig rates(Svc::RateLogic::EVERY_MAX, 1, 2);

    // OVERRIDE_PACKET_SEND stores a send-only override and mirrors it
    this->m_configOutInvokes = 0;
    this->sendCmd_OVERRIDE_PACKET_SEND(0, 10, Svc::TelemetrySection::REALTIME, 4, Svc::TlmSendState::SEND_NEVER);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, TlmPacketizerComponentBase::OPCODE_OVERRIDE_PACKET_SEND, 10, Fw::CmdResponse::OK);
    const Svc::PacketOverrideConfig sendOnly =
        this->sendOverride(Svc::PacketOverrideType::OVERRIDE_SEND, Svc::TlmSendState::SEND_NEVER, noRates);
    ASSERT_EVENTS_PacketOverrideSet_SIZE(1);
    ASSERT_EVENTS_PacketOverrideSet(0, Svc::TelemetrySection::REALTIME, 4, sendOnly);
    ASSERT_EQ(this->component.m_packetOverrides[0][0], sendOnly);
    ASSERT_EQ(this->m_configOutInvokes, 1u);
    ASSERT_EQ(this->m_lastConfigEntry.get_packetId(), 4u);
    ASSERT_EQ(this->m_lastConfigEntry.get_section(), Svc::TelemetrySection::REALTIME);
    ASSERT_EQ(this->m_lastConfigEntry.get_override(), sendOnly);
    this->clearHistory();

    // OVERRIDE_PACKET_RATES on the same packet keeps the send override: both fields now override
    this->sendCmd_OVERRIDE_PACKET_RATES(0, 11, Svc::TelemetrySection::REALTIME, 4, Svc::RateLogic::EVERY_MAX, 1, 2);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE(0, TlmPacketizerComponentBase::OPCODE_OVERRIDE_PACKET_RATES, 11, Fw::CmdResponse::OK);
    const Svc::PacketOverrideConfig both =
        this->sendOverride(Svc::PacketOverrideType::OVERRIDE_SEND_AND_RATES, Svc::TlmSendState::SEND_NEVER, rates);
    ASSERT_EVENTS_PacketOverrideSet(0, Svc::TelemetrySection::REALTIME, 4, both);
    ASSERT_EQ(this->component.m_packetOverrides[0][0], both);
    ASSERT_EQ(this->m_configOutInvokes, 2u);
    ASSERT_EQ(this->m_lastConfigEntry.get_override(), both);
    this->clearHistory();

    // Rates first on another packet/section: a rates-only override
    this->sendCmd_OVERRIDE_PACKET_RATES(0, 12, Svc::TelemetrySection::RECORDED, 8, Svc::RateLogic::EVERY_MAX, 1, 2);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE(0, TlmPacketizerComponentBase::OPCODE_OVERRIDE_PACKET_RATES, 12, Fw::CmdResponse::OK);
    ASSERT_EQ(
        this->component.m_packetOverrides[1][1],
        this->sendOverride(Svc::PacketOverrideType::OVERRIDE_RATES, Svc::TlmSendState::INHERIT_FROM_PARENT, rates));
    this->clearHistory();

    // Invalid section: rejected with InvalidPacketConfig, nothing stored or mirrored
    this->m_configOutInvokes = 0;
    this->sendCmd_OVERRIDE_PACKET_SEND(0, 13,
                                       static_cast<Svc::TelemetrySection::T>(Svc::TelemetrySection::NUM_SECTIONS), 8,
                                       Svc::TlmSendState::SEND_NEVER);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE(0, TlmPacketizerComponentBase::OPCODE_OVERRIDE_PACKET_SEND, 13,
                        Fw::CmdResponse::VALIDATION_ERROR);
    ASSERT_EVENTS_InvalidPacketConfig_SIZE(1);
    ASSERT_EVENTS_InvalidPacketConfig(0, 8);
    ASSERT_EVENTS_PacketOverrideSet_SIZE(0);
    this->clearHistory();

    // Unknown packet id: rejected with PacketNotFound
    this->sendCmd_OVERRIDE_PACKET_SEND(0, 14, Svc::TelemetrySection::REALTIME, 9999, Svc::TlmSendState::SEND_NEVER);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE(0, TlmPacketizerComponentBase::OPCODE_OVERRIDE_PACKET_SEND, 14,
                        Fw::CmdResponse::VALIDATION_ERROR);
    ASSERT_EVENTS_PacketNotFound_SIZE(1);
    ASSERT_EVENTS_PacketNotFound(0, 9999);
    this->clearHistory();
}

void TlmPacketizerTester ::getPacketConfigTest() {
    this->stockConfiguration();
    this->component.setPacketList(packetList, ignore, 2);
    const Svc::TlmSendConfig group = this->component.m_groupConfigs[0][1];  // packet id 4 is level 1

    // No override: GROUP_DEFAULT and the effective config is the group's
    this->sendCmd_GET_PACKET_CONFIG(0, 20, Svc::TelemetrySection::REALTIME, 4);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE(0, TlmPacketizerComponentBase::OPCODE_GET_PACKET_CONFIG, 20, Fw::CmdResponse::OK);
    ASSERT_EVENTS_PacketConfigReport_SIZE(1);
    ASSERT_EVENTS_PacketConfigReport(0, Svc::TelemetrySection::REALTIME, 4, Svc::PacketOverrideConfig(), group);
    this->clearHistory();

    // Rates override: effective keeps the group's send state and takes the override's rates
    const Svc::RateConfig rates(Svc::RateLogic::EVERY_MAX, 1, 2);
    this->sendCmd_OVERRIDE_PACKET_RATES(0, 21, Svc::TelemetrySection::REALTIME, 4, Svc::RateLogic::EVERY_MAX, 1, 2);
    this->component.doDispatch();
    this->clearHistory();
    this->sendCmd_GET_PACKET_CONFIG(0, 22, Svc::TelemetrySection::REALTIME, 4);
    this->component.doDispatch();
    ASSERT_EVENTS_PacketConfigReport(
        0, Svc::TelemetrySection::REALTIME, 4,
        this->sendOverride(Svc::PacketOverrideType::OVERRIDE_RATES, Svc::TlmSendState::INHERIT_FROM_PARENT, rates),
        Svc::TlmSendConfig(group.get_sendState(), rates));
    this->clearHistory();

    // Invalid section / unknown packet are rejected like the other per-packet commands
    this->sendCmd_GET_PACKET_CONFIG(0, 23, static_cast<Svc::TelemetrySection::T>(Svc::TelemetrySection::NUM_SECTIONS),
                                    4);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE(0, TlmPacketizerComponentBase::OPCODE_GET_PACKET_CONFIG, 23, Fw::CmdResponse::VALIDATION_ERROR);
    ASSERT_EVENTS_InvalidPacketConfig_SIZE(1);
    this->sendCmd_GET_PACKET_CONFIG(0, 24, Svc::TelemetrySection::REALTIME, 9999);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE(1, TlmPacketizerComponentBase::OPCODE_GET_PACKET_CONFIG, 24, Fw::CmdResponse::VALIDATION_ERROR);
    ASSERT_EVENTS_PacketNotFound_SIZE(1);
    ASSERT_EVENTS_PacketConfigReport_SIZE(0);
}

void TlmPacketizerTester ::groupRatesFollowThroughSendOverrideTest() {
    this->stockConfiguration();
    this->component.setPacketList(packetList, ignore, 2);
    // A send-only override must not freeze the rates: later group rate changes still reach the packet
    this->sendCmd_OVERRIDE_PACKET_SEND(0, 0, Svc::TelemetrySection::REALTIME, 4, Svc::TlmSendState::SEND_ALWAYS);
    this->component.doDispatch();
    this->sendCmd_CONFIGURE_GROUP_RATES(0, 0, Svc::TelemetrySection::REALTIME, 1, Svc::RateLogic::EVERY_MAX, 3, 7);
    this->component.doDispatch();
    this->clearHistory();
    this->sendCmd_GET_PACKET_CONFIG(0, 0, Svc::TelemetrySection::REALTIME, 4);
    this->component.doDispatch();
    ASSERT_EVENTS_PacketConfigReport(
        0, Svc::TelemetrySection::REALTIME, 4,
        this->sendOverride(Svc::PacketOverrideType::OVERRIDE_SEND, Svc::TlmSendState::SEND_ALWAYS, Svc::RateConfig()),
        Svc::TlmSendConfig(Svc::TlmSendState::SEND_ALWAYS, Svc::RateConfig(Svc::RateLogic::EVERY_MAX, 3, 7)));
}

void TlmPacketizerTester ::sendStateResolutionTest() {
    this->stockConfiguration();  // sections enabled, groups INHERIT_FROM_PARENT
    this->component.setPacketList(packetList, ignore, 2);
    // Silence packet id 8 so the REALTIME level-1 port counts packet id 4 alone
    this->sendCmd_OVERRIDE_PACKET_SEND(0, 0, Svc::TelemetrySection::REALTIME, 8, Svc::TlmSendState::SEND_NEVER);
    this->component.doDispatch();
    ASSERT_EQ(this->runAndCountRealtime(), 1u);  // packet INHERIT -> group INHERIT -> section enabled

    // Section disabled: inheritance chain ends at the section
    this->sendCmd_ENABLE_SECTION(0, 0, Svc::TelemetrySection::REALTIME, Fw::Enabled::DISABLED);
    this->component.doDispatch();
    ASSERT_EQ(this->runAndCountRealtime(), 0u);
    // Packet SEND_WHEN_SECTION_ENABLED still respects the section
    this->sendCmd_OVERRIDE_PACKET_SEND(0, 0, Svc::TelemetrySection::REALTIME, 4,
                                       Svc::TlmSendState::SEND_WHEN_SECTION_ENABLED);
    this->component.doDispatch();
    ASSERT_EQ(this->runAndCountRealtime(), 0u);
    // Packet SEND_ALWAYS ignores the section
    this->sendCmd_OVERRIDE_PACKET_SEND(0, 0, Svc::TelemetrySection::REALTIME, 4, Svc::TlmSendState::SEND_ALWAYS);
    this->component.doDispatch();
    ASSERT_EQ(this->runAndCountRealtime(), 1u);
    // Packet INHERIT -> group SEND_ALWAYS ignores the section
    this->sendCmd_OVERRIDE_PACKET_SEND(0, 0, Svc::TelemetrySection::REALTIME, 4,
                                       Svc::TlmSendState::INHERIT_FROM_PARENT);
    this->component.doDispatch();
    this->sendCmd_CONFIGURE_GROUP_SEND(0, 0, Svc::TelemetrySection::REALTIME, 1, Svc::TlmSendState::SEND_ALWAYS);
    this->component.doDispatch();
    ASSERT_EQ(this->runAndCountRealtime(), 1u);
    // Packet SEND_NEVER beats a group that sends always
    this->sendCmd_OVERRIDE_PACKET_SEND(0, 0, Svc::TelemetrySection::REALTIME, 4, Svc::TlmSendState::SEND_NEVER);
    this->component.doDispatch();
    ASSERT_EQ(this->runAndCountRealtime(), 0u);

    // Section enabled again, group SEND_NEVER: inheriting packet is silent, an enabled override is not
    this->sendCmd_ENABLE_SECTION(0, 0, Svc::TelemetrySection::REALTIME, Fw::Enabled::ENABLED);
    this->component.doDispatch();
    this->sendCmd_CONFIGURE_GROUP_SEND(0, 0, Svc::TelemetrySection::REALTIME, 1, Svc::TlmSendState::SEND_NEVER);
    this->component.doDispatch();
    this->sendCmd_CLEAR_PACKET_OVERRIDE(0, 0, Svc::TelemetrySection::REALTIME, 4);
    this->component.doDispatch();
    ASSERT_EQ(this->runAndCountRealtime(), 0u);
    this->sendCmd_OVERRIDE_PACKET_SEND(0, 0, Svc::TelemetrySection::REALTIME, 4,
                                       Svc::TlmSendState::SEND_WHEN_SECTION_ENABLED);
    this->component.doDispatch();
    ASSERT_EQ(this->runAndCountRealtime(), 1u);
    // Group SEND_WHEN_SECTION_ENABLED behaves like INHERIT at group level
    this->sendCmd_CLEAR_PACKET_OVERRIDE(0, 0, Svc::TelemetrySection::REALTIME, 4);
    this->component.doDispatch();
    this->sendCmd_CONFIGURE_GROUP_SEND(0, 0, Svc::TelemetrySection::REALTIME, 1,
                                       Svc::TlmSendState::SEND_WHEN_SECTION_ENABLED);
    this->component.doDispatch();
    ASSERT_EQ(this->runAndCountRealtime(), 1u);
}

void TlmPacketizerTester ::clearPacketOverrideTest() {
    this->stockConfiguration();
    this->component.setPacketList(packetList, ignore, 2);
    this->sendCmd_OVERRIDE_PACKET_SEND(0, 0, Svc::TelemetrySection::REALTIME, 4, Svc::TlmSendState::SEND_NEVER);
    this->component.doDispatch();
    ASSERT_from_PktSend_SIZE(0);
    this->fillPacketChannels();
    this->setTestTime(this->m_testTime);
    this->resetCounter();
    this->invoke_to_Run(0, 0);
    this->component.doDispatch();
    ASSERT_from_PktSend_SIZE(2 * Svc::TelemetrySection::NUM_SECTIONS - 1);
    this->clearHistory();

    // Clearing reverts to the group and mirrors a GROUP_DEFAULT record
    this->m_configOutInvokes = 0;
    this->sendCmd_CLEAR_PACKET_OVERRIDE(0, 30, Svc::TelemetrySection::REALTIME, 4);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE(0, TlmPacketizerComponentBase::OPCODE_CLEAR_PACKET_OVERRIDE, 30, Fw::CmdResponse::OK);
    ASSERT_EVENTS_PacketOverrideCleared_SIZE(1);
    ASSERT_EVENTS_PacketOverrideCleared(0, Svc::TelemetrySection::REALTIME, 4);
    ASSERT_EVENTS_PacketOverrideSet_SIZE(0);
    ASSERT_EQ(this->component.m_packetOverrides[0][0], Svc::PacketOverrideConfig());
    ASSERT_EQ(this->m_configOutInvokes, 1u);
    ASSERT_EQ(this->m_lastConfigEntry.get_packetId(), 4u);
    ASSERT_EQ(this->m_lastConfigEntry.get_override().get_overrideType(), Svc::PacketOverrideType::GROUP_DEFAULT);
    this->clearHistory();

    this->fillPacketChannels();
    this->setTestTime(this->m_testTime);
    this->resetCounter();
    this->invoke_to_Run(0, 0);
    this->component.doDispatch();
    ASSERT_from_PktSend_SIZE(2 * Svc::TelemetrySection::NUM_SECTIONS);
    this->clearHistory();

    // Clearing an unknown packet is rejected
    this->sendCmd_CLEAR_PACKET_OVERRIDE(0, 31, Svc::TelemetrySection::REALTIME, 9999);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE(0, TlmPacketizerComponentBase::OPCODE_CLEAR_PACKET_OVERRIDE, 31,
                        Fw::CmdResponse::VALIDATION_ERROR);
    ASSERT_EVENTS_PacketNotFound_SIZE(1);
}

void TlmPacketizerTester ::mirrorAllocFailTest() {
    this->stockConfiguration();
    this->component.setPacketList(packetList, ignore, 2);
    // Pool exhausted: the override is rolled back and the command fails, so RAM and the mirror never diverge
    this->m_allocFail = true;
    this->m_configOutInvokes = 0;
    this->sendCmd_OVERRIDE_PACKET_SEND(0, 40, Svc::TelemetrySection::REALTIME, 4, Svc::TlmSendState::SEND_NEVER);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE(0, TlmPacketizerComponentBase::OPCODE_OVERRIDE_PACKET_SEND, 40,
                        Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_EVENTS_ConfigMirrorAllocFailed_SIZE(1);
    ASSERT_EVENTS_PacketOverrideSet_SIZE(0);
    ASSERT_EQ(this->m_configOutInvokes, 0u);
    ASSERT_from_deallocate_SIZE(0);
    ASSERT_EQ(this->component.m_packetOverrides[0][0], Svc::PacketOverrideConfig());
    this->clearHistory();
    this->fillPacketChannels();
    this->setTestTime(this->m_testTime);
    this->resetCounter();
    this->invoke_to_Run(0, 0);
    this->component.doDispatch();
    ASSERT_from_PktSend_SIZE(2 * Svc::TelemetrySection::NUM_SECTIONS);
    this->clearHistory();

    // Pool recovered: the same command succeeds
    this->m_allocFail = false;
    this->sendCmd_OVERRIDE_PACKET_SEND(0, 41, Svc::TelemetrySection::REALTIME, 4, Svc::TlmSendState::SEND_NEVER);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE(0, TlmPacketizerComponentBase::OPCODE_OVERRIDE_PACKET_SEND, 41, Fw::CmdResponse::OK);
    ASSERT_EQ(this->m_configOutInvokes, 1u);
}

void TlmPacketizerTester ::mirrorReturnTest() {
    this->stockConfiguration();
    this->component.setPacketList(packetList, ignore, 2);
    this->m_configOutInvokes = 0;
    this->sendCmd_OVERRIDE_PACKET_SEND(0, 42, Svc::TelemetrySection::REALTIME, 4, Svc::TlmSendState::SEND_NEVER);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE(0, TlmPacketizerComponentBase::OPCODE_OVERRIDE_PACKET_SEND, 42, Fw::CmdResponse::OK);
    ASSERT_EQ(this->m_configOutInvokes, 1u);
    ASSERT_from_deallocate_SIZE(0);

    // The persistence component hands the mirror buffer back; it goes to the pool it was allocated from
    Fw::Buffer returned = this->m_configBuf;
    this->invoke_to_packetOverrideReturnIn(0, returned);
    ASSERT_from_deallocate_SIZE(1);
    ASSERT_EQ(this->m_configOutInvokes, 1u);
    ASSERT_EQ(this->fromPortHistory_deallocate->at(0).fwBuffer.getData(), this->m_configBufSlot);
    ASSERT_EQ(this->fromPortHistory_deallocate->at(0).fwBuffer.getSize(),
              static_cast<Fw::Buffer::SizeType>(Svc::PacketConfigEntry::SERIALIZED_SIZE));
    // Returning a mirror buffer is not a reload: the override table and the reload return port are untouched
    ASSERT_from_packetOverrideReturnOut_SIZE(0);
    ASSERT_EQ(this->component.m_packetOverrides[0][0].get_overrideType(), Svc::PacketOverrideType::OVERRIDE_SEND);
}

// Serialize the given records back-to-back into the tester's reload slot, as the persistence component would.
Fw::Buffer TlmPacketizerTester ::reloadBuffer(const Svc::PacketConfigEntry* entries, FwSizeType count) {
    Fw::Buffer buffer(this->m_reloadSlot, static_cast<Fw::Buffer::SizeType>(sizeof(this->m_reloadSlot)));
    auto serializer = buffer.getSerializer();
    for (FwSizeType i = 0; i < count; i++) {
        EXPECT_EQ(serializer.serializeFrom(entries[i]), Fw::FW_SERIALIZE_OK);
    }
    buffer.setSize(static_cast<Fw::Buffer::SizeType>(serializer.getSize()));
    return buffer;
}

void TlmPacketizerTester ::configInReloadTest() {
    this->stockConfiguration();
    this->component.setPacketList(packetList, ignore, 2);
    // Reload batch: silence packet id 4 in REALTIME, plus an unknown id that must be warned and skipped
    const Svc::PacketOverrideConfig silence =
        this->sendOverride(Svc::PacketOverrideType::OVERRIDE_SEND, Svc::TlmSendState::SEND_NEVER, Svc::RateConfig());
    const Svc::PacketConfigEntry records[2] = {
        Svc::PacketConfigEntry(4, Svc::TelemetrySection::REALTIME, silence),
        Svc::PacketConfigEntry(9999, Svc::TelemetrySection::REALTIME, silence),
    };
    Fw::Buffer buffer = this->reloadBuffer(records, 2);
    this->clearHistory();
    this->m_configOutInvokes = 0;
    this->invoke_to_packetOverrideIn(0, buffer);
    this->component.doDispatch();

    // One event per rejected record; the reload path never echoes back out packetOverrideOut
    ASSERT_EVENTS_ReloadRecordUnknownPacket_SIZE(1);
    ASSERT_EVENTS_ReloadRecordUnknownPacket(0, 9999);
    ASSERT_EVENTS_ReloadRecordInvalid_SIZE(0);
    ASSERT_EVENTS_ConfigReloadFailed_SIZE(0);
    ASSERT_EQ(this->m_configOutInvokes, 0u);
    ASSERT_EQ(this->component.m_packetOverrides[0][0], silence);
    // The exact buffer handed in goes back to the sender (same backing storage and size), not to the pool
    ASSERT_from_packetOverrideReturnOut_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_packetOverrideReturnOut->at(0).fwBuffer.getData(), buffer.getData());
    ASSERT_EQ(this->fromPortHistory_packetOverrideReturnOut->at(0).fwBuffer.getSize(), buffer.getSize());
    ASSERT_from_deallocate_SIZE(0);

    // The reloaded override took effect
    this->fillPacketChannels();
    this->setTestTime(this->m_testTime);
    this->resetCounter();
    this->invoke_to_Run(0, 0);
    this->component.doDispatch();
    ASSERT_from_PktSend_SIZE(2 * Svc::TelemetrySection::NUM_SECTIONS - 1);
    ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(Svc::TelemetrySection::REALTIME, 1)], 1u);
    ASSERT_EQ(this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(Svc::TelemetrySection::RECORDED, 1)], 2u);
}

void TlmPacketizerTester ::configInClearReloadTest() {
    this->stockConfiguration();
    this->component.setPacketList(packetList, ignore, 2);
    this->sendCmd_OVERRIDE_PACKET_SEND(0, 0, Svc::TelemetrySection::REALTIME, 4, Svc::TlmSendState::SEND_NEVER);
    this->component.doDispatch();
    // A GROUP_DEFAULT record reloads as "no override"
    const Svc::PacketConfigEntry record(4, Svc::TelemetrySection::REALTIME, Svc::PacketOverrideConfig());
    Fw::Buffer buffer = this->reloadBuffer(&record, 1);
    this->clearHistory();
    this->invoke_to_packetOverrideIn(0, buffer);
    this->component.doDispatch();
    ASSERT_EVENTS_ConfigReloadFailed_SIZE(0);
    ASSERT_EVENTS_ReloadRecordInvalid_SIZE(0);
    ASSERT_EVENTS_ReloadRecordUnknownPacket_SIZE(0);
    ASSERT_EQ(this->component.m_packetOverrides[0][0], Svc::PacketOverrideConfig());
    this->fillPacketChannels();
    this->setTestTime(this->m_testTime);
    this->resetCounter();
    this->invoke_to_Run(0, 0);
    this->component.doDispatch();
    ASSERT_from_PktSend_SIZE(2 * Svc::TelemetrySection::NUM_SECTIONS);
}

void TlmPacketizerTester ::configInMalformedReloadTest() {
    this->stockConfiguration();
    this->component.setPacketList(packetList, ignore, 2);
    const Svc::PacketOverrideConfig silence =
        this->sendOverride(Svc::PacketOverrideType::OVERRIDE_SEND, Svc::TlmSendState::SEND_NEVER, Svc::RateConfig());

    // Section out of range: the record is warned and skipped; the valid one applies. An invalid enum value
    // (patched into the third record's overrideType byte) cannot deserialize and ends the reload as malformed.
    const Svc::PacketConfigEntry records[3] = {
        Svc::PacketConfigEntry(4, static_cast<Svc::TelemetrySection::T>(Svc::TelemetrySection::NUM_SECTIONS), silence),
        Svc::PacketConfigEntry(8, Svc::TelemetrySection::REALTIME, silence),
        Svc::PacketConfigEntry(8, Svc::TelemetrySection::RECORDED, silence),
    };
    Fw::Buffer buffer = this->reloadBuffer(records, 3);
    this->m_reloadSlot[2 * Svc::PacketConfigEntry::SERIALIZED_SIZE + sizeof(U32) +
                       Svc::TelemetrySection::SERIALIZED_SIZE] = 99;
    this->clearHistory();
    this->invoke_to_packetOverrideIn(0, buffer);
    this->component.doDispatch();
    ASSERT_EVENTS_ReloadRecordInvalid_SIZE(1);
    ASSERT_EVENTS_ReloadRecordInvalid(0, 4);
    ASSERT_EVENTS_ConfigReloadFailed_SIZE(1);
    ASSERT_EVENTS_ConfigReloadFailed(0, Fw::DeserialStatus::FORMAT_ERROR);
    ASSERT_EQ(this->component.m_packetOverrides[0][1], silence);
    ASSERT_EQ(this->component.m_packetOverrides[1][1], Svc::PacketOverrideConfig());
    ASSERT_from_packetOverrideReturnOut_SIZE(1);
    this->clearHistory();

    // Trailing bytes that are not a whole record: the whole records apply, then the buffer is reported malformed
    const Svc::PacketConfigEntry record(4, Svc::TelemetrySection::REALTIME, silence);
    buffer = this->reloadBuffer(&record, 1);
    buffer.setSize(static_cast<Fw::Buffer::SizeType>(buffer.getSize() + 3));
    this->invoke_to_packetOverrideIn(0, buffer);
    this->component.doDispatch();
    ASSERT_EVENTS_ConfigReloadFailed_SIZE(1);
    ASSERT_EVENTS_ConfigReloadFailed(0, Fw::DeserialStatus::FORMAT_ERROR);
    ASSERT_EQ(this->component.m_packetOverrides[0][0], silence);
    ASSERT_from_packetOverrideReturnOut_SIZE(1);
    this->clearHistory();

    // A record shorter than SERIALIZED_SIZE alone is also malformed and applies nothing
    this->sendCmd_CLEAR_PACKET_OVERRIDE(0, 0, Svc::TelemetrySection::REALTIME, 4);
    this->component.doDispatch();
    this->clearHistory();
    buffer = this->reloadBuffer(&record, 1);
    buffer.setSize(static_cast<Fw::Buffer::SizeType>(Svc::PacketConfigEntry::SERIALIZED_SIZE - 1));
    this->invoke_to_packetOverrideIn(0, buffer);
    this->component.doDispatch();
    ASSERT_EVENTS_ConfigReloadFailed_SIZE(1);
    ASSERT_EQ(this->component.m_packetOverrides[0][0], Svc::PacketOverrideConfig());
    this->clearHistory();

    // An invalid buffer is reported as an empty reload and still returned
    Fw::Buffer invalid;
    this->invoke_to_packetOverrideIn(0, invalid);
    this->component.doDispatch();
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_ConfigReloadFailed(0, Fw::DeserialStatus::BUFFER_EMPTY);
    ASSERT_from_packetOverrideReturnOut_SIZE(1);
    ASSERT_from_deallocate_SIZE(0);
}

void TlmPacketizerTester ::staleParameterShapeTest() {
    this->stockConfiguration();
    const Svc::TlmPacketizer_SectionConfigs before = this->component.m_groupConfigs;

    // A stored value longer than the current serialized shape (e.g. saved by a previous FSW load) is rejected
    Fw::ParamBuffer stale;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stale.serializeFrom(before));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stale.serializeFrom(static_cast<U32>(0xDEADBEEF)));
    ASSERT_EQ(
        Fw::FW_DESERIALIZE_SIZE_MISMATCH,
        this->component.deserializeParam(0, TlmPacketizer::PARAMID_SECTION_CONFIGS, Fw::ParamValid::VALID, stale));
    ASSERT_EQ(before, this->component.m_groupConfigs);

    // A stored value shorter than the current shape is rejected by deserialization itself
    Fw::ParamBuffer truncated;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, truncated.serializeFrom(static_cast<U8>(0)));
    ASSERT_NE(Fw::FW_SERIALIZE_OK, this->component.deserializeParam(0, TlmPacketizer::PARAMID_SECTION_CONFIGS,
                                                                    Fw::ParamValid::VALID, truncated));
    ASSERT_EQ(before, this->component.m_groupConfigs);

    // A value of the current shape loads
    Fw::ParamBuffer current;
    Svc::TlmPacketizer_SectionConfigs silenced = before;
    silenced[0][0].set_sendState(Svc::TlmSendState::SEND_NEVER);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, current.serializeFrom(silenced));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, this->component.deserializeParam(0, TlmPacketizer::PARAMID_SECTION_CONFIGS,
                                                                    Fw::ParamValid::VALID, current));
    ASSERT_EQ(silenced, this->component.m_groupConfigs);

    // SECTION_ENABLED is guarded the same way
    const Svc::TlmPacketizer_SectionEnabled enabledBefore = this->component.m_sectionEnabled;
    Fw::ParamBuffer staleEnabled;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, staleEnabled.serializeFrom(enabledBefore));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, staleEnabled.serializeFrom(static_cast<U8>(0)));
    ASSERT_EQ(Fw::FW_DESERIALIZE_SIZE_MISMATCH,
              this->component.deserializeParam(0, TlmPacketizer::PARAMID_SECTION_ENABLED, Fw::ParamValid::VALID,
                                               staleEnabled));
    ASSERT_EQ(enabledBefore, this->component.m_sectionEnabled);
}

void TlmPacketizerTester ::groupSendCommandTest() {
    this->stockConfiguration();
    this->clearHistory();

    // Nominal: state stored, telemetered, evented, OK
    this->sendCmd_CONFIGURE_GROUP_SEND(0, 10, static_cast<TelemetrySection::T>(0), 1, Svc::TlmSendState::SEND_ALWAYS);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, TlmPacketizer::OPCODE_CONFIGURE_GROUP_SEND, 10, Fw::CmdResponse::OK);
    ASSERT_EQ(Svc::TlmSendState::SEND_ALWAYS, this->component.m_groupConfigs[0][1].get_sendState());
    ASSERT_EVENTS_GroupSendStateSet_SIZE(1);
    ASSERT_EVENTS_GroupSendStateSet(0, static_cast<TelemetrySection::T>(0), 1, Svc::TlmSendState::SEND_ALWAYS);
    ASSERT_TLM_GroupConfigs_SIZE(1);
    this->clearHistory();

    // Group out of range: rejected, nothing changes
    this->sendCmd_CONFIGURE_GROUP_SEND(0, 11, static_cast<TelemetrySection::T>(0),
                                       MAX_CONFIGURABLE_TLMPACKETIZER_GROUP + 1, Svc::TlmSendState::SEND_NEVER);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, TlmPacketizer::OPCODE_CONFIGURE_GROUP_SEND, 11, Fw::CmdResponse::VALIDATION_ERROR);
    ASSERT_EVENTS_SIZE(0);
    ASSERT_TLM_SIZE(0);
    ASSERT_EQ(Svc::TlmSendState::SEND_ALWAYS, this->component.m_groupConfigs[0][1].get_sendState());
}

void TlmPacketizerTester ::connectPorts() {
    // PktSend
    // this->component.set_PktSend_OutputPort(0, this->get_from_PktSend(0));
    // this->component.set_PktSend_OutputPort(1, this->get_from_PktSend(1));

    // Run
    this->connect_to_Run(0, this->component.get_Run_InputPort(0));

    // TlmRecv
    this->connect_to_TlmRecv(0, this->component.get_TlmRecv_InputPort(0));

    // cmdIn
    this->connect_to_cmdIn(0, this->component.get_cmdIn_InputPort(0));

    this->component.set_paramGetOut_OutputPort(0, this->get_from_paramGetOut(0));

    this->component.set_paramSetOut_OutputPort(0, this->get_from_paramSetOut(0));

    // cmdRegOut
    this->component.set_cmdRegOut_OutputPort(0, this->get_from_cmdRegOut(0));

    // cmdResponseOut
    this->component.set_cmdResponseOut_OutputPort(0, this->get_from_cmdResponseOut(0));

    // eventOut
    this->component.set_eventOut_OutputPort(0, this->get_from_eventOut(0));

    // pingIn
    this->connect_to_pingIn(0, this->component.get_pingIn_InputPort(0));

    // pingOut
    this->component.set_pingOut_OutputPort(0, this->get_from_pingOut(0));

    // textEventOut
    this->component.set_textEventOut_OutputPort(0, this->get_from_textEventOut(0));

    // timeGetOut
    this->component.set_timeGetOut_OutputPort(0, this->get_from_timeGetOut(0));

    // tlmOut
    this->component.set_tlmOut_OutputPort(0, this->get_from_tlmOut(0));

    // TlmGet
    this->connect_to_TlmGet(0, this->component.get_TlmGet_InputPort(0));

    for (FwIndexType index = 0; index < Svc::TELEMETRY_SEND_PORTS; index++) {
        this->component.set_PktSend_OutputPort(index, this->get_from_PktSend(index));
    }

    // controlIn
    this->connect_to_controlIn(0, this->component.get_controlIn_InputPort(0));

    this->connect_to_configureSectionGroupRate(0, this->component.get_configureSectionGroupRate_InputPort(0));

    // packetOverrideOut (per-packet override mirror to an external persistence component)
    this->component.set_packetOverrideOut_OutputPort(0, this->get_from_packetOverrideOut(0));

    // Buffer allocate/deallocate (stand in for a BufferManager pool) and reload-buffer return
    this->component.set_allocate_OutputPort(0, this->get_from_allocate(0));
    this->component.set_deallocate_OutputPort(0, this->get_from_deallocate(0));
    this->component.set_packetOverrideReturnOut_OutputPort(0, this->get_from_packetOverrideReturnOut(0));

    this->connect_to_packetOverrideIn(0, this->component.get_packetOverrideIn_InputPort(0));
    this->connect_to_packetOverrideReturnIn(0, this->component.get_packetOverrideReturnIn_InputPort(0));
}

void TlmPacketizerTester::textLogIn(const FwEventIdType id,          //!< The event ID
                                    const Fw::Time& timeTag,         //!< The time
                                    const Fw::LogSeverity severity,  //!< The severity
                                    const Fw::TextLogString& text    //!< The event string
) {
    TextLogEntry e = {id, timeTag, severity, text};

    printTextLogHistoryEntry(e, stdout);
}

void TlmPacketizerTester ::initComponents() {
    this->init();
    this->component.init(QUEUE_DEPTH, INSTANCE);
}

void TlmPacketizerTester ::resetCounter() {
    for (FwIndexType section = 0; section < TelemetrySection::NUM_SECTIONS; section++) {
        for (FwChanIdType group = 0; group < MAX_CONFIGURABLE_TLMPACKETIZER_GROUP; group++) {
            this->m_portOutInvokes[TlmPacketizer::sectionGroupToPort(section, group)] = 0;
        }
    }
}

}  // end namespace Svc

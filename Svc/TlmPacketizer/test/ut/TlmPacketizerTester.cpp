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

void TlmPacketizerTester ::initTest() {
    this->component.setPacketList(packetList, ignore, 2);
}

void TlmPacketizerTester ::pushTlmTest() {
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

    this->sendCmd_CONFIGURE_GROUP_RATES(0, 0, static_cast<TelemetrySection::T>(1), 2, Svc::RateLogic::SILENCED, 0, 0);
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
    this->sendCmd_ENABLE_GROUP(0, 0, static_cast<TelemetrySection::T>(0), 1, Fw::Enabled::DISABLED);
    this->component.doDispatch();
    // Expect No Packets
    this->invoke_to_Run(0, 0);
    this->component.doDispatch();
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    ASSERT_from_PktSend_SIZE(0);

    // Enable group on section, but disable section
    this->sendCmd_ENABLE_GROUP(0, 0, static_cast<TelemetrySection::T>(0), 1, Fw::Enabled::ENABLED);
    this->component.doDispatch();
    this->sendCmd_ENABLE_SECTION(0, 0, static_cast<TelemetrySection::T>(0), Fw::Enabled::DISABLED);
    this->component.doDispatch();
    // Expect No Packets
    this->invoke_to_Run(0, 0);
    this->component.doDispatch();
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    ASSERT_from_PktSend_SIZE(0);

    // Invalidate the send algorithm
    this->sendCmd_CONFIGURE_GROUP_RATES(0, 0, static_cast<TelemetrySection::T>(0), 1, Svc::RateLogic::SILENCED, 0, 0);
    this->component.doDispatch();
    // Re-enable settings using the configuration port
    this->invoke_to_configureSectionGroupRate(0, static_cast<TelemetrySection::T>(0), 1, Svc::RateLogic::EVERY_MAX, 0,
                                              0);
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

    // Disable section by port invocation, but Send Command Force Section
    this->invoke_to_controlIn(0, static_cast<TelemetrySection::T>(0), Fw::Enabled::DISABLED);
    this->component.doDispatch();
    this->sendCmd_FORCE_GROUP(0, 0, static_cast<TelemetrySection::T>(0), 1, Fw::Enabled::ENABLED);
    this->component.doDispatch();
    // Expect A Packet
    this->invoke_to_Run(0, 0);
    this->component.doDispatch();
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    ASSERT_from_PktSend_SIZE(1);
    this->clearHistory();

    // Disable group, but keep force group command active
    this->sendCmd_ENABLE_GROUP(0, 0, static_cast<TelemetrySection::T>(0), 1, Fw::Enabled::DISABLED);
    this->component.doDispatch();
    // Expect A Packet
    this->invoke_to_Run(0, 0);
    this->component.doDispatch();
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    ASSERT_from_PktSend_SIZE(1);
    this->clearHistory();

    // Disable Force Group, with Group Disabled and Section Disabled
    this->sendCmd_FORCE_GROUP(0, 0, static_cast<TelemetrySection::T>(0), 1, Fw::Enabled::DISABLED);
    this->component.doDispatch();
    // Expect No Packets
    this->invoke_to_Run(0, 0);
    this->component.doDispatch();
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    ASSERT_from_PktSend_SIZE(0);
    this->clearHistory();
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

// ----------------------------------------------------------------------
// Helper methods
// ----------------------------------------------------------------------

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

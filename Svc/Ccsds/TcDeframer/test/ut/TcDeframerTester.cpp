// ======================================================================
// \title  TcDeframerTester.cpp
// \author thomas-bc
// \brief  cpp file for TcDeframer component test harness implementation class
// ======================================================================

#include "TcDeframerTester.hpp"
#include "STest/Random/Random.hpp"
#include "Svc/Ccsds/Types/FppConstantsAc.hpp"
#include "Svc/Ccsds/Types/TCHeaderSerializableAc.hpp"
#include "Svc/Ccsds/Types/TCSegmentHeaderSerializableAc.hpp"
#include "Svc/Ccsds/Types/TCSegmentSequenceFlagsEnumAc.hpp"
#include "Svc/Ccsds/Types/TCTrailerSerializableAc.hpp"
#include "Svc/Ccsds/Utils/CRC16.hpp"

namespace Svc {

namespace Ccsds {

const FwSizeType TcDeframerTester::ALLOC_BUF_SIZE;

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

TcDeframerTester ::TcDeframerTester()
    : TcDeframerGTestBase("TcDeframerTester", TcDeframerTester::MAX_HISTORY_SIZE), component("TcDeframer") {
    this->initComponents();
    this->connectPorts();
}

TcDeframerTester ::~TcDeframerTester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void TcDeframerTester::testDataReturn() {
    U8 data[1] = {0};
    Fw::Buffer buffer(data, sizeof(data));
    ComCfg::FrameContext nullContext;
    this->invoke_to_dataReturnIn(0, buffer, nullContext);
    ASSERT_from_dataReturnOut_SIZE(1);  // incoming buffer should be deallocated
    ASSERT_FROM_PORT_HISTORY_SIZE(1);   // only port call
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).data.getData(), data);
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).data.getSize(), sizeof(data));
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).context, nullContext);
}

void TcDeframerTester::testNominalDeframing() {
    // Frame: 5 bytes (header) + bytes (data) + 2 bytes (trailer)
    U16 scId = static_cast<U16>(STest::Random::lowerUpper(0, 0x3FF));       // random 10 bit Spacecraft ID
    U8 vcId = static_cast<U8>(STest::Random::lowerUpper(0, 0x3F));          // random 6 bit virtual channel ID
    U8 seqCount = static_cast<U8>(STest::Random::lowerUpper(0, 0xFF));      // random 8 bit sequence count
    U8 payloadLength = static_cast<U8>(STest::Random::lowerUpper(1, 200));  // bytes of data, random length
    U8 payload[payloadLength];
    // Initialize payload with some data
    for (FwIndexType i = 0; i < payloadLength; i++) {
        payload[i] = static_cast<U8>(i % std::numeric_limits<U8>::max());
    }

    Fw::Buffer buffer = this->assembleFrameBuffer(payload, payloadLength, scId, vcId, seqCount);
    ComCfg::FrameContext nullContext;

    this->setComponentState(scId, vcId, seqCount);
    this->invoke_to_dataIn(0, buffer, nullContext);

    ASSERT_from_dataOut_SIZE(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(1);  // only one port call
    Fw::Buffer outBuffer = this->fromPortHistory_dataOut->at(0).data;
    ASSERT_EQ(outBuffer.getSize(), payloadLength);
    for (FwIndexType i = 0; i < payloadLength; i++) {
        ASSERT_EQ(outBuffer.getData()[i], payload[i]);
    }
    // The frame's VCID is carried on the emitted context: Svc.Ccsds.AesGcmDecryptor builds its
    // AAD from this field, so a frame on VC != 0 fails its MAC check if it is not set here
    ASSERT_EQ(this->fromPortHistory_dataOut->at(0).context.get_vcId(), vcId);
}

void TcDeframerTester::testInvalidScId() {
    // Frame: 5 bytes (header) + 1 byte (data) + 2 bytes (trailer)
    U16 scId = static_cast<U16>(STest::Random::lowerUpper(1, 0x3FF));    // random 10 bit Spacecraft ID
    U8 dataLength = static_cast<U8>(STest::Random::lowerUpper(1, 200));  // bytes of data, random length
    U8 data[dataLength];

    // Assemble frame with incorrect scId value
    Fw::Buffer buffer = this->assembleFrameBuffer(data, dataLength, static_cast<U16>(scId - 1));
    ComCfg::FrameContext nullContext;

    this->setComponentState(scId);
    this->invoke_to_dataIn(0, buffer, nullContext);

    ASSERT_from_dataOut_SIZE(0);
    ASSERT_from_dataReturnOut_SIZE(1);  // invalid buffer was deallocated
    ASSERT_FROM_PORT_HISTORY_SIZE(2);   // two port calls, one for dataReturn, one for errorNotify
    ASSERT_from_errorNotify(0, Svc::Ccsds::FrameError::TC_INVALID_SCID);  // errorNotify port called with invalid SCID
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).data.getData(), buffer.getData());
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).data.getSize(), buffer.getSize());
    ASSERT_EVENTS_SIZE(1);                      // exactly 1 event emitted
    ASSERT_EVENTS_InvalidSpacecraftId_SIZE(1);  // event was emitted for invalid spacecraft ID
    ASSERT_EVENTS_InvalidSpacecraftId(0, static_cast<U16>(scId - 1),
                                      scId);  // event was emitted for invalid spacecraft ID
}

void TcDeframerTester::testInvalidVcId() {
    U8 vcId = static_cast<U8>(STest::Random::lowerUpper(1, 0x3F));       // random 6 bit VCID
    U8 dataLength = static_cast<U8>(STest::Random::lowerUpper(1, 200));  // bytes of data, random length
    U8 data[dataLength];

    // Assemble frame with incorrect vcId value
    Fw::Buffer buffer = this->assembleFrameBuffer(data, dataLength, 0, static_cast<U8>(vcId - 1));
    ComCfg::FrameContext nullContext;

    this->setComponentState(0, vcId, 0, false);  // set the component in mode where only one VCID is accepted
    this->invoke_to_dataIn(0, buffer, nullContext);

    ASSERT_from_dataOut_SIZE(0);
    ASSERT_from_dataReturnOut_SIZE(1);  // invalid buffer was deallocated
    ASSERT_FROM_PORT_HISTORY_SIZE(2);   // two port calls, one for dataReturn, one for errorNotify
    ASSERT_from_errorNotify(0, Svc::Ccsds::FrameError::TC_INVALID_VCID);  // errorNotify port called with invalid VCID
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).data.getData(), buffer.getData());
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).data.getSize(), buffer.getSize());
    ASSERT_EVENTS_SIZE(1);                                           // exactly 1 event emitted
    ASSERT_EVENTS_InvalidVcId_SIZE(1);                               // event was emitted for invalid VCID
    ASSERT_EVENTS_InvalidVcId(0, static_cast<U16>(vcId - 1), vcId);  // event was emitted for invalid VCID
}

void TcDeframerTester::testInvalidLengthToken() {
    U8 dataLength = static_cast<U8>(STest::Random::lowerUpper(1, 200));  // bytes of data, random length
    U8 data[dataLength];
    U8 incorrectLengthToken = static_cast<U8>(dataLength + TCHeader::SERIALIZED_SIZE + TCTrailer::SERIALIZED_SIZE + 1);

    Fw::Buffer buffer = this->assembleFrameBuffer(data, dataLength);
    buffer.getData()[3] = incorrectLengthToken;  // Override length token to invalid value
    ComCfg::FrameContext nullContext;

    this->setComponentState();
    this->invoke_to_dataIn(0, buffer, nullContext);

    ASSERT_from_dataOut_SIZE(0);
    ASSERT_from_dataReturnOut_SIZE(1);  // invalid buffer was deallocated
    ASSERT_FROM_PORT_HISTORY_SIZE(2);   // two port calls, one for dataReturn, one for errorNotify
    ASSERT_from_errorNotify(0, Svc::Ccsds::FrameError::TC_INVALID_LENGTH);
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).data.getData(), buffer.getData());
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).data.getSize(), buffer.getSize());
    ASSERT_EVENTS_SIZE(1);                     // exactly 1 event emitted
    ASSERT_EVENTS_InvalidFrameLength_SIZE(1);  // event was emitted for invalid frame length
    // event logs size in bytes which is length token + 1
    ASSERT_EVENTS_InvalidFrameLength(
        0, static_cast<U16>(incorrectLengthToken + 1),
        static_cast<FwSizeType>(dataLength + TCHeader::SERIALIZED_SIZE + TCTrailer::SERIALIZED_SIZE));
}

void TcDeframerTester::testInvalidCrc() {
    U8 dataLength = static_cast<U8>(STest::Random::lowerUpper(1, 200));  // bytes of data, random length
    U8 data[dataLength];

    Fw::Buffer buffer = this->assembleFrameBuffer(data, dataLength);
    // Increment CRC to corrupt its value
    buffer.getData()[TCHeader::SERIALIZED_SIZE + dataLength + 1]++;
    ComCfg::FrameContext nullContext;

    this->setComponentState();
    this->invoke_to_dataIn(0, buffer, nullContext);

    // Invalid CRC drops the frame
    ASSERT_from_dataOut_SIZE(0);
    ASSERT_from_dataReturnOut_SIZE(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(2);  // two port calls, one for dataReturn, one for errorNotify
    ASSERT_from_errorNotify(0, Svc::Ccsds::FrameError::TC_INVALID_CRC);
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).data.getData(), buffer.getData());
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).data.getSize(), buffer.getSize());
    ASSERT_EVENTS_SIZE(1);  // exactly 1 event emitted
    ASSERT_EVENTS_InvalidCrc_SIZE(1);
}

// ----------------------------------------------------------------------
// Segmentation tests (CCSDS 232.0-B-4 Section 4.1.3.3)
// ----------------------------------------------------------------------

void TcDeframerTester::testSegmentedUnsegmentedPassthrough() {
    this->enableSegmentation();
    U8 vcId = static_cast<U8>(STest::Random::lowerUpper(0, 0x3F));
    U8 dataLength = static_cast<U8>(STest::Random::lowerUpper(1, 200));
    U8 payload[200];
    fillPattern(payload, dataLength, 0x11);

    Fw::Buffer frame =
        this->assembleSegmentFrameBuffer(TCSegmentSequenceFlags::UNSEGMENTED, this->m_mapId, payload, dataLength, vcId);
    ComCfg::FrameContext nullContext;
    this->invoke_to_dataIn(0, frame, nullContext);

    // Unsegmented data is forwarded zero-copy from the frame, minus header, segment header and trailer
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    ASSERT_from_allocate_SIZE(0);
    this->assertDataOutEquals(payload, dataLength);
    ASSERT_EQ(this->fromPortHistory_dataOut->at(0).data.getData(),
              frame.getOriginalData() + TCHeader::SERIALIZED_SIZE + TCSegmentHeader::SERIALIZED_SIZE);
    ASSERT_EQ(this->fromPortHistory_dataOut->at(0).context.get_vcId(), vcId);
    ASSERT_EVENTS_SIZE(0);

    // The frame buffer is not owned by the deframer and goes back upstream on return
    Fw::Buffer returned = this->fromPortHistory_dataOut->at(0).data;
    this->clearHistory();
    this->invoke_to_dataReturnIn(0, returned, nullContext);
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    ASSERT_from_dataReturnOut_SIZE(1);
    ASSERT_from_deallocate_SIZE(0);
}

void TcDeframerTester::testSegmentedTwoSegments() {
    this->enableSegmentation();
    U8 firstLength = static_cast<U8>(STest::Random::lowerUpper(1, 200));
    U8 lastLength = static_cast<U8>(STest::Random::lowerUpper(1, 200));
    U8 expected[400];
    fillPattern(expected, static_cast<FwSizeType>(firstLength + lastLength), 0x22);

    this->sendConsumedSegment(TCSegmentSequenceFlags::FIRST, expected, firstLength);
    ASSERT_from_allocate_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_allocate->at(0).size, ALLOC_BUF_SIZE);

    this->clearHistory();
    Fw::Buffer lastFrame = this->assembleSegmentFrameBuffer(TCSegmentSequenceFlags::LAST, this->m_mapId,
                                                            &expected[firstLength], lastLength);
    ComCfg::FrameContext nullContext;
    this->invoke_to_dataIn(0, lastFrame, nullContext);

    // Last segment completes the packet: reassembled packet downstream, frame back upstream
    ASSERT_FROM_PORT_HISTORY_SIZE(2);
    ASSERT_from_dataReturnOut_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).data.getOriginalData(), lastFrame.getOriginalData());
    this->assertDataOutEquals(expected, static_cast<FwSizeType>(firstLength + lastLength));
    ASSERT_EQ(this->fromPortHistory_dataOut->at(0).data.getData(), this->m_allocPool[0]);
    ASSERT_EVENTS_SIZE(0);
    ASSERT_TLM_SpanningPacketsReassembled_SIZE(1);
    ASSERT_TLM_SpanningPacketsReassembled(0, 1);

    // The reassembled packet is owned by the deframer and is deallocated on return
    Fw::Buffer packet = this->fromPortHistory_dataOut->at(0).data;
    this->clearHistory();
    this->invoke_to_dataReturnIn(0, packet, nullContext);
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    ASSERT_from_deallocate_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_deallocate->at(0).fwBuffer.getData(), this->m_allocPool[0]);
    ASSERT_EQ(this->fromPortHistory_deallocate->at(0).fwBuffer.getSize(), ALLOC_BUF_SIZE);

    // Returning it a second time must not be treated as owned
    this->clearHistory();
    this->invoke_to_dataReturnIn(0, packet, nullContext);
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    ASSERT_from_dataReturnOut_SIZE(1);
}

void TcDeframerTester::testSegmentedManySegments() {
    this->enableSegmentation();
    const FwSizeType continuingCount = static_cast<FwSizeType>(STest::Random::lowerUpper(1, 6));
    U8 expected[ALLOC_BUF_SIZE];
    FwSizeType total = 0;

    U8 length = static_cast<U8>(STest::Random::lowerUpper(1, 60));
    fillPattern(&expected[total], length, 0x33);
    this->sendConsumedSegment(TCSegmentSequenceFlags::FIRST, &expected[total], length);
    total += length;

    for (FwSizeType i = 0; i < continuingCount; i++) {
        length = static_cast<U8>(STest::Random::lowerUpper(1, 60));
        fillPattern(&expected[total], length, static_cast<U8>(0x40 + i));
        this->sendConsumedSegment(TCSegmentSequenceFlags::CONTINUING, &expected[total], length);
        total += length;
    }

    this->clearHistory();
    length = static_cast<U8>(STest::Random::lowerUpper(1, 60));
    fillPattern(&expected[total], length, 0x55);
    Fw::Buffer lastFrame =
        this->assembleSegmentFrameBuffer(TCSegmentSequenceFlags::LAST, this->m_mapId, &expected[total], length);
    total += length;
    ComCfg::FrameContext nullContext;
    this->invoke_to_dataIn(0, lastFrame, nullContext);

    ASSERT_FROM_PORT_HISTORY_SIZE(2);
    ASSERT_from_dataReturnOut_SIZE(1);
    this->assertDataOutEquals(expected, total);
    ASSERT_EVENTS_SIZE(0);
    ASSERT_TLM_SpanningPacketsReassembled(0, 1);
}

void TcDeframerTester::testSegmentedUnexpectedSegment() {
    this->enableSegmentation();
    U8 payload[10];
    fillPattern(payload, sizeof(payload), 0x66);
    ComCfg::FrameContext nullContext;
    const U8 flags[2] = {TCSegmentSequenceFlags::CONTINUING, TCSegmentSequenceFlags::LAST};

    for (FwSizeType i = 0; i < 2; i++) {
        this->clearHistory();
        Fw::Buffer frame = this->assembleSegmentFrameBuffer(flags[i], this->m_mapId, payload, sizeof(payload));
        this->invoke_to_dataIn(0, frame, nullContext);

        // No packet in progress: segment dropped, frame returned, error reported
        ASSERT_FROM_PORT_HISTORY_SIZE(2);
        ASSERT_from_dataOut_SIZE(0);
        ASSERT_from_allocate_SIZE(0);
        ASSERT_from_dataReturnOut_SIZE(1);
        ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).data.getOriginalData(), frame.getOriginalData());
        ASSERT_from_errorNotify(0, FrameError::TC_SEGMENT_UNEXPECTED);
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_UnexpectedSegment_SIZE(1);
        ASSERT_EVENTS_UnexpectedSegment(0, static_cast<TCSegmentSequenceFlags::T>(flags[i]));
    }
}

void TcDeframerTester::testSegmentedFirstRestartsPacket() {
    this->enableSegmentation();
    U8 stale[20];
    U8 expected[40];
    fillPattern(stale, sizeof(stale), 0x77);
    fillPattern(expected, sizeof(expected), 0x88);
    ComCfg::FrameContext nullContext;

    this->sendConsumedSegment(TCSegmentSequenceFlags::FIRST, stale, sizeof(stale));

    // A new first segment abandons the packet in progress and starts over
    this->clearHistory();
    Fw::Buffer frame = this->assembleSegmentFrameBuffer(TCSegmentSequenceFlags::FIRST, this->m_mapId, expected, 20);
    this->invoke_to_dataIn(0, frame, nullContext);
    ASSERT_FROM_PORT_HISTORY_SIZE(3);
    ASSERT_from_dataOut_SIZE(0);
    ASSERT_from_deallocate_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_deallocate->at(0).fwBuffer.getData(), this->m_allocPool[0]);
    ASSERT_from_allocate_SIZE(1);
    ASSERT_from_dataReturnOut_SIZE(1);
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_SpanningPacketAbandoned_SIZE(1);
    ASSERT_EVENTS_SpanningPacketAbandoned(0, sizeof(stale));

    this->clearHistory();
    frame = this->assembleSegmentFrameBuffer(TCSegmentSequenceFlags::LAST, this->m_mapId, &expected[20], 20);
    this->invoke_to_dataIn(0, frame, nullContext);
    ASSERT_FROM_PORT_HISTORY_SIZE(2);
    this->assertDataOutEquals(expected, sizeof(expected));
    ASSERT_EQ(this->fromPortHistory_dataOut->at(0).data.getData(), this->m_allocPool[1]);
}

void TcDeframerTester::testSegmentedUnsegmentedAbandonsPacket() {
    this->enableSegmentation();
    U8 stale[20];
    U8 whole[30];
    fillPattern(stale, sizeof(stale), 0x99);
    fillPattern(whole, sizeof(whole), 0xAA);
    ComCfg::FrameContext nullContext;

    this->sendConsumedSegment(TCSegmentSequenceFlags::FIRST, stale, sizeof(stale));

    this->clearHistory();
    Fw::Buffer frame =
        this->assembleSegmentFrameBuffer(TCSegmentSequenceFlags::UNSEGMENTED, this->m_mapId, whole, sizeof(whole));
    this->invoke_to_dataIn(0, frame, nullContext);

    // Packet in progress is abandoned, unsegmented data still forwarded zero-copy
    ASSERT_FROM_PORT_HISTORY_SIZE(2);
    ASSERT_from_deallocate_SIZE(1);
    this->assertDataOutEquals(whole, sizeof(whole));
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_SpanningPacketAbandoned(0, sizeof(stale));
}

void TcDeframerTester::testSegmentedInvalidMapId() {
    U8 mapId = static_cast<U8>(STest::Random::lowerUpper(0, 0x3F));
    U8 wrongMapId = static_cast<U8>((mapId + STest::Random::lowerUpper(1, 0x3F)) & TCSubfields::SegmentMapIdMask);
    this->enableSegmentation(mapId);
    U8 payload[10];
    fillPattern(payload, sizeof(payload), 0xBB);
    ComCfg::FrameContext nullContext;

    Fw::Buffer frame =
        this->assembleSegmentFrameBuffer(TCSegmentSequenceFlags::UNSEGMENTED, wrongMapId, payload, sizeof(payload));
    this->invoke_to_dataIn(0, frame, nullContext);

    ASSERT_FROM_PORT_HISTORY_SIZE(2);
    ASSERT_from_dataOut_SIZE(0);
    ASSERT_from_dataReturnOut_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).data.getOriginalData(), frame.getOriginalData());
    ASSERT_from_errorNotify(0, FrameError::TC_INVALID_MAP_ID);
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_InvalidMapId_SIZE(1);
    ASSERT_EVENTS_InvalidMapId(0, wrongMapId, mapId);
}

void TcDeframerTester::testSegmentedAllocFailure() {
    this->enableSegmentation();
    U8 payload[10];
    fillPattern(payload, sizeof(payload), 0xCC);
    ComCfg::FrameContext nullContext;

    this->m_failNextAlloc = true;
    Fw::Buffer frame =
        this->assembleSegmentFrameBuffer(TCSegmentSequenceFlags::FIRST, this->m_mapId, payload, sizeof(payload));
    this->invoke_to_dataIn(0, frame, nullContext);

    // Allocation failed: nothing to deallocate, segment dropped, frame returned
    ASSERT_FROM_PORT_HISTORY_SIZE(3);
    ASSERT_from_allocate_SIZE(1);
    ASSERT_from_deallocate_SIZE(0);
    ASSERT_from_dataOut_SIZE(0);
    ASSERT_from_dataReturnOut_SIZE(1);
    ASSERT_from_errorNotify(0, FrameError::TC_SEGMENT_ALLOC_FAILED);
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_SpanningPacketAllocFailed_SIZE(1);
    ASSERT_EVENTS_SpanningPacketAllocFailed(0, ALLOC_BUF_SIZE);

    // No packet is in progress afterwards
    this->clearHistory();
    frame = this->assembleSegmentFrameBuffer(TCSegmentSequenceFlags::LAST, this->m_mapId, payload, sizeof(payload));
    this->invoke_to_dataIn(0, frame, nullContext);
    ASSERT_from_dataOut_SIZE(0);
    ASSERT_EVENTS_UnexpectedSegment_SIZE(1);
}

void TcDeframerTester::testSegmentedOverflow() {
    const FwSizeType maxSize = 100;
    this->enableSegmentation(0, maxSize);
    U8 payload[80];
    fillPattern(payload, sizeof(payload), 0xDD);
    ComCfg::FrameContext nullContext;

    this->sendConsumedSegment(TCSegmentSequenceFlags::FIRST, payload, sizeof(payload));
    ASSERT_EQ(this->fromPortHistory_allocate->at(0).size, maxSize);

    // Second segment would exceed the maximum: packet discarded
    this->clearHistory();
    Fw::Buffer frame =
        this->assembleSegmentFrameBuffer(TCSegmentSequenceFlags::CONTINUING, this->m_mapId, payload, sizeof(payload));
    this->invoke_to_dataIn(0, frame, nullContext);
    ASSERT_FROM_PORT_HISTORY_SIZE(3);
    ASSERT_from_dataOut_SIZE(0);
    ASSERT_from_deallocate_SIZE(1);
    ASSERT_from_dataReturnOut_SIZE(1);
    ASSERT_from_errorNotify(0, FrameError::TC_SEGMENT_OVERFLOW);
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_SpanningPacketOverflow_SIZE(1);
    ASSERT_EVENTS_SpanningPacketOverflow(0, 2 * sizeof(payload), maxSize);

    // No packet is in progress afterwards
    this->clearHistory();
    frame = this->assembleSegmentFrameBuffer(TCSegmentSequenceFlags::LAST, this->m_mapId, payload, sizeof(payload));
    this->invoke_to_dataIn(0, frame, nullContext);
    ASSERT_from_dataOut_SIZE(0);
    ASSERT_EVENTS_UnexpectedSegment_SIZE(1);
}

void TcDeframerTester::testSegmentedInFlightLimit() {
    this->enableSegmentation();
    U8 payload[10];
    fillPattern(payload, sizeof(payload), 0xEE);
    ComCfg::FrameContext nullContext;
    Fw::Buffer frame;

    // Fill the in-flight table without returning any packet
    for (FwSizeType i = 0; i < TcDeframer_MaxSpanningPacketsInFlight; i++) {
        this->sendConsumedSegment(TCSegmentSequenceFlags::FIRST, payload, sizeof(payload));
        this->clearHistory();
        frame = this->assembleSegmentFrameBuffer(TCSegmentSequenceFlags::LAST, this->m_mapId, payload, sizeof(payload));
        this->invoke_to_dataIn(0, frame, nullContext);
        ASSERT_from_dataOut_SIZE(1);
        ASSERT_EQ(this->fromPortHistory_dataOut->at(0).data.getData(), this->m_allocPool[i]);
    }

    // One more completed packet cannot be tracked: it is dropped and its buffer deallocated
    this->sendConsumedSegment(TCSegmentSequenceFlags::FIRST, payload, sizeof(payload));
    this->clearHistory();
    frame = this->assembleSegmentFrameBuffer(TCSegmentSequenceFlags::LAST, this->m_mapId, payload, sizeof(payload));
    this->invoke_to_dataIn(0, frame, nullContext);
    ASSERT_FROM_PORT_HISTORY_SIZE(3);
    ASSERT_from_dataOut_SIZE(0);
    ASSERT_from_deallocate_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_deallocate->at(0).fwBuffer.getData(),
              this->m_allocPool[TcDeframer_MaxSpanningPacketsInFlight]);
    ASSERT_from_dataReturnOut_SIZE(1);
    ASSERT_from_errorNotify(0, FrameError::TC_SEGMENT_IN_FLIGHT_LIMIT);
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_SpanningPacketInFlightLimit(0, TcDeframer_MaxSpanningPacketsInFlight);

    // Returning one tracked packet frees a slot; a new packet is then delivered again
    this->clearHistory();
    Fw::Buffer tracked(this->m_allocPool[3], sizeof(payload) * 2);
    this->invoke_to_dataReturnIn(0, tracked, nullContext);
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    ASSERT_from_deallocate_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_deallocate->at(0).fwBuffer.getData(), this->m_allocPool[3]);

    this->sendConsumedSegment(TCSegmentSequenceFlags::FIRST, payload, sizeof(payload));
    this->clearHistory();
    frame = this->assembleSegmentFrameBuffer(TCSegmentSequenceFlags::LAST, this->m_mapId, payload, sizeof(payload));
    this->invoke_to_dataIn(0, frame, nullContext);
    ASSERT_from_dataOut_SIZE(1);
    ASSERT_EVENTS_SIZE(0);
}

void TcDeframerTester::testSegmentedEmptySegment() {
    this->enableSegmentation();
    ComCfg::FrameContext nullContext;

    // Data field holding only a segment header carries no data: invalid
    Fw::Buffer frame = this->assembleSegmentFrameBuffer(TCSegmentSequenceFlags::UNSEGMENTED, this->m_mapId, nullptr, 0);
    this->invoke_to_dataIn(0, frame, nullContext);

    ASSERT_FROM_PORT_HISTORY_SIZE(2);
    ASSERT_from_dataOut_SIZE(0);
    ASSERT_from_dataReturnOut_SIZE(1);
    ASSERT_from_errorNotify(0, FrameError::TC_INVALID_LENGTH);
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_InvalidPacket_SIZE(1);
}

// ----------------------------------------------------------------------
// Handlers for typed from ports
// ----------------------------------------------------------------------

Fw::Buffer TcDeframerTester::from_allocate_handler(FwIndexType portNum, FwSizeType size) {
    this->pushFromPortEntry_allocate(size);
    if (this->m_failNextAlloc) {
        this->m_failNextAlloc = false;
        return Fw::Buffer();
    }
    if (size > ALLOC_BUF_SIZE) {
        return Fw::Buffer();
    }
    Fw::Buffer allocated(this->m_allocPool[this->m_nextAlloc], size);
    this->m_nextAlloc = (this->m_nextAlloc + 1) % ALLOC_POOL_COUNT;
    return allocated;
}

// ----------------------------------------------------------------------
// Helpers
// ----------------------------------------------------------------------

void TcDeframerTester::setComponentState(U16 scid, U8 vcid, U8 sequenceNumber, bool acceptAllVcid) {
    this->component.configure(vcid, scid, acceptAllVcid);
}

void TcDeframerTester::enableSegmentation(U8 mapId, FwSizeType maxSpanningPacketSize) {
    this->m_mapId = mapId;
    this->setComponentState();
    this->component.configureSegmentation(true, mapId, maxSpanningPacketSize);
}

Fw::Buffer TcDeframerTester::assembleSegmentFrameBuffer(U8 sequenceFlags,
                                                        U8 mapId,
                                                        const U8* data,
                                                        U8 dataLength,
                                                        U8 vcid) {
    FW_ASSERT(static_cast<FwSizeType>(dataLength) + TCSegmentHeader::SERIALIZED_SIZE <= sizeof(this->m_segmentData));
    this->m_segmentData[0] = static_cast<U8>((sequenceFlags << TCSubfields::SegmentSequenceFlagsOffset) |
                                             (mapId & TCSubfields::SegmentMapIdMask));
    if (dataLength > 0) {
        ::memcpy(&this->m_segmentData[TCSegmentHeader::SERIALIZED_SIZE], data, dataLength);
    }
    return this->assembleFrameBuffer(this->m_segmentData,
                                     static_cast<U8>(dataLength + TCSegmentHeader::SERIALIZED_SIZE), 0, vcid);
}

void TcDeframerTester::fillPattern(U8* data, FwSizeType length, U8 seed) {
    for (FwSizeType i = 0; i < length; i++) {
        data[i] = static_cast<U8>((seed + i) & 0xFF);
    }
}

void TcDeframerTester::sendConsumedSegment(U8 sequenceFlags, const U8* data, U8 dataLength) {
    this->clearHistory();
    Fw::Buffer frame = this->assembleSegmentFrameBuffer(sequenceFlags, this->m_mapId, data, dataLength);
    ComCfg::FrameContext nullContext;
    this->invoke_to_dataIn(0, frame, nullContext);
    ASSERT_from_dataOut_SIZE(0);
    ASSERT_from_dataReturnOut_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).data.getOriginalData(), frame.getOriginalData());
    ASSERT_from_errorNotify_SIZE(0);
}

void TcDeframerTester::assertDataOutEquals(const U8* expected, FwSizeType expectedLength) {
    ASSERT_from_dataOut_SIZE(1);
    const Fw::Buffer& out = this->fromPortHistory_dataOut->at(0).data;
    ASSERT_EQ(out.getSize(), expectedLength);
    for (FwSizeType i = 0; i < expectedLength; i++) {
        ASSERT_EQ(out.getData()[i], expected[i]) << "Mismatch at byte " << i;
    }
}

Fw::Buffer TcDeframerTester::assembleFrameBuffer(U8* data, U8 dataLength, U16 scid, U8 vcid, U8 seqNumber) {
    ::memset(this->m_frameData, 0, sizeof(this->m_frameData));
    U16 frameLength = static_cast<U16>(TCHeader::SERIALIZED_SIZE + dataLength + TCTrailer::SERIALIZED_SIZE);
    U16 frameLengthToken = static_cast<U16>(frameLength - 1);  // length token is length - 1
    // Header
    this->m_frameData[0] = static_cast<U8>(scid >> 8);
    this->m_frameData[1] = static_cast<U8>(scid & 0xFF);
    this->m_frameData[2] = static_cast<U8>((vcid << 2) | static_cast<U8>((frameLengthToken >> 8) & 0x03));
    this->m_frameData[3] = static_cast<U8>(frameLengthToken & 0xFF);
    this->m_frameData[4] = seqNumber;

    // Data
    memcpy(&this->m_frameData[TCHeader::SERIALIZED_SIZE], data, dataLength);

    // CRC trailer
    U16 crc = Ccsds::Utils::CRC16::compute(this->m_frameData, TCHeader::SERIALIZED_SIZE + dataLength);
    this->m_frameData[TCHeader::SERIALIZED_SIZE + dataLength] = static_cast<U8>(crc >> 8);
    this->m_frameData[TCHeader::SERIALIZED_SIZE + dataLength + 1] = static_cast<U8>(crc & 0xFF);

    return Fw::Buffer(this->m_frameData, frameLength);
}

}  // namespace Ccsds
}  // namespace Svc

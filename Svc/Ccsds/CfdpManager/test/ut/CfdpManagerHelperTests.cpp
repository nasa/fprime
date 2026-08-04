// ======================================================================
// \title  CfdpManagerHelperTests.cpp
// \brief  Unit tests for CFDP pure helper code exercised through the
//         CfdpManager unit-test executable.
//
// These tests target code that is compiled into the CfdpManager UT
// executable (and therefore reported in CfdpManager/coverage/summary.txt)
// but is otherwise only exercised indirectly by the full-protocol tests:
//   - TLV classes         (Types/Tlv.cpp)
//   - Circular list        (Clist.cpp)
//   - CFDP status helpers  (Utils.cpp)
//   - PDU codecs           (Types/FileDataPdu.cpp, NakPdu.cpp, PduHeader.cpp)
//
// They use plain GoogleTest TEST() cases (no CfdpManager harness) and
// share the single main() defined in CfdpManagerTestMain.cpp.
// ======================================================================

#include <gtest/gtest.h>
#include <Fw/Types/SerialBuffer.hpp>
#include <Svc/Ccsds/CfdpManager/Clist.hpp>
#include <Svc/Ccsds/CfdpManager/Types/PduBase.hpp>
#include <Svc/Ccsds/CfdpManager/Utils.hpp>
#include <cstring>

using namespace Svc::Ccsds;
using namespace Svc::Ccsds::Cfdp;

// ======================================================================
// Phase A1 - TLV / TlvData / TlvList (Types/Tlv.cpp)
// ======================================================================

TEST(TlvHelper, EntityIdInitAndGetters) {
    Tlv tlv;
    const EntityId testEid = 42;
    tlv.initialize(testEid);

    EXPECT_EQ(TlvType::TLV_TYPE_ENTITY_ID, tlv.getType());
    EXPECT_EQ(sizeof(EntityId), tlv.getData().getLength());
    EXPECT_EQ(testEid, tlv.getData().getEntityId());
}

TEST(TlvHelper, RawDataInitAndGetters) {
    Tlv tlv;
    const U8 testData[] = {0x01, 0x02, 0x03, 0x04, 0x05};
    const U8 testDataLen = sizeof(testData);
    tlv.initialize(TlvType::TLV_TYPE_MESSAGE_TO_USER, testData, testDataLen);

    EXPECT_EQ(TlvType::TLV_TYPE_MESSAGE_TO_USER, tlv.getType());
    EXPECT_EQ(testDataLen, tlv.getData().getLength());
    EXPECT_EQ(0, memcmp(testData, tlv.getData().getData(), testDataLen));
}

TEST(TlvHelper, EncodedSize) {
    Tlv tlv;
    const U8 testData[] = {0xAA, 0xBB, 0xCC};
    tlv.initialize(TlvType::TLV_TYPE_FLOW_LABEL, testData, sizeof(testData));

    // Type(1) + Length(1) + Data(3) = 5
    EXPECT_EQ(5U, tlv.getEncodedSize());
}

TEST(TlvHelper, RoundTripEntityId) {
    Tlv txTlv;
    const EntityId testEid = 123;
    txTlv.initialize(testEid);

    U8 buffer[256];
    Fw::SerialBuffer serialBuffer(buffer, sizeof(buffer));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, txTlv.toSerialBuffer(serialBuffer));

    serialBuffer.resetSer();
    serialBuffer.fill();
    Tlv rxTlv;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, rxTlv.fromSerialBuffer(serialBuffer));

    EXPECT_EQ(TlvType::TLV_TYPE_ENTITY_ID, rxTlv.getType());
    EXPECT_EQ(testEid, rxTlv.getData().getEntityId());
}

TEST(TlvHelper, RoundTripRawData) {
    Tlv txTlv;
    const U8 testData[] = {0xDE, 0xAD, 0xBE, 0xEF};
    txTlv.initialize(TlvType::TLV_TYPE_MESSAGE_TO_USER, testData, sizeof(testData));

    U8 buffer[256];
    Fw::SerialBuffer serialBuffer(buffer, sizeof(buffer));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, txTlv.toSerialBuffer(serialBuffer));

    serialBuffer.resetSer();
    serialBuffer.fill();
    Tlv rxTlv;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, rxTlv.fromSerialBuffer(serialBuffer));

    EXPECT_EQ(TlvType::TLV_TYPE_MESSAGE_TO_USER, rxTlv.getType());
    EXPECT_EQ(sizeof(testData), rxTlv.getData().getLength());
    EXPECT_EQ(0, memcmp(testData, rxTlv.getData().getData(), sizeof(testData)));
}

TEST(TlvHelper, RoundTripMaxData255) {
    Tlv txTlv;
    U8 testData[255];
    for (U16 i = 0; i < 255; i++) {
        testData[i] = static_cast<U8>(i);
    }
    txTlv.initialize(TlvType::TLV_TYPE_MESSAGE_TO_USER, testData, 255);

    U8 buffer[512];
    Fw::SerialBuffer serialBuffer(buffer, sizeof(buffer));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, txTlv.toSerialBuffer(serialBuffer));

    serialBuffer.resetSer();
    serialBuffer.fill();
    Tlv rxTlv;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, rxTlv.fromSerialBuffer(serialBuffer));

    EXPECT_EQ(255, rxTlv.getData().getLength());
    EXPECT_EQ(0, memcmp(testData, rxTlv.getData().getData(), 255));
}

TEST(TlvHelper, DeserializeEntityIdTooLong) {
    // Hand-craft a TLV: type = ENTITY_ID, length > sizeof(EntityId)
    U8 buffer[8];
    buffer[0] = static_cast<U8>(TlvType::TLV_TYPE_ENTITY_ID);
    buffer[1] = static_cast<U8>(sizeof(EntityId) + 1);  // one byte too long
    memset(&buffer[2], 0, sizeof(buffer) - 2);

    Fw::SerialBuffer serialBuffer(buffer, sizeof(buffer));
    serialBuffer.fill();

    Tlv rxTlv;
    EXPECT_EQ(Fw::FW_DESERIALIZE_FORMAT_ERROR, rxTlv.fromSerialBuffer(serialBuffer));
}

TEST(TlvHelper, ListAppendUpToMaxAndReject) {
    TlvList list;
    for (U8 i = 0; i < MaxTlv; i++) {
        Tlv tlv;
        tlv.initialize(static_cast<EntityId>(100 + i));
        ASSERT_TRUE(list.appendTlv(tlv)) << "Failed to append TLV " << static_cast<int>(i);
    }
    EXPECT_EQ(MaxTlv, list.getNumTlv());

    // One more should be rejected
    Tlv extra;
    extra.initialize(999);
    EXPECT_FALSE(list.appendTlv(extra));
    EXPECT_EQ(MaxTlv, list.getNumTlv());
}

TEST(TlvHelper, ListClearAndGetTlv) {
    TlvList list;
    for (U8 i = 0; i < 3; i++) {
        Tlv tlv;
        tlv.initialize(static_cast<EntityId>(i));
        ASSERT_TRUE(list.appendTlv(tlv));
    }
    EXPECT_EQ(3, list.getNumTlv());
    EXPECT_EQ(TlvType::TLV_TYPE_ENTITY_ID, list.getTlv(0).getType());

    list.clear();
    EXPECT_EQ(0, list.getNumTlv());

    Tlv tlv;
    tlv.initialize(100);
    ASSERT_TRUE(list.appendTlv(tlv));
    EXPECT_EQ(1, list.getNumTlv());
    EXPECT_EQ(100, list.getTlv(0).getData().getEntityId());
}

TEST(TlvHelper, ListEncodedSizeAndRoundTrip) {
    TlvList txList;

    Tlv tlv1;
    tlv1.initialize(123);
    ASSERT_TRUE(txList.appendTlv(tlv1));

    const U8 data2[] = {0xAA, 0xBB};
    Tlv tlv2;
    tlv2.initialize(TlvType::TLV_TYPE_MESSAGE_TO_USER, data2, sizeof(data2));
    ASSERT_TRUE(txList.appendTlv(tlv2));

    const U8 data3[] = {0xDE, 0xAD, 0xBE, 0xEF};
    Tlv tlv3;
    tlv3.initialize(TlvType::TLV_TYPE_FLOW_LABEL, data3, sizeof(data3));
    ASSERT_TRUE(txList.appendTlv(tlv3));

    EXPECT_EQ(tlv1.getEncodedSize() + tlv2.getEncodedSize() + tlv3.getEncodedSize(), txList.getEncodedSize());

    U8 buffer[512];
    Fw::SerialBuffer serialBuffer(buffer, sizeof(buffer));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, txList.toSerialBuffer(serialBuffer));
    U32 encodedSize = static_cast<U32>(serialBuffer.getSize());

    // Decode - drives the "read until buffer exhausted" loop
    Fw::SerialBuffer decodeBuffer(buffer, encodedSize);
    decodeBuffer.fill();
    TlvList rxList;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, rxList.fromSerialBuffer(decodeBuffer));

    EXPECT_EQ(3, rxList.getNumTlv());
    EXPECT_EQ(TlvType::TLV_TYPE_ENTITY_ID, rxList.getTlv(0).getType());
    EXPECT_EQ(123, rxList.getTlv(0).getData().getEntityId());
    EXPECT_EQ(TlvType::TLV_TYPE_MESSAGE_TO_USER, rxList.getTlv(1).getType());
    EXPECT_EQ(TlvType::TLV_TYPE_FLOW_LABEL, rxList.getTlv(2).getType());
}

// ======================================================================
// Phase A2 - Circular list (Clist.cpp)
// ======================================================================

namespace {
//! Traversal callback that counts nodes visited
CListTraverseStatus countCb(CListNode* node, void* context) {
    (void)node;
    int* count = static_cast<int*>(context);
    (*count)++;
    return CLIST_TRAVERSE_CONTINUE;
}

//! Traversal callback that stops on the first node visited
CListTraverseStatus exitFirstCb(CListNode* node, void* context) {
    (void)node;
    int* count = static_cast<int*>(context);
    (*count)++;
    return CLIST_TRAVERSE_EXIT;
}
}  // namespace

TEST(ClistHelper, InitNode) {
    CListNode node;
    CfdpCListInitNode(&node);
    EXPECT_EQ(&node, node.next);
    EXPECT_EQ(&node, node.prev);
}

TEST(ClistHelper, InsertFrontEmptyThenNonEmpty) {
    CListNode a, b;
    CfdpCListInitNode(&a);
    CfdpCListInitNode(&b);

    CListNode* head = nullptr;
    CfdpCListInsertFront(&head, &a);  // empty-list path
    EXPECT_EQ(&a, head);
    EXPECT_EQ(&a, a.next);

    CfdpCListInsertFront(&head, &b);  // non-empty path, b becomes new head
    EXPECT_EQ(&b, head);
    EXPECT_EQ(&a, b.next);
    EXPECT_EQ(&a, b.prev);
    EXPECT_EQ(&b, a.next);
    EXPECT_EQ(&b, a.prev);
}

TEST(ClistHelper, InsertBackEmptyThenNonEmpty) {
    CListNode a, b;
    CfdpCListInitNode(&a);
    CfdpCListInitNode(&b);

    CListNode* head = nullptr;
    CfdpCListInsertBack(&head, &a);  // empty-list path
    EXPECT_EQ(&a, head);

    CfdpCListInsertBack(&head, &b);  // non-empty path, head stays a
    EXPECT_EQ(&a, head);
    EXPECT_EQ(&b, a.next);
    EXPECT_EQ(&a, b.next);
}

TEST(ClistHelper, PopReturnsFrontAndEmpties) {
    CListNode a, b;
    CfdpCListInitNode(&a);
    CfdpCListInitNode(&b);

    CListNode* head = nullptr;
    CfdpCListInsertBack(&head, &a);
    CfdpCListInsertBack(&head, &b);

    CListNode* popped = CfdpCListPop(&head);
    EXPECT_EQ(&a, popped);
    EXPECT_EQ(&b, head);

    // Pop from empty list returns nullptr
    (void)CfdpCListPop(&head);
    EXPECT_EQ(nullptr, head);
    EXPECT_EQ(nullptr, CfdpCListPop(&head));
}

TEST(ClistHelper, RemoveSingleNode) {
    CListNode a;
    CfdpCListInitNode(&a);
    CListNode* head = nullptr;
    CfdpCListInsertBack(&head, &a);

    CfdpCListRemove(&head, &a);  // only-node path
    EXPECT_EQ(nullptr, head);
    // Node is re-initialized to point to itself
    EXPECT_EQ(&a, a.next);
    EXPECT_EQ(&a, a.prev);
}

TEST(ClistHelper, RemoveHeadNode) {
    CListNode a, b, c;
    CfdpCListInitNode(&a);
    CfdpCListInitNode(&b);
    CfdpCListInitNode(&c);
    CListNode* head = nullptr;
    CfdpCListInsertBack(&head, &a);
    CfdpCListInsertBack(&head, &b);
    CfdpCListInsertBack(&head, &c);

    CfdpCListRemove(&head, &a);  // remove-head path
    EXPECT_EQ(&b, head);
    EXPECT_EQ(&c, b.prev);
    EXPECT_EQ(&b, c.next);
}

TEST(ClistHelper, RemoveMiddleNode) {
    CListNode a, b, c;
    CfdpCListInitNode(&a);
    CfdpCListInitNode(&b);
    CfdpCListInitNode(&c);
    CListNode* head = nullptr;
    CfdpCListInsertBack(&head, &a);
    CfdpCListInsertBack(&head, &b);
    CfdpCListInsertBack(&head, &c);

    CfdpCListRemove(&head, &b);  // general (middle) path
    EXPECT_EQ(&a, head);
    EXPECT_EQ(&c, a.next);
    EXPECT_EQ(&a, c.next);
}

TEST(ClistHelper, InsertAfter) {
    CListNode a, b, c;
    CfdpCListInitNode(&a);
    CfdpCListInitNode(&b);
    CfdpCListInitNode(&c);
    CListNode* head = nullptr;
    CfdpCListInsertBack(&head, &a);
    CfdpCListInsertBack(&head, &b);

    CfdpCListInsertAfter(&head, &a, &c);  // insert c after a
    EXPECT_EQ(&c, a.next);
    EXPECT_EQ(&a, c.prev);
    EXPECT_EQ(&b, c.next);
    EXPECT_EQ(&c, b.prev);
}

TEST(ClistHelper, TraverseForwardAllNodes) {
    CListNode a, b, c;
    CfdpCListInitNode(&a);
    CfdpCListInitNode(&b);
    CfdpCListInitNode(&c);
    CListNode* head = nullptr;
    CfdpCListInsertBack(&head, &a);
    CfdpCListInsertBack(&head, &b);
    CfdpCListInsertBack(&head, &c);

    int count = 0;
    CfdpCListTraverse(head, countCb, &count);
    EXPECT_EQ(3, count);
}

TEST(ClistHelper, TraverseForwardEarlyExit) {
    // Cover the early-exit break in CfdpCListTraverse. The trailing FW_ASSERT(last)
    // only holds when traversal stops on the terminal node, so use a single-node
    // list: its sole node is also the last node, and the EXIT callback breaks with
    // last already true.
    CListNode a;
    CfdpCListInitNode(&a);
    CListNode* head = nullptr;
    CfdpCListInsertBack(&head, &a);

    int count = 0;
    CfdpCListTraverse(head, exitFirstCb, &count);
    EXPECT_EQ(1, count);
}

TEST(ClistHelper, TraverseReverseAllNodes) {
    CListNode a, b, c;
    CfdpCListInitNode(&a);
    CfdpCListInitNode(&b);
    CfdpCListInitNode(&c);
    CListNode* head = nullptr;
    CfdpCListInsertBack(&head, &a);
    CfdpCListInsertBack(&head, &b);
    CfdpCListInsertBack(&head, &c);

    int count = 0;
    CfdpCListTraverseR(head, countCb, &count);
    EXPECT_EQ(3, count);
}

TEST(ClistHelper, TraverseReverseEarlyExit) {
    // Cover the early-exit break in CfdpCListTraverseR. As with the forward case,
    // the trailing FW_ASSERT(last) only holds when traversal stops on the terminal
    // node, so use a single-node list.
    CListNode a;
    CfdpCListInitNode(&a);
    CListNode* head = nullptr;
    CfdpCListInsertBack(&head, &a);

    int count = 0;
    CfdpCListTraverseR(head, exitFirstCb, &count);
    EXPECT_EQ(1, count);
}

TEST(ClistHelper, TraverseNullStartIsNoOp) {
    int count = 0;
    CfdpCListTraverse(nullptr, countCb, &count);
    EXPECT_EQ(0, count);
}

TEST(ClistHelper, TraverseReverseNullEndIsNoOp) {
    int count = 0;
    CfdpCListTraverseR(nullptr, countCb, &count);
    EXPECT_EQ(0, count);
}

// ======================================================================
// Phase A3 - CFDP status helpers (Utils.cpp)
// ======================================================================

TEST(UtilsHelper, TxnStatusIsError) {
    EXPECT_FALSE(TxnStatusIsError(TxnStatus::TXN_STATUS_UNDEFINED));
    EXPECT_FALSE(TxnStatusIsError(TxnStatus::TXN_STATUS_NO_ERROR));
    EXPECT_TRUE(TxnStatusIsError(TxnStatus::TXN_STATUS_POS_ACK_LIMIT_REACHED));
    EXPECT_TRUE(TxnStatusIsError(TxnStatus::TXN_STATUS_PROTOCOL_ERROR));
}

TEST(UtilsHelper, TxnStatusToConditionCodeNoError) {
    // Non-error statuses map to NO_ERROR
    EXPECT_EQ(ConditionCode::CONDITION_CODE_NO_ERROR, TxnStatusToConditionCode(TxnStatus::TXN_STATUS_UNDEFINED));
    EXPECT_EQ(ConditionCode::CONDITION_CODE_NO_ERROR, TxnStatusToConditionCode(TxnStatus::TXN_STATUS_NO_ERROR));
}

TEST(UtilsHelper, TxnStatusToConditionCodeDirectMap) {
    // The in-range error statuses map directly to the same numeric CFDP condition code
    const TxnStatus statuses[] = {
        TxnStatus::TXN_STATUS_POS_ACK_LIMIT_REACHED,     TxnStatus::TXN_STATUS_KEEP_ALIVE_LIMIT_REACHED,
        TxnStatus::TXN_STATUS_INVALID_TRANSMISSION_MODE, TxnStatus::TXN_STATUS_FILESTORE_REJECTION,
        TxnStatus::TXN_STATUS_FILE_CHECKSUM_FAILURE,     TxnStatus::TXN_STATUS_FILE_SIZE_ERROR,
        TxnStatus::TXN_STATUS_NAK_LIMIT_REACHED,         TxnStatus::TXN_STATUS_INACTIVITY_DETECTED,
        TxnStatus::TXN_STATUS_INVALID_FILE_STRUCTURE,    TxnStatus::TXN_STATUS_CHECK_LIMIT_REACHED,
        TxnStatus::TXN_STATUS_UNSUPPORTED_CHECKSUM_TYPE, TxnStatus::TXN_STATUS_SUSPEND_REQUEST_RECEIVED,
        TxnStatus::TXN_STATUS_CANCEL_REQUEST_RECEIVED};
    for (const auto& status : statuses) {
        EXPECT_EQ(static_cast<ConditionCode>(status), TxnStatusToConditionCode(status))
            << "Mismatch for txn status " << static_cast<int>(status);
    }
}

TEST(UtilsHelper, TxnStatusToConditionCodeAckLimitMapsToInactivity) {
    EXPECT_EQ(ConditionCode::CONDITION_CODE_INACTIVITY_DETECTED,
              TxnStatusToConditionCode(TxnStatus::TXN_STATUS_ACK_LIMIT_NO_FIN));
    EXPECT_EQ(ConditionCode::CONDITION_CODE_INACTIVITY_DETECTED,
              TxnStatusToConditionCode(TxnStatus::TXN_STATUS_ACK_LIMIT_NO_EOF));
}

TEST(UtilsHelper, TxnStatusToConditionCodeDefault) {
    // Extended error codes with no direct CFDP CC map to CANCEL_REQUEST_RECEIVED
    EXPECT_EQ(ConditionCode::CONDITION_CODE_CANCEL_REQUEST_RECEIVED,
              TxnStatusToConditionCode(TxnStatus::TXN_STATUS_PROTOCOL_ERROR));
    EXPECT_EQ(ConditionCode::CONDITION_CODE_CANCEL_REQUEST_RECEIVED,
              TxnStatusToConditionCode(TxnStatus::TXN_STATUS_NAK_RESPONSE_ERROR));
    EXPECT_EQ(ConditionCode::CONDITION_CODE_CANCEL_REQUEST_RECEIVED,
              TxnStatusToConditionCode(TxnStatus::TXN_STATUS_EARLY_FIN));
}

TEST(UtilsHelper, GetTxnStatusNull) {
    EXPECT_EQ(AckTxnStatus::ACK_TXN_STATUS_UNRECOGNIZED, GetTxnStatus(nullptr));
}

// ======================================================================
// Phase B1 - FileDataPdu codec (Types/FileDataPdu.cpp)
// ======================================================================

TEST(FileDataPduHelper, GetBufferSizeAndMaxSize) {
    FileDataPdu pdu;
    const U8 testData[] = {0x01, 0x02, 0x03, 0x04, 0x05};
    pdu.initialize(PduDirection::DIRECTION_TOWARD_RECEIVER, Cfdp::Class::CLASS_2, 1, 2, 3, 100, sizeof(testData),
                   testData);

    // 32-bit large-file flag path: header + offset(4) + data
    U32 expectedSize = pdu.asHeader().getBufferSize() + 4 + static_cast<U32>(sizeof(testData));
    EXPECT_EQ(expectedSize, pdu.getBufferSize());

    // Max payload fits within one PDU and leaves room for header + offset
    EXPECT_GT(pdu.getMaxFileDataSize(), 0U);
}

TEST(FileDataPduHelper, ToBufferFromBufferRoundTrip) {
    // Exercises the toBuffer()/fromBuffer() overloads
    FileDataPdu txPdu;
    const U32 fileOffset = 1024;
    const U8 testData[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xCA, 0xFE};
    const U16 dataSize = sizeof(testData);
    txPdu.initialize(PduDirection::DIRECTION_TOWARD_RECEIVER, Cfdp::Class::CLASS_1, 50, 100, 75, fileOffset, dataSize,
                     testData);

    U8 storage[512];
    Fw::Buffer txBuffer(storage, sizeof(storage));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, txPdu.toBuffer(txBuffer));
    ASSERT_GT(txBuffer.getSize(), 0U);

    FileDataPdu rxPdu;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, rxPdu.fromBuffer(txBuffer));

    EXPECT_EQ(PduTypeEnum::FILE_DATA, rxPdu.asHeader().getType());
    EXPECT_EQ(fileOffset, rxPdu.getOffset());
    EXPECT_EQ(dataSize, rxPdu.getDataSize());
    ASSERT_NE(nullptr, rxPdu.getData());
    EXPECT_EQ(0, memcmp(testData, rxPdu.getData(), dataSize));
}

TEST(FileDataPduHelper, SerializeDeserializeRoundTrip) {
    // Exercises the Fw::Serializable serializeTo()/deserializeFrom() overloads
    FileDataPdu txPdu;
    const U8 testData[] = {0x11, 0x22, 0x33};
    txPdu.initialize(PduDirection::DIRECTION_TOWARD_RECEIVER, Cfdp::Class::CLASS_2, 1, 2, 3, 2048, sizeof(testData),
                     testData);

    U8 storage[512];
    Fw::SerialBuffer sbTx(storage, sizeof(storage));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, txPdu.serializeTo(sbTx));

    FileDataPdu rxPdu;
    Fw::SerialBuffer sbRx(storage, static_cast<Fw::Serializable::SizeType>(sbTx.getSize()));
    sbRx.setBuffLen(sbTx.getSize());
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, rxPdu.deserializeFrom(sbRx));

    EXPECT_EQ(2048U, rxPdu.getOffset());
    EXPECT_EQ(sizeof(testData), rxPdu.getDataSize());
}

TEST(FileDataPduHelper, FromBufferWrongType) {
    // Build a directive (NAK) PDU buffer and attempt to parse it as file data
    NakPdu nak;
    nak.initialize(PduDirection::DIRECTION_TOWARD_SENDER, Cfdp::Class::CLASS_2, 1, 2, 3, 0, 4096);

    U8 storage[512];
    Fw::SerialBuffer sb(storage, sizeof(storage));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, nak.serializeTo(sb));

    Fw::Buffer nakBuffer(storage, static_cast<U32>(sb.getSize()));
    FileDataPdu rxPdu;
    EXPECT_EQ(Fw::FW_DESERIALIZE_TYPE_MISMATCH, rxPdu.fromBuffer(nakBuffer));
}

TEST(FileDataPduHelper, EmptyPayloadToBuffer) {
    FileDataPdu pdu;
    pdu.initialize(PduDirection::DIRECTION_TOWARD_RECEIVER, Cfdp::Class::CLASS_2, 1, 2, 3, 0, 0, nullptr);

    U8 storage[512];
    Fw::Buffer txBuffer(storage, sizeof(storage));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, pdu.toBuffer(txBuffer));
    ASSERT_GT(txBuffer.getSize(), 0U);
}

// ======================================================================
// Phase B2 - NakPdu codec (Types/NakPdu.cpp)
// ======================================================================

TEST(NakPduHelper, GetBufferSizeWithSegments) {
    NakPdu pdu;
    pdu.initialize(PduDirection::DIRECTION_TOWARD_SENDER, Cfdp::Class::CLASS_2, 1, 2, 3, 0, 4096);

    U32 baseSize = pdu.getBufferSize();
    ASSERT_TRUE(pdu.addSegment(100, 200));
    EXPECT_EQ(baseSize + 8, pdu.getBufferSize());  // 2 * sizeof(FileSize)
    ASSERT_TRUE(pdu.addSegment(300, 400));
    EXPECT_EQ(baseSize + 16, pdu.getBufferSize());
}

TEST(NakPduHelper, RoundTripNoSegments) {
    NakPdu txPdu;
    txPdu.initialize(PduDirection::DIRECTION_TOWARD_SENDER, Cfdp::Class::CLASS_2, 50, 100, 75, 1024, 8192);

    U8 storage[512];
    Fw::SerialBuffer sbTx(storage, sizeof(storage));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, txPdu.serializeTo(sbTx));

    NakPdu rxPdu;
    Fw::SerialBuffer sbRx(storage, static_cast<Fw::Serializable::SizeType>(sbTx.getSize()));
    sbRx.setBuffLen(sbTx.getSize());
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, rxPdu.deserializeFrom(sbRx));

    EXPECT_EQ(PduTypeEnum::NEGATIVE_ACK, rxPdu.asHeader().getType());
    EXPECT_EQ(1024U, rxPdu.getScopeStart());
    EXPECT_EQ(8192U, rxPdu.getScopeEnd());
    EXPECT_EQ(0, rxPdu.getNumSegments());
}

TEST(NakPduHelper, RoundTripWithSegments) {
    NakPdu txPdu;
    txPdu.initialize(PduDirection::DIRECTION_TOWARD_SENDER, Cfdp::Class::CLASS_2, 1, 2, 3, 0, 10000);
    ASSERT_TRUE(txPdu.addSegment(100, 200));
    ASSERT_TRUE(txPdu.addSegment(500, 750));
    EXPECT_EQ(2, txPdu.getNumSegments());

    U8 storage[512];
    Fw::SerialBuffer sbTx(storage, sizeof(storage));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, txPdu.serializeTo(sbTx));

    NakPdu rxPdu;
    Fw::SerialBuffer sbRx(storage, static_cast<Fw::Serializable::SizeType>(sbTx.getSize()));
    sbRx.setBuffLen(sbTx.getSize());
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, rxPdu.deserializeFrom(sbRx));

    ASSERT_EQ(2, rxPdu.getNumSegments());
    EXPECT_EQ(100U, rxPdu.getSegment(0).offsetStart);
    EXPECT_EQ(200U, rxPdu.getSegment(0).offsetEnd);
    EXPECT_EQ(500U, rxPdu.getSegment(1).offsetStart);
    EXPECT_EQ(750U, rxPdu.getSegment(1).offsetEnd);
}

TEST(NakPduHelper, AddSegmentRejectsWhenFull) {
    NakPdu pdu;
    pdu.initialize(PduDirection::DIRECTION_TOWARD_SENDER, Cfdp::Class::CLASS_2, 1, 2, 3, 0, 100000);
    for (U8 i = 0; i < NakMaxSegments; i++) {
        ASSERT_TRUE(pdu.addSegment(i * 1000, i * 1000 + 500)) << "Failed to add segment " << static_cast<int>(i);
    }
    EXPECT_EQ(NakMaxSegments, pdu.getNumSegments());
    EXPECT_FALSE(pdu.addSegment(999000, 999500));
    EXPECT_EQ(NakMaxSegments, pdu.getNumSegments());
}

TEST(NakPduHelper, ClearSegments) {
    NakPdu pdu;
    pdu.initialize(PduDirection::DIRECTION_TOWARD_SENDER, Cfdp::Class::CLASS_2, 1, 2, 3, 0, 4096);
    ASSERT_TRUE(pdu.addSegment(100, 200));
    ASSERT_TRUE(pdu.addSegment(300, 400));
    EXPECT_EQ(2, pdu.getNumSegments());
    pdu.clearSegments();
    EXPECT_EQ(0, pdu.getNumSegments());
}

TEST(NakPduHelper, DeserializeWrongType) {
    // Feed a FILE_DATA header to NakPdu::deserializeFrom
    FileDataPdu fd;
    const U8 data[] = {0x01, 0x02};
    fd.initialize(PduDirection::DIRECTION_TOWARD_RECEIVER, Cfdp::Class::CLASS_2, 1, 2, 3, 0, sizeof(data), data);

    U8 storage[512];
    Fw::SerialBuffer sb(storage, sizeof(storage));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, fd.serializeTo(sb));

    NakPdu rxPdu;
    Fw::SerialBuffer sbRx(storage, static_cast<Fw::Serializable::SizeType>(sb.getSize()));
    sbRx.setBuffLen(sb.getSize());
    EXPECT_EQ(Fw::FW_DESERIALIZE_TYPE_MISMATCH, rxPdu.deserializeFrom(sbRx));
}

TEST(NakPduHelper, DeserializeWrongDirectiveCode) {
    // Feed a directive PDU with a non-NAK directive code (EOF) to NakPdu::deserializeFrom
    EofPdu eof;
    eof.initialize(PduDirection::DIRECTION_TOWARD_RECEIVER, Cfdp::Class::CLASS_2, 1, 2, 3,
                   ConditionCode::CONDITION_CODE_NO_ERROR, 0x12345678, 4096);

    U8 storage[512];
    Fw::SerialBuffer sb(storage, sizeof(storage));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, eof.serializeTo(sb));

    NakPdu rxPdu;
    Fw::SerialBuffer sbRx(storage, static_cast<Fw::Serializable::SizeType>(sb.getSize()));
    sbRx.setBuffLen(sb.getSize());
    EXPECT_EQ(Fw::FW_DESERIALIZE_TYPE_MISMATCH, rxPdu.deserializeFrom(sbRx));
}

// ======================================================================
// Phase B3 - PduHeader helpers (Types/PduHeader.cpp)
// ======================================================================

TEST(PduHeaderHelper, GetValueEncodedSize) {
    // Returns the minimum byte count n such that value < 0x100^n (boundary is >=),
    // capped at 8. Exercises the whole loop including the cap.
    EXPECT_EQ(1, PduHeader::getValueEncodedSize(0xFF));                  // fits in 1 byte
    EXPECT_EQ(2, PduHeader::getValueEncodedSize(0x100));                 // boundary -> 2 bytes
    EXPECT_EQ(3, PduHeader::getValueEncodedSize(0x10000));               // 2^16 -> 3 bytes
    EXPECT_EQ(4, PduHeader::getValueEncodedSize(0x1000000));             // 2^24 -> 4 bytes
    EXPECT_EQ(5, PduHeader::getValueEncodedSize(0x100000000ULL));        // 2^32 -> 5 bytes
    EXPECT_EQ(8, PduHeader::getValueEncodedSize(0x100000000000000ULL));  // 2^56 -> 8 (cap)
}

namespace {
//! Serialize a directive PDU into an Fw::Buffer for peekPduType()
template <typename PduT>
Fw::Buffer serializeToFwBuffer(const PduT& pdu, U8* storage, U32 storageSize) {
    Fw::SerialBuffer sb(storage, storageSize);
    EXPECT_EQ(Fw::FW_SERIALIZE_OK, pdu.serializeTo(sb));
    return Fw::Buffer(storage, static_cast<U32>(sb.getSize()));
}
}  // namespace

TEST(PduHeaderHelper, PeekPduTypeAllDirectives) {
    U8 storage[512];

    // FILE_DATA
    {
        FileDataPdu pdu;
        const U8 data[] = {0xAA};
        pdu.initialize(PduDirection::DIRECTION_TOWARD_RECEIVER, Cfdp::Class::CLASS_2, 1, 2, 3, 0, sizeof(data), data);
        Fw::Buffer buf(storage, sizeof(storage));
        ASSERT_EQ(Fw::FW_SERIALIZE_OK, pdu.toBuffer(buf));
        EXPECT_EQ(PduTypeEnum::FILE_DATA, peekPduType(buf));
    }

    // METADATA
    {
        MetadataPdu pdu;
        pdu.initialize(PduDirection::DIRECTION_TOWARD_RECEIVER, Cfdp::Class::CLASS_2, 1, 2, 3, 1024, "s", "d",
                       ChecksumType::CHECKSUM_TYPE_MODULAR, 0);
        Fw::Buffer buf = serializeToFwBuffer(pdu, storage, sizeof(storage));
        EXPECT_EQ(PduTypeEnum::METADATA, peekPduType(buf));
    }

    // END_OF_FILE
    {
        EofPdu pdu;
        pdu.initialize(PduDirection::DIRECTION_TOWARD_RECEIVER, Cfdp::Class::CLASS_2, 1, 2, 3,
                       ConditionCode::CONDITION_CODE_NO_ERROR, 0, 0);
        Fw::Buffer buf = serializeToFwBuffer(pdu, storage, sizeof(storage));
        EXPECT_EQ(PduTypeEnum::END_OF_FILE, peekPduType(buf));
    }

    // FINISHED
    {
        FinPdu pdu;
        pdu.initialize(PduDirection::DIRECTION_TOWARD_SENDER, Cfdp::Class::CLASS_2, 1, 2, 3,
                       ConditionCode::CONDITION_CODE_NO_ERROR, FinDeliveryCode::FIN_DELIVERY_CODE_COMPLETE,
                       FinFileStatus::FIN_FILE_STATUS_RETAINED);
        Fw::Buffer buf = serializeToFwBuffer(pdu, storage, sizeof(storage));
        EXPECT_EQ(PduTypeEnum::FINISHED, peekPduType(buf));
    }

    // ACKNOWLEDGMENT
    {
        AckPdu pdu;
        pdu.initialize(PduDirection::DIRECTION_TOWARD_SENDER, Cfdp::Class::CLASS_2, 1, 2, 3,
                       FileDirective::FILE_DIRECTIVE_END_OF_FILE, 0, ConditionCode::CONDITION_CODE_NO_ERROR,
                       AckTxnStatus::ACK_TXN_STATUS_ACTIVE);
        Fw::Buffer buf = serializeToFwBuffer(pdu, storage, sizeof(storage));
        EXPECT_EQ(PduTypeEnum::ACKNOWLEDGMENT, peekPduType(buf));
    }

    // NEGATIVE_ACK
    {
        NakPdu pdu;
        pdu.initialize(PduDirection::DIRECTION_TOWARD_SENDER, Cfdp::Class::CLASS_2, 1, 2, 3, 0, 4096);
        Fw::Buffer buf = serializeToFwBuffer(pdu, storage, sizeof(storage));
        EXPECT_EQ(PduTypeEnum::NEGATIVE_ACK, peekPduType(buf));
    }
}

TEST(PduHeaderHelper, PeekPduTypeTooShortReturnsNone) {
    U8 storage[4] = {0, 0, 0, 0};
    Fw::Buffer buf(storage, sizeof(storage));  // smaller than MIN_HEADERSIZE (7)
    EXPECT_EQ(PduTypeEnum::NONE, peekPduType(buf));
}

TEST(PduHeaderHelper, PeekPduTypeUnknownDirectiveReturnsNone) {
    // Build a valid directive PDU, then corrupt its directive code byte.
    // The directive code is the first byte after the header.
    EofPdu pdu;
    pdu.initialize(PduDirection::DIRECTION_TOWARD_RECEIVER, Cfdp::Class::CLASS_2, 1, 2, 3,
                   ConditionCode::CONDITION_CODE_NO_ERROR, 0, 0);

    U8 storage[512];
    Fw::Buffer buf = serializeToFwBuffer(pdu, storage, sizeof(storage));
    ASSERT_EQ(PduTypeEnum::END_OF_FILE, peekPduType(buf));

    // Overwrite the directive code with an unknown value
    U32 headerSize = pdu.asHeader().getBufferSize();
    ASSERT_LT(headerSize, buf.getSize());
    storage[headerSize] = 99;  // not a valid FileDirective
    EXPECT_EQ(PduTypeEnum::NONE, peekPduType(buf));
}

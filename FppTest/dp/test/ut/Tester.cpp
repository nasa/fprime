// ======================================================================
// \title  DpTest.hpp
// \author bocchino
// \brief  cpp file for DpTest test harness implementation class
// ======================================================================

#include <cstring>

#include "FppTest/dp/FppConstantsAc.hpp"
#include "FppTest/dp/test/ut/Tester.hpp"
#include "Fw/Types/ExternalString.hpp"
#include "STest/Pick/Pick.hpp"

namespace FppTest {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

Tester::Tester()
    : DpTestGTestBase("Tester", Tester::MAX_HISTORY_SIZE),
      container1Data{},
      container1Buffer(this->container1Data, sizeof this->container1Data),
      container2Data{},
      container2Buffer(this->container2Data, sizeof this->container2Data),
      container3Data{},
      container3Buffer(this->container3Data, sizeof this->container3Data),
      container4Data{},
      container4Buffer(this->container4Data, sizeof this->container4Data),
      container5Data{},
      container5Buffer(this->container5Data, sizeof this->container5Data),
      container6Data{},
      container6Buffer(this->container6Data, sizeof this->container6Data),
      container7Data{},
      container7Buffer(this->container7Data, sizeof this->container7Data),
      component("DpTest",
                STest::Pick::any(),
                STest::Pick::any(),
                this->u8ArrayRecordData,
                this->u32ArrayRecordData,
                this->dataArrayRecordData,
                this->stringRecordData) {
    this->initComponents();
    this->connectPorts();
    this->component.setIdBase(ID_BASE);
    // Fill in arrays and strings with random data
    for (U8& elt : this->u8ArrayRecordData) {
        elt = static_cast<U8>(STest::Pick::any());
    }
    for (U32& elt : this->u32ArrayRecordData) {
        elt = static_cast<U8>(STest::Pick::any());
    }
    for (DpTest_Data& elt : this->dataArrayRecordData) {
        elt.set(static_cast<U16>(STest::Pick::any()));
    }
    generateRandomString(this->stringRecordData);
}

Tester::~Tester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void Tester::schedIn_OK() {
    this->clearHistory();
    this->invoke_to_schedIn(0, 0);
    this->component.doDispatch();
    ASSERT_PRODUCT_REQUEST_SIZE(6);
    ASSERT_PRODUCT_REQUEST(0, ID_BASE + DpTest::ContainerId::Container1, FwSizeType(DpTest::CONTAINER_1_PACKET_SIZE));
    ASSERT_PRODUCT_REQUEST(1, ID_BASE + DpTest::ContainerId::Container2, FwSizeType(DpTest::CONTAINER_2_PACKET_SIZE));
    ASSERT_PRODUCT_REQUEST(2, ID_BASE + DpTest::ContainerId::Container3, FwSizeType(DpTest::CONTAINER_3_PACKET_SIZE));
    ASSERT_PRODUCT_REQUEST(3, ID_BASE + DpTest::ContainerId::Container4, FwSizeType(DpTest::CONTAINER_4_PACKET_SIZE));
    ASSERT_PRODUCT_REQUEST(4, ID_BASE + DpTest::ContainerId::Container5, FwSizeType(DpTest::CONTAINER_5_PACKET_SIZE));
    ASSERT_PRODUCT_REQUEST(5, ID_BASE + DpTest::ContainerId::Container6, FwSizeType(DpTest::CONTAINER_6_PACKET_SIZE));
    ASSERT_PRODUCT_GET_SIZE(5);
    ASSERT_PRODUCT_GET(0, ID_BASE + DpTest::ContainerId::Container1, FwSizeType(DpTest::CONTAINER_1_PACKET_SIZE));
    ASSERT_PRODUCT_GET(1, ID_BASE + DpTest::ContainerId::Container2, FwSizeType(DpTest::CONTAINER_2_PACKET_SIZE));
    ASSERT_PRODUCT_GET(2, ID_BASE + DpTest::ContainerId::Container3, FwSizeType(DpTest::CONTAINER_3_PACKET_SIZE));
    ASSERT_PRODUCT_GET(3, ID_BASE + DpTest::ContainerId::Container4, FwSizeType(DpTest::CONTAINER_4_PACKET_SIZE));
    ASSERT_PRODUCT_GET(4, ID_BASE + DpTest::ContainerId::Container5, FwSizeType(DpTest::CONTAINER_5_PACKET_SIZE));
}

void Tester::productRecvIn_Container1_SUCCESS() {
    Fw::Buffer buffer;
    FwSizeType expectedNumElts;
    // Clear the history
    this->clearHistory();
    // Check the record size
    constexpr FwSizeType recordSize = DpTestComponentBase::SIZE_OF_U32Record_RECORD;
    ASSERT_EQ(recordSize, sizeof(FwDpIdType) + sizeof(U32));
    // Invoke the port and check the header
    this->productRecvIn_InvokeAndCheckHeader(DpTest::ContainerId::Container1, sizeof(U32),
                                             DpTest::ContainerPriority::Container1, this->container1Buffer, buffer,
                                             expectedNumElts);
    // Check the data
    Fw::SerializeBufferBase& serialRepr = buffer.getSerializeRepr();
    Fw::TestUtil::DpContainerHeader::checkDeserialAtOffset(serialRepr, Fw::DpContainer::DATA_OFFSET);
    for (FwSizeType i = 0; i < expectedNumElts; ++i) {
        FwDpIdType id;
        U32 elt;
        auto status = serialRepr.deserialize(id);
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
        const FwDpIdType expectedId = this->component.getIdBase() + DpTest::RecordId::U32Record;
        ASSERT_EQ(id, expectedId);
        status = serialRepr.deserialize(elt);
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
        ASSERT_EQ(elt, this->component.u32RecordData);
    }
}

void Tester::productRecvIn_Container1_FAILURE() {
    this->clearHistory();
    productRecvIn_CheckFailure(DpTest::ContainerId::Container1, this->container1Buffer);
}

void Tester::productRecvIn_Container2_SUCCESS() {
    Fw::Buffer buffer;
    FwSizeType expectedNumElts;
    // Clear the history
    this->clearHistory();
    // Check the record size
    constexpr FwSizeType recordSize = DpTestComponentBase::SIZE_OF_DataRecord_RECORD;
    ASSERT_EQ(recordSize, sizeof(FwDpIdType) + DpTest_Data::SERIALIZED_SIZE);
    // Invoke the port and check the header
    this->productRecvIn_InvokeAndCheckHeader(DpTest::ContainerId::Container2, DpTest_Data::SERIALIZED_SIZE,
                                             DpTest::ContainerPriority::Container2, this->container2Buffer, buffer,
                                             expectedNumElts);
    // Check the data
    Fw::SerializeBufferBase& serialRepr = buffer.getSerializeRepr();
    Fw::TestUtil::DpContainerHeader::checkDeserialAtOffset(serialRepr, Fw::DpContainer::DATA_OFFSET);
    for (FwSizeType i = 0; i < expectedNumElts; ++i) {
        FwDpIdType id;
        DpTest_Data elt;
        auto status = serialRepr.deserialize(id);
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
        const FwDpIdType expectedId = this->component.getIdBase() + DpTest::RecordId::DataRecord;
        ASSERT_EQ(id, expectedId);
        status = serialRepr.deserialize(elt);
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
        ASSERT_EQ(elt.getu16Field(), this->component.dataRecordData);
    }
}

void Tester::productRecvIn_Container2_FAILURE() {
    this->clearHistory();
    productRecvIn_CheckFailure(DpTest::ContainerId::Container2, this->container2Buffer);
}

void Tester::productRecvIn_Container3_SUCCESS() {
    Fw::Buffer buffer;
    FwSizeType expectedNumElts;
    // Clear the history
    this->clearHistory();
    // Compute the data element size
    const FwSizeType arraySize = this->u8ArrayRecordData.size();
    const FwSizeType dataEltSize = sizeof(FwSizeStoreType) + arraySize;
    // Check the record size
    const FwSizeType recordSize = DpTestComponentBase::SIZE_OF_U8ArrayRecord_RECORD(arraySize);
    ASSERT_EQ(recordSize, sizeof(FwDpIdType) + dataEltSize);
    // Invoke the port and check the header
    this->productRecvIn_InvokeAndCheckHeader(DpTest::ContainerId::Container3, dataEltSize,
                                             DpTest::ContainerPriority::Container3, this->container3Buffer, buffer,
                                             expectedNumElts);

    // Check the data
    Fw::SerializeBufferBase& serialRepr = buffer.getSerializeRepr();
    Fw::TestUtil::DpContainerHeader::checkDeserialAtOffset(serialRepr, Fw::DpContainer::DATA_OFFSET);
    for (FwSizeType i = 0; i < expectedNumElts; ++i) {
        FwDpIdType id;
        auto status = serialRepr.deserialize(id);
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
        const FwDpIdType expectedId = this->component.getIdBase() + DpTest::RecordId::U8ArrayRecord;
        ASSERT_EQ(id, expectedId);
        FwSizeType size;
        status = serialRepr.deserializeSize(size);
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
        ASSERT_EQ(size, this->u8ArrayRecordData.size());
        const U8* const buffAddr = serialRepr.getBuffAddr();
        for (FwSizeType j = 0; j < size; ++j) {
            U8 byte;
            status = serialRepr.deserialize(byte);
            ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
            ASSERT_EQ(byte, this->u8ArrayRecordData.at(j));
        }
    }
}

void Tester::productRecvIn_Container3_FAILURE() {
    this->clearHistory();
    productRecvIn_CheckFailure(DpTest::ContainerId::Container3, this->container3Buffer);
}

void Tester::productRecvIn_Container4_SUCCESS() {
    Fw::Buffer buffer;
    FwSizeType expectedNumElts;
    // Clear the history
    this->clearHistory();
    // Compute the data element size
    const FwSizeType arraySize = this->u32ArrayRecordData.size();
    const FwSizeType dataEltSize = sizeof(FwSizeStoreType) + arraySize * sizeof(U32);
    // Check the record size
    const FwSizeType recordSize = DpTestComponentBase::SIZE_OF_U32ArrayRecord_RECORD(arraySize);
    ASSERT_EQ(recordSize, sizeof(FwDpIdType) + dataEltSize);
    // Invoke the port and check the header
    this->productRecvIn_InvokeAndCheckHeader(DpTest::ContainerId::Container4, dataEltSize,
                                             DpTest::ContainerPriority::Container4, this->container4Buffer, buffer,
                                             expectedNumElts);

    // Check the data
    Fw::SerializeBufferBase& serialRepr = buffer.getSerializeRepr();
    Fw::TestUtil::DpContainerHeader::checkDeserialAtOffset(serialRepr, Fw::DpContainer::DATA_OFFSET);
    for (FwSizeType i = 0; i < expectedNumElts; ++i) {
        FwDpIdType id;
        auto status = serialRepr.deserialize(id);
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
        const FwDpIdType expectedId = this->component.getIdBase() + DpTest::RecordId::U32ArrayRecord;
        ASSERT_EQ(id, expectedId);
        FwSizeType size;
        status = serialRepr.deserializeSize(size);
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
        ASSERT_EQ(size, this->u32ArrayRecordData.size());
        const U8* const buffAddr = serialRepr.getBuffAddr();
        for (FwSizeType j = 0; j < size; ++j) {
            U32 elt;
            status = serialRepr.deserialize(elt);
            ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
            ASSERT_EQ(elt, this->u32ArrayRecordData.at(j));
        }
    }
}

void Tester::productRecvIn_Container4_FAILURE() {
    productRecvIn_CheckFailure(DpTest::ContainerId::Container4, this->container4Buffer);
    this->clearHistory();
}

void Tester::productRecvIn_Container5_SUCCESS() {
    Fw::Buffer buffer;
    FwSizeType expectedNumElts;
    // Clear the history
    this->clearHistory();
    // Compute the data element size
    const FwSizeType arraySize = this->dataArrayRecordData.size();
    const FwSizeType dataEltSize = sizeof(FwSizeStoreType) + arraySize * DpTest_Data::SERIALIZED_SIZE;
    // Check the record size
    const FwSizeType recordSize = DpTestComponentBase::SIZE_OF_DataArrayRecord_RECORD(arraySize);
    ASSERT_EQ(recordSize, sizeof(FwDpIdType) + dataEltSize);
    // Invoke the port and check the header
    this->productRecvIn_InvokeAndCheckHeader(DpTest::ContainerId::Container5, dataEltSize,
                                             DpTest::ContainerPriority::Container5, this->container5Buffer, buffer,
                                             expectedNumElts);

    // Check the data
    Fw::SerializeBufferBase& serialRepr = buffer.getSerializeRepr();
    Fw::TestUtil::DpContainerHeader::checkDeserialAtOffset(serialRepr, Fw::DpContainer::DATA_OFFSET);
    for (FwSizeType i = 0; i < expectedNumElts; ++i) {
        FwDpIdType id;
        auto status = serialRepr.deserialize(id);
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
        const FwDpIdType expectedId = this->component.getIdBase() + DpTest::RecordId::DataArrayRecord;
        ASSERT_EQ(id, expectedId);
        FwSizeType size;
        status = serialRepr.deserializeSize(size);
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
        ASSERT_EQ(size, this->dataArrayRecordData.size());
        const U8* const buffAddr = serialRepr.getBuffAddr();
        for (FwSizeType j = 0; j < size; ++j) {
            DpTest_Data elt;
            status = serialRepr.deserialize(elt);
            ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
            ASSERT_EQ(elt, this->dataArrayRecordData.at(j));
        }
    }
}

void Tester::productRecvIn_Container5_FAILURE() {
    this->clearHistory();
    productRecvIn_CheckFailure(DpTest::ContainerId::Container5, this->container5Buffer);
}

void Tester::productRecvIn_Container6_SUCCESS() {
    Fw::Buffer buffer;
    FwSizeType expectedNumElts;
    // Clear the history
    this->clearHistory();
    // Check the record size
    constexpr FwSizeType recordSize = DpTestComponentBase::SIZE_OF_StringRecord_RECORD;
    ASSERT_EQ(recordSize, sizeof(FwDpIdType) + Fw::StringBase::STATIC_SERIALIZED_SIZE(DpTest_stringSize));
    // Construct the possibly truncated string
    char esData[Fw::StringBase::BUFFER_SIZE(DpTest_stringSize)];
    Fw::ExternalString es(esData, sizeof esData, this->stringRecordData);
    // Invoke the port and check the header
    this->productRecvIn_InvokeAndCheckHeader(DpTest::ContainerId::Container6, es.serializedSize(),
                                             DpTest::ContainerPriority::Container6, this->container6Buffer, buffer,
                                             expectedNumElts);
    // Check the data
    Fw::SerializeBufferBase& serialRepr = buffer.getSerializeRepr();
    Fw::TestUtil::DpContainerHeader::checkDeserialAtOffset(serialRepr, Fw::DpContainer::DATA_OFFSET);
    for (FwSizeType i = 0; i < expectedNumElts; ++i) {
        FwDpIdType id;
        Fw::String elt;
        auto status = serialRepr.deserialize(id);
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
        const FwDpIdType expectedId = this->component.getIdBase() + DpTest::RecordId::StringRecord;
        ASSERT_EQ(id, expectedId);
        status = serialRepr.deserialize(elt);
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
        ASSERT_EQ(elt, es);
    }
}

void Tester::productRecvIn_Container6_FAILURE() {
    this->clearHistory();
    productRecvIn_CheckFailure(DpTest::ContainerId::Container6, this->container6Buffer);
}

void Tester::productRecvIn_Container7_SUCCESS() {
    Fw::Buffer buffer;
    FwSizeType expectedNumElts;
    // Clear the history
    this->clearHistory();
    // Check the record size
    const FwSizeType arraySize = DpTest::STRING_ARRAY_RECORD_ARRAY_SIZE;
    const FwSizeType recordSize = DpTestComponentBase::SIZE_OF_StringArrayRecord_RECORD(arraySize);
    const FwSizeType expectedRecordSize = sizeof(FwDpIdType) + sizeof(FwSizeStoreType) +
                                          arraySize * Fw::StringBase::STATIC_SERIALIZED_SIZE(DpTest_stringSize);
    ASSERT_EQ(recordSize, expectedRecordSize);
    // Construct the possibly truncated string
    char esData[Fw::StringBase::BUFFER_SIZE(DpTest_stringSize)];
    Fw::ExternalString es(esData, sizeof esData, this->stringRecordData);
    const FwSizeType dataEltSize = sizeof(FwSizeStoreType) + arraySize * es.serializedSize();
    // Invoke the port and check the header
    this->productRecvIn_InvokeAndCheckHeader(DpTest::ContainerId::Container7, dataEltSize,
                                             DpTest::ContainerPriority::Container7, this->container7Buffer, buffer,
                                             expectedNumElts);
    // Check the data
    Fw::SerializeBufferBase& serialRepr = buffer.getSerializeRepr();
    Fw::TestUtil::DpContainerHeader::checkDeserialAtOffset(serialRepr, Fw::DpContainer::DATA_OFFSET);
    for (FwSizeType i = 0; i < expectedNumElts; ++i) {
        FwDpIdType id;
        auto status = serialRepr.deserialize(id);
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
        const FwDpIdType expectedId = this->component.getIdBase() + DpTest::RecordId::StringArrayRecord;
        ASSERT_EQ(id, expectedId);
        FwSizeType size;
        status = serialRepr.deserializeSize(size);
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
        ASSERT_EQ(size, arraySize);
        const U8* const buffAddr = serialRepr.getBuffAddr();
        for (FwSizeType j = 0; j < size; ++j) {
            Fw::String elt;
            status = serialRepr.deserialize(elt);
            ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
            ASSERT_EQ(elt, es);
        }
    }
}

void Tester::productRecvIn_Container7_FAILURE() {
    this->clearHistory();
    productRecvIn_CheckFailure(DpTest::ContainerId::Container7, this->container7Buffer);
}

// ----------------------------------------------------------------------
// Helper methods
// ----------------------------------------------------------------------

Fw::Time Tester::randomizeTestTime() {
    const U32 seconds = STest::Pick::any();
    const U32 useconds = STest::Pick::startLength(0, 1000000);
    const Fw::Time time(seconds, useconds);
    this->setTestTime(time);
    this->component.setSendTime(time);
    return time;
}

void Tester::generateRandomString(Fw::StringBase& str) {
    char buffer[Fw::StringBase::BUFFER_SIZE(MAX_STRING_LENGTH)];
    // Pick a random string length
    const FwSizeType length = STest::Pick::lowerUpper(0, MAX_STRING_LENGTH);
    // Fill buffer with a random null-terminated string with that length
    FwSizeType i = 0;
    for (; i < length; i++) {
        U32 u32 = STest::Pick::lowerUpper(1, std::numeric_limits<char>::max());
        buffer[i] = static_cast<char>(u32);
    }
    FW_ASSERT(i <= sizeof buffer, static_cast<FwAssertArgType>(i), static_cast<FwAssertArgType>(MAX_STRING_LENGTH));
    buffer[i] = 0;
    // Copy the contents of buffer into str
    str.format("%s", buffer);
}

void Tester::productRecvIn_InvokeAndCheckHeader(FwDpIdType id,
                                                FwSizeType dataEltSize,
                                                FwDpPriorityType priority,
                                                Fw::Buffer inputBuffer,
                                                Fw::Buffer& outputBuffer,
                                                FwSizeType& expectedNumElts) {
    const auto globalId = ID_BASE + id;
    // Set the test time
    const Fw::Time timeTag = this->randomizeTestTime();
    // Invoke the productRecvIn port
    this->sendProductResponse(globalId, inputBuffer, Fw::Success::SUCCESS);
    this->component.doDispatch();
    // Check the port history size
    ASSERT_PRODUCT_SEND_SIZE(1);
    // Compute the expected data size
    const auto& entry = this->productSendHistory->at(0);
    const auto bufferSize = entry.buffer.getSize();
    FW_ASSERT(bufferSize >= Fw::DpContainer::MIN_PACKET_SIZE);
    const auto dataCapacity = bufferSize - Fw::DpContainer::MIN_PACKET_SIZE;
    const auto eltSize = sizeof(FwDpIdType) + dataEltSize;
    expectedNumElts = dataCapacity / eltSize;
    const auto expectedDataSize = expectedNumElts * eltSize;
    // DP state should be the default value
    Fw::DpState dpState;
    // Set up the expected user data
    Fw::DpContainer::Header::UserData userData;
    memset(&userData[0], 0, sizeof userData);
    // Check the history entry
    // This sets the output buffer and sets the deserialization pointer
    // to the start of the data payload
    ASSERT_PRODUCT_SEND(0, globalId, priority, timeTag, 0, userData, dpState, expectedDataSize, outputBuffer);
}

void Tester::productRecvIn_CheckFailure(FwDpIdType id, Fw::Buffer buffer) {
    // Invoke the port
    const auto globalId = ID_BASE + id;
    this->sendProductResponse(globalId, buffer, Fw::Success::FAILURE);
    this->component.doDispatch();
    // Check the port history size
    ASSERT_PRODUCT_SEND_SIZE(0);
}

// ----------------------------------------------------------------------
// Handlers for typed from ports
// ----------------------------------------------------------------------

Fw::Success::T Tester::productGet_handler(FwDpIdType id, FwSizeType size, Fw::Buffer& buffer) {
    this->pushProductGetEntry(id, size);
    Fw::Success status = Fw::Success::FAILURE;
    FW_ASSERT(id >= ID_BASE, id, ID_BASE);
    const FwDpIdType localId = id - ID_BASE;
    switch (localId) {
        case DpTest::ContainerId::Container1:
            FW_ASSERT(size == DpTest::CONTAINER_1_PACKET_SIZE);
            buffer = this->container1Buffer;
            status = Fw::Success::SUCCESS;
            break;
        case DpTest::ContainerId::Container2:
            FW_ASSERT(size == DpTest::CONTAINER_2_PACKET_SIZE);
            buffer = this->container2Buffer;
            status = Fw::Success::SUCCESS;
            break;
        case DpTest::ContainerId::Container3:
            // Make this one fail for testing purposes
            break;
        case DpTest::ContainerId::Container4:
            FW_ASSERT(size == DpTest::CONTAINER_4_PACKET_SIZE);
            buffer = this->container4Buffer;
            status = Fw::Success::SUCCESS;
            break;
        case DpTest::ContainerId::Container5:
            FW_ASSERT(size == DpTest::CONTAINER_5_PACKET_SIZE);
            buffer = this->container5Buffer;
            status = Fw::Success::SUCCESS;
            break;
        default:
            break;
    }
    return status;
}

}  // end namespace FppTest

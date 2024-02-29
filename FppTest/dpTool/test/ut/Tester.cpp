// ======================================================================
// \title  DpTool.hpp
// \author bocchino
// \brief  cpp file for DpTool test harness implementation class
// ======================================================================

#include <cstdio>
#include <cstring>

#include "FppTest/dpTool/test/ut/Tester.hpp"
#include "STest/Pick/Pick.hpp"
#include <fstream>
#include <iostream>

namespace FppTest {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

Tester::Tester()
    : DpToolGTestBase("Tester", Tester::MAX_HISTORY_SIZE),
        container1Data{},
        container1Buffer(this->container1Data, sizeof this->container1Data),

        // Fill in dpTool_Data
        component("DpTool") {
        this->initComponents();
        this->connectPorts();
        this->component.setIdBase(ID_BASE);
}

Tester::~Tester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void Tester::makeU32() {
     Fw::Buffer buffer;
    FwSizeType expectedNumElts;
    const FwSizeType dataEltSize = sizeof(U32);
    // Invoke the port and check the header
    this->component.recordTest = U32Record;
    this->productRecvIn_InvokeAndCheckHeader(DpTool::ContainerId::Container1, dataEltSize,
                                             DpTool::ContainerPriority::Container1, this->container1Buffer, buffer,
                                             expectedNumElts);


    writeBinaryFile("makeU32.bin", buffer);

}

void Tester::makeComplex() {
     Fw::Buffer buffer;
    FwSizeType expectedNumElts;
    const FwSizeType dataEltSize = DpTool_Complex::SERIALIZED_SIZE;
    // Invoke the port and check the header
    this->component.recordTest = ComplexRecord;
    this->productRecvIn_InvokeAndCheckHeader(DpTool::ContainerId::Container1, dataEltSize,
                                             DpTool::ContainerPriority::Container1, this->container1Buffer, buffer,
                                             expectedNumElts);

    writeBinaryFile("makeComplex.bin", buffer);

}

void Tester::makeU8Array() {
    Fw::Buffer buffer;
    FwSizeType expectedNumElts;
    const FwSizeType dataEltSize = sizeof(FwSizeType) + DpTool::RECORD_ARRAY_SIZE * sizeof(U8);
    // Invoke the port and check the header
    this->component.recordTest = U8ArrayRecord;
    this->productRecvIn_InvokeAndCheckHeader(DpTool::ContainerId::Container1, dataEltSize,
                                             DpTool::ContainerPriority::Container1, this->container1Buffer, buffer,
                                             expectedNumElts);

    writeBinaryFile("makeU8Array.bin", buffer);

}

void Tester::makeU32Array() {
    Fw::Buffer buffer;
    FwSizeType expectedNumElts;
    const FwSizeType dataEltSize = sizeof(FwSizeType) + DpTool::RECORD_ARRAY_SIZE * sizeof(U32);

    // Invoke the port and check the header
    this->component.recordTest = U32ArrayRecord;
    this->productRecvIn_InvokeAndCheckHeader(DpTool::ContainerId::Container1, dataEltSize,
                                             DpTool::ContainerPriority::Container1, this->container1Buffer, buffer,
                                             expectedNumElts);

    writeBinaryFile("makeU32Array.bin", buffer);

}

void Tester::makeDataArray() {
     Fw::Buffer buffer;
    FwSizeType expectedNumElts;
    const FwSizeType dataEltSize = sizeof(FwSizeType) + DpTool::RECORD_ARRAY_SIZE * DpTool_Data::SERIALIZED_SIZE;
    // Invoke the port and check the header
    this->component.recordTest = DataArrayRecord;
    this->productRecvIn_InvokeAndCheckHeader(DpTool::ContainerId::Container1, dataEltSize,
                                             DpTool::ContainerPriority::Container1, this->container1Buffer, buffer,
                                             expectedNumElts);

    writeBinaryFile("makeDataArray.bin", buffer);

}

void Tester::makeEnum() {
     Fw::Buffer buffer;
    FwSizeType expectedNumElts;

    const FwSizeType dataEltSize = DpTool_EnumStruct::SERIALIZED_SIZE;
    // Invoke the port and check the header
    this->component.recordTest = EnumRecord;
    this->productRecvIn_InvokeAndCheckHeader(DpTool::ContainerId::Container1, dataEltSize,
                                             DpTool::ContainerPriority::Container1, this->container1Buffer, buffer,
                                             expectedNumElts);

    writeBinaryFile("makeEnum.bin", buffer);

}

void Tester::makeBool() {
     Fw::Buffer buffer;
    FwSizeType expectedNumElts;

    const FwSizeType dataEltSize = sizeof(bool);
    // Invoke the port and check the header
    this->component.recordTest = BoolRecord;
    this->productRecvIn_InvokeAndCheckHeader(DpTool::ContainerId::Container1, dataEltSize,
                                             DpTool::ContainerPriority::Container1, this->container1Buffer, buffer,
                                             expectedNumElts);

    writeBinaryFile("makeBool.bin", buffer);

}

void Tester::makeF32() {
     Fw::Buffer buffer;
    FwSizeType expectedNumElts;

    const FwSizeType dataEltSize = sizeof(F32);
    // Invoke the port and check the header
    this->component.recordTest = F32Record;
    this->productRecvIn_InvokeAndCheckHeader(DpTool::ContainerId::Container1, dataEltSize,
                                             DpTool::ContainerPriority::Container1, this->container1Buffer, buffer,
                                             expectedNumElts);

    writeBinaryFile("makeF32.bin", buffer);

}

void Tester::makeF64() {
     Fw::Buffer buffer;
    FwSizeType expectedNumElts;

    const FwSizeType dataEltSize = sizeof(F64);
    // Invoke the port and check the header
    this->component.recordTest = F64Record;
    this->productRecvIn_InvokeAndCheckHeader(DpTool::ContainerId::Container1, dataEltSize,
                                             DpTool::ContainerPriority::Container1, this->container1Buffer, buffer,
                                             expectedNumElts);

    writeBinaryFile("makeF64.bin", buffer);

}

void Tester::makeI8() {
     Fw::Buffer buffer;
    FwSizeType expectedNumElts;
    const FwSizeType dataEltSize = sizeof(I8);
    // Invoke the port and check the header
    this->component.recordTest = I8Record;
    this->productRecvIn_InvokeAndCheckHeader(DpTool::ContainerId::Container1, dataEltSize,
                                             DpTool::ContainerPriority::Container1, this->container1Buffer, buffer,
                                             expectedNumElts);


    writeBinaryFile("makeI8.bin", buffer);

}

void Tester::makeI16() {
     Fw::Buffer buffer;
    FwSizeType expectedNumElts;
    const FwSizeType dataEltSize = sizeof(I16);
    // Invoke the port and check the header
    this->component.recordTest = I16Record;
    this->productRecvIn_InvokeAndCheckHeader(DpTool::ContainerId::Container1, dataEltSize,
                                             DpTool::ContainerPriority::Container1, this->container1Buffer, buffer,
                                             expectedNumElts);


    writeBinaryFile("makeI16.bin", buffer);

}

void Tester::makeI32() {
     Fw::Buffer buffer;
    FwSizeType expectedNumElts;
    const FwSizeType dataEltSize = sizeof(I32);
    // Invoke the port and check the header
    this->component.recordTest = I32Record;
    this->productRecvIn_InvokeAndCheckHeader(DpTool::ContainerId::Container1, dataEltSize,
                                             DpTool::ContainerPriority::Container1, this->container1Buffer, buffer,
                                             expectedNumElts);


    writeBinaryFile("makeI32.bin", buffer);

}

void Tester::makeI64() {
     Fw::Buffer buffer;
    FwSizeType expectedNumElts;
    const FwSizeType dataEltSize = sizeof(I64);
    // Invoke the port and check the header
    this->component.recordTest = I64Record;
    this->productRecvIn_InvokeAndCheckHeader(DpTool::ContainerId::Container1, dataEltSize,
                                             DpTool::ContainerPriority::Container1, this->container1Buffer, buffer,
                                             expectedNumElts);


    writeBinaryFile("makeI64.bin", buffer);

}

void Tester::makeFppArray() {
     Fw::Buffer buffer;
    FwSizeType expectedNumElts;
    const FwSizeType dataEltSize = DpTool_ArrayData::SERIALIZED_SIZE;
    // Invoke the port and check the header
    this->component.recordTest = FppArrayRecord;
    this->productRecvIn_InvokeAndCheckHeader(DpTool::ContainerId::Container1, dataEltSize,
                                             DpTool::ContainerPriority::Container1, this->container1Buffer, buffer,
                                             expectedNumElts);


    writeBinaryFile("makeFppArray.bin", buffer);

}


void Tester::schedIn_OK() {
    this->invoke_to_schedIn(0, 0);
    this->component.doDispatch();
}

// ----------------------------------------------------------------------
// Helper methods
// ----------------------------------------------------------------------

void Tester::writeBinaryFile(const char *fileName, Fw::Buffer buffer) {
    
     std::string fullPath = "../../../dpTool/" + std::string(fileName);
    // Open a file in binary mode
    std::ofstream file(fullPath, std::ios::out | std::ios::binary);
    if (!file) {
        printf("Cannot open file\n");
        return;
    }

    // Write the binary data to the file
    file.write(reinterpret_cast<const char*>(buffer.getData()), buffer.getSize());

    // Close the file
    file.close();
}

Fw::Time Tester::randomizeTestTime() {
    const U32 seconds = STest::Pick::any();
    const U32 useconds = STest::Pick::startLength(0, 1000000);
    const Fw::Time time(seconds, useconds);
    this->setTestTime(time);
    this->component.setSendTime(time);
    return time;
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
    return status;
}

}  // end namespace FppTest

// ======================================================================
// \title  DpTest.cpp
// \author bocchino
// \brief  cpp file for DpTest component implementation class
// ======================================================================

#include <cstdio>

#include "FppTest/dp/DpTest.hpp"
#include "Fw/Types/Assert.hpp"

namespace FppTest {

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

DpTest::DpTest(const char* const compName,
               U32 u32RecordData,
               U16 dataRecordData,
               const U8ArrayRecordData& u8ArrayRecordData,
               const U32ArrayRecordData& u32ArrayRecordData,
               const DataArrayRecordData& dataArrayRecordData,
               const Fw::StringBase& a_stringRecordData)
    : DpTestComponentBase(compName),
      m_container(),
      u32RecordData(u32RecordData),
      dataRecordData(dataRecordData),
      u8ArrayRecordData(u8ArrayRecordData),
      u32ArrayRecordData(u32ArrayRecordData),
      dataArrayRecordData(dataArrayRecordData),
      stringRecordData(a_stringRecordData),
      sendTime(Fw::ZERO_TIME) {
    for (auto& elt : this->stringArrayRecordData) {
        elt = &a_stringRecordData;
    }
}

DpTest ::~DpTest() {}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void DpTest::schedIn_handler(const NATIVE_INT_TYPE portNum, U32 context) {
    // Request a buffer for Container 1
    this->dpRequest_Container1(CONTAINER_1_DATA_SIZE);
    // Request a buffer for Container 2
    this->dpRequest_Container2(CONTAINER_2_DATA_SIZE);
    // Request a buffer for Container 3
    this->dpRequest_Container3(CONTAINER_3_DATA_SIZE);
    // Request a buffer for Container 4
    this->dpRequest_Container4(CONTAINER_4_DATA_SIZE);
    // Request a buffer for Container 5
    this->dpRequest_Container5(CONTAINER_5_DATA_SIZE);
    // Request a buffer for Container 6
    this->dpRequest_Container6(CONTAINER_6_DATA_SIZE);
    // Get a buffer for Container 1
    {
        Fw::Success status = this->dpGet_Container1(CONTAINER_1_DATA_SIZE, this->m_container);
        FW_ASSERT(status == Fw::Success::SUCCESS, status);
        // Check the container
        this->checkContainer(this->m_container, ContainerId::Container1, CONTAINER_1_PACKET_SIZE,
                             DpTest::ContainerPriority::Container1);
    }
    // Get a buffer for Container 2
    {
        Fw::Success status = this->dpGet_Container2(CONTAINER_2_DATA_SIZE, this->m_container);
        FW_ASSERT(status == Fw::Success::SUCCESS);
        // Check the container
        this->checkContainer(this->m_container, ContainerId::Container2, CONTAINER_2_PACKET_SIZE,
                             DpTest::ContainerPriority::Container2);
    }
    // Get a buffer for Container 3
    {
        Fw::Success status = this->dpGet_Container3(CONTAINER_3_DATA_SIZE, this->m_container);
        // This one should fail
        FW_ASSERT(status == Fw::Success::FAILURE);
    }
    // Get a buffer for Container 4
    {
        Fw::Success status = this->dpGet_Container4(CONTAINER_4_DATA_SIZE, this->m_container);
        FW_ASSERT(status == Fw::Success::SUCCESS);
        // Check the container
        this->checkContainer(this->m_container, ContainerId::Container4, CONTAINER_4_PACKET_SIZE,
                             DpTest::ContainerPriority::Container4);
    }
    // Get a buffer for Container 5
    {
        Fw::Success status = this->dpGet_Container5(CONTAINER_5_DATA_SIZE, this->m_container);
        FW_ASSERT(status == Fw::Success::SUCCESS);
        // Check the container
        this->checkContainer(this->m_container, ContainerId::Container5, CONTAINER_5_PACKET_SIZE,
                             DpTest::ContainerPriority::Container5);
    }
}

// ----------------------------------------------------------------------
// Data product handler implementations
// ----------------------------------------------------------------------

void DpTest ::dpRecv_Container1_handler(DpContainer& container, Fw::Success::T status) {
    // Test container assignment
    this->m_container = container;
    this->checkContainerEmpty(this->m_container);
    if (status == Fw::Success::SUCCESS) {
        auto serializeStatus = Fw::FW_SERIALIZE_OK;
        for (FwSizeType i = 0; i < CONTAINER_1_DATA_SIZE; ++i) {
            serializeStatus = this->m_container.serializeRecord_U32Record(this->u32RecordData);
            if (serializeStatus == Fw::FW_SERIALIZE_NO_ROOM_LEFT) {
                break;
            }
            FW_ASSERT(serializeStatus == Fw::FW_SERIALIZE_OK, status);
        }
        // Use the time stamp from the time get port
        this->dpSend(this->m_container);
        // Check that buffer is no longer valid
        FW_ASSERT(!this->m_container.getBuffer().isValid());
    }
}

void DpTest ::dpRecv_Container2_handler(DpContainer& container, Fw::Success::T status) {
    // Test container assignment
    this->m_container = container;
    this->checkContainerEmpty(this->m_container);
    if (status == Fw::Success::SUCCESS) {
        const DpTest_Data dataRecord(this->dataRecordData);
        auto serializeStatus = Fw::FW_SERIALIZE_OK;
        for (FwSizeType i = 0; i < CONTAINER_2_DATA_SIZE; ++i) {
            serializeStatus = this->m_container.serializeRecord_DataRecord(dataRecord);
            if (serializeStatus == Fw::FW_SERIALIZE_NO_ROOM_LEFT) {
                break;
            }
            FW_ASSERT(serializeStatus == Fw::FW_SERIALIZE_OK, status);
        }
        // Provide an explicit time stamp
        this->dpSend(this->m_container, this->sendTime);
        // Check that buffer is no longer valid
        FW_ASSERT(!this->m_container.getBuffer().isValid());
    }
}

void DpTest ::dpRecv_Container3_handler(DpContainer& container, Fw::Success::T status) {
    // Test container assignment
    this->m_container = container;
    this->checkContainerEmpty(this->m_container);
    if (status == Fw::Success::SUCCESS) {
        auto serializeStatus = Fw::FW_SERIALIZE_OK;
        for (FwSizeType i = 0; i < CONTAINER_3_DATA_SIZE; ++i) {
            serializeStatus = this->m_container.serializeRecord_U8ArrayRecord(this->u8ArrayRecordData.data(),
                                                                              this->u8ArrayRecordData.size());
            if (serializeStatus == Fw::FW_SERIALIZE_NO_ROOM_LEFT) {
                break;
            }
            FW_ASSERT(serializeStatus == Fw::FW_SERIALIZE_OK, status);
        }
        // Use the time stamp from the time get port
        this->dpSend(this->m_container);
        // Check that buffer is no longer valid
        FW_ASSERT(!this->m_container.getBuffer().isValid());
    }
}

void DpTest ::dpRecv_Container4_handler(DpContainer& container, Fw::Success::T status) {
    // Test container assignment
    this->m_container = container;
    this->checkContainerEmpty(this->m_container);
    if (status == Fw::Success::SUCCESS) {
        auto serializeStatus = Fw::FW_SERIALIZE_OK;
        for (FwSizeType i = 0; i < CONTAINER_4_DATA_SIZE; ++i) {
            serializeStatus = this->m_container.serializeRecord_U32ArrayRecord(this->u32ArrayRecordData.data(),
                                                                               this->u32ArrayRecordData.size());
            if (serializeStatus == Fw::FW_SERIALIZE_NO_ROOM_LEFT) {
                break;
            }
            FW_ASSERT(serializeStatus == Fw::FW_SERIALIZE_OK, status);
        }
        // Use the time stamp from the time get port
        this->dpSend(this->m_container);
        // Check that buffer is no longer valid
        FW_ASSERT(!this->m_container.getBuffer().isValid());
    }
}

void DpTest ::dpRecv_Container5_handler(DpContainer& container, Fw::Success::T status) {
    // Test container assignment
    this->m_container = container;
    this->checkContainerEmpty(this->m_container);
    if (status == Fw::Success::SUCCESS) {
        auto serializeStatus = Fw::FW_SERIALIZE_OK;
        for (FwSizeType i = 0; i < CONTAINER_5_DATA_SIZE; ++i) {
            serializeStatus = this->m_container.serializeRecord_DataArrayRecord(this->dataArrayRecordData.data(),
                                                                                this->dataArrayRecordData.size());
            if (serializeStatus == Fw::FW_SERIALIZE_NO_ROOM_LEFT) {
                break;
            }
            FW_ASSERT(serializeStatus == Fw::FW_SERIALIZE_OK, status);
        }
        // Use the time stamp from the time get port
        this->dpSend(this->m_container);
        // Check that buffer is no longer valid
        FW_ASSERT(!this->m_container.getBuffer().isValid());
    }
}

void DpTest ::dpRecv_Container6_handler(DpContainer& container, Fw::Success::T status) {
    // Test container assignment
    this->m_container = container;
    this->checkContainerEmpty(this->m_container);
    if (status == Fw::Success::SUCCESS) {
        auto serializeStatus = Fw::FW_SERIALIZE_OK;
        for (FwSizeType i = 0; i < CONTAINER_6_DATA_SIZE; ++i) {
            serializeStatus = this->m_container.serializeRecord_StringRecord(this->stringRecordData);
            if (serializeStatus == Fw::FW_SERIALIZE_NO_ROOM_LEFT) {
                break;
            }
            FW_ASSERT(serializeStatus == Fw::FW_SERIALIZE_OK, status);
        }
        // Use the time stamp from the time get port
        this->dpSend(this->m_container);
        // Check that buffer is no longer valid
        FW_ASSERT(!this->m_container.getBuffer().isValid());
    }
}

void DpTest ::dpRecv_Container7_handler(DpContainer& container, Fw::Success::T status) {
    // Test container assignment
    this->m_container = container;
    this->checkContainerEmpty(this->m_container);
    if (status == Fw::Success::SUCCESS) {
        auto serializeStatus = Fw::FW_SERIALIZE_OK;
        for (FwSizeType i = 0; i < CONTAINER_7_DATA_SIZE; ++i) {
            serializeStatus = this->m_container.serializeRecord_StringArrayRecord(
                this->stringArrayRecordData, FW_NUM_ARRAY_ELEMENTS(this->stringArrayRecordData));
            if (serializeStatus == Fw::FW_SERIALIZE_NO_ROOM_LEFT) {
                break;
            }
            FW_ASSERT(serializeStatus == Fw::FW_SERIALIZE_OK, status);
        }
        // Use the time stamp from the time get port
        this->dpSend(this->m_container);
        // Check that buffer is no longer valid
        FW_ASSERT(!this->m_container.getBuffer().isValid());
    }
}

// ----------------------------------------------------------------------
// Private helper functions
// ----------------------------------------------------------------------

void DpTest::checkContainerEmpty(const DpContainer& container) const {
    const FwSizeType dataSize = container.getDataSize();
    FW_ASSERT(dataSize == 0, static_cast<FwAssertArgType>(dataSize));
    const Fw::SerializeBufferBase& buffer = container.m_dataBuffer;
    const FwSizeType buffLength = buffer.getBuffLength();
    FW_ASSERT(buffLength == 0, static_cast<FwAssertArgType>(buffLength));
    const FwSizeType buffLeft = buffer.getBuffLeft();
    FW_ASSERT(buffLeft == 0, static_cast<FwAssertArgType>(buffLeft));
}

void DpTest::checkContainer(const DpContainer& container,
                            FwDpIdType localId,
                            FwSizeType size,
                            FwDpPriorityType priority) const {
    this->checkContainerEmpty(container);
    FW_ASSERT(container.getBaseId() == this->getIdBase(), container.getBaseId(), this->getIdBase());
    FW_ASSERT(container.getId() == container.getBaseId() + localId, container.getId(), container.getBaseId(),
              ContainerId::Container1);
    FW_ASSERT(container.getBuffer().getSize() == size, container.getBuffer().getSize(), size);
    FW_ASSERT(container.getPriority() == priority, container.getPriority(), priority);
}

}  // end namespace FppTest

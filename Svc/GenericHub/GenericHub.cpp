// ======================================================================
// \title  GenericHub.cpp
// \author mstarch
// \brief  cpp file for GenericHub component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <Fw/FPrimeBasicTypes.hpp>
#include <Svc/GenericHub/GenericHub.hpp>
#include "Fw/Logger/Logger.hpp"
#include "Fw/Types/Assert.hpp"

// Required port serialization or the hub cannot work
static_assert(FW_PORT_SERIALIZATION, "FW_PORT_SERIALIZATION must be enabled to use GenericHub");

namespace Svc {

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

GenericHub::GenericHub(const char* const compName) : GenericHubComponentBase(compName) {}

GenericHub::~GenericHub() {}

void GenericHub::send_data(const HubType type, const FwIndexType port, const U8* data, const FwSizeType size) {
    FW_ASSERT(data != nullptr);
    Fw::SerializeStatus status;
    // Buffer to send and a buffer used to write to it
    Fw::Buffer outgoing = allocate_out(0, static_cast<U32>(size + sizeof(U32) + sizeof(U32) + sizeof(FwBuffSizeType)));
    auto serialize = outgoing.getSerializer();
    // Write data to our buffer
    status = serialize.serializeFrom(static_cast<U32>(type));
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(status));
    status = serialize.serializeFrom(static_cast<U32>(port));
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(status));
    status = serialize.serializeFrom(data, size);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(status));
    outgoing.setSize(static_cast<U32>(serialize.getSize()));
    toBufferDriver_out(0, outgoing);
}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void GenericHub::bufferIn_handler(const FwIndexType portNum, Fw::Buffer& fwBuffer) {
    send_data(HUB_TYPE_BUFFER, portNum, fwBuffer.getData(), fwBuffer.getSize());
    bufferInReturn_out(portNum, fwBuffer);
}

void GenericHub::bufferOutReturn_handler(FwIndexType portNum, Fw::Buffer& fwBuffer) {
    // Return the buffer
    fromBufferDriverReturn_out(0, fwBuffer);
}

void GenericHub::fromBufferDriver_handler(const FwIndexType portNum, Fw::Buffer& fwBuffer) {
    HubType type = HUB_TYPE_MAX;
    U32 type_in = 0;
    U32 port = 0;
    FwBuffSizeType size = 0;
    Fw::SerializeStatus status = Fw::FW_SERIALIZE_OK;

    // Representation of incoming data prepped for serialization
    auto incoming = fwBuffer.getDeserializer();
    status = incoming.deserializeTo(type_in);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(status));
    type = static_cast<HubType>(type_in);
    FW_ASSERT(type < HUB_TYPE_MAX, type);
    status = incoming.deserializeTo(port);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(status));
    status = incoming.deserializeTo(size);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(status));

    // invokeSerial deserializes arguments before calling a normal invoke, this will return ownership immediately
    U8* rawData = fwBuffer.getData() + sizeof(U32) + sizeof(U32) + sizeof(FwBuffSizeType);
    U32 rawSize = static_cast<U32>(fwBuffer.getSize() - sizeof(U32) - sizeof(U32) - sizeof(FwBuffSizeType));
    FW_ASSERT(rawSize == static_cast<U32>(size));
    if (type == HUB_TYPE_PORT) {
        // Com buffer representations should be copied before the call returns, so we need not "allocate" new data
        Fw::ExternalSerializeBuffer wrapper(rawData, rawSize);
        status = wrapper.setBuffLen(rawSize);
        FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(status));
        serialOut_out(static_cast<FwIndexType>(port), wrapper);
        // Deallocate the existing buffer
        fromBufferDriverReturn_out(0, fwBuffer);
    } else if (type == HUB_TYPE_BUFFER) {
        // Fw::Buffers can reuse the existing data buffer as the storage type!  No deallocation done.
        fwBuffer.set(rawData, rawSize, fwBuffer.getContext());
        bufferOut_out(static_cast<FwIndexType>(port), fwBuffer);
    } else if (type == HUB_TYPE_EVENT) {
        FwEventIdType id;
        Fw::Time timeTag;
        Fw::LogSeverity severity;
        Fw::LogBuffer args;

        // Deserialize tokens for events
        status = incoming.deserializeTo(id);
        FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(status));
        status = incoming.deserializeTo(timeTag);
        FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(status));
        status = incoming.deserializeTo(severity);
        FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(status));
        status = incoming.deserializeTo(args);
        FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(status));

        // Send it!
        this->eventOut_out(static_cast<FwIndexType>(port), id, timeTag, severity, args);

        // Deallocate the existing buffer
        fromBufferDriverReturn_out(0, fwBuffer);
    } else if (type == HUB_TYPE_CHANNEL) {
        FwChanIdType id;
        Fw::Time timeTag;
        Fw::TlmBuffer val;

        // Deserialize tokens for channels
        status = incoming.deserializeTo(id);
        FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(status));
        status = incoming.deserializeTo(timeTag);
        FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(status));
        status = incoming.deserializeTo(val);
        FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(status));

        // Send it!
        this->tlmOut_out(static_cast<FwIndexType>(port), id, timeTag, val);

        // Return the received buffer
        fromBufferDriverReturn_out(0, fwBuffer);
    }
}

void GenericHub::toBufferDriverReturn_handler(FwIndexType portNum, Fw::Buffer& fwBuffer) {
    // Deallocate the existing buffer
    deallocate_out(portNum, fwBuffer);
}

void GenericHub::eventIn_handler(const FwIndexType portNum,
                                 FwEventIdType id,
                                 Fw::Time& timeTag,
                                 const Fw::LogSeverity& severity,
                                 Fw::LogBuffer& args) {
    Fw::SerializeStatus status = Fw::FW_SERIALIZE_OK;
    U8 buffer[sizeof(FwEventIdType) + Fw::Time::SERIALIZED_SIZE + Fw::LogSeverity::SERIALIZED_SIZE +
              FW_LOG_BUFFER_MAX_SIZE];
    Fw::ExternalSerializeBuffer serializer(buffer, sizeof(buffer));
    serializer.resetSer();
    status = serializer.serializeFrom(id);
    FW_ASSERT(status == Fw::SerializeStatus::FW_SERIALIZE_OK);
    status = serializer.serializeFrom(timeTag);
    FW_ASSERT(status == Fw::SerializeStatus::FW_SERIALIZE_OK);
    status = serializer.serializeFrom(severity);
    FW_ASSERT(status == Fw::SerializeStatus::FW_SERIALIZE_OK);
    status = serializer.serializeFrom(args);
    FW_ASSERT(status == Fw::SerializeStatus::FW_SERIALIZE_OK);
    FwSizeType size = serializer.getSize();
    this->send_data(HubType::HUB_TYPE_EVENT, portNum, buffer, size);
}

void GenericHub::tlmIn_handler(const FwIndexType portNum, FwChanIdType id, Fw::Time& timeTag, Fw::TlmBuffer& val) {
    Fw::SerializeStatus status = Fw::FW_SERIALIZE_OK;
    U8 buffer[sizeof(FwChanIdType) + Fw::Time::SERIALIZED_SIZE + FW_TLM_BUFFER_MAX_SIZE];
    Fw::ExternalSerializeBuffer serializer(buffer, sizeof(buffer));
    serializer.resetSer();
    status = serializer.serializeFrom(id);
    FW_ASSERT(status == Fw::SerializeStatus::FW_SERIALIZE_OK);
    status = serializer.serializeFrom(timeTag);
    FW_ASSERT(status == Fw::SerializeStatus::FW_SERIALIZE_OK);
    status = serializer.serializeFrom(val);
    FW_ASSERT(status == Fw::SerializeStatus::FW_SERIALIZE_OK);
    FwSizeType size = serializer.getSize();
    this->send_data(HubType::HUB_TYPE_CHANNEL, portNum, buffer, size);
}

// ----------------------------------------------------------------------
// Handler implementations for user-defined serial input ports
// ----------------------------------------------------------------------

void GenericHub::serialIn_handler(FwIndexType portNum,            /*!< The port number*/
                                  Fw::SerializeBufferBase& Buffer /*!< The serialization buffer*/
) {
    send_data(HUB_TYPE_PORT, portNum, Buffer.getBuffAddr(), Buffer.getSize());
}

}  // end namespace Svc

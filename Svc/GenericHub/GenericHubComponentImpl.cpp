// ======================================================================
// \title  GenericHubComponentImpl.cpp
// \author mstarch
// \brief  cpp file for GenericHub component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <FpConfig.hpp>
#include <Svc/GenericHub/GenericHubComponentImpl.hpp>
#include "Fw/Logger/Logger.hpp"
#include "Fw/Types/Assert.hpp"

// Required port serialization or the hub cannot work
static_assert(FW_PORT_SERIALIZATION, "FW_PORT_SERIALIZATION must be enabled to use GenericHub");

namespace Svc {

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

GenericHubComponentImpl ::GenericHubComponentImpl(const char* const compName) : GenericHubComponentBase(compName) {}

void GenericHubComponentImpl ::init(const NATIVE_INT_TYPE instance) {
    GenericHubComponentBase::init(instance);
}

GenericHubComponentImpl ::~GenericHubComponentImpl() {}

void GenericHubComponentImpl ::send_data(const HubType type,
                                         const NATIVE_INT_TYPE port,
                                         const U8* data,
                                         const U32 size) {
    FW_ASSERT(data != nullptr);
    Fw::SerializeStatus status;
    // Buffer to send and a buffer used to write to it
    Fw::Buffer outgoing = dataOutAllocate_out(0, size + sizeof(U32) + sizeof(U32) + sizeof(FwBuffSizeType));
    Fw::SerializeBufferBase& serialize = outgoing.getSerializeRepr();
    // Write data to our buffer
    status = serialize.serialize(static_cast<U32>(type));
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<NATIVE_INT_TYPE>(status));
    status = serialize.serialize(static_cast<U32>(port));
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<NATIVE_INT_TYPE>(status));
    status = serialize.serialize(data, size);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<NATIVE_INT_TYPE>(status));
    outgoing.setSize(serialize.getBuffLength());
    dataOut_out(0, outgoing);
}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void GenericHubComponentImpl ::buffersIn_handler(const NATIVE_INT_TYPE portNum, Fw::Buffer& fwBuffer) {
    send_data(HUB_TYPE_BUFFER, portNum, fwBuffer.getData(), fwBuffer.getSize());
    bufferDeallocate_out(0, fwBuffer);
}

void GenericHubComponentImpl ::dataIn_handler(const NATIVE_INT_TYPE portNum, Fw::Buffer& fwBuffer) {
    HubType type = HUB_TYPE_MAX;
    U32 type_in = 0;
    U32 port = 0;
    FwBuffSizeType size = 0;
    Fw::SerializeStatus status = Fw::FW_SERIALIZE_OK;

    // Representation of incoming data prepped for serialization
    Fw::SerializeBufferBase& incoming = fwBuffer.getSerializeRepr();
    FW_ASSERT(incoming.setBuffLen(fwBuffer.getSize()) == Fw::FW_SERIALIZE_OK);

    // Must inform buffer that there is *real* data in the buffer
    status = incoming.setBuffLen(fwBuffer.getSize());
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<NATIVE_INT_TYPE>(status));
    status = incoming.deserialize(type_in);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<NATIVE_INT_TYPE>(status));
    type = static_cast<HubType>(type_in);
    FW_ASSERT(type < HUB_TYPE_MAX, type);
    status = incoming.deserialize(port);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<NATIVE_INT_TYPE>(status));
    status = incoming.deserialize(size);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<NATIVE_INT_TYPE>(status));

    // invokeSerial deserializes arguments before calling a normal invoke, this will return ownership immediately
    U8* rawData = fwBuffer.getData() + sizeof(U32) + sizeof(U32) + sizeof(FwBuffSizeType);
    U32 rawSize = fwBuffer.getSize() - sizeof(U32) - sizeof(U32) - sizeof(FwBuffSizeType);
    FW_ASSERT(rawSize == static_cast<U32>(size));
    if (type == HUB_TYPE_PORT) {
        // Com buffer representations should be copied before the call returns, so we need not "allocate" new data
        Fw::ExternalSerializeBuffer wrapper(rawData, rawSize);
        status = wrapper.setBuffLen(rawSize);
        FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<NATIVE_INT_TYPE>(status));
        portOut_out(port, wrapper);
        // Deallocate the existing buffer
        dataInDeallocate_out(0, fwBuffer);
    } else if (type == HUB_TYPE_BUFFER) {
        // Fw::Buffers can reuse the existing data buffer as the storage type!  No deallocation done.
        fwBuffer.set(rawData, rawSize, fwBuffer.getContext());
        buffersOut_out(port, fwBuffer);
    } else if (type == HUB_TYPE_EVENT) {
        FwEventIdType id;
        Fw::Time timeTag;
        Fw::LogSeverity severity;
        Fw::LogBuffer args;

        // Deserialize tokens for events
        status = incoming.deserialize(id);
        FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<NATIVE_INT_TYPE>(status));
        status = incoming.deserialize(timeTag);
        FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<NATIVE_INT_TYPE>(status));
        status = incoming.deserialize(severity);
        FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<NATIVE_INT_TYPE>(status));
        status = incoming.deserialize(args);
        FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<NATIVE_INT_TYPE>(status));

        // Send it!
        this->LogSend_out(port, id, timeTag, severity, args);

        // Deallocate the existing buffer
        dataInDeallocate_out(0, fwBuffer);
    } else if (type == HUB_TYPE_CHANNEL) {
        FwChanIdType id;
        Fw::Time timeTag;
        Fw::TlmBuffer val;

        // Deserialize tokens for channels
        status = incoming.deserialize(id);
        FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<NATIVE_INT_TYPE>(status));
        status = incoming.deserialize(timeTag);
        FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<NATIVE_INT_TYPE>(status));
        status = incoming.deserialize(val);
        FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<NATIVE_INT_TYPE>(status));

        // Send it!
        this->TlmSend_out(port, id, timeTag, val);

        // Deallocate the existing buffer
        dataInDeallocate_out(0, fwBuffer);
    }
}

void GenericHubComponentImpl ::LogRecv_handler(const NATIVE_INT_TYPE portNum,
                                  FwEventIdType id,
                                  Fw::Time& timeTag,
                                  const Fw::LogSeverity& severity,
                                  Fw::LogBuffer& args) {
    U8 buffer[sizeof(FwEventIdType) + Fw::Time::SERIALIZED_SIZE + Fw::LogSeverity::SERIALIZED_SIZE + FW_LOG_BUFFER_MAX_SIZE];
    Fw::ExternalSerializeBuffer serializer(buffer, sizeof(buffer));
    serializer.resetSer();
    FW_ASSERT(serializer.serialize(id) == Fw::SerializeStatus::FW_SERIALIZE_OK);;
    FW_ASSERT(serializer.serialize(timeTag) == Fw::SerializeStatus::FW_SERIALIZE_OK);
    FW_ASSERT(serializer.serialize(severity) == Fw::SerializeStatus::FW_SERIALIZE_OK);
    FW_ASSERT(serializer.serialize(args) == Fw::SerializeStatus::FW_SERIALIZE_OK);
    U32 size = serializer.getBuffLength();
    this->send_data(HubType::HUB_TYPE_EVENT, portNum, buffer, size);

}

void GenericHubComponentImpl ::TlmRecv_handler(const NATIVE_INT_TYPE portNum,
                                  FwChanIdType id,
                                  Fw::Time& timeTag,
                                  Fw::TlmBuffer& val) {
    U8 buffer[sizeof(FwChanIdType) + Fw::Time::SERIALIZED_SIZE + FW_TLM_BUFFER_MAX_SIZE];
    Fw::ExternalSerializeBuffer serializer(buffer, sizeof(buffer));
    serializer.resetSer();
    FW_ASSERT(serializer.serialize(id) == Fw::SerializeStatus::FW_SERIALIZE_OK);
    FW_ASSERT(serializer.serialize(timeTag) == Fw::SerializeStatus::FW_SERIALIZE_OK);
    FW_ASSERT(serializer.serialize(val) == Fw::SerializeStatus::FW_SERIALIZE_OK);
    U32 size = serializer.getBuffLength();
    this->send_data(HubType::HUB_TYPE_CHANNEL, portNum, buffer, size);
}

// ----------------------------------------------------------------------
// Handler implementations for user-defined serial input ports
// ----------------------------------------------------------------------

void GenericHubComponentImpl ::portIn_handler(NATIVE_INT_TYPE portNum,        /*!< The port number*/
                                              Fw::SerializeBufferBase& Buffer /*!< The serialization buffer*/
) {
    send_data(HUB_TYPE_PORT, portNum, Buffer.getBuffAddr(), Buffer.getBuffLength());
}

}  // end namespace Svc

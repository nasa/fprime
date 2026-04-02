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
    status = serialize.serializeFrom(data, static_cast<FwBuffSizeType>(size));
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(status));
    outgoing.setSize(serialize.getSize());
    toBufferDriver_out(0, outgoing);
}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void GenericHub::bufferIn_handler(const FwIndexType portNum, Fw::Buffer& fwBuffer) {
    this->send_data(HUB_TYPE_BUFFER, portNum, fwBuffer.getData(), fwBuffer.getSize());
    bufferInReturn_out(portNum, fwBuffer);
}

void GenericHub::bufferOutReturn_handler(FwIndexType portNum, Fw::Buffer& fwBuffer) {
    // Return the buffer
    fromBufferDriverReturn_out(0, fwBuffer);
}

void GenericHub ::cmdDispIn_handler(FwIndexType portNum, Fw::ComBuffer& data, U32 context) {
    Fw::SerializeStatus status;
    // Buffer to send and a buffer used to write to it
    U8 buffer[Fw::ComBuffer::SERIALIZED_SIZE];

    Fw::ExternalSerializeBuffer serializer(buffer, sizeof(buffer));
    serializer.resetSer();

    status = serializer.serializeFrom(data.getBuffAddr(), data.getSize(), Fw::Serialization::OMIT_LENGTH);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(status));
    status = serializer.serializeFrom(context);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(status));
    FwSizeType size = serializer.getSize();

    this->send_data(HUB_TYPE_CMD_DISP, portNum, buffer, size);
}

void GenericHub ::cmdRespIn_handler(FwIndexType portNum,
                                    FwOpcodeType opCode,
                                    U32 cmdSeq,
                                    const Fw::CmdResponse& response) {
    Fw::SerializeStatus status = Fw::FW_SERIALIZE_OK;
    U8 buffer[sizeof(opCode) + sizeof(cmdSeq) + Fw::CmdResponse::SERIALIZED_SIZE];
    Fw::ExternalSerializeBuffer serializer(buffer, sizeof(buffer));
    serializer.resetSer();

    status = serializer.serializeFrom(opCode);
    FW_ASSERT(status == Fw::SerializeStatus::FW_SERIALIZE_OK);
    status = serializer.serializeFrom(cmdSeq);
    FW_ASSERT(status == Fw::SerializeStatus::FW_SERIALIZE_OK);
    status = serializer.serializeFrom(response);
    FW_ASSERT(status == Fw::SerializeStatus::FW_SERIALIZE_OK);
    FwSizeType size = serializer.getSize();

    this->send_data(HUB_TYPE_CMD_RESP, portNum, buffer, size);
}

void GenericHub::fromBufferDriver_handler(const FwIndexType portNum, Fw::Buffer& fwBuffer) {
    HubType type = HUB_TYPE_MAX;
    U32 type_in = 0;
    U32 port = 0;
    FwBuffSizeType size = 0;
    Fw::SerializeStatus status = Fw::FW_SERIALIZE_OK;

    // Representation of incoming data prepped for serialization
    auto incoming = fwBuffer.getDeserializer();
    // Check the size of the data first
    if (fwBuffer.getSize() < (sizeof(U32) + sizeof(U32) + sizeof(FwBuffSizeType))) {
        status = Fw::FW_DESERIALIZE_SIZE_MISMATCH;
    } else {
        status = incoming.deserializeTo(type_in);
    }
    // If the deserialization was good, but the type is invalid, set invalid data
    if ((status == Fw::FW_SERIALIZE_OK) && (type_in >= HUB_TYPE_MAX)) {
        status = Fw::FW_DESERIALIZE_INVALID_DATA;
    }
    // If the deserialization was good and the type was valid, set the type and move onto port deserialization
    else if (status == Fw::FW_SERIALIZE_OK) {
        type = static_cast<HubType>(type_in);
        status = incoming.deserializeTo(port);
    }
    // If the deserialization was good, move onto size deserialization
    if (status == Fw::FW_SERIALIZE_OK) {
        status = incoming.deserializeTo(size);
    }
    // All deserialization looks good, check that the size matches the buffer size before calling the appropriate ports
    if (status == Fw::FW_SERIALIZE_OK &&
        (size == (fwBuffer.getSize() - sizeof(U32) - sizeof(U32) - sizeof(FwBuffSizeType)))) {
        // invokeSerial deserializes arguments before calling a normal invoke, this will return ownership immediately
        U8* rawData = fwBuffer.getData() + sizeof(U32) + sizeof(U32) + sizeof(FwBuffSizeType);
        FwSizeType rawSize = fwBuffer.getSize() - sizeof(U32) - sizeof(U32) - sizeof(FwBuffSizeType);
        if (type == HUB_TYPE_PORT) {
            // Com buffer representations should be copied before the call returns, so we need not "allocate" new data
            Fw::ExternalSerializeBuffer wrapper(rawData, rawSize);
            status = wrapper.setBuffLen(rawSize);
            FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(status));
            // Confirm that the port is valid and connected before calling out
            if (port < this->getNum_serialOut_OutputPorts() &&
                this->isConnected_serialOut_OutputPort(static_cast<FwIndexType>(port))) {
                serialOut_out(static_cast<FwIndexType>(port), wrapper);
            }
            // Deallocate the existing buffer
            fromBufferDriverReturn_out(0, fwBuffer);
        } else if (type == HUB_TYPE_BUFFER) {
            // Fw::Buffers can reuse the existing data buffer as the storage type!  No deallocation done.
            fwBuffer.set(rawData, rawSize, fwBuffer.getContext());
            // Confirm that the port is valid and connected before calling out
            if (port < this->getNum_bufferOut_OutputPorts() &&
                this->isConnected_bufferOut_OutputPort(static_cast<FwIndexType>(port))) {
                bufferOut_out(static_cast<FwIndexType>(port), fwBuffer);
            } else {
                // Return the buffer if the port is invalid or not connected to avoid leaks
                fromBufferDriverReturn_out(0, fwBuffer);
            }
        } else if (type == HUB_TYPE_EVENT) {
            FwEventIdType id;
            Fw::Time timeTag;
            Fw::LogSeverity severity;
            Fw::LogBuffer args;

            // Deserialize tokens for events
            status = incoming.deserializeTo(id);
            if (status == Fw::FW_SERIALIZE_OK) {
                status = incoming.deserializeTo(timeTag);
            }
            if (status == Fw::FW_SERIALIZE_OK) {
                status = incoming.deserializeTo(severity);
            }
            if (status == Fw::FW_SERIALIZE_OK) {
                status = incoming.deserializeTo(args);
            }

            // Send it!
            if ((status == Fw::FW_SERIALIZE_OK) && (port < this->getNum_eventOut_OutputPorts()) &&
                this->isConnected_eventOut_OutputPort(static_cast<FwIndexType>(port))) {
                this->eventOut_out(static_cast<FwIndexType>(port), id, timeTag, severity, args);
            }
            // Deallocate the existing buffer
            fromBufferDriverReturn_out(0, fwBuffer);
        } else if (type == HUB_TYPE_CHANNEL) {
            FwChanIdType id;
            Fw::Time timeTag;
            Fw::TlmBuffer val;

            // Deserialize tokens for channels
            status = incoming.deserializeTo(id);
            if (status == Fw::FW_SERIALIZE_OK) {
                status = incoming.deserializeTo(timeTag);
            }
            if (status == Fw::FW_SERIALIZE_OK) {
                status = incoming.deserializeTo(val);
            }
            if ((status == Fw::FW_SERIALIZE_OK) && (port < this->getNum_tlmOut_OutputPorts()) &&
                this->isConnected_tlmOut_OutputPort(static_cast<FwIndexType>(port))) {
                // Send it!
                this->tlmOut_out(static_cast<FwIndexType>(port), id, timeTag, val);
            }

            // Return the received buffer
            fromBufferDriverReturn_out(0, fwBuffer);
        } else if (type == HUB_TYPE_CMD_DISP) {
            U32 context;
            // Check that the size is sufficient for the context
            if (rawSize < sizeof(U32) || (rawSize - sizeof(U32)) > Fw::ComBuffer::SERIALIZED_SIZE) {
                status = Fw::FW_DESERIALIZE_SIZE_MISMATCH;
            }
            // Shift the command buffer out and deserialize the context
            if (status == Fw::FW_SERIALIZE_OK) {
                Fw::ComBuffer wrapper(rawData, (rawSize - sizeof(U32)));
                status = wrapper.setBuffLen(rawSize - sizeof(U32));
                FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(status));
                // Skip the command buffer that has already been wrapped
                status = incoming.deserializeSkip(rawSize - sizeof(U32));
                FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(status));
                status = incoming.deserializeTo(context);
                // Send it!
                if ((status == Fw::FW_SERIALIZE_OK) && (port < this->getNum_cmdDispOut_OutputPorts()) &&
                    this->isConnected_cmdDispOut_OutputPort(static_cast<FwIndexType>(port))) {
                    this->cmdDispOut_out(static_cast<FwIndexType>(port), wrapper, context);
                }
            }
            // Deallocate the existing buffer
            fromBufferDriverReturn_out(0, fwBuffer);
        } else if (type == HUB_TYPE_CMD_RESP) {
            FwOpcodeType opCode;
            U32 cmdSeq;
            Fw::CmdResponse response;

            // Deserialize tokens for channels
            status = incoming.deserializeTo(opCode);
            if (status == Fw::FW_SERIALIZE_OK) {
                status = incoming.deserializeTo(cmdSeq);
            }
            if (status == Fw::FW_SERIALIZE_OK) {
                status = incoming.deserializeTo(response);
            }
            // Send it!
            if ((status == Fw::FW_SERIALIZE_OK) && (port < this->getNum_cmdRespOut_OutputPorts()) &&
                this->isConnected_cmdRespOut_OutputPort(static_cast<FwIndexType>(port))) {
                this->cmdRespOut_out(static_cast<FwIndexType>(port), opCode, cmdSeq, response);
            }
            // Return the received buffer
            fromBufferDriverReturn_out(0, fwBuffer);
        }
    } else {
        // On deserialization failure, return the buffer to avoid leaks
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

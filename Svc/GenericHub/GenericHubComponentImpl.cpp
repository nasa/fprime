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

#include <Svc/GenericHub/GenericHubComponentImpl.hpp>
#include "Fw/Logger/Logger.hpp"
#include "Fw/Types/Assert.hpp"
#include "Fw/Types/BasicTypes.hpp"
#include "string.h"

// Required port serialization or the hub cannot work
FW_STATIC_ASSERT(FW_PORT_SERIALIZATION);

namespace Svc {

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

GenericHubComponentImpl ::GenericHubComponentImpl(const char* const compName) : GenericHubComponentBase(compName) {
    (void)::memset(m_data_in, 0, sizeof(m_data_in));
    (void)::memset(m_data_out, 0, sizeof(m_data_out));
}

void GenericHubComponentImpl ::init(const NATIVE_INT_TYPE instance) {
    GenericHubComponentBase::init(instance);
}

GenericHubComponentImpl ::~GenericHubComponentImpl(void) {}

void GenericHubComponentImpl ::send_data(const HubType type,
                                         const NATIVE_INT_TYPE port,
                                         const U8* data,
                                         const U32 size) {
    FW_ASSERT(data != NULL);
    Fw::SerializeStatus status;
    // Buffer to send and a buffer used to write to it
    Fw::Buffer buffer_out(m_data_out, sizeof(m_data_out));
    Fw::ExternalSerializeBuffer buffer_out_wrapper(reinterpret_cast<U8*>(m_data_out), sizeof(m_data_out));
    // Write data to our buffer
    status = buffer_out_wrapper.serialize(static_cast<U32>(type));
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<NATIVE_INT_TYPE>(status));
    status = buffer_out_wrapper.serialize(static_cast<U32>(port));
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<NATIVE_INT_TYPE>(status));
    status = buffer_out_wrapper.serialize(data, size);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<NATIVE_INT_TYPE>(status));
    buffer_out.setSize(buffer_out_wrapper.getBuffLength());
    Drv::SendStatus sendStatus = dataOut_out(0, buffer_out);
    // Log bad statuses
    if (sendStatus != Drv::SEND_OK) {
        Fw::Logger::logMsg("[ERROR] Failed to send in generic hub", sendStatus);
    }
}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void GenericHubComponentImpl ::buffersIn_handler(const NATIVE_INT_TYPE portNum, Fw::Buffer& fwBuffer) {
    send_data(HUB_TYPE_BUFFER, portNum, fwBuffer.getData(), fwBuffer.getSize());
}

void GenericHubComponentImpl ::dataIn_handler(const NATIVE_INT_TYPE portNum,
                                              Fw::Buffer& fwBuffer,
                                              Drv::RecvStatus recvStatus) {
    HubType type = HUB_TYPE_MAX;
    U32 type_in = 0;
    U32 port = 0;
    Fw::SerializeStatus status = Fw::FW_SERIALIZE_OK;
    Fw::ExternalSerializeBuffer buffer_in_wrapper(fwBuffer.getData(), fwBuffer.getSize());
    Fw::ExternalSerializeBuffer buffer_out_wrapper(reinterpret_cast<U8*>(m_data_in), sizeof(m_data_in));
    // Only handle good statuses
    if (recvStatus == Drv::RECV_OK) {
        // Must inform buffer that there is *real* data in the buffer
        status = buffer_in_wrapper.setBuffLen(fwBuffer.getSize());
        FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<NATIVE_INT_TYPE>(status));
        status = buffer_in_wrapper.deserialize(type_in);
        FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<NATIVE_INT_TYPE>(status));
        type = static_cast<HubType>(type_in);
        FW_ASSERT(type < HUB_TYPE_MAX, type);
        status = buffer_in_wrapper.deserialize(port);
        FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<NATIVE_INT_TYPE>(status));
        status = buffer_in_wrapper.deserialize(buffer_out_wrapper);
        FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<NATIVE_INT_TYPE>(status));
        // invokeSerial deserializes arguments before calling a normal invoke, this will return ownership immediately
        if (type == HUB_TYPE_PORT) {
            portOut_out(port, buffer_out_wrapper);
        } else if (type == HUB_TYPE_BUFFER) {
            Fw::Buffer buffer(buffer_out_wrapper.getBuffAddr(), buffer_out_wrapper.getBuffLength());
            buffersOut_out(port, buffer);
        }
    } else {
        Fw::Logger::logMsg("[ERROR] Failed to receive in generic hub", recvStatus);
    }
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

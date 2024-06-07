// ======================================================================
// \title  Deframer.cpp
// \author mstarch, bocchino
// \brief  cpp file for Deframer component implementation class
//
// \copyright
// Copyright 2009-2022, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <cstring>

#include "Fw/Logger/Logger.hpp"
#include <FpConfig.hpp>
#include "Svc/Deframer/Deframer.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Static assertions
// ----------------------------------------------------------------------

static_assert(
    DeframerCfg::POLL_BUFFER_SIZE > 0,
    "poll buffer size must be greater than zero"
);
static_assert(
    DeframerCfg::RING_BUFFER_SIZE > 0,
    "ring buffer size must be greater than zero"
);

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

Deframer ::Deframer(const char* const compName) :
    DeframerComponentBase(compName),
    DeframingProtocolInterface(),
    m_protocol(nullptr),
    m_inRing(m_ringBuffer, sizeof m_ringBuffer)
{
    (void) memset(m_pollBuffer, 0, sizeof m_pollBuffer);
}

void Deframer ::init(const NATIVE_INT_TYPE instance) {
    DeframerComponentBase::init(instance);
}

Deframer ::~Deframer() {}

void Deframer ::setup(DeframingProtocol& protocol) {
    // Check that this is the first time we are calling setup
    FW_ASSERT(m_protocol == nullptr);
    // Assign the protocol passed in to m_protocol
    m_protocol = &protocol;
    // Pass *this as the DeframingProtocolInstance to protocol setup
    // Deframer is derived from and implements DeframingProtocolInterface
    protocol.setup(*this);
}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void Deframer ::framedIn_handler(
    const NATIVE_INT_TYPE portNum,
    Fw::Buffer& recvBuffer,
    const Drv::RecvStatus& recvStatus
) {
    // Check whether there is data to process
    if (recvStatus.e == Drv::RecvStatus::RECV_OK) {
        // There is: process the data
        processBuffer(recvBuffer);
    }
    // Deallocate the buffer
    framedDeallocate_out(0, recvBuffer);
}

void Deframer ::schedIn_handler(
    const NATIVE_INT_TYPE portNum,
    U32 context
) {
    // Check for data
    Fw::Buffer buffer(m_pollBuffer, sizeof(m_pollBuffer));
    const Drv::PollStatus status = framedPoll_out(0, buffer);
    if (status.e == Drv::PollStatus::POLL_OK) {
        // Data exists: process it
        processBuffer(buffer);
    }
}

// ----------------------------------------------------------------------
// Implementation of DeframingProtocolInterface
// ----------------------------------------------------------------------

Fw::Buffer Deframer ::allocate(const U32 size)  {
    return bufferAllocate_out(0, size);
}

void Deframer ::route(Fw::Buffer& packetBuffer) {
    // Send the buffer out
    bufferOut_out(0, packetBuffer);
}

// ----------------------------------------------------------------------
// Helper methods
// ----------------------------------------------------------------------

void Deframer ::processBuffer(Fw::Buffer& buffer) {

    const U32 bufferSize = buffer.getSize();
    U8 *const bufferData = buffer.getData();
    // Current offset into buffer
    U32 offset = 0;
    // Remaining data in buffer
    U32 remaining = bufferSize;

    for (U32 i = 0; i < bufferSize; ++i) {
        // If there is no data left, exit the loop
        if (remaining == 0) {
            break;
        }
        // Compute the size of data to serialize
        const NATIVE_UINT_TYPE ringFreeSize = m_inRing.get_free_size();
        const NATIVE_UINT_TYPE serSize = (ringFreeSize <= remaining) ?
            ringFreeSize : static_cast<NATIVE_UINT_TYPE>(remaining);
        // Serialize data into the ring buffer
        const Fw::SerializeStatus status =
            m_inRing.serialize(&bufferData[offset], serSize);
        // If data does not fit, there is a coding error
        FW_ASSERT(
            status == Fw::FW_SERIALIZE_OK,
            static_cast<FwAssertArgType>(status),
            static_cast<FwAssertArgType>(offset),
            static_cast<FwAssertArgType>(serSize));
        // Process the data
        processRing();
        // Update buffer offset and remaining
        offset += serSize;
        remaining -= serSize;
    }

    // In every iteration, either remaining == 0 and we break out
    // of the loop, or we consume at least one byte from the buffer.
    // So there should be no data left in the buffer.
    FW_ASSERT(remaining == 0, static_cast<FwAssertArgType>(remaining));

}

void Deframer ::processRing() {

    FW_ASSERT(m_protocol != nullptr);

    // The number of remaining bytes in the ring buffer
    U32 remaining = 0;
    // The protocol status
    DeframingProtocol::DeframingStatus status =
        DeframingProtocol::DEFRAMING_STATUS_SUCCESS;
    // The ring buffer capacity
    const NATIVE_UINT_TYPE ringCapacity = m_inRing.get_capacity();

    // Process the ring buffer looking for at least the header
    for (U32 i = 0; i < ringCapacity; i++) {
        // Get the number of bytes remaining in the ring buffer
        remaining = m_inRing.get_allocated_size();
        // If there are none, we are done
        if (remaining == 0) {
            break;
        }
        // Needed is an out-only variable
        // Initialize it to zero
        U32 needed = 0;
        // Call the deframe method of the protocol, getting
        // needed and status
        status = m_protocol->deframe(m_inRing, needed);
        // Deframing protocol must not consume data in the ring buffer
        FW_ASSERT(
            m_inRing.get_allocated_size() == remaining,
            static_cast<FwAssertArgType>(m_inRing.get_allocated_size()),
            static_cast<FwAssertArgType>(remaining)
        );
        // On successful deframing, consume data from the ring buffer now
        if (status == DeframingProtocol::DEFRAMING_STATUS_SUCCESS) {
            // If deframing succeeded, protocol should set needed
            // to a non-zero value
            FW_ASSERT(needed != 0);
            FW_ASSERT(
                needed <= remaining,
                static_cast<FwAssertArgType>(needed),
                static_cast<FwAssertArgType>(remaining));
            m_inRing.rotate(needed);
            FW_ASSERT(
                m_inRing.get_allocated_size() == remaining - needed,
                static_cast<FwAssertArgType>(m_inRing.get_allocated_size()),
                static_cast<FwAssertArgType>(remaining),
                static_cast<FwAssertArgType>(needed)
            );
        }
        // More data needed
        else if (status == DeframingProtocol::DEFRAMING_MORE_NEEDED) {
            // Deframing protocol should not report "more is needed"
            // unless more is needed
            FW_ASSERT(
                needed > remaining,
                static_cast<FwAssertArgType>(needed),
                static_cast<FwAssertArgType>(remaining));
            // Break out of loop: suspend deframing until we receive
            // another buffer
            break;
        }
        // Error occurred
        else {
            // Skip one byte of bad data
            m_inRing.rotate(1);
            FW_ASSERT(
                m_inRing.get_allocated_size() == remaining - 1,
                static_cast<FwAssertArgType>(m_inRing.get_allocated_size()),
                static_cast<FwAssertArgType>(remaining)
            );
            // Log checksum errors
            // This is likely a real error, not an artifact of other data corruption
            if (status == DeframingProtocol::DEFRAMING_INVALID_CHECKSUM) {
                Fw::Logger::logMsg("[ERROR] Deframing checksum validation failed\n");
            }
        }
    }

    // If more not needed, circular buffer should be empty
    if (status != DeframingProtocol::DEFRAMING_MORE_NEEDED) {
        FW_ASSERT(remaining == 0, static_cast<FwAssertArgType>(remaining));
    }

}

}  // end namespace Svc

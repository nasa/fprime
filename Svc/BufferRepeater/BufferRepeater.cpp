// ======================================================================
// \title  BufferRepeater.cpp
// \author lestarch
// \brief  cpp file for GenericRepeater component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <FpConfig.hpp>
#include <Svc/BufferRepeater/BufferRepeater.hpp>

namespace Svc {

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

BufferRepeater ::BufferRepeater(const char* const compName)
    : BufferRepeaterComponentBase(compName),
      m_allocation_failure_response(BufferRepeater::NUM_BUFFER_REPEATER_FAILURE_OPTIONS) {}

void BufferRepeater ::init(const NATIVE_INT_TYPE instance) {
    BufferRepeaterComponentBase::init(instance);
}

BufferRepeater ::~BufferRepeater() {}

void BufferRepeater ::configure(BufferRepeater::BufferRepeaterFailureOption allocation_failure_response) {
    this->m_allocation_failure_response = allocation_failure_response;
}

bool BufferRepeater ::check_allocation(FwIndexType index,
                                       const Fw::Buffer& new_allocation,
                                       const Fw::Buffer& incoming_buffer) {
    FW_ASSERT(index < NUM_PORTOUT_OUTPUT_PORTS, index);
    bool is_valid = (new_allocation.getData() != nullptr) && (new_allocation.getSize() >= incoming_buffer.getSize());

    // Respond to invalid buffer allocation
    if (!is_valid) {
        switch (this->m_allocation_failure_response) {
            case NO_RESPONSE_ON_OUT_OF_MEMORY:
                // No response intended
                break;
            case WARNING_ON_OUT_OF_MEMORY:
                this->log_WARNING_HI_AllocationSoftFailure(index, incoming_buffer.getSize());
                break;
            case FATAL_ON_OUT_OF_MEMORY:
                this->log_FATAL_AllocationHardFailure(index, incoming_buffer.getSize());
                break;
            default:
                FW_ASSERT(0);
                break;
        }
    }
    return is_valid;
}

// ----------------------------------------------------------------------
// Handler implementations for user-defined serial input ports
// ----------------------------------------------------------------------

void BufferRepeater ::portIn_handler(NATIVE_INT_TYPE portNum, /*!< The port number*/
                                     Fw::Buffer& buffer       /*!< The serialization buffer*/
) {
    FW_ASSERT(this->m_allocation_failure_response < NUM_BUFFER_REPEATER_FAILURE_OPTIONS);
    for (FwIndexType i = 0; i < NUM_PORTOUT_OUTPUT_PORTS; i++) {
        if (isConnected_portOut_OutputPort(i)) {
            Fw::Buffer new_allocation = this->allocate_out(0, buffer.getSize());
            if (this->check_allocation(i, new_allocation, buffer)) {
                // Clone the data and send it
                ::memcpy(new_allocation.getData(), buffer.getData(), buffer.getSize());
                new_allocation.setSize(buffer.getSize());
                this->portOut_out(i, new_allocation);
            }
        }
    }
    this->deallocate_out(0, buffer);
}
}  // end namespace Svc

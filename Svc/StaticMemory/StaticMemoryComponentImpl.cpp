// ======================================================================
// \title  StaticMemoryComponentImpl.cpp
// \author mstarch
// \brief  cpp file for StaticMemory component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <Fw/FPrimeBasicTypes.hpp>
#include <Svc/StaticMemory/StaticMemoryComponentImpl.hpp>
#include "Fw/Types/Assert.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

StaticMemoryComponentImpl ::StaticMemoryComponentImpl(const char* const compName)
    : StaticMemoryComponentBase(compName) {
    static_assert(
        (StaticMemoryComponentBase::NUM_BUFFERALLOCATE_INPUT_PORTS >= 0) &&
            (StaticMemoryComponentBase::NUM_BUFFERALLOCATE_INPUT_PORTS <= std::numeric_limits<FwIndexType>::max()),
        "NUM_BUFFERALLOCATE_INPUT_PORTS must fit in the positive range of FwIndexType");
    for (FwIndexType i = 0; i < static_cast<FwIndexType>(FW_NUM_ARRAY_ELEMENTS(m_allocated)); i++) {
        m_allocated[i] = false;
    }
}

StaticMemoryComponentImpl ::~StaticMemoryComponentImpl() {}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void StaticMemoryComponentImpl ::bufferDeallocate_handler(const FwIndexType portNum, Fw::Buffer& fwBuffer) {
    FW_ASSERT(portNum < static_cast<FwIndexType>(FW_NUM_ARRAY_ELEMENTS(m_static_memory)));
    FW_ASSERT(m_allocated[portNum],
              static_cast<FwAssertArgType>(portNum));  // It is also an error to deallocate before returning
    // Check the memory returned is within the region
    FW_ASSERT(fwBuffer.getData() >= m_static_memory[portNum]);
    FW_ASSERT((fwBuffer.getData() + fwBuffer.getSize()) <= (m_static_memory[portNum] + sizeof(m_static_memory[0])),
              static_cast<FwAssertArgType>(fwBuffer.getSize()),
              static_cast<FwAssertArgType>(sizeof(m_static_memory[0])));
    m_allocated[portNum] = false;
}

Fw::Buffer StaticMemoryComponentImpl ::bufferAllocate_handler(const FwIndexType portNum, FwSizeType size) {
    FW_ASSERT(portNum < static_cast<FwIndexType>(FW_NUM_ARRAY_ELEMENTS(m_static_memory)));
    FW_ASSERT(size <=
              sizeof(m_static_memory[portNum]));  // It is a topology error to ask for too much from this component
    FW_ASSERT(not m_allocated[portNum],
              static_cast<FwAssertArgType>(portNum));  // It is also an error to allocate again before returning
    m_allocated[portNum] = true;
    Fw::Buffer buffer(m_static_memory[portNum], sizeof(m_static_memory[0]));
    return buffer;
}

}  // end namespace Svc

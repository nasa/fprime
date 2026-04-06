/**
 * \file DirectPortCallsExample.cpp
 * \author F Prime Architecture Team
 * \brief Example implementation of autocoded components with direct port calls
 *
 * \copyright
 * Copyright 2009-2026, by the California Institute of Technology.
 * ALL RIGHTS RESERVED.  United States Government Sponsorship
 * acknowledged.
 */

#include "Fw/Ports/DirectPortCallsExample.hpp"

// ======================================================================
// ExampleSenderComponent implementation
// ======================================================================

ExampleSenderComponent::ExampleSenderComponent(const char* name)
    : Fw::ComponentBase(name)
#if FW_DIRECT_PORT_CALLS
      ,
      m_receiver1_ref(nullptr),
      m_receiver2_ref(nullptr)
#endif
{
}

ExampleSenderComponent::~ExampleSenderComponent() {}

void ExampleSenderComponent::noArgsOut_out(FwIndexType portNum) {
#if FW_DIRECT_PORT_CALLS
    // Direct mode: use switch statement to directly invoke receiver
    switch (portNum) {
        case 0:
            FW_ASSERT(m_receiver1_ref != nullptr);
            m_receiver1_ref->noArgsAsync_handler(0);
            break;
        case 1:
            FW_ASSERT(m_receiver2_ref != nullptr);
            m_receiver2_ref->noArgsAsync_handler(0);
            break;
        default:
            FW_ASSERT(false, portNum);
            break;
    }
#else
    // Standard mode: use port objects with dynamic dispatch
    if (this->m_noArgsOut_OutputPort[portNum].isConnected()) {
        this->m_noArgsOut_OutputPort[portNum].invoke(portNum);
    }
#endif
}

void ExampleSenderComponent::dataOut_out(FwIndexType portNum, U32 arg1, U32 arg2) {
#if FW_DIRECT_PORT_CALLS
    // Direct mode: use switch statement to directly invoke receiver with arguments
    switch (portNum) {
        case 0:
            FW_ASSERT(m_receiver1_ref != nullptr);
            m_receiver1_ref->dataAsync_handler(0, arg1, arg2);
            break;
        case 1:
            FW_ASSERT(m_receiver2_ref != nullptr);
            m_receiver2_ref->dataAsync_handler(0, arg1, arg2);
            break;
        default:
            FW_ASSERT(false, portNum);
            break;
    }
#else
    // Standard mode: use port objects with dynamic dispatch
    if (this->m_dataOut_OutputPort[portNum].isConnected()) {
        this->m_dataOut_OutputPort[portNum].invoke(portNum, arg1, arg2);
    }
#endif
}

// ======================================================================
// ExampleReceiverComponent implementation
// ======================================================================

ExampleReceiverComponent::ExampleReceiverComponent(const char* name)
    : Fw::ComponentBase(name) {}

ExampleReceiverComponent::~ExampleReceiverComponent() {}

Fw::InputPort<void>* ExampleReceiverComponent::get_noArgsAsync_InputPort(FwIndexType portNum) {
    FW_ASSERT(portNum < 2, portNum);
    return &this->m_noArgsAsync_InputPort[portNum];
}

Fw::InputPort<void>* ExampleReceiverComponent::get_dataAsync_InputPort(FwIndexType portNum) {
    FW_ASSERT(portNum < 2, portNum);
    return &this->m_dataAsync_InputPort[portNum];
}


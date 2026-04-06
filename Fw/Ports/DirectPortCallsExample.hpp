/**
 * \file DirectPortCallsExample.hpp
 * \author F Prime Architecture Team
 * \brief Example of autocoded component with direct port calls support
 *
 * This file shows how the FPP compiler should generate component port methods
 * when FW_DIRECT_PORT_CALLS is enabled.
 *
 * \copyright
 * Copyright 2009-2026, by the California Institute of Technology.
 * ALL RIGHTS RESERVED.  United States Government Sponsorship
 * acknowledged.
 */

#ifndef DIRECT_PORT_CALLS_EXAMPLE_HPP
#define DIRECT_PORT_CALLS_EXAMPLE_EXAMPLE_HPP

#include "FpConfig.h"
#include "Fw/Comp/ComponentBase.hpp"
#include "Fw/Port/InputPort.hpp"
#include "Fw/Port/OutputPort.hpp"

/**
 * \class ExampleSenderComponent
 * \brief Example sender component with output ports
 *
 * This demonstrates what an autocoded sender component looks like
 * when compiled with both FW_DIRECT_PORT_CALLS=0 and FW_DIRECT_PORT_CALLS=1
 */
class ExampleSenderComponent : public Fw::ComponentBase {
  public:
    ExampleSenderComponent(const char* name);
    virtual ~ExampleSenderComponent();

  protected:
    // Output port invocation methods
    // These methods are called by user code and are autocoded by FPP

    /**
     * Invoke output port for "noArgsOut"
     * \param portNum the output port index
     */
    void noArgsOut_out(FwIndexType portNum);

    /**
     * Invoke output port for "dataOut" with arguments
     * \param portNum the output port index
     * \param arg1 first argument
     * \param arg2 second argument
     */
    void dataOut_out(FwIndexType portNum, U32 arg1, U32 arg2);

#if FW_DIRECT_PORT_CALLS
    // When direct port calls are enabled, these component references
    // are set by the topology to point directly to receiver components
    // This eliminates the need for dynamic port object initialization

    class ExampleReceiverComponent* m_receiver1_ref;
    class ExampleReceiverComponent* m_receiver2_ref;

#else
    // When direct port calls are disabled, we use standard F Prime ports
    // with dynamic dispatch through port objects

    Fw::OutputPort<void> m_noArgsOut_OutputPort[2];
    Fw::OutputPort<void> m_dataOut_OutputPort[2];

#endif  // FW_DIRECT_PORT_CALLS

  private:
    // User-implemented handlers (in derived class)
    // These are called by handlePortData_noArgsOut, etc.
};

/**
 * \class ExampleReceiverComponent
 * \brief Example receiver component with input ports
 *
 * This demonstrates what an autocoded receiver component looks like
 */
class ExampleReceiverComponent : public Fw::ComponentBase {
  public:
    ExampleReceiverComponent(const char* name);
    virtual ~ExampleReceiverComponent();

  protected:
    // Input port handler methods
    // These are called directly (in direct mode) or through port objects (standard mode)

#if FW_DIRECT_PORT_CALLS
    // Direct handlers - called directly from senders in direct mode
    virtual void noArgsAsync_handler(FwIndexType portNum) = 0;
    virtual void dataAsync_handler(FwIndexType portNum, U32 arg1, U32 arg2) = 0;

#else
    // In standard mode, these are called through port invoke mechanisms

#endif  // FW_DIRECT_PORT_CALLS

    // Input port accessors (always present for topology setup)
    Fw::InputPort<void>* get_noArgsAsync_InputPort(FwIndexType portNum);
    Fw::InputPort<void>* get_dataAsync_InputPort(FwIndexType portNum);

  private:
    // Standard port objects (always present for compatibility)
    Fw::InputPort<void> m_noArgsAsync_InputPort[2];
    Fw::InputPort<void> m_dataAsync_InputPort[2];
};

#endif  // DIRECT_PORT_CALLS_EXAMPLE_HPP


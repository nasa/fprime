// ======================================================================
// \title  AComponentAc.cpp
// \author bocchino
// \brief  cpp file for A component base class
// ======================================================================

#include "Fw/Types/Assert.hpp"
#include "Fw/Types/ExternalString.hpp"
#if FW_ENABLE_TEXT_LOGGING
#include "Fw/Types/String.hpp"
#endif
#include "FppTest/topology/components/A/AComponentAc.hpp"

namespace FppTest {

// ----------------------------------------------------------------------
// Component initialization
// ----------------------------------------------------------------------

void AComponentBase::init(FwEnumStoreType instance) {
    // Initialize base class
    Fw::PassiveComponentBase::init(instance);
}

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

AComponentBase::AComponentBase(const char* compName) : Fw::PassiveComponentBase(compName) {}

AComponentBase::~AComponentBase() {}

#if !FW_DIRECT_PORT_CALLS

// ----------------------------------------------------------------------
// Getters for typed input ports
// ----------------------------------------------------------------------

FppTest::InputDataPort* AComponentBase ::get_dataIn_InputPort(FwIndexType portNum) {
    FW_ASSERT((0 <= portNum) && (portNum < NUM_DATAIN_INPUT_PORTS), static_cast<FwAssertArgType>(portNum));

    return &this->m_dataIn_InputPort[portNum];
}

#endif

#if !FW_DIRECT_PORT_CALLS

// ----------------------------------------------------------------------
// Connect typed input ports to typed output ports
// ----------------------------------------------------------------------

void AComponentBase ::set_dataOut_OutputPort(FwIndexType portNum, FppTest::InputDataPort* port) {
    FW_ASSERT((0 <= portNum) && (portNum < NUM_DATAOUT_OUTPUT_PORTS), static_cast<FwAssertArgType>(portNum));

    this->m_dataOut_OutputPort[portNum].addCallPort(port);
}

#endif

#if !FW_DIRECT_PORT_CALLS && FW_PORT_SERIALIZATION

// ----------------------------------------------------------------------
// Connect serial input ports to typed output ports
// ----------------------------------------------------------------------

void AComponentBase ::set_dataOut_OutputPort(FwIndexType portNum, Fw::InputSerializePort* port) {
    FW_ASSERT((0 <= portNum) && (portNum < NUM_DATAOUT_OUTPUT_PORTS), static_cast<FwAssertArgType>(portNum));

    this->m_dataOut_OutputPort[portNum].registerSerialPort(port);
}

#endif

#if !FW_DIRECT_PORT_CALLS

// ----------------------------------------------------------------------
// Connection status queries for typed output ports
// ----------------------------------------------------------------------

bool AComponentBase::isConnected_dataOut_OutputPort(FwIndexType portNum) {
    FW_ASSERT((0 <= portNum) && (portNum < NUM_DATAOUT_OUTPUT_PORTS), static_cast<FwAssertArgType>(portNum));
    return this->m_dataOut_OutputPort[portNum].isConnected();
}

#endif

// ----------------------------------------------------------------------
// Port handler base-class functions for typed input ports
//
// Call these functions directly to bypass the corresponding ports
// ----------------------------------------------------------------------

void AComponentBase::dataIn_handlerBase(FwIndexType portNum, U32 data) {
    // Make sure port number is valid
    FW_ASSERT((0 <= portNum) && (portNum < NUM_DATAIN_INPUT_PORTS), static_cast<FwAssertArgType>(portNum));
    // Call handler function
    this->dataIn_handler(portNum, data);
}

#if !FW_DIRECT_PORT_CALLS

// ----------------------------------------------------------------------
// Invocation functions for typed output ports
// ----------------------------------------------------------------------


void AComponentBase::dataOut_out(FwIndexType portNum, U32 data) {
    FW_ASSERT(this->isConnected_dataOut_OutputPort(portNum), static_cast<FwAssertArgType>(portNum));

    this->m_dataOut_OutputPort[portNum].invoke(data);
}

#endif

}  // namespace FppTest
